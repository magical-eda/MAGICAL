#include "DetailedNCR.h"

PROJECT_NAMESPACE_BEGIN

void DetailedNCR::dummyPinShape()
{
    for (IndexType netIdx = 0; netIdx < _db.drDB().numNets(); ++netIdx)
    {
        for (IndexType pinIdx = 0; pinIdx < _db.drDB().net(netIdx).numPins(); ++pinIdx)
        {
            IndexType gloPinIdx = _db.drDB().net(netIdx).pin(pinIdx);
            auto &pin = _db.drDB().pin(gloPinIdx);
            XY<LocType> loc = pin.loc();
            pin.addPinShapePoint(XY<LocType>(loc.x() - 70, loc.y() - 70)); 
            pin.addPinShapePoint(XY<LocType>(loc.x() + 70, loc.y() - 70)); 
            pin.addPinShapePoint(XY<LocType>(loc.x() - 70, loc.y() + 70)); 
            pin.addPinShapePoint(XY<LocType>(loc.x() + 70, loc.y() + 70)); 
            pin.init();
            for (const auto &rect : pin.shapeRects())
            {
                _shapeMapPtr->insertFixedShape(rect, DRUtil::routeLayer2AllLayer(pin.layer()), ShapeID(netIdx, -pinIdx - 1));
            }
        }
    }
}

