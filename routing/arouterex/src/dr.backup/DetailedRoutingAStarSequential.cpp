#include "DetailedRoutingAStarSequential.h"

PROJECT_NAMESPACE_BEGIN

using ASTAR = DetailedRoutingAStarSequential;

bool ASTAR::runKernel()
{
    /// init
    if (!init()) { return false; }
    /// Routing
    if (!ncrRouting()) { return false; }
    return true;
}

/*------------------------------*/ 
/* Initialization               */
/*------------------------------*/ 
bool ASTAR::init()
{
    /// initialize the searching grid
    if (!initGrid()) { return false; }

    /// initialize the VIA-related things
    if (!initVia()) { return false; }

    /// initialize the net heap
    if (!initNetOrdering()) { return false; }

    return true;
}

bool ASTAR::initGrid()
{
    /// init the node sizes
    std::vector<XY<IndexType>> xySizes;
    for (IndexType layer = 0;  layer < _db.drDB().grid3D().layerSize(); ++layer)
    {
        IndexType numX = _db.drDB().grid3D().at(layer).xSize();
        IndexType numY = _db.drDB().grid3D().at(layer).ySize();
        xySizes.emplace_back(XY<IndexType>(numX, numY));
    }
#ifdef DEBUG_DR_
    for (IndexType idx = 0; idx < xySizes.size(); ++idx)
    {
        DBG(" %s: Constructing grid layer  %d * %d \n", __FUNCTION__, xySizes.at(idx).x(), xySizes.at(idx).y());
    }
#endif
    initNode3D(xySizes);
    return true;
}

bool ASTAR::initVia()
{
    if (!_fixedViaSelector.init()) { return false; }
    return true;
}

bool ASTAR::initNetOrdering()
{
    /// Init the iterations of nets
    _netIterations.resize(_db.grDB().nets().size(), 0);
    /// init ignore GR list
    _ignoreGRList.resize(_db.drDB().netArray().size(), false);
    /// Init the heap
    for (IndexType netIdx = 0; netIdx < _db.drDB().netArray().size(); ++netIdx)
    {
        _netHeap.push(NetNode(_db, netIdx, 0)); /// could also maintain a vector of handle type
    }
    return true;
}

/*------------------------------*/ 
/* Routing flow                 */
/*------------------------------*/ 

bool ASTAR::ncrRouting()
{
    IndexType iteration = 0;
    while (!_netHeap.empty())
    {
        IndexType netIdx = _netHeap.top().netIdx;
        /// Change mode
        if (_netIterations.at(netIdx) > 20)
        {
            _useGlobalLayer = false;
        }
        if (_netIterations.at(netIdx ) > 50)
        {
            _useGlobalGuide = false;
        }
        if (_netIterations.at(netIdx ) > 100)
        {
            return false;
        }
#ifdef DEBUG_DR_
        INF("%s: routing net %d Iteration %d left %d \n", __FUNCTION__, netIdx, _netHeap.top().iteration, _netHeap.size());
#endif
        if (iteration < _netHeap.top().iteration)
        {
            iteration = _netHeap.top().iteration;
            INF("%s: Iteration: %d \n", __FUNCTION__, iteration);
        }
        _netHeap.pop();
        /// normal net
        routeOneNormalNet(netIdx);
    }
    return true;
}


bool ASTAR::routeOneNormalNet(IndexType netIdx)
{
    _netIdx = netIdx;
    auto &net = _db.drDB().netArray().at(netIdx);
    if (net.routed()) { return true; }
    if (_ignoreGRList.at(netIdx)) { _useGlobalGuide = false; }
    for (IndexType subNetIdx = 0; subNetIdx < net.branches().size(); ++subNetIdx)
    {
        bool terminate = routeSubNet(subNetIdx);
        /// ripup if faianoynmousg
        if (!terminate)
        {
            _useGlobalGuide = true;
            return false;
        }
        net.setRouted(true);
    }
    _useGlobalGuide = true;
    return true;
}

