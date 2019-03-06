#include "DetailedGridlessAStar.h"

PROJECT_NAMESPACE_BEGIN

using ASTAR = DetailedGridlessAStar;

void ASTAR::initBeforeSolve()
{
    _shapeMap->setNetIdx(_netIdx);
    _searchRange = Box<LocType> (_goal.loc, _goal.loc);
    _searchRange.join(_sources.at(0).loc);
    /// TODO: get a better strategy than hard code this
#if 1
    for (const auto & source : _sources)
    {
        _searchRange.join(source.loc);
    }
    _searchRange.unionBox(_goalRegion);
#endif
    _searchRange.enlargeBy(DETAILED_ASTAR_SEARCH_RANGE_EXTENSION);
    // Reserve the hashmap 
    //IntType reserved = abs(_goal.loc.x() - _sources.at(0).loc.x()) / _stepSize.at(0) + abs(_goal.loc.y() - _sources.at(0).loc.y()) / _stepSize.at(0);
    //_nodeMap.reserve(reserved * 10); 
}



void ASTAR::compressResult()
{
    if (_result.size() <= 2) { return; }
    std::vector<NodeKey> compressed;
    IndexType mode = 0; // 1 horizontal, 2 vertical, 3, via
    
    compressed.emplace_back(_result.front()); // The first node must be in
    mode = nodeKeyRelation(_result.at(0), _result.at(1)); // Start from the relation between the first and the second node

    /// Iterate through the uncompressed result, and save the corner node
    for (IndexType nodeIdx = 1; nodeIdx < _result.size() - 1; ++nodeIdx)
    {
        // For via, every child node must be added
        if (mode == 3)
        {
            compressed.emplace_back(_result.at(nodeIdx));
        }
        IndexType newMode = nodeKeyRelation(_result.at(nodeIdx), _result.at(nodeIdx + 1));
        if (newMode != mode)
        {
            // if via, then it has been added. Otherwise, everytime the mode changed, the previous one must be maintained
            if (mode != 3)
            {
                compressed.emplace_back(_result.at(nodeIdx));
            }
            mode = newMode;
        }
    }
    // The last one must be added
    compressed.emplace_back(_result.back());
    // Move the compressed to result, so that _result now is the compressed one
    _result.clear(); //< moveVector<>() insert to the end
    ::klib::moveVector<NodeKey>(compressed, _result);
}

