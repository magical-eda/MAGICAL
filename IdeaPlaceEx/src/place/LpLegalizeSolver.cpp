#include "CGLegalizer.h"

PROJECT_NAMESPACE_BEGIN

RealType LpLegalizeSolver::evaluateObj()
{
    return _ilpModel.evaluateObjective();
}

void LpLegalizeSolver::exportSolution()
{
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        auto var = _ilpModel.variableSolution(AT(_locs, cellIdx));
        // convert to cell original location
        if (_isHor)
        {
            _db.cell(cellIdx).setXLoc(static_cast<LocType>(var - _db.cell(cellIdx).cellBBox().xLo()));
        }
        else
        {
            _db.cell(cellIdx).setYLoc(static_cast<LocType>(var - _db.cell(cellIdx).cellBBox().yLo()));
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
    _ilpModel.setObjective(_obj);
    _ilpModel.setOptimizeType(limbo::solvers::MIN);
    _params.setVerbose(2); // 2: SEVERE
    SolverType _solver(&_ilpModel);
    _optimStatus = _solver(&_params);
    if (_optimStatus == limbo::solvers::UNBOUNDED)
    {
        ERR("LP legalization solver: LP unbounded \n");
        return false;
    }
    else if (_optimStatus == limbo::solvers::OPTIMAL)
    {
        INF("LP legalization solver: LP optimal \n");
        return true;
    }
    else if (_optimStatus == limbo::solvers::INFEASIBLE)
    {
        ERR("LP legalization solver: LP infeasible \n");
        return false;
    }
    else
    {
        ERR("LP legalization solver: Unknown LP status %d \n", _optimStatus);
        return false;
    }
}

void LpLegalizeSolver::configureObjFunc()
{
    // Wirelength
    if (_optHpwl)
    {
        for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
        {
            auto weight = _db.net(netIdx).weight();
            _obj += weight * (AT(_wlR, netIdx) - AT(_wlL, netIdx));
        }
    }
    // Area
    if (_optArea)
    {
        _obj += _dim;
    }
}

void LpLegalizeSolver::addIlpVars()
{
    // Calculate how many variables are there in the ILP model
    numVars = _db.numCells() + _db.numNets() * 2 * _optHpwl + 1 * _optArea;
    // Reserve the variables in the model
    _ilpModel.reserveVariables(numVars);
    // add coordinate variables
    // NOTE: the _locs variables here are general location variables
    _locs.resize(_db.numCells());
    for (IndexType i = 0; i < _db.numCells(); ++i)
    {
        AT(_locs, i) = _ilpModel.addVariable(0, std::numeric_limits<RealType>::max(),
                                                limbo::solvers::CONTINUOUS, 
                                                "loc_" + std::to_string(i));
    }
    // Add wire length variables
    _wlL.resize(_db.numNets());
    _wlR.resize(_db.numNets());
    // add HPWL variables
    if (_optHpwl == 1)
    {
        for (IndexType i =0; i < _db.numNets(); ++i)
        {
            AT(_wlL, i) = _ilpModel.addVariable(0, std::numeric_limits<RealType>::max(),
                                                limbo::solvers::CONTINUOUS,
                                                "wll_" + std::to_string(i));
            AT(_wlR, i) = _ilpModel.addVariable(0, std::numeric_limits<RealType>::max(),
                                                limbo::solvers::CONTINUOUS,
                                                "wll_" + std::to_string(i));
        }
    }
    if (_optArea == 1)
    {
        _dim = _ilpModel.addVariable(0, std::numeric_limits<RealType>::max(),
                                    limbo::solvers::CONTINUOUS,
                                    "dim");
    }
    // Symmetric group axis variables
    _symLocs.resize(_db.numSymGroups());
    for (IndexType i = 0; i < _db.numSymGroups(); ++i)
    {
        AT(_symLocs, i) = _ilpModel.addVariable(0,
                std::numeric_limits<RealType>::max(),
                limbo::solvers::CONTINUOUS,
                "symLoc_"+std::to_string(i));
    }

}

void LpLegalizeSolver::addIlpConstraints()
{
    // Add boundary constraint
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
                _ilpModel.addConstraint(AT(_locs, i) <= _wStar - _db.cell(i).cellBBox().xLen());
            }
            else
            {
#ifdef DEBUG_LEGALIZE
                DBG("Add boundary constraint: y_%d <= %f - %d \n", i, _wStar, _db.cell(i).cellBBox().yLen());
#endif
                _ilpModel.addConstraint(AT(_locs, i) <= _wStar - _db.cell(i).cellBBox().yLen());
            }
        }
        else // if (_optArea == 0)
        {
            if (_isHor)
            {
                // 0 <= x_i <= W - w_i
                _ilpModel.addConstraint(AT(_locs, i) - _dim <= - _db.cell(i).cellBBox().xLen());
            }
            else
            {
                _ilpModel.addConstraint(AT(_locs, i) - _dim <= - _db.cell(i).cellBBox().yLen());
            }
        }
    }

    // Add topology constraints
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
        spacing += 340;
        // Add the constraint 
        // x_i + w_i + spacing <= x_j
        _ilpModel.addConstraint(AT(_locs, sourceIdx) - AT(_locs, targetIdx) <= - cellDim - spacing);
