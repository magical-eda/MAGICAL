#include "top_floorplan.hpp"

PROJECT_NAMESPACE_BEGIN

struct SymNetName
{
    SymNetName(const std::string &one, const std::string &otherOne)
    {
        left = one;
        right = otherOne;
        if (left > right)
        {
            std::swap(left, right);
        }
    }
    bool operator==(const SymNetName& rhs) const
    {
        if (left != rhs.left)
        {
            Assert(right != rhs.right);
            return false;
        }
        if (right != rhs.right)
        {
            Assert(left != rhs.left);
            return false;
        }
        return true;
    }
    bool operator<(const SymNetName& rhs) const
    {
        if (left != rhs.left)
        {
            return left < rhs.left;
        }
        return right < rhs.right;
    }
    std::string left, right;
};

void parseSymnetFile(const std::string &symnetFile, std::set<SymNetName> &nameSet)
{
    std::ifstream inf(symnetFile.c_str());
    if (!inf.is_open()) 
    {
        ERR("TopFloorplanProblem::Symnet parser:%s: cannot open file: %s \n", __FUNCTION__ , symnetFile.c_str());
        return;
    }
    INF("TopFloorplanProblem::Symnet parser: reading %s...\n", symnetFile.c_str());
    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::istringstream iss(lineStr);
        std::vector<std::string> split;
        std::string token;
        while(std::getline(iss, token, ' '))
        {
            split.emplace_back(token);
        }
        if (split.size() == 2)
        {
            nameSet.insert(SymNetName(split.at(0), split.at(1)));
        }
    }

}

void TopFloorplanProblem::initProblem(const DesignDB& dDb, const CktGraph &ckt, const std::string &symnetFileDir)
{
    std::unordered_map<std::string, std::set<SymNetName>> symnetNames;
    // Extracted the information from the CktGraph
    for (const CktNode &cktNode : ckt.nodeArray())
    {
        if (cktNode.isLeaf())
        {
            continue;
        }
        if (dDb.subCktConst(cktNode.subgraphIdx()).implType() != ImplType::UNSET)
        {
            // only care about subckt
            continue;
        }
        std::string cktSymnetName = symnetFileDir + "/" + cktNode.refName() + ".symnet";
        parseSymnetFile(cktSymnetName, symnetNames[cktNode.refName()]);
    }

    // Add pins
    _numAsymPins = 0;
    _numSymPriPins = 0;
    _numSymSecPins = 0;
    for (const Pin& pin : ckt.pinArray()) 
    {
        auto addDontCarePin = [&]()
        {
            PinIdx pinIdx;
            pinIdx.pinType = FpPinType::OTHER;
            _pinIdx.emplace_back(pinIdx);
        };
        if (pin.pinType() != PinType::UNSET)
        {
            // NWELL, PSUB
            addDontCarePin();
            continue;
        }
        const auto& cktNode = ckt.nodeArray().at(pin.nodeIdx());
        if (cktNode.isLeaf()) 
        {
            addDontCarePin();
            continue;
        }
        if (dDb.subCktConst(cktNode.subgraphIdx()).implType() != ImplType::UNSET)
        {
            // only care about subckt
            addDontCarePin();
            continue;
        }
        const auto& subCkt = dDb.subCktConst(cktNode.subgraphIdx());
        const auto& internalNetName = subCkt.netArray().at(pin.intNetIdx()).name();
        bool isAsym = true;
        for (const auto& symnet : symnetNames[cktNode.refName()])
        {
            if (symnet.left == internalNetName)
            {
                isAsym = false;
                PinIdx pinIdx;
                pinIdx.pinType = FpPinType::SYM_PRI;
                pinIdx.idx = _numSymPriPins;
                ++_numSymPriPins;
                pinIdx.cellIdx = pin.nodeIdx();
                _pinIdx.emplace_back(pinIdx);
                break;
            }
            if (symnet.right == internalNetName)
            {
                isAsym = false;
                PinIdx pinIdx;
                pinIdx.pinType = FpPinType::SYM_SCE;
                pinIdx.idx = _numSymSecPins;
                ++_numSymSecPins;
                pinIdx.cellIdx = pin.nodeIdx();
                _pinIdx.emplace_back(pinIdx);
                break;
            }
        }
        if (isAsym)
        {
            PinIdx pinIdx;
            pinIdx.pinType = FpPinType::ASYM;
            pinIdx.idx = _numAsymPins;
            ++_numAsymPins;
            pinIdx.cellIdx = pin.nodeIdx();
            _pinIdx.emplace_back(pinIdx);
        }
    }
    Assert(_numSymPriPins == _numSymSecPins);
    // Obtain the lower level layout
    for (IndexType cellIdx = 0; cellIdx < ckt.nodeArray().size(); ++cellIdx)
    {
        const CktNode &cktNode = ckt.nodeArray().at(cellIdx);
        IndexType subCktIdx = cktNode.subgraphIdx();
        const CktGraph &subCkt = dDb.subCktConst(subCktIdx);
        Box<LocType> cellLayoutBBox = subCkt.layoutConst().boundary();
        if (cktNode.flipVertFlag())
        {
            cellLayoutBBox.flipHor();
        }
        cellLayoutBBox.offsetBy(cktNode.offsetConst());
        _cellBBox.emplace_back(cellLayoutBBox);
    }
    // Obtain the nets information
    for (IndexType netIdx = 0; netIdx < ckt.numNets(); ++netIdx)
    {
        FpNet net;
        const auto& cktNet = ckt.netArray().at(netIdx);
        net.pins = cktNet.pinIdxArrayConst();
        _nets.emplace_back(net);
    }
}