IndexType ASTAR::nodeKeyRelation(const NodeKey &node1, const NodeKey &node2) const
{
    //DBG("%s %s \n", node1.toStr().c_str(), node2.toStr().c_str());
    if (node1.layer != node2.layer)
    {
        Assert(node1.loc == node2.loc);
        return 3;
    }
    if (node1.loc.x() != node2.loc.x())
    {
        Assert(node1.loc.y() == node2.loc.y());
        return 1;
    }
    if(node1.loc.y() != node2.loc.y())
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

void ASTAR::setEffortMode(IntType effort)
{
    IntType decode;
    decode = effort;
    if (decode / 4 != 0)
    {
        decode = decode % 4;
        _allowOverLap = true;
    }
    else
    {
        _allowOverLap = false;
    }
    if (decode / 2 != 0)
    {
        decode = decode % 2;
        _allowWrongWay = true;
    }
    else
    {
        _allowWrongWay = false;
    }
    if (decode != 0)
    {
        _allowDetouringNeighborLayer = true;
    }
    else
    {
        _allowDetouringNeighborLayer = false;
    }
    configEffortModes();
}

void ASTAR::configEffortModes()
{
    _shapeMap->setIgnoreRoutedShapes(_allowOverLap);
}

/*------------------------------*/ 
/* Overrid Function             */
/*------------------------------*/ 

bool ASTAR::isValid(const NodeKey &nodeKey) const 
{
    /// Check layer
    if (nodeKey.layer < _layerLo) { return false; }
    if (nodeKey.layer > _layerHi) { return false; }
    /// Check die boundary
    if (nodeKey.loc.x() < _searchBox.xLo()) { return false; }
    if (nodeKey.loc.y() < _searchBox.yLo()) { return false; }
    if (nodeKey.loc.x() > _searchBox.xHi()) { return false; }
    if (nodeKey.loc.y() > _searchBox.yHi()) { return false; }
    /// Check search range
    if (nodeKey.loc.x() < _searchRange.xLo()) { return false; }
    if (nodeKey.loc.y() < _searchRange.yLo()) { return false; }
    if (nodeKey.loc.x() > _searchRange.xHi()) { return false; }
    if (nodeKey.loc.y() > _searchRange.yHi()) { return false; }
    /// Check symmetry
    if (this->isSymMode())
    {
        // The first net is on the "smaller" side, just make sure it won't exceed the axis
        if (_symNet.symType() == SymType::HORIZONTAL)
        {
            /// TODO: remove hard-coded
            if (nodeKey.loc.x() >= _symNet.symAxis() - 140)
            {
                return false;
            }
        }
        else
        {
            if (nodeKey.loc.y() >= _symNet.symAxis() - 140)
            {
                return false;
            }
        }
        auto sympt = this->symmetryPoint(nodeKey);
    /// Check die boundary
    if (sympt.loc.x() < _searchBox.xLo()) { return false; }
    if (sympt.loc.y() < _searchBox.yLo()) { return false; }
    if (sympt.loc.x() > _searchBox.xHi()) { return false; }
    if (sympt.loc.y() > _searchBox.yHi()) { return false; }
    }
    return true;
}
CostType ASTAR::distance(const NodeKey &from, const NodeKey &to)
{
    CostType distance = static_cast<CostType>(::klib::manhattanDistance<LocType>(from.loc, to.loc)) + static_cast<CostType>(::klib::idxDif(from.layer, to.layer)) * DR_VIA_DISTANCE;
    CostType hisCost = this->hisCost(from, to);
    CostType wrongWay = 0;
    CostType globalRouting = 0; // Penaly for violating global routing solution
    CostType pinAccessPenalty = 0;
    CostType secondHisCost = 0; 
    CostType shortWirePenalty = 0;
    if (this->isSymMode())
    {
        secondHisCost = this->hisCost(this->symmetryPoint(from), this->symmetryPoint(to));
    }
#if 0
    if (from.layer == to.layer)
    {
        if (from.layer < 2)
        {
            pinAccessPenalty = VIA_DISTANCE * ::klib::manhattanDistance<LocType>(from.loc, to.loc) * 1000;
        }
    }
#endif

#if 1
    if (this->needVia(from, to))
    {
        if (node(from).affix.sourceNode().loc != from.loc)
        {
            if (::klib::manhattanDistance<LocType>(node(from).affix.sourceNode().loc, from.loc) < 100)
            {
                shortWirePenalty = 100;
            }
        }
    }
#endif
    if (from.layer != to.layer)
    {
        // On two layers
        Assert(from.loc.x() == to.loc.x());
        Assert(from.loc.y() == to.loc.y());
    }
    else if (from.loc.x() != to.loc.x())
    {
        // Horizontal
        Assert(from.loc.y() == to.loc.y());
        Assert(from.layer == to.layer);
        if (_routingDir.at(from.layer) != RouteDirType::BOTH && _routingDir.at(to.layer) != RouteDirType::HORIZONTAL)
        {
            wrongWay += WRONG_WAY_PENALTY * abs(from.loc.x() - to.loc.y());
        }
    }
    else
    {
        // Vertical
        if (_routingDir.at(from.layer) != RouteDirType::BOTH && _routingDir.at(to.layer) != RouteDirType::VERTICAL)
        {
            wrongWay += WRONG_WAY_PENALTY * abs(from.loc.y() - to.loc.y());
        }
    }
    // Global routing penalty 
    if (!_roughGrid->netInGlobalRoutingGuide(_netIdx, to.loc, to.layer))
    {
        globalRouting += GLOBAL_ROUTING_SOLUTION_VIOLATION_PENALTY * (::klib::manhattanDistance<LocType>(from.loc, to.loc) + ::klib::absDif(from.layer, to.layer) * DR_VIA_DISTANCE);
    }

    return distance + hisCost + secondHisCost + wrongWay + globalRouting + pinAccessPenalty + shortWirePenalty;
}

void ASTAR::neighbors(const NodeKey &nodeKey, std::vector<NodeKey> &list) 
{
    // up and down
    if (isValid(NodeKey(nodeKey.loc, nodeKey.layer - 1)))
    {
        list.emplace_back(NodeKey(nodeKey.loc, nodeKey.layer - 1));
    }
    if (isValid(NodeKey(nodeKey.loc, nodeKey.layer + 1)))
    {
        list.emplace_back(NodeKey(nodeKey.loc, nodeKey.layer + 1));
    }
    LocType stepSize = _stepSize.at(nodeKey.layer); // The min step size of the search
    // Reduce the stepsize if this is from primary source
    if (::klib::manhattanDistance<LocType>(nodeKey.loc, _sources.at(0).loc) < stepSize)
    {
        stepSize = std::max(stepSize / 2, 10);
    }
    else if (::klib::manhattanDistance<LocType>(nodeKey.loc, _goal.loc) < stepSize)
    {
        stepSize = std::max(stepSize / 2, 10);
    }
#if 0
    if (abs(nodeKey.loc.x() - _goal.loc.x()) < 10 || abs(nodeKey.loc.y() - _goal.loc.y()) < 10)
    {
        if (::klib::manhattanDistance<LocType>(nodeKey.loc, _goal.loc) < stepSize / 4)
        {
            if (::klib::idxDif(nodeKey.layer, _goal.layer ) <= 1)
            {
                stepSize = 5;
            }
        }
    }
#endif
    if (_goalRegion.contain(nodeKey.loc))
    {
        if (::klib::idxDif(nodeKey.layer, _goal.layer ) <= 1)
        {
            /// TODO: get a better strategy for this 
            // 5 is from the minimum grid for the technology 
            stepSize = std::min(10, stepSize);
            // It has been closed to pin, don't need to take long steps
            _steps.resize(1);
        }
    }
    if (_routingDir.at(nodeKey.layer) == RouteDirType::HORIZONTAL || _allowWrongWay || _routingDir.at(nodeKey.layer) == RouteDirType::BOTH)
    {
        // +x direction
        for (auto step : _steps)
        {
            if (isValid(NodeKey(XY<LocType>(nodeKey.loc.x() + step * stepSize, nodeKey.loc.y()), nodeKey.layer)))
            {
                //if (nodeKey.loc.x() + static_cast<LocType>(step * stepSize) <= _goal.loc.x())
                if (1)
                {
                    list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x() + step * stepSize, nodeKey.loc.y()), nodeKey.layer));
                }
            }
            else { break; }
        }
        // -x direction
        for (auto step : _steps)
        {
            if (isValid(NodeKey(XY<LocType>(nodeKey.loc.x() - step * stepSize, nodeKey.loc.y()), nodeKey.layer)))
            {
                if (1)
                //if (nodeKey.loc.x() - static_cast<LocType>(step * stepSize) >=  _goal.loc.x())
                {
                    list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x() - step * stepSize, nodeKey.loc.y()), nodeKey.layer));
                }
            }
            else { break; }
        }
    }
    if (_routingDir.at(nodeKey.layer) == RouteDirType::VERTICAL || _allowWrongWay || _routingDir.at(nodeKey.layer) == RouteDirType::BOTH)
    {
        // +y direction
        for (auto step : _steps)
        {
            if (isValid(NodeKey(XY<LocType>(nodeKey.loc.x(), nodeKey.loc.y() + step * stepSize), nodeKey.layer)))
            {
                if (1)
                //if (nodeKey.loc.y() + static_cast<LocType>(step * stepSize) <= _goal.loc.y())
                {
                    list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x(), nodeKey.loc.y() + step * stepSize), nodeKey.layer));
                }
            }
            else { break; }
        }
        // -y direction
        for (auto step : _steps)
        {
            if (isValid(NodeKey(XY<LocType>(nodeKey.loc.x(), nodeKey.loc.y() - step * stepSize), nodeKey.layer)))
            {
                if (1)
                //if (nodeKey.loc.y() - static_cast<LocType>(step * stepSize) >= _goal.loc.y())
                {
                    list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x(), nodeKey.loc.y() - step * stepSize), nodeKey.layer));
                }
            }
            else { break; }
        }
    }
    /// Also add the candidates smaller than min step size to compensate the un-aligned pins
    // clear the situation that the difference between the goal and source are not multiplier of the step size
    LocType minStep = stepSize * static_cast<LocType>(_steps.at(0));
    if (nodeKey.layer == _goal.layer)
    {
        // Won't add candidate here if they are on the different layers
        if (nodeKey.loc.x() == _goal.loc.x())
        {
            if (abs(nodeKey.loc.y() - _goal.loc.y()) < minStep)
            {
                if (_routingDir.at(nodeKey.layer) == RouteDirType::VERTICAL || _allowWrongWay || _routingDir.at(nodeKey.layer) == RouteDirType::BOTH)
                {
                    list.emplace_back(_goal);
                }
            }
        }
        else if (nodeKey.loc.y() == _goal.loc.y())
        {
            if (abs(nodeKey.loc.x() - _goal.loc.x()) < minStep)
            {
                if (_routingDir.at(nodeKey.layer) == RouteDirType::HORIZONTAL || _allowWrongWay || _routingDir.at(nodeKey.layer) == RouteDirType::BOTH)
                {
                    list.emplace_back(_goal);
                }
            }
        }
    }
    else
    {
        // If either is small
        if (abs(nodeKey.loc.y() - _goal.loc.y()) < minStep && nodeKey.loc.y() != _goal.loc.y())
        {
            // make sure it is not zigzag, espeically too short between two via
            if (_routingDir.at(nodeKey.layer) == RouteDirType::VERTICAL || _allowWrongWay || _routingDir.at(nodeKey.layer) == RouteDirType::BOTH)
            {
                if (isValid(NodeKey(XY<LocType>(nodeKey.loc.x(), _goal.loc.y()), nodeKey.layer)))
                {
                    list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x(), _goal.loc.y()), nodeKey.layer));
                }
            }
        }
        if (abs(nodeKey.loc.x() - _goal.loc.x() ) < minStep && nodeKey.loc.x() != _goal.loc.x())
        {
            if (_routingDir.at(nodeKey.layer) == RouteDirType::HORIZONTAL || _allowWrongWay || _routingDir.at(nodeKey.layer) == RouteDirType::BOTH)
            {
                if (isValid(NodeKey(XY<LocType>(_goal.loc.x(), nodeKey.loc.y()), nodeKey.layer)))
                {
                    list.emplace_back(NodeKey(XY<LocType>(_goal.loc.x(), nodeKey.loc.y()), nodeKey.layer));
                }
            }
        }
    }
}