void DetailedNCR::configurePathSearch(IndexType netIdx, IndexType sourceTermIdx, IndexType targetTermIdx)
{
    _netIdx = netIdx;
    const auto &net = _db.drDB().net(netIdx);
    bool sym = net.isSym();
    bool selfSym = net.isSelfSym();
    Assert(!sym || !selfSym);
    SymNet symNet;
    if (sym)
    {
#ifdef DEBUG_DR_
        DBG("%s: solving symmetry \n", __FUNCTION__);
#endif 
        symNet = _db.symNet(net.symNetIdx());
        _symNet = symNet;
    }
    if (selfSym)
    {
        symNet = _db.selfSymNet(net.selfSymNetIdx()).dummySymNet();
        _symNet = symNet;
        sym = true;
    }
    const auto &sourceTerm = _db.terminal(sourceTermIdx);
    const auto &targetTerm = _db.terminal(targetTermIdx);

    IndexType netIdx2 = symNet.secondNetIdx();
    IndexType sourceTermIdxSym = INDEX_TYPE_MAX;
    IndexType targetTermIdxSym = INDEX_TYPE_MAX;
    if (sym)
    { 
        sourceTermIdxSym = symNet.pairedTerminal(sourceTermIdx);
        targetTermIdxSym = symNet.pairedTerminal(targetTermIdx);
    }
    
    // Configure the shapeID
    ShapeID sourceShapeID = ShapeID();
    sourceShapeID.configTerminal(sourceTermIdx, sourceTerm.primarySearchPointShape(), true);
    ShapeID targetShapeID = ShapeID();
    targetShapeID.configTerminal(targetTermIdx, targetTerm.primarySearchPointShape(), true);

    // Set the goal pin region
    _shapeComp.computeFixedShape(targetShapeID);
    _pathSearch->setGoalRegion(_shapeComp.result().front().first);

    ShapeID sourceShapeSym = ShapeID();
    ShapeID targetShapeSym = ShapeID();
    if (sym)
    {
        sourceShapeSym.configTerminal(sourceTermIdxSym, _db.terminal(sourceTermIdxSym).primarySearchPointShape(), true);
        targetShapeSym.configTerminal(targetTermIdxSym, _db.terminal(targetTermIdxSym).primarySearchPointShape(), true);
    }

#ifdef DEBUG_DR_
    DBG("%s: terminal %d to %d \n", __FUNCTION__, sourceTermIdx, targetTermIdx);
#endif


    // Set the search settings
    _pathSearch->resetAlgo();
    _pathSearch->setNetIdx(netIdx);
    if (sym)
    {
        _pathSearch->setSymNet(symNet);
    }
    else
    {
        _pathSearch->setNoSym();
    }

    // Set primary source node
    if (sym)
    {
        if (symNet.symType() == SymType::HORIZONTAL)
        {
            if (sourceTerm.primarySearchPoint().loc().x() < symNet.symAxis())
            {
                _pathSearch->addSourceShapeAlgo(NodeKey(sourceTerm.primarySearchPoint().loc(), sourceTerm.primarySearchPoint().layer()), sourceShapeID);
                _pathSearch->addSymNodeSourceShape(NodeKey(sourceTerm.primarySearchPoint().loc(), sourceTerm.primarySearchPoint().layer()), sourceShapeSym);
            }
        }
        else if (symNet.symType() == SymType::VERTICAL)
        {
            if (sourceTerm.primarySearchPoint().loc().y() < symNet.symAxis())
            {
                _pathSearch->addSourceShapeAlgo(NodeKey(sourceTerm.primarySearchPoint().loc(), sourceTerm.primarySearchPoint().layer()), sourceShapeID);
                _pathSearch->addSymNodeSourceShape(NodeKey(sourceTerm.primarySearchPoint().loc(), sourceTerm.primarySearchPoint().layer()), sourceShapeSym);
            }
        }
    }
    else
    {
        // No sym, directly add the primary search point
        _pathSearch->addSourceShapeAlgo(NodeKey(sourceTerm.primarySearchPoint().loc(), sourceTerm.primarySearchPoint().layer()), sourceShapeID);
    }
    //_pathSearch->addSourceAlgo(NodeKey(sourceTerm.primarySearchPoint().loc(), sourceTerm.primarySearchPoint().layer()));
    _primarySource = sourceTerm.primarySearchPoint();
    // Set primary target
    _parimaryTarget = targetTerm.primarySearchPoint();
    _pathSearch->setGoalAlgo(NodeKey(targetTerm.primarySearchPoint().loc(), targetTerm.primarySearchPoint().layer()));

    // Set the source and target shape ID, so that they won't be checked in the shape map
    _shapeMapPtr->setSourceShapeID(sourceShapeID);
    _pathSearch->setSourceShapeID(sourceShapeID);
    _shapeMapPtr->setTargetShapeID(targetShapeID);
    _pathSearch->setTargetShapeID(targetShapeID);
    if (sym)
    {
        _pathSearch->setSymTargetShape(targetShapeSym);
    }


    // Add other search points
    for (auto searchPt : sourceTerm.otherSearchPoints())
    {
        if (sym)
        {
            if (symNet.symType() == SymType::HORIZONTAL)
            {
                if (searchPt.loc().x() < symNet.symAxis())
                {
                    _pathSearch->addSourceShapeAlgo(NodeKey(searchPt.loc(), searchPt.layer()), sourceShapeID);
                    _pathSearch->addSymNodeSourceShape(NodeKey(searchPt.loc(), searchPt.layer()), sourceShapeSym);
                }
            }
            else if (symNet.symType() == SymType::VERTICAL)
            {
                if (searchPt.loc().y() < symNet.symAxis())
                {
                    _pathSearch->addSourceShapeAlgo(NodeKey(searchPt.loc(), searchPt.layer()), sourceShapeID);
                    _pathSearch->addSymNodeSourceShape(NodeKey(searchPt.loc(), searchPt.layer()), sourceShapeSym);
                }
            }
        }
        else
        {
            // No sym, directly add the search point
            _pathSearch->addSourceShapeAlgo(NodeKey(searchPt.loc(), searchPt.layer()), sourceShapeID);
        }
    }
    // Add other nodes as source
    for (IndexType nodeIdx = 1; nodeIdx < net.numNodes(); ++nodeIdx)
    {
        const auto &node =net.node(nodeIdx);
        if (node.isFake())
        {
            continue;
        }
       if (sym)
       {
           auto symNodeIdx = _db.drDB().net(netIdx2).existNode(symNet.symLoc(node.location()), node.layer());
           if (symNet.symType() == SymType::HORIZONTAL)
           {
               if (node.location().x() < symNet.symAxis())
               {
                   _pathSearch->addSourceShapeAlgo(NodeKey(node.location(), node.layer()), ShapeID(_netIdx, nodeIdx));
                   _pathSearch->addSymNodeSourceShape(NodeKey(node.location(), node.layer()), ShapeID(netIdx2, symNodeIdx));
               }
           }
           else if (symNet.symType() == SymType::VERTICAL)
           {
               if (node.location().y() < symNet.symAxis())
               {
                   _pathSearch->addSourceShapeAlgo(NodeKey(node.location(), node.layer()), ShapeID(_netIdx, nodeIdx));
                   _pathSearch->addSymNodeSourceShape(NodeKey(node.location(), node.layer()), ShapeID(netIdx2, symNodeIdx));
               }
           }
       }
       else
       {
           // No sym, directly add the search point
           _pathSearch->addSourceShapeAlgo(NodeKey(node.location(), node.layer()), ShapeID(_netIdx, nodeIdx));
       }
    }

    // Set routing types
    _pathSearch->setWireType(_wireTypes);
}
bool DetailedNCR::routeSubNet(IndexType netIdx, IndexType sourceTermIdx, IndexType targetTermIdx)
{
    this->configurePathSearch(netIdx, sourceTermIdx, targetTermIdx);
    // Set the effort level to be the most conservative
    _pathSearch->setEffortMode(0);
#ifdef DEBUG_DR_
    DBG("%s: solve %s %d to %s %d \n", __FUNCTION__, _primarySource.loc().toStr().c_str(), _primarySource.layer(), _parimaryTarget.loc().toStr().c_str(), _parimaryTarget.layer());
#endif
    // Try search the paths
    if (_pathSearch->solvePathSearchAlgo())
    {
        // If successful, assign the solution
        assignSol();
        return true;
    }
    else
    {
        // If not success, try finding the path enaabanoynmousg overlaps between nets, so that find a path avoid blockage. Use this path to reroute
        this->configurePathSearch(netIdx, sourceTermIdx, targetTermIdx);
        // Reroute with enable overlap between routed shapes
        _pathSearch->setEffortMode(4);
        if (_pathSearch->solvePathSearchAlgo())
        {
            // If successful, find the conflicting nets and do rip-up
            this->ripupPathSearchResult();
            return false;
        }
        else
        {
#ifdef DEBUG_DR_
            std::string debugName = "debug/" + std::to_string(netIdx) + "." + std::to_string(sourceTermIdx) + "explore.gds";
            _pathSearch->debugInterface(debugName);
#endif
            ERR("%s: routing fails for net %d \n", __FUNCTION__, netIdx);
            ERR("%s: there is no legal path found for the net %s \n", __FUNCTION__, _db.grDB().nets().at(netIdx).name().c_str());
            throw NCR_NOPATH_EXCEPTION();
        }
    }
    return true;
}