bool ASTAR::routeSubNet(IndexType subNetIdx)
{
#ifdef DEBUG_DR_
    DBG("%s: route net %d subnet %d \n", __FUNCTION__, _netIdx, subNetIdx);
#endif
    auto &net = _db.drDB().netArray().at(_netIdx);
    /// Turn off force route
    _forceRoute = false;
    auto &fromPin = net.pinArray().at(net.branches().at(subNetIdx).first);
    auto &toPin = net.pinArray().at(net.branches().at(subNetIdx).second);

    XYZ<IndexType> fromXYZ = _db.drDB().locToDrXYZ(fromPin.loc(), fromPin.layer());
    XYZ<IndexType> toXYZ = _db.drDB().locToDrXYZ(toPin.loc(), toPin.layer());

#ifdef DEBUG_DR_
    _primarySource = fromXYZ;
    DBG("Search from %s to %s \n", fromXYZ.toStr().c_str(), toXYZ.toStr().c_str());
#endif

    /// push sources and target to the A* solver
    addSource(fromXYZ);
    setGoal(toXYZ);

    /// Add existing segment to the sources
    for (auto &node : net.drNodes())
    {
        addSource(node);
    }

    bool terminate = solve();
    /// If didn't get the result, force route it
    if (!terminate)
    {
        reset();
        _forceRoute = true;
        /// push sources and target to the A* solver
        addSource(fromXYZ);
        setGoal(toXYZ);

        /// Add existing segment to the sources
        for (auto &node : net.drNodes())
        {
            addSource(node);
        }
#ifdef DEBUG_DR_
        DBG("%s: Perform force routing %s to %s \n" ,__FUNCTION__, _primarySource.toStr().c_str(), _goal.toStr().c_str());
#endif
        bool forceSuccess = solve();
        //Assert(forceSuccess);
        if (!forceSuccess)
        {
#ifdef DEBUG_DR_
            DBG("%s: add net %d to ignore GR list \n", __FUNCTION__, _netIdx);
#endif
            _ignoreGRList.at(_netIdx) = true;
            this->reset();
            _netIterations.at(_netIdx)++;
            _netHeap.push(NetNode(_db, _netIdx, _netIterations.at(_netIdx)));
            return false;
        }
    }
    if (terminate)
    {
        /// Assign the edges to the net
        assignSol();
    }
    else
    {
        ripup();
    }
    return terminate;
}

void ASTAR::ripup()
{
    /// The path need to ripup has been inside the net
    auto &net = _db.drDB().netArray().at(_netIdx);
    std::set<IndexType> ripupNetList; ///< the list of net to be cleared
    /// Get the results as a set of grid cells
    const auto& result = solution();
    for (const auto & node : result)
    {
        auto &gridCell = _db.drDB().grid3D().gridCell(node);
        /// Increment the hisCost, if has been occupied, increment twice
        gridCell.setHisCost(gridCell.hisCost() + HIS_COST_INCREMENT *200);
        if (gridCell.isOccupied())
        {
            gridCell.setHisCost(gridCell.hisCost() + HIS_COST_INCREMENT * 200);
            /// Add the net into the ripup list
            ripupNetList.insert(gridCell.netIdxOccupying()); 
        }
        /// TODO: rip up the neighboring nodes?
    }
    /// clear the remaining nodes in the net
    ripupNetList.insert(_netIdx);
    for (const auto &node : net.drNodes())
    {
        auto &gridCell = _db.drDB().grid3D().gridCell(node);
        /// Increment the hisCost, if has been occupied, increment twice
        gridCell.setHisCost(gridCell.hisCost() + HIS_COST_INCREMENT * 200);
    }
    /// Could be redunant
    for (IndexType netIdx : ripupNetList)
    {
        clearNet(netIdx);
    }
    //net.drNodes().clear();
    ///push to heap
    for (IndexType netIdx : ripupNetList)
    {
        _netIterations.at(netIdx)++;
        _netHeap.push(NetNode(_db, netIdx, _netIterations.at(netIdx)));
        _db.drDB().netArray().at(netIdx).setRouted(false);
    }
}

