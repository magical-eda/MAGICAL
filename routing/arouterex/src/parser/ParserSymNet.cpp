#include "ParserSymNet.h"

PROJECT_NAMESPACE_BEGIN

bool ParserSymNet::read(const std::string &filename)
{
    std::ifstream inf(filename.c_str());
    if (!inf.is_open()) 
    {
        ERR("Symnet parser:%s: cannot open file: %s \n", __FUNCTION__ , filename.c_str());
        Assert(false);
        return false;
    }
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
            _pairs.emplace_back(std::make_pair(split.at(0), split.at(1)));
        }
        if (split.size() == 1)
        {
            _selfs.emplace_back(split.at(0));
        }
    }

    /// Process the pairs read from the files
    if (!processNamePair())
    {
        return false;
    }
    return true;
}

bool ParserSymNet::processNamePair()
{
    /// Build a look up table for net names
    std::unordered_map<std::string, IndexType> netNameMap;
    for (IndexType netIdx = 0; netIdx < _db.grDB().nets().size(); ++netIdx)
    {
        const auto &net = _db.grDB().nets().at(netIdx);
        netNameMap[net.name()] =  netIdx;
    }
    /// If the pairs match the record of nets name, add them as a symmetry net
    for (const auto &pair : _pairs)
    {
        if (netNameMap.find(pair.first) != netNameMap.end() && netNameMap.find(pair.second) != netNameMap.end())
        {
            IndexType netIdx1 = netNameMap[pair.first];
            IndexType netIdx2 = netNameMap[pair.second];
            IndexType symNetIdx = _db.numSymNets();
            _db.symNetArray().emplace_back(SymNet(netIdx1, netIdx2));
            _db.grDB().nets().at(netIdx1).setSymNetIdx(symNetIdx);
            _db.grDB().nets().at(netIdx2).setSymNetIdx(symNetIdx);
        }
        else
        {
            ERR("Symnet parser: symmetry net pair: %s %s are not inside the netlist! Ignored. \n", pair.first.c_str(), pair.second.c_str());
        }
    }

    // Self
    for (const auto &self : _selfs)
    {
        if (netNameMap.find(self) != netNameMap.end())
        {
            IndexType netIdx1 = netNameMap[self];
            IndexType symNetIdx = _db.numSelfSymNets();
            _db.selfSymNetArray().emplace_back(SelfSymNet(netIdx1));
            _db.grDB().nets().at(netIdx1).setSelfSymIdx(symNetIdx);
        }
        else
        {
            ERR("Symnet parser: self symmetry net: %s  are not inside the netlist! Ignored. \n", self.c_str());
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