bool DetailedNCR::routeSymNet(IndexType netIdx)
{
    IndexType symNetIdx = _db.drDB().net(netIdx).symNetIdx();
    _symNet = _db.symNet(symNetIdx);
#ifdef DEBUG_DR_
    const auto &net1 = _db.grDB().nets().at(_symNet.firstNetIdx());
    const auto &net2 = _db.grDB().nets().at(_symNet.secondNetIdx());
    DBG("%s: routing symmetry net pair: %s %d %s %d \n", __FUNCTION__, net1.name().c_str(), _symNet.firstNetIdx() , net2.name().c_str(), _symNet.secondNetIdx());
#endif
    IndexType netIdx1 = _symNet.firstNetIdx();
    IndexType netIdx2 = _symNet.secondNetIdx();
    // Only route once
    if (netIdx2 == netIdx)
    {
        return true;
    }
    // Set the root
    IndexType term1 = _db.drDB().net(netIdx1).terminal(0);
    const auto &firstTerm = _db.terminal(term1);
    DRBase::initNetSourceNode(netIdx1, firstTerm.primarySearchPoint());
    // Set the init point as the symmetry point
    auto netLoc = firstTerm.primarySearchPoint();
    auto sourcePoint = _symNet.symLoc(netLoc.loc());
#ifdef DEBUG_DR_
    DBG("Sym source %s \n", sourcePoint.toStr().c_str());
#endif
    netLoc.setLoc(sourcePoint);
    DRBase::initNetSourceNode(netIdx2, netLoc);
    try
    {
        return routeOneNormalNet(netIdx1);
    }
    catch (NCR_NOPATH_EXCEPTION e)
    {
        waiveSymPair(symNetIdx);
        return false;
    }
}

bool DetailedNCR::routeSelfSymNet(IndexType netIdx)
{
#ifdef DEBUG_DR_
    DBG("%s: route self sym %d %s\n", __FUNCTION__, netIdx, _db.grDB().nets().at(netIdx).name().c_str());
#endif
    // Try route self sym net by utilizing the existing symmetry net functions
    const auto &net = _db.drDB().net(netIdx);
    // Set the middle terminal as the starting point of the routing
    const auto &selfSym = _db.selfSymNet(net.selfSymNetIdx());
    IndexType midTermIdx = selfSym.midTermIdx();
    const auto &midTerm = _db.terminal(midTermIdx);
    DRBase::initNetSourceNode(netIdx, midTerm.primarySearchPoint());
    // If the middle primary search point is not at the axis, add a fake node
    if (midTerm.primarySearchPoint().loc() != selfSym.symLoc(midTerm.primarySearchPoint().loc()))
    {
        DRBase::addFakeNodeFromParent(netIdx, 0, NetNodeLoc(selfSym.symLoc(midTerm.primarySearchPoint().loc()), midTerm.primarySearchPoint().layer()));
    }

    // Configure the net wiring width
    this->configureWireType(netIdx);

    // Then try routes like symmetry net
    for (IndexType branchIdx = 0; branchIdx < selfSym.branches().size(); ++ branchIdx)
    {
        IndexType termIdx1 = selfSym.branches().at(branchIdx).first;
        IndexType termIdx2 = selfSym.branches().at(branchIdx).second;
        try
        {
            if (!routeSubNet(netIdx, termIdx1, termIdx2))
            {
                return false;
            }
        }
        catch (NCR_NOPATH_EXCEPTION e)
        {
            waiveSelfSymNet(net.selfSymNetIdx());
            return false;
        }
    }
    return true;
}

