/**
 * @file ParserGuide.h
 * @brief Parsing in the guide for the routing
 * @author Keren Zhu
 * @date 02/11/2019
 */

#ifndef AROUTER_PARSER_GUIDE_H_
#define AROUTER_PARSER_GUIDE_H_

#include "global/global.h"
#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class PROJECT_NAMESPACE::ParserGuide
/// @brief Parser for the global routing guidance
class ParserGuide
{
    public:
        explicit ParserGuide(Database &db) : _db(db) {}
        /*------------------------------*/ 
        /* Parsing wrapper              */
        /*------------------------------*/ 
        /// @brief parse the guide from a configure file
        /// @return if successful
        bool parse(const std::string &configFile);
    private:
        /// @brief parse a routing guide file
        /// @return if successful
        bool parseGuideFile(const std::string &guideFile, IndexType netIdx);
        /// @brief init the RouteGuide in db
        void initRouteGuide(IndexType numX, IndexType numY, IntType range);
        /// @brief set the net name map
        void initNetNameMap();
    private:
        Database &_db;
        std::unordered_map<std::string, IndexType> _netNameMap;
};

PROJECT_NAMESPACE_END

#endif //AROUTER_PARSER_GUIDE_H_