bool ASTAR::isOccupied(const NodeKey &from, const NodeKey &to)
{
    // Set the source shape to shape path
    if (node(from).affix.sourceShape() == ShapeID() )
    {
        Assert(0);
        _shapeMap->setSourceShapeID(_sourceShape);
    }
    else
    {
        _shapeMap->setSourceShapeID(node(from).affix.sourceShape());
    }
    // Set the deafult primary target shape
    _shapeMap->setTargetShapeID(_targetShape);
    _shapeMap->setNetIdx(_netIdx);
    
    ShapeID symShape = node(from).affix.sourceShape();
    if (this->isSymMode())
    {
        symShape = _sourceShapeMap.at(symShape);
    }
    _shapeMap->clearConflictList();
    if (from.layer == to.layer)
    {
        bool firstOccupy =  !wireLegal(from, to, false);
        // Also test on the symmetry
        if (this->isSymMode())
        //if (0)
        {
            _shapeMap->setSourceShapeID(symShape);
            _shapeMap->setTargetShapeID(_symTargetShape);
            _shapeMap->setNetIdx(_symNet.secondNetIdx());
            bool secondOccupy =  !wireLegal(from, to, true);
            return firstOccupy || secondOccupy;
        }
        return firstOccupy;
    }
    else 
    {
        // On the different layer
        Assert(from.loc == to.loc);
        bool firstOccupy = !viaLegal(from, to, false);
        // Also test on the symmetry
        if (this->isSymMode())
        //if (0)
        {
            _shapeMap->setSourceShapeID(symShape);
            _shapeMap->setTargetShapeID(_symTargetShape);
            _shapeMap->setNetIdx(_symNet.secondNetIdx());
            bool secondOccupy =  !viaLegal(from, to, true);
            return firstOccupy || secondOccupy;
        }
        return firstOccupy;
    }
}