bool DetailedNCR::routeOneNormalNet(IndexType netIdx)
{
#ifdef DEBUG_DR_
    DBG("%s: route %d %s\n", __FUNCTION__, netIdx, _db.grDB().nets().at(netIdx).name().c_str());
#endif
    // If no multiple terminals, don't need to perform routing
    if (_db.drDB().net(netIdx).numTerminals() <= 1)
    {
        return true;
    }
    // Set the source of routing shape tree
    IndexType firstTermIdx = _db.drDB().net(netIdx).branches().at(0).first;
    const auto &firstTerm = _db.terminal(firstTermIdx);
    DRBase::initNetSourceNode(netIdx, firstTerm.primarySearchPoint());

    // Configure the net wiring width
    this->configureWireType(netIdx);

    for (IndexType branchIdx = 0; branchIdx < _db.drDB().net(netIdx).branches().size(); ++branchIdx)
    {
        IndexType sourceTermIdx = _db.drDB().net(netIdx).branches().at(branchIdx).first;
        IndexType targetTermIdx = _db.drDB().net(netIdx).branches().at(branchIdx).second;
        if (!routeSubNet(netIdx, sourceTermIdx, targetTermIdx))
        {
            return false;
        }
    }
    return true;
}

bool DetailedNCR::ncrFlow()
{
    init();
    DRBase::addBlockagesToShapeMap();
    DRBase::addPinsAndTerminalsToShapeMap();
#ifdef DEBUG_DR_
    _shapeMapPtr->debugInterface();
#endif
    IntType iteration = -1;
    while(!_netHeap.empty())
    {
        IndexType netIdx = _netHeap.top().netIdx;
        if (iteration < (int)(_netHeap.top().iteration))
        {
            iteration = _netHeap.top().iteration;
            INF("Detailed ncr: iteration %d left %d \n", iteration, _netHeap.size());
        }
        if (iteration > 100)
        {
            INF("Detailed ncr: iteration exceed the maximum, terminate \n");
        }
        _netHeap.pop();

        if (!_db.drDB().net(netIdx).routed())
        {
            if (_db.drDB().net(netIdx).numTerminals() <=1)
            {
                _db.drDB().net(netIdx).setRouted(true);
                continue;
            }
            try
            {
                if (_db.drDB().net(netIdx).isSym())
                {
                    routeSymNet(netIdx);
                }
                else if (_db.drDB().net(netIdx).isSelfSym())
                {
                    routeSelfSymNet(netIdx);
                }
                else
                {
                    routeOneNormalNet(netIdx);
                }
            }
            catch (NCR_NOPATH_EXCEPTION &e)
            {
                ERR("Detailed ncr: no legal path found! Unroutable. Terminate \n");
                return false;
            }
        }
    }
    if (_netHeap.size() > 0)
    {
        ERR("Detailed ncr: detailed routing fails. No feasible solution is found \n");
        return false;
    }
    INF("Detailed ncr: routing finished \n");
    return true;
}

/*------------------------------*/ 
/* Init path search             */
/*------------------------------*/ 
bool DetailedNCR::init()
{
    if (!initHisCostMap()) { WRN("DetailedNCR: initialize history cost map failed! \n"); return false; }
    if (!initShapeMap()) { WRN("DetailedNCR: initialize shape map failed! \n"); return false; }
    if (!initViaSelect()) {WRN("DetailedNCR: initialize via selector failed! \n"); return false;}

    /// Use gridless A star search
    if (!initPathSearchGridlessAstar()) { WRN("DetailedNCR: initialize the path search solver failed! \n"); return false; }

    // Net Ordering
    if (!initNetOrdering()) { WRN("DetailedNCR: initializing net ordering failed!\n"); return false; }
    return true;
}

bool DetailedNCR::initNetOrdering()
{
    // initialize the hpwl
    _db.calculateHpwls();
    // Initializing the iteration number record for each net
    _netIterations.resize(_db.drDB().nets().size(), 0);
    // Init the heap
    for (IndexType netIdx = 0; netIdx <  _db.drDB().nets().size(); ++netIdx)
    {
        _netHeap.push(NetNode(_db, netIdx, 0)); // could also maintain a vector of handle type?
    }
    return true;
}

bool DetailedNCR::initHisCostMap()
{
    // Make a approprite size of hisCostMap
    
    LocType minPitch = _db.drDB().pitch(0);
    const auto &dieBox = _db.drDB().dieBox();
    LocType dieXLen = dieBox.xLen();
    LocType dieYLen = dieBox.yLen();
    // Calculate the number of pitch
    LocType numX = dieXLen * 2 / minPitch;
    LocType numY = dieYLen * 2 / minPitch;
    // Reduce the size if too large
    numX = std::min(numX, 1000);
    numY = std::min(numY, 1000);
    // Calculate the corresponding node size
    LocType nodeWidth = dieXLen / numX;
    LocType nodeHeight = dieYLen / numY;

    // Set up the history cost map
    IndexType numLayers = _db.drDB().numLayers();
    _hisCostMap = HisCostMapRoughGrid(dieBox, nodeWidth, nodeHeight, numLayers);


    return true;
}

bool DetailedNCR::initShapeMap()
{
    _shapeMap.init();
    // Add to the base class
    DRBase::_shapeMapPtr = ::klib::createSharedPtr<ShapeMap>(_shapeMap);
    return true;
}

