#include "PostPowerMesh.h"

PROJECT_NAMESPACE_BEGIN

bool PostPowerMesh::build(IndexType netIdx, const std::vector<IndexType> &ports, std::shared_ptr<ShapeMap> shapeMapPtr)
{
    if (!_db.progArgs().isRouteGuideSet())
    {
        return true;
    }
    if (netIdx > _db.drDB().numNets() - 1)
    {
        return false;
    }
    _shapeMap = shapeMapPtr;
    // Check the ports are valid terminal in the net
    Assert(checkPortsValid(netIdx, ports));
    INF("Post processing net %s: add power mesh \n", _db.grDB().net(netIdx).name().c_str());
    std::vector<GuidePath> guide; 
    if (!guidePath(netIdx, ports, guide))
    {
        return false;
    }
    routePath(netIdx, guide);
    return true;
}

bool PostPowerMesh::routePath(IndexType netIdx, std::vector<GuidePath> &guides)
{
    _netIdxInRoute = netIdx;
    // Init the path search
    routePathInit();
    for (auto &guide : guides)
    {
        routeCore(guide);
    }
    return true;
}

void PostPowerMesh::routeCore(GuidePath &guide)
{
    // Set the search options 
    routeConfigurePathSearch(guide);
    // Set the effort level to be the most conservative
    _pathSearch->setEffortMode(3);
#ifdef DEBUG_DR_
    DBG("%s: solve %s %d to %s %d \n", __FUNCTION__, _primarySource.loc().toStr().c_str(), _primarySource.layer(), _parimaryTarget.loc().toStr().c_str(), _parimaryTarget.layer());
#endif
    if (_pathSearch->solvePathSearchAlgo())
    {
        // If successful, assign the solution
        routeAssignSol();
    }
    // No rip-up and reroute. If fails, let it be.
}

void PostPowerMesh::routeAssignSol()
{
    // Don't really need to maintain the tree structure here, just add them as seperate paths
    const std::vector<NodeKey> &nodeVec = _pathSearch->resultAlgo();
    for (const auto & iter : nodeVec)
    {
        DBG("%s \n", iter.toStr().c_str());
    }
    // Just add a fake nodes from the root of the tree
    IndexType nodeIdx = DRBase::addFakeNodeFromParent(_netIdxInRoute, 0, NetNodeLoc(nodeVec.back().loc, nodeVec.back().layer));
    for (IntType idx =  nodeVec.size() - 2; idx >= 0; --idx)
    {
        if (nodeVec.at(idx).layer != nodeVec.at(idx + 1).layer)
        {
            // Add a via shape
            IndexType viaType = _pathSearch->viaType(nodeVec.at(idx));
            nodeIdx = DRBase::addViaNodeFromParent(_netIdxInRoute, nodeIdx, NetNodeLoc(nodeVec.at(idx).loc, nodeVec.at(idx).layer), NetNodeVIA(viaType));
#ifdef DEBUG_DR_
            //DBG("Add via %s %d \n", nodeVec.at(idx).toStr().c_str(), nodeIdx);
#endif
        }
        else
        {
            // Add a wire shape
            nodeIdx = DRBase::addWireNodeFromParent(_netIdxInRoute, nodeIdx, NetNodeLoc(nodeVec.at(idx).loc, nodeVec.at(idx).layer), _wireTypes.at(nodeVec.at(idx).layer));
#ifdef DEBUG_DR_
            //DBG("Added wire %s %d \n", nodeVec.at(idx).toStr().c_str(), nodeIdx);
#endif
        }
    }
}

