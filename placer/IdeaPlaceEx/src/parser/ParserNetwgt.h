/**
 * @file ParserNetwgt.h
 * @brief Parser for .netwgt file
 * @author Keren Zhu
 * @date 10/02/2019
 */

#ifndef IDEAPLACE_NETWGT_H_
#define IDEAPLACE_NETWGT_H_

#include "db/Database.h"
#include <unordered_map>

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::ParserNetwgt
/// @brief the parser for .netwgt files
class ParserNetwgt
{
    public:
        /// @brief constructor
        /// @param The reference to the placement database
        explicit ParserNetwgt(Database &db) : _db(db) {}
        /// @brief parsing in the file
        /// @param input file name
        /// @return if the parsing is successful
        bool read(const std::string &filename);
    private:
        Database &_db; ///< The reference to the placement database
};

inline bool ParserNetwgt::read(const std::string &filename)
{
    std::ifstream inf(filename.c_str());
    if (!inf.is_open()) 
    {
        ERR("ParserNetwgt::%s: cannot open file: %s \n", __FUNCTION__ , filename.c_str());
        Assert(false);
        return false;
    }
    // Make a map of the net names
    std::unordered_map<std::string, IndexType> netNameMap;
    for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
    {
        netNameMap[_db.net(netIdx).name()] = netIdx;
    }
    // Read in the file
    std::string line;
    while (std::getline(inf, line))
    {
        std::istringstream iss(line);
        std::string netName;
        iss >> netName;
        IntType netWgt;
        iss >> netWgt;
        auto mapIter = netNameMap.find(netName);
        if (mapIter == netNameMap.end())
        {
            ERR("ParserNetwgt::%s: Net %s is not found in the placement database \n", __FUNCTION__, netName.c_str());
            Assert(false);
            return false;
        }
        else
        {
            IndexType netIdx = mapIter->second;
            _db.net(netIdx).setWeight(netWgt);
        }
    }
    return true;
}

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_NETWGT_H_
