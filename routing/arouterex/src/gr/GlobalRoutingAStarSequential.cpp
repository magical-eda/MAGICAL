#include "GlobalRoutingAStarSequential.h"

PROJECT_NAMESPACE_BEGIN

bool GlobalRoutingAStarSequential::runKernel()
{
#ifdef DEBUG_GR_
    DBG("Global routing a* search sequential runKernel()\n");
#endif
    // init
    if (!init()) { return false; }
    // Routing
    if (!ncrRouting()) { return false; }

    return true;
}

bool GlobalRoutingAStarSequential::init()
{
    // turn to 3D grid mode
    _db.grDB().gridMap().setMode2D(false);
    // init the node sizes
    std::vector<XY<IndexType>> xySizes;
    for (IndexType layer = 0; layer < _db.grDB().gridMap().numLayers(); ++layer)
    {
        xySizes.emplace_back(XY<IndexType>(_db.grDB().gridMap().numX(), _db.grDB().gridMap().numY()));
    }
    initNode3D(xySizes);
    // Init the iterations of nets
    _netIterations.resize(_db.grDB().nets().size(), 0);
    // Init the heap
    for (IndexType netIdx = 0; netIdx < _db.grDB().nets().size(); ++netIdx)
    {
        _netHeap.push(NetNode(_db, netIdx, 0)); // could also maintain a vector of handle type
    }
    // Init the g cells history costs
    initGCells();
    return true;
}

void GlobalRoutingAStarSequential::initGCells()
{
    // init the history cost of gcells based on pin count
    for (auto &gcell : _db.grDB().gridMap().gridCells())
    {
        gcell.setHisCost(gcell.pins().size() * GRID_CELL_HIS_COST_PIN);
    }
}

bool GlobalRoutingAStarSequential::ncrRouting()
{
    IndexType iteration = 0;
    while (!_netHeap.empty())
    {
        IndexType netIdx = _netHeap.top().netIdx;
        if (iteration < _netHeap.top().iteration)
        {
            iteration = _netHeap.top().iteration;
            INF("%s: Iteration: %d left %d \n", __FUNCTION__, iteration, _netHeap.size());
        }
#ifdef DEBUG_GR_
        INF("%s: routing net %d Iteration %d left %d \n", __FUNCTION__, netIdx, _netHeap.top().iteration, _netHeap.size());
#endif
        _netHeap.pop();
        // If the net has been routed, skip the routing 
        if (_db.grDB().nets().at(netIdx).isRouted())
        {
            continue;
        }
        // Set net to be routed. If need rip-up, this setting will be set false there
        _db.grDB().nets().at(netIdx).setIsRouted(true);
        // Calculate the search range
        const auto & idxBBox = _db.grDB().nets().at(netIdx).gridBBox();
        _searchBBox.set(idxBBox.xLo(), idxBBox.yLo(), idxBBox.xHi(), idxBBox.yHi());
        _searchBBox.enlargeBy(GLOBAL_ROUTING_SEARCH_RANGE);
        if (_db.grDB().nets().at(netIdx).isSym())
        {
#ifdef DEBUG_GR_
            DBG("%s: routing symmetric net \n", __FUNCTION__);
#endif
            routeOneSymNet(_db.grDB().nets().at(netIdx).symNetIdx());
        }
        else
        {
#ifdef DEBUG_GR_
            DBG("%s: routing normal net \n", __FUNCTION__);
#endif
            // normal net
            routeOneNormalNet(netIdx);
        }
    }
    return true;
}

bool GlobalRoutingAStarSequential::routeOneNormalNet(IndexType netIdx)
{
    _symRoutingMode = false;
    for (IndexType subNetIdx : _db.grDB().nets().at(netIdx).subNets())
    {
        bool terminate = routeSubNet(subNetIdx);
        // ripup if faianoynmousg
        if (!terminate)
        {
            // ripup the net, update the cost and push the nets to the heap
            this->ripUpSubNet(subNetIdx);
            return false;
        }
    }
    return true;
}

bool GlobalRoutingAStarSequential::routeOneSymNet(IndexType symNetIdx)
{
    // Set sym net routing mode
    _symNetPtr = std::make_unique<SymNet>(_db.symNet(symNetIdx));
    _symRoutingMode = true;

    auto &net1 = _db.grDB().nets().at(_symNetPtr->firstNetIdx());
    auto &net2 = _db.grDB().nets().at(_symNetPtr->secondNetIdx());

    // Try route net1
    bool terminate = true;
    for (IndexType subNetIdx : net1.subNets())
    {
        terminate = routeSubNet(subNetIdx);
        // ripup if faianoynmousg
        if (!terminate)
        {
            // ripup the net, update the cost and push the nets to the heap
            this->ripUpSubNet(subNetIdx);
            break;
        }
    }
    // match the routing for the other net
    if (terminate)
    {
        net2.setIsRouted(true);
        matchSymRoutingPath();
    }
    //Reset sym net routing mode
    _symNetPtr = nullptr;
    _symRoutingMode = false;
    return true;
}