/* Solving the problem */

void IlpTopFloorplanProblem::verticalSweepLine()
{
    auto getXLoFunc = [&](IndexType cellIdx)
    {
        return _problem._cellBBox.at(cellIdx).xLo();
    };
    auto getYLoFunc = [&](IndexType cellIdx)
    {
        return _problem._cellBBox.at(cellIdx).yLo();
    };
    auto getXHiFunc = [&](IndexType cellIdx)
    {
        return _problem._cellBBox.at(cellIdx).xHi();
    };
    auto getYHiFunc = [&](IndexType cellIdx)
    {
        return _problem._cellBBox.at(cellIdx).yHi();
    };
    SweeplineConstraintGraphGenerator sweep(_problem._cellBBox.size(), getYLoFunc, getYHiFunc, getXLoFunc, getXHiFunc);
    sweep.solve();
    for (const auto &edge : sweep.constrEdges())
    {
        _verConstrGraph.emplace_back(edge); /// This implementation is not optimized
    }
}

void IlpTopFloorplanProblem::addVariables()
{
    // For each pair of sym pins, add a binary variable
    Assert(_problem._numSymPriPins == _problem._numSymSecPins);
    for (IndexType symPairIdx = 0; symPairIdx < _problem._numSymPriPins; ++symPairIdx)
    {
        _symPinAssignVars.emplace_back(lp_trait::addVar(_solver));
        // Binary
        lp_trait::setVarInteger(_solver, _symPinAssignVars.back());
        lp_trait::setVarLowerBound(_solver, _symPinAssignVars.back(), 0);
        lp_trait::setVarUpperBound(_solver, _symPinAssignVars.back(), 1);
    }
    // For each asym net, add a binary variable
    for (IndexType asymPinIdx = 0; asymPinIdx < _problem._numAsymPins; ++asymPinIdx)
    {
        _aSymAssignVars.emplace_back(lp_trait::addVar(_solver));
        // Binary
        lp_trait::setVarInteger(_solver, _aSymAssignVars.back());
        lp_trait::setVarLowerBound(_solver, _aSymAssignVars.back(), 0);
        lp_trait::setVarUpperBound(_solver, _aSymAssignVars.back(), 1);
    }
    // Extra resources for each module
    for (IndexType cellIdx = 0; cellIdx < _problem._cellBBox.size(); ++cellIdx)
    {
        _extraResourcesVars.emplace_back(lp_trait::addVar(_solver));
        // Integer
        lp_trait::setVarInteger(_solver, _extraResourcesVars.back());
        lp_trait::setVarLowerBound(_solver, _extraResourcesVars.back(), 0);
    }
    // Each module has a variable indicate its yLo
    for (IndexType cellIdx = 0; cellIdx < _problem._cellBBox.size(); ++cellIdx)
    {
        _yLoVars.emplace_back(lp_trait::addVar(_solver));
        // Continuous
        lp_trait::setVarContinuous(_solver, _yLoVars.back());
        lp_trait::setVarLowerBound(_solver, _yLoVars.back(), 0);
    }
    // Add crossing indicating variables for each net
    for (IndexType netIdx = 0; netIdx < _problem._nets.size(); ++netIdx)
    {
        _crossVars.emplace_back(std::vector<lp_variable_type>());
        auto numPins = _problem._nets.at(netIdx).pins.size();
        if (numPins < 2) continue;
        auto numVarsNeeded = (numPins - 1) * (numPins) / 2; // 1 + 2...+n-1
        for (IndexType crossIdx = 0; crossIdx < numVarsNeeded; ++crossIdx)
        {
            _crossVars.back().emplace_back(lp_trait::addVar(_solver));
            // Binary
            lp_trait::setVarInteger(_solver, _crossVars.back().back());
            lp_trait::setVarLowerBound(_solver, _crossVars.back().back(), 0);
            lp_trait::setVarUpperBound(_solver, _crossVars.back().back(), 1);
        }
    }
    // Add the boundary variable
    _yHiVar = lp_trait::addVar(_solver);
    // Continuous
    lp_trait::setVarContinuous(_solver, _yHiVar);
    lp_trait::setVarLowerBound(_solver, _yHiVar, 0);
}

