#include "CGLegalizer.h"
#include "signalPathMgr.h"

PROJECT_NAMESPACE_BEGIN

RealType LpLegalizeSolver::evaluateObj()
{
    return lp_trait::evaluateExpr(_solver, _obj);
}

void LpLegalizeSolver::exportSolution()
{
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        auto var = lp_trait::solution(_solver, _locs.at(cellIdx));
        // convert to cell original location
        if (_isHor)
        {
            _db.cell(cellIdx).setXLo(::klib::autoRound<LocType>(var) + _db.parameters().layoutOffset());
        }
        else
        {
            _db.cell(cellIdx).setYLo(::klib::autoRound<LocType>(var) + _db.parameters().layoutOffset());
        }
    }
}

bool LpLegalizeSolver::solve()
{
    // Add variables
    addIlpVars();
    // add constraints
    addIlpConstraints();
    // Configure the objective function
    configureObjFunc();
    // Solve the LP problem
    return solveLp();
}

bool LpLegalizeSolver::solveLp()
{
    lp_trait::setNumThreads(_solver, _db.parameters().numThreads());
    lp_trait::setObjectiveMinimize(_solver);
    lp_trait::setObjective(_solver, _obj);
    lp_trait::solve(_solver);
    if (lp_trait::isUnbounded(_solver))
    {
        ERR("LP legalization solver: LP unbounded \n");
        return false;
    }
    else if (lp_trait::isOptimal(_solver))
    {
        INF("LP legalization solver: LP optimal \n");
        return true;
    }
    else if (lp_trait::isInfeasible(_solver))
    {
        ERR("LP legalization solver: LP infeasible \n");
        return false;
    }
    else if (lp_trait::isSuboptimal(_solver))
    {
        ERR("LP legalization solver: LP suboptimal \n");
        return false;
    }
    else
    {
        ERR("LP legalization solver: Unknown LP status %s \n", lp_trait::statusStr(_solver).c_str());
        return false;
    }
}

void LpLegalizeSolver::addWirelengthObj()
{
    if (_optHpwl == 1)
    {
        bool hasAtLeastOneNet = false;
        for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
        {
            if (_db.net(netIdx).numPinIdx() == 0)
            {
                continue;
            }
            if (_db.net(netIdx).numPinIdx() == 1 && !_db.net(netIdx).isValidVirtualPin())
            {
                continue;
            }
            hasAtLeastOneNet = true;
            auto weight = _db.net(netIdx).weight();
            _obj += weight * (_wlR[netIdx] - _wlL[netIdx]);
        }
        if (!hasAtLeastOneNet)
        {
            ERR("LP Legalizer:: No valid net \n");
            //Assert(false);
        }
    }
}

void LpLegalizeSolver::addAreaObj()
{
    if (_optArea == 1)
    {
        _obj += _dim;
    }
}

void LpLegalizeSolver::addSymObj()
{
    if (_isHor)
    {
        for (IndexType symGrpIdx = 0; symGrpIdx < _symRexLeft.size(); ++symGrpIdx)
        {
            _obj += _largeNum * (_symRexRight.at(symGrpIdx) - _symRexLeft.at(symGrpIdx));
        }
    }
    else
    {
        // Force they have the same y coordinate
        for (IndexType symGroupIdx = 0;  symGroupIdx < _db.numSymGroups(); ++symGroupIdx)
        {
            const auto & symGroup = _db.symGroup(symGroupIdx);
            for (IndexType symPairIdx = 0; symPairIdx < symGroup.numSymPairs(); ++symPairIdx)
            {
                const auto &symPair = symGroup.symPair(symPairIdx);
                IndexType bCellIdx = symPair.firstCell();
                IndexType tCellIdx = symPair.secondCell();
                if (_db.cell(bCellIdx).yLoc() > _db.cell(tCellIdx).yLoc())
                {
                    std::swap(tCellIdx, bCellIdx);
                }
                //  + M *( y_t - y_b)
                _obj += _largeNum * (_locs.at(tCellIdx) - _locs.at(bCellIdx));
            }
        }
    }
}

void LpLegalizeSolver::configureObjFunc()
{
    // Wirelength
    this->addWirelengthObj();
    // Area
    this->addAreaObj();
    if (_relaxEqualityConstraint)
    {
        addSymObj();
    }
}