bool ASTAR::isClosed(const NodeKey &nodeKey) const
{
    if (SKIP_GLOBAL_ROUTING)
    {
        return false;
    }
    if (_roughGrid->netInGlobalRoutingGuide(_netIdx, nodeKey.loc, nodeKey.layer))
    {
        return false;
    }
    if (_allowDetouringNeighborLayer)
    {
        if (_roughGrid->netInGlobalRoutingPenalty(_netIdx, nodeKey.loc, nodeKey.layer))
        {
            return false;
        }
    }
#if 0
    else
    {
        if (_roughGrid->netInGlobalRoutingGuide(_netIdx, nodeKey.loc, nodeKey.layer -1))
        {
            std::cout<<" Lower ok net "<<_netIdx<<" "<<nodeKey.loc.toStr()<<" "<< nodeKey.layer;
        }
        if (_roughGrid->netInGlobalRoutingGuide(_netIdx, nodeKey.loc, nodeKey.layer +1))
        {
            std::cout<<" Upper ok net "<<_netIdx<<" "<<nodeKey.loc.toStr()<<" "<< nodeKey.layer;
        }
        std::cout<<std::endl;
    }
#endif
    return true;
}

bool ASTAR::isGoal(const NodeKey &nodeKey)
{
    /// Check whether reach the goal
    if (nodeKey == _goal)
    {
        return true;
    }
    if (nodeKey.layer != _goal.layer)
    {
        return false;
    }
#if 0
    if (abs(nodeKey.loc.x() - _goal.loc.x()) > 20000 || abs(nodeKey.loc.y() - _goal.loc.y()) > 2000)
    {
        return false;
    }
#endif
    /// Check if reaching the pin
    // The primary net
    bool primaryNetReach = false;
    auto & node = this->node(nodeKey);
    if (node.affix.hasVia())
    {
        auto rect = _shapeComp->computeViaBottomMetalShape(nodeKey.loc, nodeKey.layer, NetNodeVIA(node.affix.viaType()));
        //auto cut = _shapeComp->computeViaCutBBox(nodeKey.loc, nodeKey.layer, NetNodeVIA(node.affix.viaType()));
        //if (_shapeComp->rectInTerminal(_targetShape, rect, nodeKey.layer)  && _shapeComp->rectInTerminal(_targetShape, cut, nodeKey.layer))
        if (_shapeComp->rectInTerminal(_targetShape, rect, nodeKey.layer))
        {
            primaryNetReach =  true;
        }
    }
    else
    {
        if (_shapeComp->reachTerminal(_targetShape, node.affix.sourceNode().loc, node.affix.sourceNode().layer, nodeKey.loc, nodeKey.layer))
        {
            primaryNetReach =  true;
        }
    }
    if (!this->isSymMode() && primaryNetReach)
    {
        _goal = nodeKey;
        return true;
    }
    // Check if the secondary also reach
    if (this->isSymMode() && primaryNetReach)
    {
        bool secondaryReach = false;
        if (node.affix.hasVia())
        {
            auto rect = _shapeComp->computeViaBottomMetalShape(_symNet.symLoc(nodeKey.loc), nodeKey.layer, NetNodeVIA(node.affix.viaType()));
            //auto cut = _shapeComp->computeViaCutBBox(_symNet.symLoc(nodeKey.loc), nodeKey.layer, NetNodeVIA(node.affix.viaType()));
            //if (_shapeComp->rectInTerminal(_symTargetShape, rect, nodeKey.layer)  && _shapeComp->rectInTerminal(_symTargetShape, cut, nodeKey.layer))
            if (_shapeComp->rectInTerminal(_symTargetShape, rect, nodeKey.layer))
            {
                secondaryReach =  true;
            }
        }
        else
        {
            if (_shapeComp->reachTerminal(_symTargetShape, _symNet.symLoc(node.affix.sourceNode().loc), node.affix.sourceNode().layer, _symNet.symLoc(nodeKey.loc), nodeKey.layer))
            {
                secondaryReach =  true;
            }
        }
        if (secondaryReach)
        {
            _goal = nodeKey;
            return true;
        }
    }
    return false;
}