void ASTAR::clearNet(IndexType netIdx)
{
    auto &net = _db.drDB().netArray().at(netIdx);
    for (const auto &node : net.drNodes())
    {
        auto &gridCell = _db.drDB().grid3D().gridCell(node);
        gridCell.clearOcp();
    }
    net.drNodes().clear();
    /// clear via
    _db.drDB().clearNetVia(netIdx);
}

void ASTAR::assignSol()
{
    auto &net = _db.drDB().netArray().at(_netIdx);
    auto result  = this->solution();
    for (const auto &node : result)
    {
#ifdef DEBUG_DR_
        //DBG("%s: add node %s \n", __FUNCTION__, node.toStr().c_str());
#endif
        net.drNodes().insert(node);
        _db.drDB().grid3D().gridCell(node).occupy(_netIdx);
    }
    if (result.size() == 0) { return; }
    for (IndexType nodeIdx = 0; nodeIdx < result.size() - 1; ++nodeIdx)
    {
        const auto &thisNode = result.at(nodeIdx);
        const auto &parentNode = result.at(nodeIdx + 1);
        if (thisNode.z() != parentNode.z())
        {
            XYZ<IndexType> lowerNode;
            XYZ<IndexType> higherNode;
            if (thisNode.z() < parentNode.z())
            {
                lowerNode = thisNode;
                higherNode = parentNode;
            }
            else
            {
                lowerNode = parentNode;
                higherNode = thisNode;
            }
            /// Insert in the net records
            //net.drVias().insert(FixedViaNode(lowerNode, higherNode, node(thisNode).affix.fixedViaID));
            /// Only store the viaType in "to" node
            _db.drDB().addFixedVia(_netIdx, lowerNode, higherNode, node(thisNode).affix.fixedViaID); 
            /// TODO: add some information about the via into the gridcell
        }
    }
}