IndexType LpLegalizeSolver::numVars() const
{
    auto numLocVars = _db.numCells();
    auto numHpwlVars = _db.numNets() * 2 * _optHpwl;
    auto numBoundaryVars = 1 * _optArea;
    IndexType numSymVars;
    if (_isMultipleSymGrp)
    {
        numSymVars = _db.numSymGroups();
    }
    else
    {
        numSymVars = 1;
    }
    return numLocVars + numHpwlVars + numBoundaryVars + numSymVars;
}

void LpLegalizeSolver::addLocVars()
{
    // NOTE: the _locs variables here are general location variables
    _locs.resize(_db.numCells());
    for (IndexType i = 0; i < _db.numCells(); ++i)
    {
        _locs.at(i) = lp_trait::addVar(_solver);
    }
}

void LpLegalizeSolver::addWirelengthVars()
{
    // add HPWL variables
    if (_optHpwl == 1)
    {
        _wlL.resize(_db.numNets());
        _wlR.resize(_db.numNets());
        for (IndexType i =0; i < _db.numNets(); ++i)
        {
            _wlL.at(i) = lp_trait::addVar(_solver);
            _wlR.at(i) = lp_trait::addVar(_solver);
        }
    }
}

void LpLegalizeSolver::addAreaVars()
{
    if (_optArea == 1)
    {
        _dim = lp_trait::addVar(_solver);
    }
}

void LpLegalizeSolver::addSymVars()
{
    if (_relaxEqualityConstraint)
    {
        if (_isMultipleSymGrp)
        {
            // Symmetric group axis variables
            _symRexLeft.resize(_db.numSymGroups());
            _symRexRight.resize(_db.numSymGroups());
            for (IndexType i = 0; i < _db.numSymGroups(); ++i)
            {
                _symRexLeft.at(i) = lp_trait::addVar(_solver);
                _symRexRight.at(i) = lp_trait::addVar(_solver);
            }
        }
        else
        {
            _symRexLeft.resize(1);
            _symRexLeft[0] = lp_trait::addVar(_solver);
            _symRexRight.resize(1);
            _symRexRight[0] = lp_trait::addVar(_solver);
        }
        return;
    }
    if (_isMultipleSymGrp)
    {
        // Symmetric group axis variables
        _symLocs.resize(_db.numSymGroups());
        for (IndexType i = 0; i < _db.numSymGroups(); ++i)
        {
            _symLocs.at(i) = lp_trait::addVar(_solver);
        }
    }
    else
    {
        _symLocs.resize(1);
        _symLocs[0] = lp_trait::addVar(_solver);
    }
}

void LpLegalizeSolver::addIlpVars()
{
    // Calculate how many variables are there in the ILP model
    //auto numVars = this->numVars();
    // Reserve the variables in the model
    //_ilpModel.reserveVariables(numVars);

    // add coordinate variables
    this->addLocVars();
    // Add wire length variables
    this->addWirelengthVars();
    // Add area variables
    this->addAreaVars();
    // Add symmetric variables
    this->addSymVars();

}

void LpLegalizeSolver::addBoundaryConstraints()
{
    for (IndexType i = 0;  i < _db.numCells(); ++i)
    {
        if (_optArea == 0)
        {
            if (_isHor)
            {
                // 0 <= x_i <= W* - w_i
#ifdef DEBUG_LEGALIZE
                DBG("Add boundary constraint: x_%d <= %f - %d \n", i, _wStar, _db.cell(i).cellBBox().xLen());
#endif
                lp_trait::addConstr(_solver, _locs.at(i) <= _wStar + _db.parameters().layoutOffset() - _db.cell(i).cellBBox().xLen());
            }
            else
            {
#ifdef DEBUG_LEGALIZE
                DBG("Add boundary constraint: y_%d <= %f - %d \n", i, _wStar, _db.cell(i).cellBBox().yLen());
#endif
                lp_trait::addConstr(_solver, _locs.at(i) <= _wStar + _db.parameters().layoutOffset() - _db.cell(i).cellBBox().yLen());
            }
            lp_trait::addConstr(_solver, _locs.at(i) >= + _db.parameters().layoutOffset());
        }
        else // if (_optArea == 0)
        {
            if (_isHor)
            {
                // 0 <= x_i <= W - w_i
                lp_trait::addConstr(_solver, _locs.at(i) - _dim <= - _db.cell(i).cellBBox().xLen());
            }
            else
            {
                lp_trait::addConstr(_solver, _locs.at(i) - _dim <= - _db.cell(i).cellBBox().yLen());
            }
        }
    }
}