bool DetailedNCR::initViaSelect()
{
    _viaSelect.init();
    return true;
}

/*------------------------------*/ 
/* Init GridlessAstar           */
/*------------------------------*/ 
bool DetailedNCR::initPathSearchGridlessAstar()
{
    // Use the gridless astar search for detailed routing
    auto pAstar = std::make_shared<DetailedGridlessAStar>(); 
    // The querough grid. which include the global routing solution-related information
    pAstar->initRoughGrid(::klib::createSharedPtr<RoughGrid>(_db.drDB().roughGrid()));
    // initializing the search region, routing direction and other design informawtion for the path search solver
    initGridlessAstarDesign(pAstar);
    // The history cost map
    pAstar->initHisCostMap(::klib::createSharedPtr<HisCostMap>(_hisCostMap));
    // The shape map
    pAstar->initShapeMap(::klib::createSharedPtr<ShapeMap>(_shapeMap));
    // The via selector
    pAstar->initViaSelect(::klib::createSharedPtr<FixedViaSelector>(_viaSelect));
    // THe shape constructor
    pAstar->initShapeConstrutor(::klib::createSharedPtr<ShapeConstructor>(_shapeComp));

    // cast the class solver ptr to the gridless A* search
    _pathSearch = std::move(pAstar);
    return true;
}


bool DetailedNCR::initGridlessAstarDesign(std::shared_ptr<DetailedGridlessAStar> pSolver)
{
    // init the routing direction
    pSolver->setRoutingDirection(_db.drDB().routeDirArray());
    // die region
    pSolver->setSearchBox(_db.drDB().dieBox());
    // Set up the min step size for each layer: here the step size is the min spacing + min width
#if 1
    pSolver->setStepSize(_db.drDB().pitchArray());
#else
    std::vector<LocType> metalMinSp;
    for (IndexType idx = 0; idx < _instr.numRouteLayers(); ++idx)
    {
        metalMinSp.emplace_back(_instr.metalMinSpacing(idx));
    }
    pSolver->setStepSize(metalMinSp);
#endif
    // Set up the steps
    pSolver->setSteps({1,  10, 100} );
    // Set up the lowest and highest layer
    pSolver->setLayerLo(0);
    pSolver->setLayerHi(_db.drDB().numLayers() - 1);
    return true;
}

/*------------------------------*/ 
/* Routing flow                 */
/*------------------------------*/ 
void DetailedNCR::assignSol()
{
    // The result from the path searcher should already compressed to only have essential corner nodes
    assignSolNodes(_netIdx, _pathSearch->resultAlgo(), false);
    _db.drDB().net(_netIdx).setRouted(true);
    // The symmetry
    if (_pathSearch->isSymMode())
    {
        IndexType secondNetIdx = _pathSearch->currentSymNet().secondNetIdx();
        assignSolNodes(secondNetIdx, _pathSearch->symResultAlgo(), true);
        //_db.drDB().net(secondNetIdx).copySymRouteTree(_db.drDB().net(_netIdx), _pathSearch->currentSymNet());
        // Add 
        _db.drDB().net(secondNetIdx).setRouted(true);
    }
}