void PostPowerMesh::routeConfigurePathSearch(GuidePath &guide)
{
    IndexType netIdx = _db.terminal(guide.portTermIdx).netIdx();
    const auto &net = _db.drDB().net(netIdx);
    // Search from pin to port
    const auto &sourceTerm = _db.terminal(guide.pinTermIdx);
    const auto &targetTerm = _db.terminal(guide.portTermIdx);

    // Configure the shapeID
    /// FIXME: here assuming each pin has one shape
    ShapeID sourceShapeID = ShapeID();
    sourceShapeID.configTerminal(guide.pinTermIdx, sourceTerm.primarySearchPointShape(), true);
    ShapeID targetShapeID = ShapeID();
    targetShapeID.configTerminal(guide.portTermIdx, targetTerm.primarySearchPointShape(), true);

    // Set the goal pin region
    _shapeComp.computeFixedShape(targetShapeID);
    _pathSearch->setGoalRegion(_shapeComp.result().front().first);

    // Set the search settings
    _pathSearch->resetAlgo();
    _pathSearch->setNetIdx(netIdx);
    _pathSearch->setNoSym();


    // Set primary source node
    _pathSearch->addSourceShapeAlgo(NodeKey(sourceTerm.primarySearchPoint().loc(), sourceTerm.primarySearchPoint().layer()), sourceShapeID);
    _primarySource = sourceTerm.primarySearchPoint();
    // Primary target node
    _parimaryTarget = guide.targetLoc;
    _pathSearch->setGoalAlgo(NodeKey(guide.targetLoc.loc(), guide.targetLoc.layer()));
    // Set the source and target shape ID, so that they won't be checked in the shape map
    _shapeMapPtr->setSourceShapeID(sourceShapeID);
    _pathSearch->setSourceShapeID(sourceShapeID);
    _shapeMapPtr->setTargetShapeID(targetShapeID);
    _pathSearch->setTargetShapeID(targetShapeID);
    // Add other search points
    for (auto searchPt : sourceTerm.otherSearchPoints())
    {
        // No sym, directly add the search point
        _pathSearch->addSourceShapeAlgo(NodeKey(searchPt.loc(), searchPt.layer()), sourceShapeID);
    }
    // Add other net nodes
    for (IndexType nodeIdx : guide.netNodes)
    {
        const auto &node =net.node(nodeIdx);
        if (node.isFake())
        {
            continue;
        }
       _pathSearch->addSourceShapeAlgo(NodeKey(node.location(), node.layer()), ShapeID(netIdx, nodeIdx));
    }

    // Set routing types
    _pathSearch->setWireType(_wireTypes);
}

void PostPowerMesh::routePathInit()
{
    DRBase::_shapeMapPtr = _shapeMap;
    _viaSelect.init();
    // Use the gridless astar search for detailed routing
    auto pAstar = std::make_shared<DetailedGridlessAStar>(); 
    // The querough grid. which include the global routing solution-related information
    pAstar->initRoughGrid(::klib::createSharedPtr<RoughGrid>(_db.drDB().roughGrid()));
    // initializing the search region, routing direction and other design informawtion for the path search solver
    // init the routing direction
    pAstar->setRoutingDirection(_db.drDB().routeDirArray());
    // die region
    auto dieBox = _db.drDB().dieBox();
    dieBox.enlargeBy(3000);
    pAstar->setSearchBox(dieBox);
    // Set up the min step size for each layer: here the step size is the min spacing + min width
#if 1
    pAstar->setStepSize(_db.drDB().pitchArray());
#else
    std::vector<LocType> metalMinSp;
    for (IndexType idx = 0; idx < _instr.numRouteLayers(); ++idx)
    {
        metalMinSp.emplace_back(_instr.metalMinSpacing(idx) * 2);
    }
    pSolver->setStepSize(metalMinSp);
#endif
    // Set up the steps
    pAstar->setSteps({1,  10, 100} );
    // Set up the lowest and highest layer
    pAstar->setLayerLo(0);
    pAstar->setLayerHi(_db.drDB().numLayers() - 1);
    // The shape map
    pAstar->initShapeMap(_shapeMap);
    // The via selector
    pAstar->initViaSelect(::klib::createSharedPtr<FixedViaSelector>(_viaSelect));
    // THe shape constructor
    pAstar->initShapeConstrutor(::klib::createSharedPtr<ShapeConstructor>(_shapeComp));
    // Route guide is not set. inside the path search there is a nullptr

    // Active region
    pAstar->initActiveRegion(::klib::createSharedPtr<ActiveRegion>(_db.activeRegion()));
    // Only allow routing in guide region
    pAstar->setOnlyRouteGuideConfident(true);
    pAstar->initRouteGuide(::klib::createSharedPtr<RouteGuide>(_db.routeGuide()));
    // cast the class solver ptr to the gridless A* search
    _pathSearch = std::move(pAstar);

    // Wire type
    _wireTypes.clear();
    for (IndexType layer = 0; layer < _db.drDB().numLayers(); ++layer)
    {
        LocType minWidth = _instr.metalMinWidth(layer) * 4;
        _wireTypes.emplace_back(NetNodeWire(minWidth /2  , minWidth / 2));
    }
}