void LpLegalizeSolver::addTopologyConstraints()
{
    for (auto & edge : _constrains.edges())
    {
        IndexType sourceIdx = edge.source();
        IndexType targetIdx = edge.target();
        if (sourceIdx == targetIdx)
        {
            continue;
        }
        if (sourceIdx == _db.numCells() || targetIdx == _db.numCells() + 1)
        {
            // the s, t constraints
            continue;
        }
        LocType cellDim; // width or height
        auto spacingBox = _db.cellSpacing(sourceIdx, targetIdx);
        LocType spacing;
        // Force cell1 to be lower/left to the cell2. Therefore only using spacingBox.xLo() and .yLo()
        if (_isHor)
        {
            cellDim = _db.cell(sourceIdx).cellBBox().xLen();
            spacing = spacingBox.xLo();
        }
        else
        {
            cellDim = _db.cell(sourceIdx).cellBBox().yLen();
            spacing = spacingBox.yLo();
        }
        // Add the constraint 
        // x_i + w_i + spacing <= x_j
        lp_trait::addConstr(_solver, _locs.at(sourceIdx) - _locs.at(targetIdx) <= - cellDim - spacing);
#ifdef DEBUG_LEGALIZE
        DBG("Add spacing constrain: from %d to %d, <= -celldim %d - spacing %d = %d \n", sourceIdx, targetIdx, cellDim, spacing, -cellDim - spacing);
#endif
    }

}


void LpLegalizeSolver::addSymmetryConstraints()
{
    if (_relaxEqualityConstraint)
    {
        addSymmetryConstraintsRex();
    }
    else
    {
        addSymmetryConstraintsWithEqu();
    }
}

void LpLegalizeSolver::addSymmetryConstraintsRex()
{
    if (_isHor)
    {
        for (IndexType symGroupIdx = 0;  symGroupIdx < _db.numSymGroups(); ++symGroupIdx)
        {
            const auto & symGroup = _db.symGroup(symGroupIdx);
            lp_variable_type *leftSymLoc, *rightSymLoc;
            if (_isMultipleSymGrp)
            {
                leftSymLoc = & _symRexLeft.at(symGroupIdx);
                rightSymLoc = & _symRexRight.at(symGroupIdx);
            }
            else
            {
                leftSymLoc = & _symRexLeft.at(0);
                rightSymLoc = & _symRexRight.at(0);
            }
            lp_trait::addConstr(_solver, *rightSymLoc - *leftSymLoc >= 0);
            for (IndexType symPairIdx = 0; symPairIdx < symGroup.numSymPairs(); ++symPairIdx)
            {
                const auto &symPair = symGroup.symPair(symPairIdx);
                // x1 + x2 + width  <= right * 2
                // x1 + x2 + width >= left * 2
                lp_variable_type x1_ = _locs.at(symPair.firstCell());
                lp_variable_type x2_ = _locs.at(symPair.secondCell());
                auto width_ = _db.cell(symPair.firstCell()).cellBBox().xLen(); // Two cells are equal in width <- assumption
                lp_trait::addConstr(_solver, x1_ + x2_  -2*(*rightSymLoc) <=  - width_);
                lp_trait::addConstr(_solver, x1_ + x2_ + -2*(*leftSymLoc) >= - width_);
            }
            for (IndexType ssIdx = 0; ssIdx < symGroup.numSelfSyms(); ++ssIdx)
            {
                IndexType cellIdx = symGroup.selfSym(ssIdx);
                lp_variable_type x_ = _locs.at(cellIdx);
                auto width_ = _db.cell(cellIdx).cellBBox().xLen();
                // x + width /2 <= right
                // x + width /2 >= left
                lp_trait::addConstr(_solver, x_ - (*rightSymLoc) <= -width_ / 2);
                lp_trait::addConstr(_solver, x_ - (*leftSymLoc) >= -width_ / 2);
            }
        }
    }
    else
    {
        // Force they have the same y coordinate
        for (IndexType symGroupIdx = 0;  symGroupIdx < _db.numSymGroups(); ++symGroupIdx)
        {
            const auto & symGroup = _db.symGroup(symGroupIdx);
            for (IndexType symPairIdx = 0; symPairIdx < symGroup.numSymPairs(); ++symPairIdx)
            {
                const auto &symPair = symGroup.symPair(symPairIdx);
                IndexType bCellIdx = symPair.firstCell();
                IndexType tCellIdx = symPair.secondCell();
                if (_db.cell(bCellIdx).yLoc() > _db.cell(tCellIdx).yLoc())
                {
                    std::swap(tCellIdx, bCellIdx);
                }
                // y_b - y_t <= 0
                lp_trait::addConstr(_solver, _locs.at(bCellIdx) - _locs.at(tCellIdx) <= 0.0);
            }
        }
    }
}