void IlpTopFloorplanProblem::addYLoConstr()
{
    for (const auto &edge : _verConstrGraph)
    {
        if (edge.source() == _problem._cellBBox.size()) continue; // source node
        if (edge.target() == _problem._cellBBox.size() + 1) continue; // target node
        auto cellHeight = _problem._cellBBox.at(edge.source()).yLen();
        // y_i + h_i + m * s_i <= y_j
        lp_trait::addConstr(_solver, 
                _yLoVars.at(edge.source()) - _yLoVars.at(edge.target())
                + _extraResourcesVars.at(edge.source()) * _problem.resourcePerLen
                <=
                - cellHeight);
    }
}

void IlpTopFloorplanProblem::addPinResrouceConstr()
{
    std::vector<IntType> pinIdxToResourceCost;
    pinIdxToResourceCost.resize(_problem._pinIdx.size(), 1); /// TODO: use realistic modeling
    for (IndexType cellIdx = 0; cellIdx < _problem._cellBBox.size(); ++cellIdx)
    {
        std::vector<IndexType> asymPinIdx;
        std::vector<IntType> asymPinResources;
        IndexType symPairResources = 0;
        // Collect the pins in the cell
        /// Not a optimized implementation
        for (IndexType pinIdx = 0; pinIdx < _problem._pinIdx.size(); ++pinIdx)
        {
            const auto &fpPin = _problem._pinIdx.at(pinIdx);
            if (fpPin.cellIdx != cellIdx) continue;
            if (fpPin.pinType == TopFloorplanProblem::FpPinType::SYM_PRI)
            {
                symPairResources += pinIdxToResourceCost.at(pinIdx);
            }
            else if (fpPin.pinType == TopFloorplanProblem::FpPinType::ASYM)
            {
                asymPinIdx.emplace_back(fpPin.idx);
                asymPinResources.emplace_back(pinIdxToResourceCost.at(pinIdx));
            }
        }
        // Calculate the resources available
        IntType baseResource = _problem._cellBBox.at(cellIdx).yLen() / _problem.resourcePerLen;
        IntType baseAsymResource = baseResource - symPairResources;
        // x_i in the left of the equations
        lp_expr_type leftExpr, rightExpr;
        IntType totalAsymResources = 0;
        for (IndexType asymIdx = 0; asymIdx <  asymPinIdx.size(); ++asymIdx)
        {
            IndexType pinIdx = asymPinIdx.at(asymIdx);
            IntType resource = asymPinResources.at(asymIdx);
            leftExpr += _aSymAssignVars.at(pinIdx) * resource;
            rightExpr += - _aSymAssignVars.at(pinIdx) * resource;
            totalAsymResources += resource;
        }
        // Left constraint
        // sum xi * ki <= reource + extra resource
        lp_trait::addConstr(_solver,
                leftExpr - _extraResourcesVars.at(cellIdx) <= 
                baseAsymResource);
        // Right constraint
        // sum (1 - xi) * ki <= resource + extra resource
        lp_trait::addConstr(_solver,
                rightExpr - _extraResourcesVars.at(cellIdx) <=
                baseAsymResource - totalAsymResources);
    }
}