bool PostPowerMesh::guidePath(IndexType netIdx, const std::vector<IndexType> &ports, std::vector<GuidePath> &guide)
{
    if (!_db.routeGuide().hasGuide(netIdx))
    {
        ERR("PostPowerMesh:%s: the given net has no guide \n");
        return false;
    }
    // Build a 2D map with same size of the route guide (also should be the same as global routing grid)
    Vector2D<PowerMeshSearchNode> grid; 
    grid.resize(_db.routeGuide().numX(), _db.routeGuide().numY());
    Assert(_db.routeGuide().numX() == _db.grDB().numGridCellX());
    Assert(_db.routeGuide().numY() == _db.grDB().numGridCellY());

    std::set<XY<IndexType>> sources; // Use vector might be better in runtime 

    if (!guidePathPrepareGrid(netIdx, ports, grid, sources))
    {
        return false;
    }

    // Find candidate terminals
    std::set<TermPair> termPairs;
    if (!guidePathCandidates(grid, sources, termPairs, netIdx))
    {
        return false;
    }
    // Plan paths
    guidePathPlanPath(netIdx, guide, termPairs, ports);
    return true;
}

void PostPowerMesh::guidePathPlanPath(IndexType netIdx, std::vector<GuidePath> &guides, std::set<TermPair> &pairs, const std::vector<IndexType> &ports)
{
    for (const auto & pair : pairs)
    {
        guides.emplace_back(GuidePath());
        bool first = false;
        for (IndexType port : ports)
        {
            if (port == pair.termLo)
            {
                first = true;
                break;
            }
        }
        if (first)
        {
            guides.back().portTermIdx = _db.drDB().net(netIdx).terminal(pair.termLo);
            guides.back().pinTermIdx = _db.drDB().net(netIdx).terminal(pair.termHi);
        }
        else
        {
            guides.back().portTermIdx = _db.drDB().net(netIdx).terminal(pair.termHi);
            guides.back().pinTermIdx = _db.drDB().net(netIdx).terminal(pair.termLo);
        }
        guidePathPlanPathSearchPoint(netIdx, guides.back());
    }
}

