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
        DBG("name %s refName %s \n", cktNode.name().c_str(), cktNode.refName().c_str());
        std::string cktSymnetName = symnetFileDir + "/" + cktNode.refName() + ".symnet";
        parseSymnetFile(cktSymnetName, symnetNames[cktNode.refName()]);
        for (const auto& symnet : symnetNames[cktNode.refName()])
        {
            DBG("read %s %s \n", symnet.left.c_str(), symnet.right.c_str());
        }
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
        }
    }
    Assert(_numSymPriPins == _numSymSecPins);
    DBG("three number %d %d %d \n", _numAsymPins, _numSymPriPins, _numSymSecPins);
    Assert(false);
}

PROJECT_NAMESPACE_END