void DetailedNCR::assignSolNodes(IndexType netIdx, const std::vector<NodeKey> &nodeVec, bool sym)
{
#ifdef DEBUG_DR_
    //DBG("parimary source %s %d target %s %d\n", _primarySource.loc().toStr().c_str(), _primarySource.layer(), _parimaryTarget.loc().toStr().c_str(), _parimaryTarget.layer());
    //DBG("%s: path search solution : \n", __FUNCTION__);
    for (const auto & iter : nodeVec)
    {
        //DBG("%s \n", iter.toStr().c_str());
    }
#endif
    // Try avoid reusing the nodes
    // Find the first node that does not belong to the existing shapes
    IntType startIdx = 0;
    // If the last one is not belonging to the existing shapes, it may because it is on an "other" search point, then add a fake connection between the primary search node and the search point
    bool beginNode = true;
    if (_db.drDB().existNode(netIdx, nodeVec.back().loc, nodeVec.back().layer) == INDEX_TYPE_MAX)
    {
        if (_db.drDB().betweenNodes(netIdx,  nodeVec.back().loc, nodeVec.back().layer).first == INDEX_TYPE_MAX)
        {
            beginNode = false;
        }
    }
    // Find whether the orgin is a node
    IndexType nodeIdx = 0;
    if (beginNode)
    {
        for (IntType idx = nodeVec.size() - 1; idx >= 0; --idx)
        {
            const auto &node = nodeVec.at(idx);
            if (_db.drDB().existNode(netIdx, node.loc, node.layer) == INDEX_TYPE_MAX)
            {
                if (_db.drDB().betweenNodes(netIdx, node.loc, node.layer).first == INDEX_TYPE_MAX)
                {
                    // This idx is not in the shapes
                    startIdx = idx + 1;     
                    break;
                }
            }
        }
        // Find whether the orgin is a node
        nodeIdx = _db.drDB().existNode(netIdx, nodeVec.at(startIdx).loc, nodeVec.at(startIdx).layer);
        if (nodeIdx == INDEX_TYPE_MAX)
        {
            // The source is between two nodes, resolved it and make a new node
            std::pair<IndexType, IndexType> parentChildNode = _db.drDB().betweenNodes(netIdx, nodeVec.at(startIdx).loc, nodeVec.at(startIdx).layer);
            // It can only be a metal. Becasue vias inlcude all the nodes in between
            nodeIdx = DRBase::insertNodeInWire(netIdx, parentChildNode.first, parentChildNode.second, NetNodeLoc(nodeVec.at(startIdx).loc, nodeVec.at(startIdx).layer) );
        }
    }
    else // The first node is not inside the routing tree of the net
    {
        startIdx = nodeVec.size() - 1;
        // Find the primary search point node
        if (sym)
        {
            nodeIdx = _db.drDB().existNode(netIdx, _symNet.symLoc(_primarySource.loc()), _primarySource.layer());
            nodeIdx = DRBase::addFakeNodeFromParent(netIdx, nodeIdx, NetNodeLoc(nodeVec.back().loc, nodeVec.back().layer));
        }
        else
        {
            nodeIdx = _db.drDB().existNode(netIdx, _primarySource.loc(), _primarySource.layer());
            nodeIdx = DRBase::addFakeNodeFromParent(netIdx, nodeIdx, NetNodeLoc(nodeVec.back().loc, nodeVec.back().layer));
        }

    }

    // Add shapes along the path
    for (IntType idx =  startIdx - 1; idx >= 0; --idx)
    {
        if (nodeVec.at(idx).layer != nodeVec.at(idx + 1).layer)
        {
            // Add a via shape
            IndexType viaType = _pathSearch->viaType(nodeVec.at(idx));
            if (sym)
            {
                auto loc = nodeVec.at(idx).loc;
                loc = _symNet.symLoc(loc);
                NodeKey symKey =NodeKey(loc, nodeVec.at(idx).layer);
                viaType = _pathSearch->viaType(symKey);
            }
            nodeIdx = DRBase::addViaNodeFromParent(netIdx, nodeIdx, NetNodeLoc(nodeVec.at(idx).loc, nodeVec.at(idx).layer), NetNodeVIA(viaType));
#ifdef DEBUG_DR_
            //DBG("Add via %s %d \n", nodeVec.at(idx).toStr().c_str(), nodeIdx);
#endif
        }
        else
        {
            // Add a wire shape
            nodeIdx = DRBase::addWireNodeFromParent(netIdx, nodeIdx, NetNodeLoc(nodeVec.at(idx).loc, nodeVec.at(idx).layer), _wireTypes.at(nodeVec.at(idx).layer));
#ifdef DEBUG_DR_
            //DBG("Added wire %s %d \n", nodeVec.at(idx).toStr().c_str(), nodeIdx);
#endif
        }
    }
    // If the front ndoe is not the primary goal, add a fake shape between them
    if (sym)
    {
        if (nodeVec.front().layer != _parimaryTarget.layer() || nodeVec.front().loc != _symNet.symLoc(_parimaryTarget.loc()))
        {
            NetNodeLoc target = _parimaryTarget;
            target.setLoc(_symNet.symLoc(target.loc()));
            nodeIdx = DRBase::addFakeNodeFromParent(netIdx, nodeIdx, target);
        }
    }
    else
    {
        if (nodeVec.front().layer != _parimaryTarget.layer() || nodeVec.front().loc != _parimaryTarget.loc())
        {
            nodeIdx = DRBase::addFakeNodeFromParent(netIdx, nodeIdx, _parimaryTarget);
        }
    }
    // Set the last node to be physical pin
    DRBase::setNodePhysicalPin(netIdx, nodeIdx);
}

