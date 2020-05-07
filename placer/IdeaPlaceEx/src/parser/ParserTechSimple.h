/**
 * @file ParserTechSimple.h
 * @brief Parser for Tech simple file
 * @author Keren Zhu
 * @date 10/04/2019
 */

#ifndef IDEAPLACE_PARSER_TECHSIMPLE_H_
#define IDEAPLACE_PARSER_TECHSIMPLE_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::PaserTechSimple
/// @brief Parsing in the Techfile.simple. This file is simply the layer name + GDS layer
class ParserTechSimple
{
    public:
        /// @brief constructor
        /// @param Reference to the placement engine database
        explicit ParserTechSimple(Database &db) : _db(db) {}
        /// @brief parsing in the file
        /// @param input file name
        /// @return if the parsing is successful
        bool read(const std::string &filename);
    private:
        Database &_db; ///< The placement engine database
};

inline bool ParserTechSimple::read(const std::string &filename)
{
    std::ifstream inf(filename.c_str());
    if (!inf.is_open()) 
    {
        ERR("ParserTechSimple::%s: cannot open file: %s \n", __FUNCTION__ , filename.c_str());
        Assert(false);
        return false;
    }
    // Read in the file
    std::string line;
    while (std::getline(inf, line))
    {
        // Split the line into words
        std::vector<std::string> words;
        std::istringstream iss(line);
        std::string layerName;
        IndexType gdsLayer = INDEX_TYPE_MAX;
        iss >> layerName;
        iss >> gdsLayer;
        // Add to the database
        _db.tech().addGdsLayer(gdsLayer);
    }
    _db.tech().initRuleDataStructure();
    return true;
}

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PARSER_TECHSIMPLE_H_