void ASTAR::updateNodeParent(const NodeKey &from, const NodeKey &to, CostType tentativeGCost) 
{
#ifdef DEBUG_ASTAR
    DBG("%s: from %s to %s \n", __FUNCTION__, from.toStr().c_str(), to.toStr().c_str());
#endif
    // Note: gCost of each cost is initialized as INT_TYPE_MAX, so that it should be larger than any actual cost.
    // If this assumption fails, than undefined behavior.
    // Update: 2018/08/22 Disabled here
    //AssertMsg(!klib::idxAddOverflow(node(from).gCost, distance(from, to)), "%s gCost overflow! %u %u \n", __PRETTY_FUNCTION__, node(from).gCost, distance(from, to));
    if (tentativeGCost >= node(to).gCost)
    {
        return;
    }

    // Record this path and push to the openList
    IntType hCost = heuristicCostEstimate(to);// + viaPenalty;
    // Update: 2018/08/22 Disabled
    //AssertMsg(!klib::idxAddOverflow(tentativeGCost, heuristicCostEstimate(to)), "%s hCost overflow! %u %u \n", __PRETTY_FUNCTION__, tentativeGCost, hCost);
    Assert(node(from).parentLoc != to.loc || node(from).parentLayer != to.layer);
    node(to).parentLoc = from.loc;
    node(to).parentLayer = from.layer;
    node(to).gCost = tentativeGCost;
    node(to).hCost = hCost;
    // Update the original source shape
    node(to).affix.setSourceShape(node(from).affix.sourceShape());

    // Update affix information
    if (this->isNodeNewShape(from, to))
    {
        node(to).affix.setSourceNode(from);
    }
    else
    {
        node(to).affix.setSourceNode(node(from).affix.sourceNode());
    }
    if (this->needVia(from, to))
    {
        node(to).affix.addVia(this->chooseVIA(std::min(to.layer, from.layer)));// Here pick the minimum area
    }


    // Push/update openList
    if (node(to).isInList)
    {
        decreaseOpen(to);
    }
    else // Not in open list
    {
        pushNodeToOpen(to);
    }
}