bool GlobalRoutingAStarSequential::routeSubNet(IndexType subNetIdx)
{
#ifdef DEBUG_GR_
    //DBG("%s: route subnet %d \n", __FUNCTION__, subNetIdx);
#endif
    // set the sub net index 
    _subNetIdx = subNetIdx;
    // Turn off force route
    _forceRoute = false;
    auto &subNet = _db.grDB().subNets().at(subNetIdx);
    IndexType netIdx = subNet.netIdx();
    auto &fromPin = _db.grDB().pin(subNet.sourcePinIdx());
    auto &toPin = _db.grDB().pin(subNet.targetPinIdx());

    XYZ<IndexType> fromXYZ = XYZ<IndexType>(fromPin.gridIdx(), fromPin.layer());
    XYZ<IndexType> toXYZ = XYZ<IndexType>(toPin.gridIdx(), toPin.layer());


    // If two pins are inside one grid cell, skip the search
    if (fromXYZ == toXYZ) { return true; }

    // push sources and target to the A* solver
    addSource(fromXYZ);
    setGoal(toXYZ);

    // Add existing segment to the sources
    for (auto &pair : _db.grDB().edgeMap().at(netIdx))
    {
        addSource(pair.first.begin());
        addSource(pair.first.end());
    }

    bool terminate = solve();
    // If didn't get the result, force route it
    if (!terminate)
    {
        reset();
        _forceRoute = true;
        // push sources and target to the A* solver
        addSource(fromXYZ);
        setGoal(toXYZ);

        // Add existing segment to the sources
        for (auto &pair : _db.grDB().edgeMap().at(netIdx))
        {
            addSource(pair.first.begin());
            addSource(pair.first.end());
        }
        bool forceSuccess = solve();
        Assert(forceSuccess);
    }
    // Assign the edges to the net
    assignSol();
    subNet.setRouted(terminate);
    return terminate;
}

void GlobalRoutingAStarSequential::assignSol()
{
    // Get the results as a set of grid cells
    std::vector<XYZ<IndexType>> result = solution();
    auto &subNet = _db.grDB().subNets().at(_subNetIdx);
    auto &sourcePin = _db.grDB().pin(subNet.sourcePinIdx());
    auto sourceNode = XYZ<IndexType>(sourcePin.gridIdx(), sourcePin.layer());
    // Retrieve the path to the subNet source pin
    if (result.back() != sourceNode)
    {
        auto vec = _db.grDB().retrieveNetPath(subNet.netIdx(), result.back(), sourceNode);
        std::reverse(vec.begin(), vec.end());
        result.reserve(result.size() + vec.size());
        result.insert(result.end(), vec.begin(), vec.end());
    }
    // Add edges to the subNets
    for (IndexType idx = 0; idx < result.size() - 1; ++idx)
    {
        Assert(result.at(idx).ptPos(result.at(idx + 1)) != PtPosType::NOT_NEIGHBOR);
        _db.grDB().addEdge2SubNet(_subNetIdx, result.at(idx), result.at(idx+1));
    }
}

void GlobalRoutingAStarSequential::matchSymRoutingPath()
{
    auto &net1 = _db.grDB().nets().at(_symNetPtr->firstNetIdx());
    auto &net2 = _db.grDB().nets().at(_symNetPtr->secondNetIdx());
    
    // iterate through all the edges of all the subnet of the net1, and add the symmetried correspondant to net2
    // Their subNet should be one-to-one corresponding
    for (IndexType idx = 0; idx < net1.subNets().size(); ++idx)
    {
        auto &subNet1 = _db.grDB().subNets().at(net1.subNets().at(idx));
        auto &subNetIdx2 = net2.subNets().at(idx);
        for (auto &edge : subNet1.edges())
        {
            // The symmetried nodes for the edge in the subnet 
            XYZ<IndexType> node1 = XYZ<IndexType>(_symNetPtr->symGC(edge.begin().xy()), edge.begin().z());
            XYZ<IndexType> node2 = XYZ<IndexType>(_symNetPtr->symGC(edge.end().xy()), edge.end().z());
            // add the edge to the net2
            _db.grDB().addEdge2SubNet(subNetIdx2, node1, node2);
        }
    }
}