void IlpTopFloorplanProblem::addCrossConstr()
{
    for (IndexType netIdx = 0; netIdx < _problem._nets.size(); ++netIdx)
    {
        const auto &net = _problem._nets.at(netIdx);
        auto getFpPin = [&](IndexType idx)
        {
            IndexType pinIdx = net.pins.at(idx);
            const auto &fpPin = _problem._pinIdx.at(pinIdx);
            return fpPin;
        };
        auto minusPinAssignExpr = [&](const TopFloorplanProblem::PinIdx &fpPin, lp_expr_type &expr)
        {
            if (fpPin.pinType == TopFloorplanProblem::FpPinType::SYM_SCE)
            {
                expr +=  _symPinAssignVars.at(fpPin.idx) - 1;
            }
            else if (fpPin.pinType == TopFloorplanProblem::FpPinType::SYM_PRI)
            {
                expr += - _symPinAssignVars.at(fpPin.idx);
            }
            else if (fpPin.pinType == TopFloorplanProblem::FpPinType::ASYM)
            {
                expr += - _aSymAssignVars.at(fpPin.idx);
            }
        };
        auto plusPinAssignExpr = [&](const TopFloorplanProblem::PinIdx &fpPin, lp_expr_type &expr)
        {
            if (fpPin.pinType == TopFloorplanProblem::FpPinType::SYM_SCE)
            {
                expr += 1 - _symPinAssignVars.at(fpPin.idx);
            }
            else if (fpPin.pinType == TopFloorplanProblem::FpPinType::SYM_PRI)
            {
                expr += _symPinAssignVars.at(fpPin.idx);
            }
            else if (fpPin.pinType == TopFloorplanProblem::FpPinType::ASYM)
            {
                expr += _aSymAssignVars.at(fpPin.idx);
            }
        };
        for (IndexType firstPinIdx = 0; firstPinIdx < net.pins.size(); ++firstPinIdx)
        {
            const auto &firstFpPin = getFpPin(firstPinIdx);
            lp_expr_type firstPinExpr;
            for (IndexType secondPinIdx = firstPinIdx + 1; secondPinIdx < net.pins.size(); ++secondPinIdx)
            {
                const auto &secondFpPin = getFpPin(secondPinIdx);
                const auto &crossVar = crossVariable(netIdx, firstPinIdx, secondPinIdx);
                lp_expr_type expr1, expr2, expr3, expr4;
                // expr1: c_i <=  x_i + x_j
                expr1 += crossVar;
                minusPinAssignExpr(firstFpPin, expr1);
                minusPinAssignExpr(secondFpPin, expr1);
                lp_trait::addConstr(_solver, expr1 <= 0);
                // expr2 : c_i >= x_i - x_j
                expr2 += crossVar;
                minusPinAssignExpr(firstFpPin, expr2);
                plusPinAssignExpr(secondFpPin, expr2);
                lp_trait::addConstr(_solver, expr2 >= 0);
                // expr3 : c_i >= x_j - x_i
                expr3 +=  crossVar;
                plusPinAssignExpr(firstFpPin, expr3);
                minusPinAssignExpr(secondFpPin, expr3);
                lp_trait::addConstr(_solver, expr3 >= 0);
                // expr4 : c_i <= 2 - x_i - x_j
                expr4 += crossVar;
                plusPinAssignExpr(firstFpPin, expr4);
                plusPinAssignExpr(secondFpPin, expr4);
                lp_trait::addConstr(_solver, expr4 <= 2);
            }
        }
    }
}

void IlpTopFloorplanProblem::addBoundaryConstr()
{
    for (IndexType cellIdx = 0; cellIdx < _problem._cellBBox.size(); ++cellIdx)
    {
        // y_i + h_i <= yHi
        lp_trait::addConstr(_solver,
                _yLoVars.at(cellIdx) - _yHiVar
                <= _problem._cellBBox.at(cellIdx).yLen());
    }
}

void IlpTopFloorplanProblem::addConstr()
{
    // Add the nonoverlapping constraints between modules
    addYLoConstr();
    // Add the pin resources constraints
    addPinResrouceConstr();
    // Add cross constraints
    addCrossConstr();
    // Add boundary constraints
    addBoundaryConstr();
}

bool IlpTopFloorplanProblem::solve()
{
    // Generate the vertical constraint graph with sweep line algorithm
    verticalSweepLine();
    
    // Setup ILP problem
    addVariables();
    addConstr();
    Assert(false);
    return true;
}

PROJECT_NAMESPACE_END