bool ASTAR::edgeAdm(const NodeKey &from, const NodeKey &to)
{
    if (from.layer == to.layer)
    {
        if (_routingDir.at(from.layer) == RouteDirType::FORBID)
        {
            return false;
        }
    }

    // Check whether this edge is in back and forth
    if (node(from).parentKey() == to || node(from).affix.sourceNode() == to)
    {
        return false;
    }
    return true;
}


/*------------------------------*/ 
/* Geometric shape checking     */
/*------------------------------*/ 

bool DetailedGridlessAStar::wireLegal(const NodeKey &from, const NodeKey &to, bool sym) 
{
    // Check the beginning of the shape by is from and to two different shapes
    if (this->isNodeNewShape(from, to))
    {
        if (sym)
        {
            if (_shapeMap->wireCompleteCheck(this->symmetryPoint(from).loc, this->symmetryPoint(to).loc,  
                        from.layer, _wireTypes.at(from.layer)) ==  CheckResult::ILLEGAL)
            {
                return false;
            }
        }
        else
        {
            if (_shapeMap->wireCompleteCheck(from.loc, to.loc,  from.layer, _wireTypes.at(from.layer)) ==  CheckResult::ILLEGAL)
            {
                return false;
            }
        }
    }
    else
    {
        if (sym)
        {
            if (_shapeMap->wireCompleteCheck(this->symmetryPoint(node(from).affix.sourceNode()).loc, this->symmetryPoint(to).loc,  
                        from.layer, _wireTypes.at(from.layer)) ==  CheckResult::ILLEGAL)
            {
                return false;
            }
        }
        else
        {
            if (_shapeMap->wireCompleteCheck(node(from).affix.sourceNode().loc, to.loc,  from.layer, _wireTypes.at(from.layer)) ==  CheckResult::ILLEGAL)
            {
                return false;
            }
        }
    }
    return true;
}

bool DetailedGridlessAStar::viaLegal(const NodeKey &from, const NodeKey &to, bool sym)
{
#if 0
    if (to == NodeKey(XY<LocType>(66299, 16199), 1))
    {
        MsgPrinter::screenOn();
        DBG("%s: newShape from %s to %s \n", __FUNCTION__, from.toStr().c_str(), to.toStr().c_str());
    }
#endif
    Assert(from.loc == to.loc);
    IndexType lowerLayer = std::min(from.layer, to.layer);
    IndexType viaType = this->chooseVIA(lowerLayer);
    /// TODO: handle multiple cut
    // Via has to be a new shape
    if (sym)
    {
        if (_shapeMap->viaCompleteCheck(this->symmetryPoint(from).loc, lowerLayer, NetNodeVIA(viaType), _symNet.secondNetIdx()) == CheckResult::ILLEGAL)
        {
            return false;
        }
    }
    else
    {
        if (_shapeMap->viaCompleteCheck(from.loc, lowerLayer, NetNodeVIA(viaType), _netIdx) == CheckResult::ILLEGAL)
        {
            return false;
        }
    }
    //MsgPrinter::screenOff();
    return true;
}