void PostPowerMesh::guidePathPlanPathSearchPoint(IndexType netIdx, GuidePath &guide)
{
    const auto &portTerm = _db.terminal(guide.portTermIdx);
    const auto &pinTerm = _db.terminal(guide.pinTermIdx);
    
    // Find the best search point for the port (considering the ports are usually large in size)
    guide.targetLoc = portTerm.primarySearchPoint();
    LocType manDist = ::klib::manhattanDistance<LocType>(portTerm.primarySearchPoint().loc(), pinTerm.primarySearchPoint().loc());
    for (const auto &ptIn : portTerm.otherSearchPoints())
    {
        for (const auto &ptOut : pinTerm.otherSearchPoints())
        {
            LocType otherMan = ::klib::manhattanDistance<LocType>(ptIn.loc(), ptOut.loc());
            if (otherMan < manDist)
            {
                manDist = otherMan;
                guide.targetLoc = ptIn;
            }
        }
        LocType otherMan = ::klib::manhattanDistance<LocType>(ptIn.loc(), pinTerm.primarySearchPoint().loc());
        if (otherMan < manDist)
        {
            manDist = otherMan;
            guide.targetLoc = ptIn;
        }
    }
    // Then find other net nodes
    auto bbox = pinTerm.bbox();
    bbox.enlargeBy(POWER_MESH_NET_NODE_SEARCH_RANGE);

    for (IndexType nodeIdx = 0; nodeIdx < _db.drDB().net(netIdx).numNodes(); ++nodeIdx)
    {
        const auto &node = _db.drDB().net(netIdx).node(nodeIdx);
        if (node.isFake())
        {
            continue;
        }
        if (bbox.contain(node.location()))
        {
            guide.netNodes.emplace_back(nodeIdx);
        }
    }
    DBG("%s \n", guide.toStr().c_str());
}

bool PostPowerMesh::guidePathCandidates(Vector2D<PowerMeshSearchNode> &grid, std::set<XY<IndexType>> &sources, std::set<TermPair> &pairs, IndexType netIdx)
{
    // Find the candidate node pair
    std::set<NodePair> nodePairs;
    for (const auto &source : sources)
    {
        guidePathCandidateSearch(grid, source, nodePairs, netIdx);
    }
    // Prune the node pairs
    guidePathPruneNodePair(nodePairs, netIdx, grid);

    // Find candidate terminal pairs
    guidePathCandidateTerms(pairs, netIdx, nodePairs, grid);
    return true;
}

void PostPowerMesh::guidePathCandidateTerms(std::set<TermPair> &candidates, IndexType netIdx, std::set<NodePair> &nodePairs, Vector2D<PowerMeshSearchNode> &grid)
{
    /*
    Vector2D<IntType> termPairsPruneLUT;
    termPairsPruneLUT.resize(_db.drDB().net(netIdx).numTerminals(), _db.drDB().net(netIdx).numTerminals(), 1); // LUT.at(term1, term2) = 0 if term1 and term2 is pruned
    guidePathCandidateTermsPruneLUT(termPairsPruneLUT, grid);
    */
    for (auto nodePair : nodePairs)
    {
        // For now, just connected ports 
        if (grid.at(nodePair.nodeLo).sourcePort != INDEX_TYPE_MAX && grid.at(nodePair.nodeHi).sourcePort != INDEX_TYPE_MAX)
        {
            // If take account of non-port to port, add here
            continue;
        }
        else if (grid.at(nodePair.nodeLo).sourcePort != INDEX_TYPE_MAX && grid.at(nodePair.nodeHi).sourcePort == INDEX_TYPE_MAX)
        {
            guidePathCandidateTermsPortAndPin(candidates, netIdx, nodePair.nodeLo, nodePair.nodeHi, grid);
        }
        else if (grid.at(nodePair.nodeLo).sourcePort == INDEX_TYPE_MAX && grid.at(nodePair.nodeHi).sourcePort != INDEX_TYPE_MAX)
        {
            guidePathCandidateTermsPortAndPin(candidates, netIdx, nodePair.nodeHi, nodePair.nodeLo, grid);
        }
    }
    Assert(checkTerminalPairValid(candidates, netIdx));
}

void PostPowerMesh::guidePathCandidateTermsPruneLUT(Vector2D<IntType> &lut, Vector2D<PowerMeshSearchNode> &grid)
{
    // Prune all teminals in the same grid cell
    for (const auto &cell : grid)
    {
        for (auto itOut = cell.pins.begin(); itOut != cell.pins.end(); ++itOut)
        {
            for (auto itIn = itOut; itIn != cell.pins.end(); ++itIn)
            {
                lut.at(*itOut, *itIn) = 0;
                lut.at(*itIn, *itOut) = 0;
            }
        }
    }
}