#ifdef DEBUG_LEGALIZE
        DBG("Add spacing constrain: from %d to %d, <= -celldim %d - spacing %d = %d \n", sourceIdx, targetIdx, cellDim, spacing, -cellDim - spacing);
#endif
    }

    // Add symmetric constraints
    if (_isHor)
    {
        // Force them to be symmetric along an axis
        for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
        {
            const auto &symGrp = _db.symGroup(symGrpIdx);

            for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
            {
                const auto &symPair = symGrp.symPair(symPairIdx);
                // x1 + x2 + width =  2 * symAxis
#ifdef DEBUG_LEGALIZE
                DBG("Add sym constraint. \n symGrp %d, cell %d %d \n width %d \n",
                        symGrpIdx, symPair.firstCell(), symPair.secondCell(), _db.cell(symPair.firstCell()).cellBBox().xLen());
#endif
                _ilpModel.addConstraint(AT(_locs, symPair.firstCell()) 
                        + AT(_locs, symPair.secondCell()) 
                        - 2*AT(_symLocs, symGrpIdx) 
                        == 
                        -_db.cell(symPair.firstCell()).cellBBox().xLen()); // Two cells are equal in width <- assumption
                AssertMsg(_db.cell(symPair.firstCell()).cellBBox().xLen() == _db.cell(symPair.secondCell()).cellBBox().xLen(), "cell %s and cell %s \n", _db.cell(symPair.firstCell()).name().c_str(),  _db.cell(symPair.secondCell()).name().c_str());
            }
            for (IndexType selfSymIdx = 0; selfSymIdx < symGrp.numSelfSyms(); ++selfSymIdx)
            {
                IndexType ssCellIdx = symGrp.selfSym(selfSymIdx);
                // x1 + width + x2 = 2 * symAxis
                _ilpModel.addConstraint(2 * AT(_locs, ssCellIdx) - 2 * AT(_symLocs, symGrpIdx)
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
                _ilpModel.addConstraint(AT(_locs, symPair.firstCell()) - AT(_locs, symPair.secondCell()) == 0.0);
            }
        }
    }

    // Add HPWL constraints
    if (_optHpwl)
    {
        for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
        {
            const auto &net = _db.net(netIdx);
            for (IndexType pinIdxInNet =0; pinIdxInNet < net.numPinIdx(); ++pinIdxInNet)
            {
                IndexType pinIdx = net.pinIdx(pinIdxInNet);
                const auto &pin = _db.pin(pinIdx);
                auto midLoc = pin.midLoc();
                if (_isHor)
                {
                    RealType loc = static_cast<RealType>(midLoc.x());
                    // wl_l <= _loc + pin_offset for all pins in the net
                    _ilpModel.addConstraint(  AT(_wlL, netIdx)
                            - AT(_locs, pin.cellIdx())
                            <=  loc);
                    // wl_r >= _loc + pin_offset for all pins in the net
                    _ilpModel.addConstraint(  AT(_wlR, netIdx)
                            - AT(_locs, pin.cellIdx()) 
                            >=  loc);
                }
                else
                {
                    RealType loc = static_cast<RealType>(midLoc.y());
                    // wl_l <= _loc + pin_offset for all pins in the net
                    _ilpModel.addConstraint(  AT(_wlL, netIdx)
                            - AT(_locs, pin.cellIdx())
                            <=  loc);
                    // wl_r >= _loc + pin_offset for all pins in the net
                    _ilpModel.addConstraint(  AT(_wlR, netIdx)
                            - AT(_locs, pin.cellIdx()) 
                            >=  loc);
                }
            }
        }
    }
}

PROJECT_NAMESPACE_END
