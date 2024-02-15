#include "VirtualPinAssigner.h"
#include <lemon/list_graph.h>
#include <lemon/network_simplex.h>
#include "util/Vector2D.h"
#include "util/linear_programming.h"
#include <chrono>

PROJECT_NAMESPACE_BEGIN

bool VirtualPinAssigner::solveFromDB()
{
#ifdef DEBUG_PINASSIGN
    DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
    IndexType iter = 0;
    reconfigureVirtualPinLocationFromDB(iter);
    while(! pinAssignmentFromDB())
    {
        ++iter;
        reconfigureVirtualPinLocationFromDB(iter);
    }
    return true;
}

void VirtualPinAssigner::reconfigureVirtualPinLocationFromDB(IndexType iter)
{
#ifdef DEBUG_PINASSIGN
    DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
    Box<LocType> boundary(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN);
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &cell = _db.cell(cellIdx);
        boundary.unionBox(cell.cellBBoxOff());
    }
    boundary.enlargeBy(iter * _boundary.yLen() / 10);
    reconfigureVirtualPinLocations(boundary);
}


bool VirtualPinAssigner::pinAssignmentFromDB()
{
#ifdef DEBUG_PINASSIGN
    DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
    auto cellLocQueryFunc = [&] (IndexType cellIdx)
    {
        const auto &cell = _db.cell(cellIdx);
        return cell.loc();
    };
    return pinAssignment(cellLocQueryFunc);
}