void PostPowerMesh::guidePathCandidateTermsPortAndPin(std::set<TermPair> &candidates, IndexType netIdx, XY<IndexType> nodeSource, XY<IndexType> nodePin, Vector2D<PowerMeshSearchNode> &grid)
{
    IndexType idx = INDEX_TYPE_MAX;
    IntType cost = INT_TYPE_MAX;
    IndexType sourceTermIdx = _db.drDB().net(netIdx).terminal(grid.at(nodeSource).sourcePort);
    const auto &sourceTerm = _db.terminal(sourceTermIdx);
    // Find the closest pair of terms
    for (auto pinIdx : grid.at(nodePin).pins)
    {
        IndexType pinTermIdx = _db.drDB().net(netIdx).terminal(pinIdx);
        const auto &pinTerm = _db.terminal(pinTermIdx);
        LocType manDist = ::klib::manhattanDistance<LocType>(sourceTerm.primarySearchPoint().loc(), pinTerm.primarySearchPoint().loc());
        for (const auto &ptIn : sourceTerm.otherSearchPoints())
        {
            for (const auto &ptOut : pinTerm.otherSearchPoints())
            {
                LocType otherMan = ::klib::manhattanDistance<LocType>(ptIn.loc(), ptOut.loc());
                if (otherMan < manDist)
                {
                    manDist = otherMan;
                }
            }
        }
        if (manDist < cost)
        {
            cost = manDist;
            idx = pinIdx;
        }
    }
    candidates.insert(TermPair(grid.at(nodeSource).sourcePort, idx));
}

