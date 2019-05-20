#include "AStarDRRouter.h"

PROJECT_NAMESPACE_BEGIN

IntType AStarDRRouter::distance(const NodeIdentifier &nodeFrom, const NodeIdentifier &nodeTo) const
{
    IntType manDist2D = klib::manhattanDistance(_searchGraph.node2XYIdx(nodeFrom), _searchGraph.node2XYIdx(nodeTo));
    IntType layerDif = klib::idxDif(nodeFrom.layer(), nodeTo.layer());

    return manDist2D * _manDistWeight + layerDif * _layerDifWeight;
}

IntType AStarDRRouter::heuristicCostEstimate(const NodeIdentifier &nodeIdenty) const 
{

    IntType manDist2D = klib::manhattanDistance(_searchGraph.node2XYIdx(nodeIdenty), _searchGraph.node2XYIdx(_goal));
    IntType layerDif = klib::idxDif(nodeIdenty.layer(), _goal.layer());

    return manDist2D * _manDistWeight + layerDif * _layerDifWeight;
}


bool AStarDRRouter::isOccupied(const NodeIdentifier &nodeIdenty) const
{
    auto drNode = _searchGraph.node2XYIdx(nodeIdenty);

#ifdef DEBUG_DR_
    if (_db.drDB().grid3D().at(nodeIdenty.layer()).gridCell(drNode).isOccupied(_netIdx)  == true)
    {
        DBG("%s: %s is occupied by net %d\n", __FUNCTION__, XYZ<IndexType>(drNode, nodeIdenty.layer()).toStr().c_str(), _db.drDB().grid3D().at(nodeIdenty.layer()).gridCell(drNode).netIdxOccupying());
    }
#endif
    return _db.drDB().grid3D().at(nodeIdenty.layer()).gridCell(drNode).isOccupied(_netIdx);
}