IntType gcd(IntType a, IntType b)
{
    for (;;)
    {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

IntType lcm(IntType a, IntType b)
{
    IntType temp = gcd(a, b);
    return temp ? (a / temp * b) : 0;
}

void VirtualPinAssigner::reconfigureVirtualPinLocations(const Box<LocType> &cellsBBox)
{
#ifdef DEBUG_PINASSIGN
    DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
    _virtualPinInterval = _db.parameters().virtualPinInterval();
    _virtualBoundaryExtension = _db.parameters().virtualBoundaryExtension();
    _boundary = cellsBBox;
    _boundary.enlargeBy(_virtualBoundaryExtension);
    LocType pinInterval = _virtualPinInterval;
    // Align to grid
    if (_db.parameters().hasGridStep())
    {
        LocType gridStep = _db.parameters().gridStep();
        LocType center = _boundary.center().x();
        LocType targetCenter = (center / gridStep) * gridStep + gridStep / 2;
        LocType targetWidth = std::max(_boundary.xHi() - targetCenter, targetCenter - _boundary.xLo());
        targetWidth = targetWidth + gridStep - (targetWidth % gridStep);
        _boundary.setXLo(targetCenter - targetWidth);
        _boundary.setYLo(_boundary.yLo() - (_boundary.yLo() % gridStep));
        _boundary.setXHi(targetCenter + targetWidth);
        _boundary.setYHi(_boundary.yHi() + gridStep -  (_boundary.yHi() % gridStep));
        pinInterval = lcm(pinInterval, gridStep);
    }
    // generate the virtual pin locations
    _virtualPins.clear();
    _topPin = VirtualPin(XY<LocType>(_boundary.center().x(), _boundary.xHi()));
    _botPin = VirtualPin(XY<LocType>(_boundary.center().x(), _boundary.xLo()));
    for (LocType x = _boundary.xLo() + pinInterval;  x < _boundary.center().x() - pinInterval / 2 ; x += pinInterval)
    {
        continue;
        LocType rightX = 2 * _boundary.center().x() - x;
        if (rightX <= x)
        {
            ERR("Pin assignment: unexpected pin location x %d center %d right x %d pinInterval %d \n", x, _boundary.center().x(), rightX, pinInterval);
            continue;
        }
        // left bottom
        _virtualPins.emplace_back(XY<LocType>(x, _boundary.yLo()));
        _virtualPins.back().setDirection(Direction2DType::SOUTH);

        // right bottom
        _virtualPins.emplace_back(XY<LocType>(rightX, _boundary.yLo()));
        _virtualPins.back().setDirection(Direction2DType::SOUTH);

        // Add to map
        _leftToRightMap[_virtualPins.size() - 2] = _virtualPins.size() - 1;

        // left top
        _virtualPins.emplace_back(XY<LocType>(x, _boundary.yHi()));
        _virtualPins.back().setDirection(Direction2DType::NORTH);

        // right top
        _virtualPins.emplace_back(XY<LocType>(rightX, _boundary.yHi()));
        _virtualPins.back().setDirection(Direction2DType::NORTH);

        // Add to map
        _leftToRightMap[_virtualPins.size() - 2] = _virtualPins.size() - 1;
    }
    for (LocType y = _boundary.yLo() + pinInterval;  y < _boundary.yHi() - pinInterval; y += pinInterval)
    {
        _virtualPins.emplace_back(XY<LocType>(_boundary.xLo(), y));
        _virtualPins.back().setDirection(Direction2DType::WEST);
        _virtualPins.emplace_back(XY<LocType>(_boundary.xHi(), y));
        _virtualPins.back().setDirection(Direction2DType::EAST);

        // Add to map
        _leftToRightMap[_virtualPins.size() - 2] = _virtualPins.size() - 1;
    }
    std::set<VirtualPin> pinSet;
    for (const auto & vp : _virtualPins)
    {
        auto findIter = pinSet.find(vp);
        Assert(findIter == pinSet.end());
        pinSet.insert(vp);
    }
}

bool VirtualPinAssigner::_networkSimplexPinAssignment(std::function<bool(IndexType)> useNetFunc,
        std::function<bool(IndexType)> usePinFunc,
        std::function<LocType(IndexType, IndexType)> netToPinCostFunc,
        std::function<void(IndexType, IndexType)> setNetToVirtualPinFunc)
{
#ifdef DEBUG_PINASSIGN
    DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
    // Prepare the nets and pins available to the problem
    std::vector<IndexType> nets;
    for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
    {
        if (useNetFunc(netIdx))
        {
            nets.emplace_back(netIdx);
        }
    }
    std::vector<IndexType> iopins; 
    for (IndexType iopinIdx = 0; iopinIdx < _virtualPins.size(); ++iopinIdx)
    {
        if (usePinFunc(iopinIdx))
        {
            iopins.emplace_back(iopinIdx);
        }
    }
    // If # of io pins < # nets, the problem is infeasible
    IndexType numNets = nets.size();
    IndexType numSites = iopins.size();
    if (numNets > numSites)
    {
        return false;
    }

    // Build the network
    // Use lemon min cost max flow with network simplex algorithm
    lemon::ListDigraph graph;
    std::vector<lemon::ListDigraph::Node> lNodes, rNodes;
    std::vector<lemon::ListDigraph::Arc> lArcs, rArcs, mArcs;
    std::vector<std::pair<IndexType, IndexType>> mArcPairs;
    // Reserve the needed space (optional)
    lNodes.reserve(numNets); rNodes.reserve(numSites);
    lArcs.reserve(numNets); rArcs.reserve(numSites); mArcs.reserve(numNets * numSites);
    mArcPairs.reserve(numNets * numSites);

    lemon::ListDigraph::ArcMap<IntType> capLo(graph); // capacity low
    lemon::ListDigraph::ArcMap<IntType> capHi(graph); // capacity high
    lemon::ListDigraph::ArcMap<IntType> costMap(graph); // Cost map

    // Adding the nodes and arcs
    // Source and target nodes
    auto s = graph.addNode();
    auto t = graph.addNode();

    // Add arcs source->left
    for (IndexType i = 0; i < numNets; ++i)
    {
        lNodes.emplace_back(graph.addNode());
        lArcs.emplace_back(graph.addArc(s, lNodes.back()));
        capLo[lArcs.back()] = 0;
        capHi[lArcs.back()] = 1;
        costMap[lArcs.back()] = 0;
    }
    // Add arcs right->target
    for (IndexType i = 0; i < numSites; ++i)
    {
        rNodes.emplace_back(graph.addNode());
        rArcs.emplace_back(graph.addArc(rNodes.back(), t));
        capLo[rArcs.back()] = 0;
        capHi[rArcs.back()] = 1;
        costMap[rArcs.back()] = 0;
    }
    // Mid arcs
    for (IndexType l = 0; l < numNets; ++l)
    {
        for (IndexType r = 0; r < numSites; ++r)
        {
            mArcs.emplace_back(graph.addArc(lNodes[l], rNodes[r]));
            mArcPairs.emplace_back(l, r);
            capLo[mArcs.back()] = 0;
            capHi[mArcs.back()] = 1;
            costMap[mArcs.back()] = netToPinCostFunc(nets[l], iopins[r]);
        }
    }

    // Solve min cost max flow sing network simplex algorithm
    lemon::NetworkSimplex<lemon::ListDigraph, IntType> networkSimplex(graph);
    networkSimplex.stSupply(s, t, numNets);
    networkSimplex.lowerMap(capLo).upperMap(capHi).costMap(costMap);
    networkSimplex.run();
    
    // Collect the solution and export into the database
    for (IndexType i = 0; i < mArcs.size(); ++i)
    {
        if (networkSimplex.flow(mArcs[i]))
        {
            const auto &pair = mArcPairs[i];
            setNetToVirtualPinFunc(nets[pair.first], iopins[pair.second]);
        }
    }
    return true;
}

bool VirtualPinAssigner::pinAssignment(std::function<XY<LocType>(IndexType)> cellLocQueryFunc)
{

#ifdef DEBUG_PINASSIGN
    DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
    assignPowerPin();
    // Calculate the current HPWLs without virtual pin
    std::vector<Box<LocType>> curNetBBox;
    curNetBBox.resize(_db.numNets());
    std::vector<char> nopinNets;
    nopinNets.resize(_db.numNets(), false);
    auto findRealPinLoc = [&](IndexType pinIdx)
    {
        XY<LocType> pinOff = _db.pin(pinIdx).midLoc();
        XY<LocType> cellLoc = cellLocQueryFunc(_db.pin(pinIdx).cellIdx());
        return cellLoc + pinOff;
    };
    for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
    {
        if (_db.net(netIdx).numPinIdx() < 1)
        {
            nopinNets.at(netIdx) = true;
            continue;
        }
        IndexType pinIdx = _db.net(netIdx).pinIdx(0);
        auto pinOff = findRealPinLoc(pinIdx);
        curNetBBox.at(netIdx) = Box<LocType>(pinOff, pinOff); // Init into an area=0 box
        // The rest pins
        for (IndexType idx = 1; idx < _db.net(netIdx).numPinIdx(); ++idx)
        {
            pinIdx = _db.net(netIdx).pinIdx(idx);
            pinOff = findRealPinLoc(pinIdx);
            curNetBBox.at(netIdx).join(pinOff);
        }
    }


    auto calculateIncreasedHpwl = [&](IndexType netIdx, IndexType pinIdx)
    {
        if (nopinNets.at(netIdx))
        {
            return 0; // don't care
        }
        const auto &virtualPinLoc = _virtualPins.at(pinIdx);
        auto difX = std::max(virtualPinLoc.x() - curNetBBox.at(netIdx).xHi(), curNetBBox.at(netIdx).xLo() - virtualPinLoc.x());
        difX = std::max(difX, 0);
        auto difY = std::max(virtualPinLoc.y() - curNetBBox.at(netIdx).yHi(), curNetBBox.at(netIdx).yLo() - virtualPinLoc.y());
        difY = std::max(difY, 0);
        return difX + difY;
    };

    auto calculateShortestManhattanDistance = [&](IndexType netIdx, IndexType ioPinIdx)
    {
        LocType dist = LOC_TYPE_MAX;
        const auto &iopinLoc = _virtualPins.at(ioPinIdx).loc();
        for (IndexType pinIdx : _db.net(netIdx).pinIdxArray())
        {
            auto pinOff = findRealPinLoc(pinIdx);
            dist = std::min(::klib::manhattanDistance(iopinLoc, pinOff), dist);
        }
        if (iopinLoc.x() < _boundary.center().x())
        {
            dist *= 1.2;
        }
        return dist;
    };

    // Calculate the added HPWL if adding the virtual pin
    auto directNetToPinCostFunc = [&](IndexType netIdx, IndexType virtualPinIdx)
    {
        return calculateShortestManhattanDistance(netIdx, virtualPinIdx);
    };

    auto useASymNet = [&](IndexType netIdx)
    {
        if (!_db.net(netIdx).isIo())
        {
            return false;
        }
        if (_db.net(netIdx).isVdd() or _db.net(netIdx).isVss())
        {
            return false;
        }
        //if (_db.net(netIdx).isSelfSym())
        //{
        //    return false;
        //}
        if (_db.net(netIdx).hasSymNet())
        {
            return false;
        }
        return true;
    };

    auto useFreePin = [&](IndexType pinIdx)
    {
        if (_virtualPins.at(pinIdx).assigned())
        {
            return false;
        }
        return true;
    };

    auto directAssignNetToPinFunc = [&](IndexType netIdx, IndexType virtualPinIdx)
    {
        AssertMsg(!_virtualPins[virtualPinIdx].assigned(), "Ideaplace: IO pin assignment: unexpected error: pin assignment conflict \n");

        _virtualPins[virtualPinIdx].assign(netIdx);
        _db.net(netIdx).setVirtualPin(_virtualPins[virtualPinIdx]);
    };

    // THe symmetric parts
    auto symPairNetToPinCostFunc = [&](IndexType netIdx, IndexType leftPinIdx)
    {
        auto rightPinIdx = _leftToRightMap.at(leftPinIdx);
        //if (_db.net(netIdx).isSelfSym())
        //{
        //    return calculateIncreasedHpwl(netIdx, leftPinIdx) + calculateIncreasedHpwl(netIdx, rightPinIdx);
        //}
        if (_db.net(netIdx).hasSymNet())
        {
            auto otherNetIdx = _db.net(netIdx).symNetIdx();
            auto netCost0 = calculateShortestManhattanDistance(netIdx, leftPinIdx) + calculateShortestManhattanDistance(otherNetIdx, rightPinIdx);
            return netCost0;
            auto netCost1 = calculateShortestManhattanDistance(otherNetIdx, leftPinIdx) + calculateShortestManhattanDistance(netIdx, rightPinIdx);
            return std::min(netCost0, netCost1);
        }
        Assert(false);
        return 0;
    };

    auto useSymNet = [&](IndexType netIdx)
    {
        if (!_db.net(netIdx).isIo())
        {
            return false;
        }
        if (_db.net(netIdx).isVdd() or _db.net(netIdx).isVss())
        {
            return false;
        }
        //if (_db.net(netIdx).isSelfSym())
        //{
        //    return true;
        //}
        if (_db.net(netIdx).hasSymNet())
        {
            if (_db.net(netIdx).isLeftSym())
            {
                return true;
            }
        }
        return false;
    };
    
    auto useLeftPin = [&](IndexType pinIdx)
    {
        if (_virtualPins.at(pinIdx).assigned())
        {
            return false;
        }
        if (_leftToRightMap.find(pinIdx) ==  _leftToRightMap.end())
        {
            return false;
        }
        return true;
    };

    auto symPairAssignNetToPinFunc = [&](IndexType netIdx, IndexType leftPinIdx)
    {
        auto rightPinIdx = _leftToRightMap.at(leftPinIdx);
        //if (_db.net(netIdx).isSelfSym())
        //{
        //    directAssignNetToPinFunc(netIdx, leftPinIdx);
        //    // TODO: also add right pin
        //}
        if (_db.net(netIdx).hasSymNet())
        {
            auto otherNetIdx = _db.net(netIdx).symNetIdx();
            //auto netCost0 = calculateShortestManhattanDistance(netIdx, leftPinIdx) + calculateShortestManhattanDistance(otherNetIdx, rightPinIdx);
            //auto netCost1 = calculateShortestManhattanDistance(otherNetIdx, leftPinIdx) + calculateShortestManhattanDistance(netIdx, rightPinIdx);
            if (1)
            //if (netCost0 <= netCost1)
            {
                // net -> left. other net -> right
                directAssignNetToPinFunc(netIdx, leftPinIdx);
                directAssignNetToPinFunc(otherNetIdx, rightPinIdx);
            }
            else
            {
                // net -> right. other net -> left
                directAssignNetToPinFunc(netIdx, rightPinIdx);
                directAssignNetToPinFunc(otherNetIdx, leftPinIdx);
            }
        }
        else
        {
            Assert(false);
        }

    };

    if (_fastMode)
    {
        if (!_networkSimplexPinAssignment(useSymNet, useLeftPin, symPairNetToPinCostFunc, symPairAssignNetToPinFunc))
        {
            return false;
        }

        return _networkSimplexPinAssignment(useASymNet, useFreePin, directNetToPinCostFunc, directAssignNetToPinFunc);
    }
    return _lpSimplexPinAssignment(useSymNet, useLeftPin, 
            useASymNet, useFreePin, 
            symPairNetToPinCostFunc, directNetToPinCostFunc,
            symPairAssignNetToPinFunc, directAssignNetToPinFunc);

}


bool VirtualPinAssigner::_lpSimplexPinAssignment(
        std::function<bool(IndexType)> isSymNetFunc,
        std::function<bool(IndexType)> isLeftPinFunc,
        std::function<bool(IndexType)> isOtherNetFunc,
        std::function<bool(IndexType)> isOtherPinFunc,
        std::function<LocType(IndexType, IndexType)> symNetToPinCostFunc,
        std::function<LocType(IndexType, IndexType)> otherNetToPinCostFunc,
        std::function<void(IndexType, IndexType)> setSymNetPairToPinFunc,
        std::function<void(IndexType, IndexType)> setOtherNetToPinFunc
        )
{
    using solver_type =  ::klib::lp::LpModel;
    using lp_type = ::klib::lp::LpTrait;
    using variable_type = lp_type::variable_type;
    using expr_type = lp_type::expr_type;

#ifdef DEBUG_PINASSIGN
    DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
    auto start = std::chrono::high_resolution_clock::now();


    // Collect the problem
    std::vector<IndexType> symNets, otherNets, symPins, otherPins;
    std::unordered_map<IndexType, IndexType> pinIdxToOtherPinIdxMap;
    std::vector<std::pair<IndexType, IndexType>> conflictPins; // .first = sym pair pin. .second = other pin
    for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
    {
        if (isSymNetFunc(netIdx))
        {
            symNets.emplace_back(netIdx);
        }
        if (isOtherNetFunc(netIdx))
        {
            otherNets.emplace_back(netIdx);
        }
    }
    for (IndexType pinIdx = 0; pinIdx < _virtualPins.size(); ++pinIdx)
    {
        if (isLeftPinFunc(pinIdx) && isOtherPinFunc(pinIdx))
        {
            symPins.emplace_back(pinIdx);
        }
        if (isOtherPinFunc(pinIdx))
        {
            otherPins.emplace_back(pinIdx);
            pinIdxToOtherPinIdxMap[pinIdx] = otherPins.size() - 1;
        }
    }
    // Construct the conflict between pin pairs and other pins
    for (IndexType idx = 0; idx < symPins.size(); ++idx)
    {
        IndexType rightPinIdx = _leftToRightMap.at(symPins[idx]);
        auto leftIdx = pinIdxToOtherPinIdxMap.at(symPins[idx]);
        auto rightIdx = pinIdxToOtherPinIdxMap.at(rightPinIdx);
        conflictPins.emplace_back(idx, leftIdx);
        conflictPins.emplace_back(idx, rightIdx);
    }

    IndexType m = symPins.size();
    Assert(2 * m == otherPins.size());
    IndexType na = otherNets.size();
    IndexType ns = symNets.size();
    Assert(2 * m == conflictPins.size());

    if (m < ns or 2 * m < ns + na)
    {
        return false;
    }

    if (na + ns == 0)
    {
        return true;
    }

    // Initalize the LP problem
    solver_type solver;
    expr_type obj;
    Vector2D<variable_type> xs(m, ns); // m * ns x decision variables. xs[i, j] -> assign symPins[i] to symNets[j] 
    Vector2D<variable_type> ys(2 * m,  na); // 2*m*na y decision variable. ys[i, j]->assign  otherPins[i] tootherNets[j]

    // Add variables to the LP problem model
    for (IndexType x = 0; x < m; ++x)
    {
        for (IndexType y = 0; y < ns; ++y)
        {
            xs.at(x, y) = lp_type::addVar(solver);
            lp_type::setVarInteger(solver, xs.at(x, y));
        }
    }
    for (IndexType x = 0; x < 2*m; ++x)
    {
        for (IndexType y = 0; y < na; ++y)
        {
            ys.at(x, y) = lp_type::addVar(solver);
            lp_type::setVarInteger(solver, ys.at(x, y));
        }
    }
    

    // configure the objective function
    // Sym nets to sym pins cost
    for (IndexType x = 0; x < m; ++x)
    {
        for (IndexType y = 0; y < ns; ++y)
        {
            obj += xs.at(x, y) * (symNetToPinCostFunc(symNets[y], symPins[x]) + 0.0) ;
        }
    }
    // Other
    for (IndexType x = 0; x < 2 * m; ++x)
    {
        for (IndexType y = 0; y < na; ++y)
        {
            obj += ys.at(x, y) * ( otherNetToPinCostFunc(otherNets[y], otherPins[x]) + 0.0);
        }
    }

    lp_type::setObjective(solver, obj);
    lp_type::setObjectiveMinimize(solver);


    // Add constraints
    // Assign all sym nets
    for (IndexType j = 0; j < ns; ++j)
    {
        expr_type lhs;
        for (IndexType i = 0; i < m; ++i)
        {
            lhs += xs.at(i, j);
        }
        lp_type::addConstr(solver, lhs == 1);
    }
    // Assign all other nets
    for (IndexType j = 0; j < na; ++j)
    {
        expr_type lhs;
        for (IndexType i = 0; i < 2 * m; ++i)
        {
            lhs += ys.at(i, j);
        }
        lp_type::addConstr(solver, lhs == 1);
    }
    // Sym pin and other pin conflict
    for (const auto & confilctPair : conflictPins)
    {
        IndexType symPinIdx = confilctPair.first;
        IndexType otherPinIdx = confilctPair.second;
        expr_type lhs;
        for (IndexType j = 0; j < ns; ++ j)
        {
            lhs += xs.at(symPinIdx, j);
        }
        for (IndexType j = 0; j < na; ++ j)
        {
            lhs +=  ys.at(otherPinIdx, j);
        }
        lp_type::addConstr(solver, lhs <= 1);
    }


    // Solve the LP
    lp_type::setNumThreads(solver, _db.parameters().numThreads());
    lp_type::solve(solver);


    bool failed = false;

    for (IndexType x = 0; x < m; ++x)
    {
        for (IndexType y = 0; y < ns; ++y)
        {
            auto sol = lp_type::solution(solver, xs.at(x, y));
            auto assigned = ::klib::autoRound<IntType>(sol);
            if (sol < 0.99 && sol > 0.001)
            {
                failed = true;
            }
            if (assigned == 1)
            {
                setSymNetPairToPinFunc(symNets.at(y), symPins.at(x));
            }
        }
    }
    for (IndexType x = 0; x < 2*m; ++x)
    {
        for (IndexType y = 0; y < na; ++y)
        {
            auto sol = lp_type::solution(solver, ys.at(x, y));
            if (sol < 0.99 && sol > 0.001)
            {
                failed = true;
            }
            auto assigned = ::klib::autoRound<IntType>(sol);
            if (assigned == 1)
            {
                setOtherNetToPinFunc(otherNets.at(y), otherPins.at(x));
            }
        }
    }

    AssertMsg(!failed, "Ideaplace:: virtual pin assignment:: ILP:: floating results \n");

    auto end = std::chrono::high_resolution_clock::now();
    std::cout<<" io pin assignment time "<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() <<" us " <<std::endl;;
    return true;
}

PROJECT_NAMESPACE_END