void PostPowerMesh::guidePathPruneNodePair(std::set<NodePair> &nodePairs, IndexType netIdx, Vector2D<PowerMeshSearchNode> &grid)
{
    std::set<NodePair> newPairs; // Really slow implementation
    for (const auto &pair : nodePairs)
    {
        bool connected = false;
        AssertMsg(pair.nodeLo != pair.nodeHi, "pair: %s %s", pair.nodeLo.toStr().c_str(), pair.nodeHi.toStr().c_str());
        if (grid.at(pair.nodeLo).sourcePort != INDEX_TYPE_MAX && grid.at(pair.nodeHi).sourcePort != INDEX_TYPE_MAX)
        {
            connected = true;
        }
        else
        {
            std::vector<IndexType> term1s;
            std::vector<IndexType> term2s;
            for (IndexType term1 : grid.at(pair.nodeLo).pins)
            {
                term1s.emplace_back(term1);
            }
            for (IndexType term2 : grid.at(pair.nodeHi).pins)
            {
                term2s.emplace_back(term2);
            }
            if (pair.nodeLo.x() > 0)
            {
                for (IndexType term1 : grid.at(pair.nodeLo.left()).pins)
                {
                    term1s.emplace_back(term1);
                }
            }
            if (pair.nodeLo.y() > 0)
            {
                for (IndexType term1 : grid.at(pair.nodeLo.bottom()).pins)
                {
                    term1s.emplace_back(term1);
                }
            }
            if (pair.nodeLo.x() < grid.xSize() - 1)
            {
                for (IndexType term1 : grid.at(pair.nodeLo.right()).pins)
                {
                    term1s.emplace_back(term1);
                }
            }
            if (pair.nodeLo.y() < grid.ySize() - 1)
            {
                for (IndexType term1 : grid.at(pair.nodeLo.top()).pins)
                {
                    term1s.emplace_back(term1);
                }
            }
            if (pair.nodeHi.x() > 0)
            {
                for (IndexType term2 : grid.at(pair.nodeHi.left()).pins)
                {
                    term2s.emplace_back(term2);
                }
            }
            if (pair.nodeHi.y() > 0)
            {
                for (IndexType term2 : grid.at(pair.nodeHi.bottom()).pins)
                {
                    term2s.emplace_back(term2);
                }
            }
            if (pair.nodeHi.x() < grid.xSize() - 1)
            {
                for (IndexType term2 : grid.at(pair.nodeHi.right()).pins)
                {
                    term2s.emplace_back(term2);
                }
            }
            if (pair.nodeHi.y() < grid.ySize() - 1)
            {
                for (IndexType term2 : grid.at(pair.nodeHi.top()).pins)
                {
                    term2s.emplace_back(term2);
                }
            }
            for (IndexType term1 : term1s)
            {
                if (connected) { break; }
                for (IndexType term2 : term2s)
                {
                    if (connected) { break; }
                    if (term1 == term2)
                    {
                        connected = true;
                        break;
                    }
                    for (const auto branch : _db.drDB().net(netIdx).branches())
                    {
                        if (branch.first == _db.drDB().net(netIdx).terminal(term1) && branch.second == _db.drDB().net(netIdx).terminal(term2)) 
                        {
                            connected = true;
                            break;
                        }
                        if (branch.first == _db.drDB().net(netIdx).terminal(term2) && branch.second == _db.drDB().net(netIdx).terminal(term1))
                        {
                            connected = true;
                            break;
                        }
                    }
                }
            }
            if (!connected)
            {
                // Prune redudant pairs
                bool shouldInsert = true;
                for (const auto &existPair : newPairs)
                {
                    XY<IndexType> difIdxExist;
                    XY<IndexType> difIdxNew;
                    XY<IndexType> sameIdx;
                    if (existPair.nodeLo == pair.nodeLo && existPair.nodeHi == pair.nodeHi)
                    {
                        shouldInsert = false;
                        break;
                    }
                    else if (existPair.nodeLo == pair.nodeLo)
                    {
                        sameIdx = pair.nodeHi;
                        difIdxExist = existPair.nodeHi;
                        difIdxNew = pair.nodeHi;
                    }
                    else if (existPair.nodeHi == pair.nodeHi)
                    {
                        sameIdx = pair.nodeLo;
                        difIdxExist = existPair.nodeLo;
                        difIdxNew = pair.nodeLo;
                    }
                    else 
                    {
                        // not similarity
                        continue;
                    }
                    // Check distance
                    IndexType idxDif = ::klib::idxDif(difIdxExist.x(), difIdxNew.x()) + ::klib::idxDif(difIdxExist.y(), difIdxNew.y());
                    if (idxDif < POWER_MESH_CLOSEST_PAIR_RANGE)
                    {
                        IndexType difOld = ::klib::idxDif(difIdxExist.x(), sameIdx.x()) + ::klib::idxDif(difIdxExist.y(), sameIdx.y());
                        IndexType difNew = ::klib::idxDif(sameIdx.x(), difIdxNew.x()) + ::klib::idxDif(sameIdx.y(), difIdxNew.y());
                        if (difOld > difNew)
                        {
                            newPairs.erase(newPairs.find(existPair));
                            shouldInsert = true;
                            break;
                        }
                        else
                        {
                            shouldInsert = false;
                            break;
                        }
                    }
                }
                if (shouldInsert)
                {
                    newPairs.insert(pair);
                }
            }
        }
    }
    nodePairs = newPairs;
}