void LpLegalizeSolver::addSymmetryConstraintsWithEqu()
{
    if (_isHor)
    {
        // Force them to be symmetric along an axis
        for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
        {
            const auto &symGrp = _db.symGroup(symGrpIdx);
            lp_variable_type *symVar;
            if (_isMultipleSymGrp)
            {
                symVar = &_symLocs[symGrpIdx];
            }
            else
            {
                symVar = &_symLocs[0];
            }

            for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
            {
                const auto &symPair = symGrp.symPair(symPairIdx);
                // x1 + x2 + width =  2 * symAxis
#ifdef DEBUG_LEGALIZE
                DBG("Add sym constraint. \n symGrp %d, cell %d %d \n width %d \n",
                        symGrpIdx, symPair.firstCell(), symPair.secondCell(), _db.cell(symPair.firstCell()).cellBBox().xLen());
#endif
                lp_trait::addConstr(_solver,_locs.at(symPair.firstCell()) 
                        + _locs.at(symPair.secondCell()) 
                        - 2 * (*symVar) 
                        == 
                        -_db.cell(symPair.firstCell()).cellBBox().xLen()); // Two cells are equal in width <- assumption
                AssertMsg(_db.cell(symPair.firstCell()).cellBBox().xLen() == _db.cell(symPair.secondCell()).cellBBox().xLen(), "cell %s and cell %s \n", _db.cell(symPair.firstCell()).name().c_str(),  _db.cell(symPair.secondCell()).name().c_str());
            }
            for (IndexType selfSymIdx = 0; selfSymIdx < symGrp.numSelfSyms(); ++selfSymIdx)
            {
                IndexType ssCellIdx = symGrp.selfSym(selfSymIdx);
#ifdef DEBUG_LEGALIZE
                DBG("Add self sym constraint. \n symGrp %d, cell %d  \n width %d \n",
                        symGrpIdx, ssCellIdx, _db.cell(ssCellIdx).cellBBox().xLen());
#endif
                // x1 + width + x2 = 2 * symAxis
                lp_trait::addConstr(_solver, 2 * _locs.at(ssCellIdx) - 2 * (*symVar)
                        == - _db.cell(ssCellIdx).cellBBox().xLen());
            }
        }
    }
    else
    {
        // Force they have the same y coordinate
        for (IndexType symGroupIdx = 0;  symGroupIdx < _db.numSymGroups(); ++symGroupIdx)
        {
            const auto & symGroup = _db.symGroup(symGroupIdx);
            for (IndexType symPairIdx = 0; symPairIdx < symGroup.numSymPairs(); ++symPairIdx)
            {
                const auto &symPair = symGroup.symPair(symPairIdx);
                // y_i = y_j
                lp_trait::addConstr(_solver, _locs.at(symPair.firstCell()) - _locs.at(symPair.secondCell()) == 0.0);
#ifdef DEBUG_LEGALIZE
                DBG("Add y sym constraint. \n symGrp %d, cell %d %d  \n",
                        symGroupIdx, symPair.firstCell(), symPair.secondCell());
#endif
            }
        }
    }
}