/*------------------------------*/ 
/* Routing support              */
/*------------------------------*/ 
IndexType ASTAR::pickFixedVia(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const
{
    XYZ<IndexType> xyzLower;
    XYZ<IndexType> xyzHigher;
    if (from.z() < to.z())
    {
        xyzLower = from;
        xyzHigher = to;
    }
    else
    {
        xyzLower = to;
        xyzHigher = from;
    }
    IndexType viaType = _fixedViaSelector.minArea(xyzLower.z()).viaIdx;
    XY<LocType> origin = _db.drDB().grid3D().layer2LayerIntersectPt(xyzLower, xyzHigher);
    Box<LocType> cutBoundingBox = _db.macroDB().fixedViaVec2D().at(xyzLower.z()).at(viaType).cutBBox();
    cutBoundingBox.offsetBy(origin);
    
    /// Find the search region need to inquiry
    LocType searchRegionDistance = cutLayerInquirySpacing(xyzLower.z());
    cutBoundingBox.enlargeBy(searchRegionDistance);

    /// Search the region in the RTree
    const std::vector<ViaPlaced*> searchResults =  _db.drDB().cutRtreeArray().at(xyzLower.z()).searchBox(cutBoundingBox);

    /// For now, check the min via spacing and if the return search is not zero, then must be problem
    if (searchResults.size() > 0)
    {
#ifdef DEBUG_ASTAR
        DBG("%s: VIA CUT spacing violation! Detour \n", __FUNCTION__);
#endif
        return -1;
    }
    return viaType;
}

LocType ASTAR::cutLayerInquirySpacing(IndexType cutLayerIdx) const
{
    return _db.techDB().cutLayers().at(cutLayerIdx).spacing;
}

/*------------------------------*/ 
/* Override Function            */
/*------------------------------*/ 
IntType ASTAR::distance(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const 
{
    auto locFrom = _db.drDB().drNode2Loc(from).center();
    auto locTo = _db.drDB().drNode2Loc(to).center();
    IntType manDist2D = klib::manhattanDistance<LocType>(locFrom, locTo);
    IntType layerDif = klib::idxDif(from.z(), to.z());
    IntType hisCostFrom = _db.drDB().grid3D().gridCell(from).hisCost();
    IntType hisCostTo = _db.drDB().grid3D().gridCell(to).hisCost();
    Assert(manDist2D + layerDif + hisCostFrom + hisCostTo >= 0);
    return manDist2D + layerDif + hisCostFrom + hisCostTo;
}

std::vector<XYZ<IndexType>> ASTAR::neighbors(const XYZ<IndexType> &xyz) const
{
    std::vector<XYZ<IndexType>> tempResult;
    auto routeDir = _db.drDB().grid3D().at(xyz.z()).routeDir();
    /// East
    if (isValid(xyz.east()) && ( routeDir == RouteDirType::HORIZONTAL || routeDir == RouteDirType::BOTH))
    {
        tempResult.emplace_back(xyz.east());
    }
    if (isValid(xyz.south()) && ( routeDir == RouteDirType::VERTICAL || routeDir == RouteDirType::BOTH))
    {
        tempResult.emplace_back(xyz.south());
    }
    /// West
    if (isValid(xyz.west()) && ( routeDir == RouteDirType::HORIZONTAL || routeDir == RouteDirType::BOTH))
    {
        tempResult.emplace_back(xyz.west());
    }
    /// North
    if (isValid(xyz.north()) && ( routeDir == RouteDirType::VERTICAL || routeDir == RouteDirType::BOTH))
    {
        tempResult.emplace_back(xyz.north());
    }
    /// Top
    if (xyz.z() < _db.drDB().grid3D().layerSize() - 1)
    {
        /// this implementation is stupid, be careful next time
        auto candidate = _db.drDB().grid3D().layer2Layer(xyz.z(), xyz.z() + 1, xyz.xy());
        for (auto &xy : candidate)
        {
            tempResult.emplace_back(XYZ<IndexType>(xy, xyz.z() + 1));
        }
    }
    /// down
    if (xyz.z() > 0)
    {
        auto candidate = _db.drDB().grid3D().layer2Layer(xyz.z(), xyz.z() - 1, xyz.xy());
        for (auto &xy : candidate)
        {
            tempResult.emplace_back(XYZ<IndexType>(xy, xyz.z() - 1));
        }
    }
    return tempResult;
}

IntType ASTAR::heuristicCostEstimate(const XYZ<IndexType> &xyz) const
{

    auto locFrom = _db.drDB().drNode2Loc(xyz).center();
    auto locTo = _db.drDB().drNode2Loc(_goal).center();
    IntType manDist2D = klib::manhattanDistance<LocType>(locFrom, locTo);
    IntType layerDif = klib::idxDif(xyz.z(), xyz.z());
    Assert(manDist2D + layerDif >= 0);

    return manDist2D + layerDif;
}

bool ASTAR::isOccupied(const XYZ<IndexType> &xyz) const
{
    if (_forceRoute)
    {
        return false;
    }
#if 0
    if (_db.drDB().grid3D().gridCell(xyz).isOccupied(_netIdx)  == true)
    {
        DBG("%s: %s is occupied by net %d\n", __FUNCTION__, xyz.toStr().c_str(), _db.drDB().grid3D().gridCell(xyz).netIdxOccupying());
    }
#endif
    return _db.drDB().grid3D().gridCell(xyz).isOccupied(_netIdx);
}

bool ASTAR::isClosed(const XYZ<IndexType> &xyz) const
{
    if (!_useGlobalGuide)
    {
        return false;
    }
    /// Check whether xyz is in the global routing guide
    auto &net = _db.drDB().netArray().at(_netIdx);
    auto &grNodes = net.grid2GridGR();
    auto &grNodes2D = net.gr2DNodes();
    auto box = _db.drDB().drNode2Loc(xyz);
    auto grNodeLL = XYZ<IndexType>(_db.drDB().grIdx2Loc().loc2XYIdx(box.ll()), xyz.z());
    if (!_useGlobalLayer)
    {
        if (grNodes2D.find(grNodeLL.xy()) != grNodes2D.end() )
        {
            //DBG("Success\n");
            return false;
        }
        else 
        {
            if (box.ur().x() < _db.drDB().grid3D().at(xyz.z()).dieBox().xHi())
            {
                if (box.ur().y() < _db.drDB().grid3D().at(xyz.z()).dieBox().yHi())
                {
                    auto grNodeHH =  XYZ<IndexType>(_db.drDB().grIdx2Loc().loc2XYIdx(box.ur()), xyz.z());
                    if (grNodes2D.find(grNodeHH.xy()) != grNodes2D.end())
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }
    if (grNodes.find(grNodeLL) != grNodes.end() )
    {
        //DBG("Success\n");
        return false;
    }
    else 
    {
        if (box.ur().x() < _db.drDB().grid3D().at(xyz.z()).dieBox().xHi())
        {
            if (box.ur().y() < _db.drDB().grid3D().at(xyz.z()).dieBox().yHi())
            {
                auto grNodeHH =  XYZ<IndexType>(_db.drDB().grIdx2Loc().loc2XYIdx(box.ur()), xyz.z());
                if (grNodes.find(grNodeHH) != grNodes.end())
                {
                    return false;
                }
            }
        }
        return true;
    }
}

void ASTAR::updateNodeParent(const XYZ<IndexType> &from, const XYZ<IndexType> &to, IntType tentativeGCost)
{
    /// if from and to are on different layers and need via
    IntType viaPenalty = 0;
    IntType fixedViaIdx = -1;
    if (needVia(from, to))
    {
        fixedViaIdx = pickFixedVia(from, to); /// -1 if no via possible
        if (fixedViaIdx == -1)
        {
            return;
        }
    }

    /// Note: gCost of each cost is initialized as INT_TYPE_MAX, so that it should be larger than any actual cost.
    /// If this assumption fails, than undefined behavior.
    /// Update: 2018/08/22 Disabled here
    //AssertMsg(!klib::idxAddOverflow(node(from).gCost, distance(from, to)), "%s gCost overflow! %u %u \n", __PRETTY_FUNCTION__, node(from).gCost, distance(from, to));
    if (tentativeGCost >= node(to).gCost)
    {
        return;
    }

    /// Check if the via is too short from the previous 
    if (needVia(from, to))
    {
        ///TODO: fill this with real number
        if (node(from).affix.fromVia != 0)
        {
            if (node(from).affix.fromVia < 3)
            {
                //DBG("%s: from %s to %s, fromVia = %d \n", __FUNCTION__, _searchGraph.node2Str(from).c_str(), _searchGraph.node2Str(to).c_str(), node(from).affix.fromVia);
                return;
            }
        }
    }

    /// Record this path and push to the openList
    IntType hCost = heuristicCostEstimate(to) + viaPenalty;
    /// Update: 2018/08/22 Disabled
    //AssertMsg(!klib::idxAddOverflow(tentativeGCost, heuristicCostEstimate(to)), "%s hCost overflow! %u %u \n", __PRETTY_FUNCTION__, tentativeGCost, hCost);
    Assert(node(from).parentXYZ != to);
    node(to).parentXYZ = from;
    node(to).gCost = tentativeGCost;
    node(to).hCost = hCost;

    /// Add fixedViaType to affix
    if (needVia(from, to))
    {
        if (from.z() < to.z())
        {
            node(to).affix.fixedViaID = fixedViaIdx;
            node(to).affix.insertFixedVia = true;
            node(to).affix.viaUp = true;
            node(to).affix.fromVia = 0;
        }
        else
        {
            node(to).affix.fixedViaID = fixedViaIdx;
            node(to).affix.insertFixedVia = true;
            node(to).affix.viaUp = false;
            node(to).affix.fromVia = 0;
        }
    }
    else
    {
        /// the distance from via  ++1
        node(to).affix.fromVia = node(from).affix.fromVia + 1;
    }

    /// Push/update openList
    if (node(to).inList)
    {
        decreaseOpen(to);
    }
    else /// Not in open list
    {
        pushNodeToOpen(to);
    }
}

PROJECT_NAMESPACE_END