void GlobalRoutingAStarSequential::ripUpSubNet(IndexType subNetIdx)
{
    auto &subNet = _db.grDB().subNets().at(subNetIdx);
    std::set<IndexType> ripupNetList;

    for (auto &edge : subNet.edges())
    {
        auto &gridEdge = _db.grDB().gridEdge(edge);
        if (gridEdge.overflow())
        {
            // add the nets to the ripuplist
            for (IndexType takesubNetIdx : gridEdge.subNets())
            {
                //DBG("%s: add subnet %d net %d to ripup list, edge cap %d \n", __FUNCTION__, takesubNetIdx,  _db.grDB().subNets().at(takesubNetIdx).netIdx(), gridEdge.totalCap());
                ripupNetList.insert(_db.grDB().subNets().at(takesubNetIdx).netIdx());
            }
            auto &node1 = _db.grDB().gridCell(edge.begin());
            auto &node2 = _db.grDB().gridCell(edge.end());
            node1.setHisCost(node1.hisCost() + HIS_COST_INCREMENT);
            node2.setHisCost(node2.hisCost() + HIS_COST_INCREMENT);
        }
    }

    //ripup nets
    for (IndexType netIdx : ripupNetList)
    {
        _db.grDB().ripupNet(netIdx);
        _netIterations.at(netIdx)++;
        _netHeap.push(NetNode(_db, netIdx, _netIterations.at(netIdx)));
        // Also set net to be unrouted
        _db.grDB().nets().at(netIdx).setIsRouted(false);
    }
}


/*------------------------------*/ 
/* Overrided from AStarBase     */
/*------------------------------*/ 

IntType GlobalRoutingAStarSequential::distance(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const 
{
    IndexType netIdx = _db.grDB().subNets().at(_subNetIdx).netIdx();
    if (_db.grDB().netHasEdge(netIdx, from, to)) { return 1; }
    IntType distance = 1;
    if (from.z() != to.z()) { distance = _db.grDB().gridMap().gridWidth() * VIA_DISTANCE; } 
    else if (from.x() != to.x()) { distance = _db.grDB().gridMap().gridWidth(); }
    else { distance = _db.grDB().gridMap().gridHeight(); }
    IntType hisCost = _db.grDB().gridCell(from).hisCost() + _db.grDB().gridCell(to).hisCost();
    return distance + hisCost;
}

std::vector<XYZ<IndexType>> GlobalRoutingAStarSequential::neighbors(const XYZ<IndexType> &xyz) const
{
    std::vector<XYZ<IndexType>> tempResult;
    // Up
    if (isValid(xyz.top()))
    {
        tempResult.emplace_back(xyz.top());
    }
    // Down
    if (isValid(xyz.bottom()))
    {
        tempResult.emplace_back(xyz.bottom());
    }
    // East
    if (isValid(xyz.east()))
    {
        tempResult.emplace_back(xyz.east());
    }
    // South
    if (isValid(xyz.south()))
    {
        tempResult.emplace_back(xyz.south());
    }
    // West
    if (isValid(xyz.west()))
    {
        tempResult.emplace_back(xyz.west());
    }
    // North
    if (isValid(xyz.north()))
    {
        tempResult.emplace_back(xyz.north());
    }
    return tempResult;
}

IntType GlobalRoutingAStarSequential::heuristicCostEstimate(const XYZ<IndexType> &xyz) const 
{
    // Differences in unit of grid cells
    IntType xDif = ::klib::absDif(xyz.x(), _goal.x());
    IntType yDif = ::klib::absDif(xyz.y(), _goal.y());
    IntType zDif = ::klib::absDif(xyz.z(), _goal.z());
    // Calculated the actual distance
    return xDif * _db.grDB().gridMap().gridWidth()
        +  yDif * _db.grDB().gridMap().gridHeight()
        +  zDif * _db.grDB().gridMap().gridWidth() * VIA_DISTANCE; 
}

bool GlobalRoutingAStarSequential::edgeAdm(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const
{
    // If force route (possibly in reroute)
    IndexType netIdx = _db.grDB().subNets().at(_subNetIdx).netIdx();
    IntType useCap = _db.grDB().nets().at(netIdx).capUse();
    // If the net has already used this edge
    if(_db.grDB().netHasEdge(netIdx, from, to)) { return true; }
    // If the symmetry edge does not have enough cap
    if (_symRoutingMode)
    {
        XYZ<IndexType> symFrom = XYZ<IndexType>(_symNetPtr->symGC(from.xy()), from.z());
        XYZ<IndexType> symTo = XYZ<IndexType>(_symNetPtr->symGC(to.xy()), to.z());
        // If the returned point is out of bound (checked by invalid), then return this edge is not possible
        if (!isValid(symFrom) || !isValid(symTo))
        {
            return false;
        }
        IntType net2UseCap = _db.grDB().nets().at(_symNetPtr->secondNetIdx()).capUse();
        if (_forceRoute)
        {
            // only need the totalcap > 0
            if (_db.grDB().gridEdge(symFrom, symTo).totalCap() <= 0)
            {
                return false;
            }
        }
        else 
        {
            if (!withinSearchRange(to))
            {
                return false;
            }
            if (!_db.grDB().gridEdge(symFrom, symTo).enough(net2UseCap))
            {
                return false;
            }
        }
    }
    // if the edge has enough cap
    if (_forceRoute)
    {
        if (_db.grDB().gridEdge(from, to).totalCap() > 0)
        {
            return true;
        }
    }
    else 
    {
        if (!withinSearchRange(to))
        {
            return false;
        }
        if (_db.grDB().gridEdge(from, to).enough(useCap))
        {
            return true;
        }
    }
    return false;
}
PROJECT_NAMESPACE_END