void LpLegalizeSolver::addHpwlConstraints()
{
    if (_optHpwl)
    {
        for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
        {
            const auto &net = _db.net(netIdx);
            for (IndexType pinIdxInNet =0; pinIdxInNet < net.numPinIdx(); ++pinIdxInNet)
            {
                IndexType pinIdx = net.pinIdx(pinIdxInNet);
                const auto &pin = _db.pin(pinIdx);
                const auto &cell = _db.cell(pin.cellIdx());
                auto midLoc = pin.midLoc();
                XY<RealType> cellLoLoc = XY<RealType>(cell.cellBBox().xLo(), cell.cellBBox().yLo());
                midLoc -= cellLoLoc;
                if (_isHor)
                {
                    RealType loc = static_cast<RealType>(midLoc.x());
                    // wl_l <= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver,  _wlL.at(netIdx)
                            - _locs.at(pin.cellIdx())
                            <=  loc);
                    // wl_r >= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver, _wlR.at(netIdx)
                            - _locs.at(pin.cellIdx()) 
                            >=  loc);
                }
                else
                {
                    RealType loc = static_cast<RealType>(midLoc.y());
                    // wl_l <= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver, _wlL.at(netIdx)
                            - _locs.at(pin.cellIdx())
                            <=  loc);
                    // wl_r >= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver, _wlR.at(netIdx)
                            - _locs.at(pin.cellIdx()) 
                            >=  loc);
                }
                lp_trait::addConstr(_solver, _wlR.at(netIdx) - _wlL.at(netIdx) >= 0);
            }
            // Wirelength with virtual pin
            if (net.isValidVirtualPin())
            {
                if (_isHor)
                {
                    RealType loc = static_cast<RealType>(net.virtualPinLoc().x());
                    // wl_l <= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver, _wlL.at(netIdx)
                            <=  std::max(loc, 0.0));
                    // wl_r >= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver, _wlR.at(netIdx)
                            >=  loc);
                }
                else
                {
                    RealType loc = static_cast<RealType>(net.virtualPinLoc().y());
                    // wl_l <= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver, _wlL.at(netIdx)
                            <=  std::max(loc, 0.0));
                    // wl_r >= _loc + pin_offset for all pins in the net
                    lp_trait::addConstr(_solver, _wlR.at(netIdx)
                            >=  loc);
                }
            }
        }
    }
}

void LpLegalizeSolver::addCurrentFlowConstraints()
{
    if (_isHor) { return; }
    if (not _useCurrentFlowConstraint) { return; }
    SigPathMgr pathMgr(_db);
    for (IndexType pathIdx = 0; pathIdx < pathMgr.vvSegList().size(); ++pathIdx)
    {
        const auto &path = pathMgr.vvSegList().at(pathIdx);
        if (not _db.signalPath(pathIdx).isPower())
        {
            continue;
        }
        for (const auto & seg : path)
        {
            IndexType sPinIdx = seg.beginPinFirstSeg();
            IndexType midPinIdxA = seg.endPinFirstSeg();
            IndexType midPinIdxB = seg.beginPinSecondSeg();
            IndexType tPinIdx = seg.endPinSecondSeg();

            const auto &sPin = _db.pin(sPinIdx);
            IndexType sCellIdx = sPin.cellIdx();
            const auto &mPinA = _db.pin(midPinIdxA);
            const auto &mPinB = _db.pin(midPinIdxB);
            IndexType mCellIdx = mPinA.cellIdx();
            const auto &tPin = _db.pin(tPinIdx);
            IndexType tCellIdx = tPin.cellIdx();

            const auto &sPinOffset = sPin.midLoc() - _db.cell(sCellIdx).cellBBox().ll();
            const auto &midPinOffsetA = mPinA.midLoc() - _db.cell(mCellIdx).cellBBox().ll();
            const auto &midPinOffsetB = mPinB.midLoc() - _db.cell(mCellIdx).cellBBox().ll();
            const auto &tPinOffset = tPin.midLoc() - _db.cell(tCellIdx).cellBBox().ll();

            lp_trait::addConstr(_solver, _locs.at(mCellIdx) - _locs.at(sCellIdx) <= sPinOffset.y() - midPinOffsetA.y());
            lp_trait::addConstr(_solver, _locs.at(tCellIdx) - _locs.at(mCellIdx) <= midPinOffsetB.y() - tPinOffset.y());
        }
    }
}

void LpLegalizeSolver::addIlpConstraints()
{
    // Add boundary constraint
    addBoundaryConstraints();
    // Add topology constraints
    addTopologyConstraints();
    // Add symmetric constraints
    addSymmetryConstraints();
    // Add HPWL constraints
    addHpwlConstraints();
    /// Add current flow constraints
    addCurrentFlowConstraints();
}

PROJECT_NAMESPACE_END