void DetailedNCR::ripupPathSearchResult()
{
    // Now is trying to route _netIdx, a result of path search is found with overlapping with other route
    // The result from the path searcher should already compressed to only have essential corner nodes
    auto &nodeVec = _pathSearch->resultAlgo();
    _shapeMapPtr->setIgnoreRoutedShapes(false);
    _shapeMapPtr->clearConflictList();

#ifdef DEBUG_DR_
    DBG("%s: path search solution : \n", __FUNCTION__);
    for (const auto & iter : nodeVec)
    {
        DBG("%s \n", iter.toStr().c_str());
    }
#endif
    // First check the symmetry one, because of the shapemap shapeID settings
    




    // Then we got the conflicting shape lists from the shape map
    const auto & shapeList = _shapeMap.conflictList();
    std::set<IndexType> conflictNets;
    // Add it self
    conflictNets.insert(_netIdx);
    if (_pathSearch->isSymMode())
    {
        conflictNets.insert(_pathSearch->currentSymNet().secondNetIdx());
    }
    //bool selfConflict = false;



    // Also the symmetry net
    if (_pathSearch->isSymMode())
    {
        auto &symNodeVec = _pathSearch->symResultAlgo();
        // Set the shapeMap shapes
        // Here need to configure using primary node
        _pathSearch->configureShapeMapIDSym(nodeVec.back());
        // Search from the end of the vector, which is essentially the starting point of the search
        for (IntType idx = symNodeVec.size() -2; idx >= 0; --idx)
        {
            if (symNodeVec.at(idx).layer != symNodeVec.at(idx + 1).layer)
            {
                // Add a via shapeXY<LocType> loc, IndexType lowerLayer, NetNodeVIA via, IndexType netIdx
                auto loc = symNodeVec.at(idx).loc;
                auto layer = std::min(symNodeVec.at(idx).layer, symNodeVec.at(idx + 1).layer);
                IndexType viaType = _pathSearch->viaType(nodeVec.at(idx));
                _shapeMap.viaCompleteCheck(loc, layer, NetNodeVIA(viaType), _netIdx);
                // Increment the history cost
                _hisCostMap.incrementHisCost(loc, symNodeVec.at(idx).layer, 1000);
            }
            else
            {
                // Add a wire shape
                auto from  = symNodeVec.at(idx).loc;
                auto to = symNodeVec.at(idx + 1).loc;
                auto layer = symNodeVec.at(idx).layer;
                _shapeMap.wireCompleteCheck(from, to, layer, _wireTypes.at(symNodeVec.at(idx).layer));
                // Increment the history cost
                _hisCostMap.incrementHisCostPath(from, to, layer, 1000);
            }
        }

        // Then we got the conflicting shape lists from the shape map
        const auto & symShapeList = _shapeMap.conflictList();
        for (auto shape : symShapeList)
        {
#ifdef DEBUG_DR_
            DBG("shape: %s \n", shape.toStr().c_str());
#endif
            Assert(shape.isRouteShape());
            conflictNets.insert(shape.net());
#if 0
            if (shape.net() == _netIdx)
            {
                selfConflict = true;
            }
#endif
            if (_db.drDB().net(shape.net()).isSym())
            {
                IndexType symNet =  _db.drDB().net(shape.net()).symNetIdx();
                IndexType otherNet = _db.symNet(symNet).secondNetIdx();
                IndexType otherotherNet = _db.symNet(symNet).firstNetIdx();
                conflictNets.insert(otherNet);
#if 0
                if (otherNet == _netIdx)
                {
                    selfConflict = true;
                }
#endif
                conflictNets.insert(otherotherNet);
#if 0
                if (otherotherNet == _netIdx)
                {
                    selfConflict = true;
                }
#endif
            }
        }

    }



    // Clear conflict list for the primary net check
    _shapeMapPtr->clearConflictList();


    // Search from the end of the vector, which is essentially the starting point of the search
    // Set the shapeMap shapes
    _pathSearch->configureShapeMapID(nodeVec.back());
    for (IntType idx = nodeVec.size() -2; idx >= 0; --idx)
    {
        if (nodeVec.at(idx).layer != nodeVec.at(idx + 1).layer)
        {
            // Add a via shapeXY<LocType> loc, IndexType lowerLayer, NetNodeVIA via, IndexType netIdx
            auto loc = nodeVec.at(idx).loc;
            auto layer = std::min(nodeVec.at(idx).layer, nodeVec.at(idx + 1).layer);
            IndexType viaType = _pathSearch->viaType(nodeVec.at(idx));
            _shapeMap.viaCompleteCheck(loc, layer, NetNodeVIA(viaType), _netIdx);
            // Increment the history cost
            _hisCostMap.incrementHisCost(loc, nodeVec.at(idx).layer, 1000);
        }
        else
        {
            // Add a wire shape
            auto from  = nodeVec.at(idx).loc;
            auto to = nodeVec.at(idx + 1).loc;
            auto layer = nodeVec.at(idx).layer;
            _shapeMap.wireCompleteCheck(from, to, layer, _wireTypes.at(nodeVec.at(idx).layer));
            // Increment the history cost
            _hisCostMap.incrementHisCostPath(from, to, layer, 1000);
        }
    }

    for (auto shape : shapeList)
    {
#ifdef DEBUG_DR_
        DBG("shape: %s \n", shape.toStr().c_str());
#endif 
        AssertMsg(shape.isRouteShape(), shape.toStr().c_str());
        conflictNets.insert(shape.net());
#if 0
        if (shape.net() == _netIdx)
        {
            selfConflict = true;
        }
#endif
        if (_db.drDB().net(shape.net()).isSym())
        {
            IndexType symNet =  _db.drDB().net(shape.net()).symNetIdx();
            IndexType otherNet = _db.symNet(symNet).secondNetIdx();
            IndexType otherotherNet = _db.symNet(symNet).firstNetIdx();
            conflictNets.insert(otherNet);
#if 0
            if (otherNet == _netIdx)
            {
                selfConflict = true;
            }
#endif
            conflictNets.insert(otherotherNet);
#if 0
            if (otherotherNet == _netIdx)
            {
                selfConflict = true;
            }
#endif
        }
    }

    
#if 0
    DRBase::ripupNet(_netIdx);
    if (_pathSearch->isSymMode())
    {
        DRBase::ripupNet(_pathSearch->currentSymNet().secondNetIdx());
    }
#endif
    for (IndexType netIdx : conflictNets)
    {
        DRBase::ripupNet(netIdx);
    }

    // Add the net to the heap again
    //if (!selfConflict)
    if (0)
    {
        _netIterations.at(_netIdx)++;
        _netHeap.push(NetNode(_db, _netIdx, _netIterations.at(_netIdx)));
        /*
        if (_pathSearch->isSymMode() && _pathSearch->currentSymNet().secondNetIdx() != _netIdx)
        {
            _netIterations.at(_pathSearch->currentSymNet().secondNetIdx())++;
            _netHeap.push(NetNode(_db, _pathSearch->currentSymNet().secondNetIdx(), _netIterations.at(_pathSearch->currentSymNet().secondNetIdx())));
        }
        */
    }
    for (IndexType conflictNet : conflictNets)
    {
        if (conflictNet != _netIdx)
        {
            _netIterations.at(conflictNet)++;
        }
        _netHeap.push(NetNode(_db, conflictNet, _netIterations.at(conflictNet)));
    }
#if 0
    // Do not rotate for self sym net
    if (selfConflict && !_db.drDB().net(_netIdx).isSelfSym())
    {
        // Rotate the vector
        auto & branches = _db.drDB().net(_netIdx).branches();
        std::rotate(branches.begin(), branches.begin() + 1, branches.end());
        if (_pathSearch->isSymMode())
        {
            auto & symBranches = _db.drDB().net(_pathSearch->currentSymNet().secondNetIdx()).branches();
            std::rotate(symBranches.begin(), symBranches.begin() + 1, symBranches.end());
        }
    }
#endif
    // Set unrouted
    _db.drDB().net(_netIdx).setRouted(false);
    if (_pathSearch->isSymMode())
    {
        _db.drDB().net(_pathSearch->currentSymNet().secondNetIdx()).setRouted(false);
    }
    for (IndexType netIdx : conflictNets)
    {
        _db.drDB().net(netIdx).setRouted(false);
    }
}