bool PostPowerMesh::guidePathPrepareGrid(IndexType netIdx, const std::vector<IndexType> &ports, Vector2D<PowerMeshSearchNode> &grid, std::set<XY<IndexType>> &sources)
{
    // Adds the ports and other pins into the grid
    for (IndexType port : ports)
    {
        const auto &term = _db.terminal(_db.drDB().net(netIdx).terminal(port));
        auto gridIdx = _db.grDB().gridIdx(term.primarySearchPoint().loc());
        grid.at(gridIdx).sourcePort = port;
        grid.at(gridIdx).pins.insert(port);
        sources.insert(gridIdx);
        for (const auto &other : term.otherSearchPoints())
        {
            gridIdx = _db.grDB().gridIdx(other.loc());
            grid.at(gridIdx).sourcePort = port;
            grid.at(gridIdx).pins.insert(port);
            sources.insert(gridIdx);
        }
    }
    for (IndexType idx = 0 ;  idx < _db.drDB().net(netIdx).terminalArray().size(); ++ idx)
    {
        IndexType termIdx = _db.drDB().net(netIdx).terminal(idx);
        // Do not count ports
        for (auto port : ports)
        {
            if (port == termIdx)
            {
                continue;
            }
        }
        const auto &term = _db.terminal(termIdx);
        auto gridIdx = _db.grDB().gridIdx(term.primarySearchPoint().loc());
        grid.at(gridIdx).pins.insert(idx);
        sources.insert(gridIdx);
        for (const auto &other : term.otherSearchPoints())
        {
            gridIdx = _db.grDB().gridIdx(other.loc());
            grid.at(gridIdx).pins.insert(idx);
            sources.insert(gridIdx);
        }
    }

    return true;
}

void PostPowerMesh::guidePathCandidateSearch(Vector2D<PowerMeshSearchNode> &grid, XY<IndexType> source, std::set<NodePair> &pairs, IndexType netIdx)
{
    Vector2D<IntType> visited;
    visited.resize(grid.xSize(), grid.ySize(), 0);

    // Do a BFS
    std::list<XY<IndexType>> queue;
    queue.emplace_back(source);
    visited.at(source) = 1;

    while (!queue.empty())
    {
        auto front = queue.front();
        queue.pop_front();
        // Up
        if (front.y() < grid.ySize() - 1)
        {
            if (guidePathCandidateSearchVisit(front.top(), queue, grid, netIdx, visited))
            {
                pairs.insert(NodePair(source, front.top()));
            }
        }
        // Down 
        if (front.y() > 1)
        {
            if (guidePathCandidateSearchVisit(front.bottom(), queue, grid, netIdx, visited))
            {
                pairs.insert(NodePair(source, front.bottom()));
            }
        }
        // Left
        if (front.x() > 1)
        {
            if (guidePathCandidateSearchVisit(front.left(), queue, grid, netIdx, visited))
            {
                pairs.insert(NodePair(source, front.left()));
            }
        }
        // Right
        if (front.x() < grid.xSize() - 1)
        {
            if (guidePathCandidateSearchVisit(front.right(), queue, grid, netIdx, visited))
            {
                pairs.insert(NodePair(source, front.right()));
            }
        }
    }
}

bool PostPowerMesh::guidePathCandidateSearchVisit(XY<IndexType> node, std::list<XY<IndexType>> &queue, Vector2D<PowerMeshSearchNode> &grid, IndexType netIdx, Vector2D<IntType> &visited)
{
    if (visited.at(node) == 1)
    {
        return false;
    }
    visited.at(node) = 1;
    if (_db.routeGuide().confident(netIdx, node))
    {
        if (!grid.at(node).empty())
        {
            // This node can be paired with the source of this search
            return true;
        }
        else
        {
            queue.emplace_back(node);
            return false;
        }
    }
    return false;
}

bool PostPowerMesh::checkPortsValid(IndexType netIdx, const std::vector<IndexType> &ports) const
{
    IndexType numTerms = _db.drDB().net(netIdx).numTerminals();
    for (auto idx : ports)
    {
        if (idx >= numTerms) { return false; }
    }
    return true;
}

bool PostPowerMesh::checkTerminalPairValid(const std::set<TermPair> &pairs, IndexType netIdx) const
{
    for (const auto pair : pairs)
    {
        IndexType term1 = _db.drDB().net(netIdx).terminal(pair.termLo);
        IndexType term2 = _db.drDB().net(netIdx).terminal(pair.termHi);
        for (const auto &branch : _db.drDB().net(netIdx).branches())
        {
            if (branch.first == term1 && branch.second == term2)
            {
                return false;
            }
            if (branch.first == term2 && branch.second == term1)
            {
                return false;
            }
        }
    }
    return true;
}
PROJECT_NAMESPACE_END