void AStarDRRouter::updateNodeParent(const NodeIdentifier &from, const NodeIdentifier &to, IntType tentativeGCost) 
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
    node(to).parent = from;
    node(to).gCost = tentativeGCost;
    node(to).hCost = hCost;

    /// Add fixedViaType to affix
    if (needVia(from, to))
    {
        if (from.layer() < to.layer())
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

////////////////////////
/// Routing operations
////////////////////////


void AStarDRRouter::initAugListGraphXYZ(const std::vector<XY<IndexType>> &sizeXY)
{
    _augListGraph.clear();

#ifdef USE_XYZ_NODEIDEN
    /// Build each layer one by one
    for (IndexType layerIdx = 0; layerIdx < sizeXY.size(); ++layerIdx)
    {
        DBG("%s: init %d \n", __FUNCTION__, layerIdx);
        const auto &layerSize = sizeXY.at(layerIdx);
        for (IndexType xIdx = 0; xIdx < layerSize.x(); ++xIdx)
        {
            for (IndexType yIdx = 0; yIdx < layerSize.y(); ++yIdx)
            {
                NodeIdentifier nodeIden = NodeIdentifier(XYZ<IndexType>(xIdx, yIdx, layerIdx));
                _augListGraph[nodeIden] = NodeInfo();
            }
        }
    }
#else
    Assert(false);
#endif
}

void AStarDRRouter::routeSingleTwoPinNet(const NodeIdentifier &from, const NodeIdentifier &to)
{
    initAugListGraph(_searchGraph.listGraph());
    setGoal(to);
    addSource(from);
    solve();
};

bool AStarDRRouter::routeTwoPinNet(IndexType netIdx, IndexType branchIdx) 
{
    /// If set true, add existing nodes as sources for multi-source search
    if (_isMultiSource)
    {
        addExistingNodes(netIdx);
    }
    const auto &net = _db.drDB().netArray().at(netIdx);
    /// This node is for sure need to be add
    const auto &fromPin = net.pinArray().at(net.branches().at(branchIdx).first);
    const auto &toPin = net.pinArray().at(net.branches().at(branchIdx).second);
    XYZ<IndexType> fromXYZIdx = _db.drDB().locToDrXYZ(fromPin.loc(), fromPin.layer());
    XYZ<IndexType> toXYZIdx = _db.drDB().locToDrXYZ(toPin.loc(), toPin.layer());
#ifdef DEBUG_DR_
    DBG("%s: Route %s to %s \n", __FUNCTION__, fromXYZIdx.toStr().c_str(), toXYZIdx.toStr().c_str());
#endif
    auto nodeFrom = _searchGraph.xyidx2Node(fromXYZIdx.xy(), fromXYZIdx.z());
    auto nodeTo = _searchGraph.xyidx2Node(toXYZIdx.xy(), toXYZIdx.z());
    setGoal(nodeTo);
    addSource(nodeFrom);
    return solve();
}


bool AStarDRRouter::routeAndAssignNet(IndexType netIdx)
{
    _netIdx = netIdx;
    /// Init the search graph
    _searchGraph.constructNetGraph(netIdx);
    initAugListGraph(_searchGraph.listGraph());

    bool success = true; /// if the routing is sucessful
    
    /// route two-pin nets one by one
    auto &net = _db.drDB().netArray().at(netIdx);
    for (IndexType branchIdx = 0; branchIdx < net.branches().size(); ++branchIdx)
    {

        reset();
        if (!routeTwoPinNet(netIdx, branchIdx))
        {
            success = false;
        }
        /// Assign
        assignResultToNode(netIdx);
    }
    _netIdx = -1;
    return success;
    
}

void AStarDRRouter::assignResultToNode(IndexType netIdx)
{
    const std::vector<NodeIdentifier> &result = results();
    auto &net = _db.drDB().netArray().at(netIdx);

    for (const auto &node : result)
    {
        XY<IndexType> dr2DNode = _searchGraph.node2XYIdx(node);
        IndexType layer = node.layer();
        XYZ<IndexType> dr3DNode = XYZ<IndexType>(dr2DNode, layer);

        /// Add to nodes
#ifdef DEBUG_DR_
        DBG( "%s: Net %u Add node:  %s %s \n", __FUNCTION__, netIdx,  dr3DNode.toStr().c_str(), node.toStr().c_str());
#endif
        net.drNodes().insert(dr3DNode);

        /// Add to the grid
        auto &gc = _db.drDB().grid3D().at(layer).gridCell(dr2DNode);
        gc.occupy(netIdx);
    }

    /// Via
    if (result.size() == 0) { return; }
    for (IndexType nodeIdx = 0; nodeIdx < result.size() - 1; ++nodeIdx)
    {
        const auto &thisNode = result.at(nodeIdx);
        const auto &parentNode = result.at(nodeIdx + 1);
        if (thisNode.layer() != parentNode.layer())
        {
            XYZ<IndexType> lowerNode;
            XYZ<IndexType> higherNode;
            if (thisNode.layer() < parentNode.layer())
            {
                lowerNode = _searchGraph.node2XYZIdx(thisNode);
                higherNode = _searchGraph.node2XYZIdx(parentNode);
            }
            else
            {
                lowerNode = _searchGraph.node2XYZIdx(parentNode);
                higherNode = _searchGraph.node2XYZIdx(thisNode);
            }
            /// Insert in the net records
            //net.drVias().insert(FixedViaNode(lowerNode, higherNode, node(thisNode).affix.fixedViaID));
            /// Only store the viaType in "to" node
            _db.drDB().addFixedVia(netIdx, lowerNode, higherNode, node(thisNode).affix.fixedViaID); 
            /// TODO: add some information about the via into the gridcell
        }
    }
}


void AStarDRRouter::addExistingNodes(IndexType netIdx) 
{
    const auto &net = _db.drDB().netArray().at(netIdx);
    for (const auto &drNode : net.drNodes())
    {
        auto node = _searchGraph.xyidx2Node(drNode.xy(), drNode.z());
        addSource(node);
    }
}


/// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 
/// Private Misc. supporting functions
/// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 

/////////////////
/// Via
/////////////////

IntType AStarDRRouter::pickFixedVia(const NodeIdentifier &from, const NodeIdentifier &to)
{
    XYZ<IndexType> xyzLower;
    XYZ<IndexType> xyzHigher;
    if (from.layer() < to.layer())
    {
        xyzLower = _searchGraph.node2XYZIdx(from);
        xyzHigher = _searchGraph.node2XYZIdx(to);
    }
    else
    {
        xyzLower = _searchGraph.node2XYZIdx(to);
        xyzHigher = _searchGraph.node2XYZIdx(from);
    }
    IndexType viaType = _fixedViaSelector->minArea(xyzLower.z()).viaIdx;
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


LocType AStarDRRouter::cutLayerInquirySpacing(IndexType cutLayerIdx)
{
    return _db.techDB().cutLayers().at(cutLayerIdx).spacing;
}
PROJECT_NAMESPACE_END