void DetailedNCR::waiveSymPair(IndexType symNetIdx)
{
    const auto & symNet = _db.symNet(symNetIdx);
    IndexType netIdx1 = symNet.firstNetIdx();
    IndexType netIdx2 = symNet.secondNetIdx();
    WRN("Detailed NCR: routing fails on symmetry pair %s %s. Try relax the constraint and re-route \n", 
            _db.grDB().nets().at(netIdx1).name().c_str(), _db.grDB().nets().at(netIdx2).name().c_str() );
    // Rip up two nets
    DRBase::ripupNet(netIdx1);
    DRBase::ripupNet(netIdx2);
    // Invalidate symmetry
    _db.drDB().net(netIdx1).invalidateSym();
    _db.drDB().net(netIdx2).invalidateSym();
    // Push back to net heap
    _db.drDB().net(netIdx1).setRouted(false);
    _db.drDB().net(netIdx2).setRouted(false);
    _netIterations.at(netIdx1)++;
    _netIterations.at(netIdx2)++;
    _netHeap.push(NetNode(_db, netIdx1, _netIterations.at(netIdx1)));
    _netHeap.push(NetNode(_db, netIdx2, _netIterations.at(netIdx2)));
}


void DetailedNCR::configureWireType(IndexType netIdx)
{
    /// TODO: NDR different width routing
    // Force them to use the same wire
    for (IndexType layer = 0; layer < _db.drDB().numLayers(); ++layer)
    {
        LocType minWidth = _instr.metalMinWidth(layer);
        _wireTypes.emplace_back(NetNodeWire(minWidth /2  , minWidth / 2));
    }

}
void DetailedNCR::waiveSelfSymNet(IndexType selfSymIdx)
{
    IndexType netIdx = _db.selfSymNet(selfSymIdx).netIdx();
    WRN("Detailed NCR: routing fails on self symmetry net %s. Try relax the constraint and re-route \n", 
            _db.grDB().nets().at(netIdx).name().c_str() );
    // Rip up the new
    DRBase::ripupNet(netIdx);
    // Invalidate the self symmetry
    _db.drDB().net(netIdx).invalidSelfSym();
    // Push back to net heap
    _db.drDB().net(netIdx).setRouted(false);
    _netIterations.at(netIdx)++;
    _netHeap.push(NetNode(_db, netIdx, _netIterations.at(netIdx)));

}

PROJECT_NAMESPACE_END