/*------------------------------*/ 
/* Process node information     */
/*------------------------------*/ 
bool DetailedGridlessAStar::isNodeNewShape(const NodeKey &from, const NodeKey &to)
{
    // If the source node of the from is itself (happen for the source node of the path search), then directly return
    if (from == node(from).affix.sourceNode())
    {
        return true;
    }
    // Determine the shape relation: horizontal, vertical or via
    IndexType fromShapeRelation = nodeKeyRelation(node(from).affix.sourceNode(), from);
    IndexType toShapeRelation = nodeKeyRelation(from, to);

    // If the from is via, then to must be a new shape (two shapes also need to be different shape)
    if (fromShapeRelation == 3)
    {
        return true;
    }

    // Otherwise, decide on whether the relations are different
    if (fromShapeRelation == toShapeRelation)
    {
        return false;
    }
    return true;
}


/*------------------------------*/ 
/* Misc.                        */
/*------------------------------*/ 

IndexType DetailedGridlessAStar::chooseVIA(IndexType layer) const
{
    return _viaSelect->minArea(layer).viaType();
}

CostType DetailedGridlessAStar::hisCost(NodeKey from, NodeKey to) const
{
    CostType hisCost = 0;
    if (from.layer != to.layer)
    {
        // On two layers
        Assert(from.loc.x() == to.loc.x());
        Assert(from.loc.y() == to.loc.y());
        hisCost += _hisCostMap->hisCost(from.loc, from.layer);
        hisCost += _hisCostMap->hisCost(to.loc, to.layer);
    }
    else if (from.loc.x() != to.loc.x())
    {
        // Horizontal
        Assert(from.loc.y() == to.loc.y());
        Assert(from.layer == to.layer);
        hisCost += _hisCostMap->hisCostPath(from.loc, to.loc, to.layer);
    }
    else
    {
        // Vertical
        hisCost += _hisCostMap->hisCostPath(from.loc, to.loc, to.layer);
    }
    return hisCost;
}

/*------------------------------*/ 
/* Debug                        */
/*------------------------------*/ 
PROJECT_NAMESPACE_END
#include "writer/gdsii/WriteGds.h"
PROJECT_NAMESPACE_BEGIN
void DetailedGridlessAStar::drawGdsExploredNode(const std::string &filename) const
{
    auto _wg = std::make_shared<WriteGds>(filename);
    if (!_wg->initWriter())
    {
        return;
    }
    if (!_wg->createLib("TOP", 2000, 1e-6/2000)) // Hardcoded number
    {
        return;
    }
    if (!_wg->writeCellBgn("INTERCONNECTION"))
    {
        return;
    }
    // Write sources
    for (const auto &source : _sources)
    {
        Box<LocType> shape = Box<LocType> (-1000, -1000, 1000, 1000 );
        XY<LocType> origin = source.loc;
        if (!_wg->writeRectangle(shape.offsetBox(origin), 1000 + source.layer, 0))
        {
            std::cout<< shape.offsetBox(origin).xLo()<<"  failed"<<std::endl;
        }
    }
    // Write goal
    
    Box<LocType> shape = Box<LocType> (-1000, -1000, 1000, 1000 );
    XY<LocType> origin = _goal.loc;
    if (!_wg->writeRectangle(shape.offsetBox(origin), 2000 + _goal.layer, 0))
    {
        std::cout<< shape.offsetBox(origin).xLo()<<"  failed"<<std::endl;
    }
    // Iterate through the explored nodes
    for (const auto &nodePair : _nodeMap)
    {
        XY<LocType> loc = nodePair.first.loc;
        IndexType layer = nodePair.first.layer;
        const auto &wireType = _wireTypes.at(layer);
        shape = Box<LocType>( -wireType.halfWidth(), -wireType.halfWidth(), wireType.halfWidth(), wireType.halfWidth());
        if (!_wg->writeRectangle(shape.offsetBox(loc), layer, 0))
        {
            std::cout<< shape.offsetBox(origin).xLo()<<"  failed"<<std::endl;
        }
    }
    
    if (!_wg->writeCellEnd())
    {
        return;
    }
    if (!_wg->endLib())
    {
        return;
    }
    DBG("%s: debug rtree shapes saved in %s \n", __FUNCTION__, filename.c_str());
    return;
}
PROJECT_NAMESPACE_END
