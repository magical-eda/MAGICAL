/**
 * @file ParseSimpleTech.h
 * @brief Parsing simple routing technology
 * @author Keren Zhu
 * @date 03/14/2019
 */

#ifndef AROUTER_PARSE_SIMPLE_TECH_H_
#define AROUTER_PARSE_SIMPLE_TECH_H_

#include "db/techDB/TechDatabase.h"
#include "db/macroDB/MacroDatabase.h"
#include "parser/lefdef/LefReaderComponents.h"

PROJECT_NAMESPACE_BEGIN

/// @class PROJECT_NAMESPACE::ParseSimpleTech
/// @brief parser for simple routing tech files
class ParseSimpleTech
{
    public:
        /// @brief constructor
        /// @param first: technology database reference for the routing flow
        /// @param second: macro databse reference for the routing flow
        explicit ParseSimpleTech(TechDatabase &techDB, MacroDatabase &macroDB) : _techDB(techDB), _macroDB(macroDB) {}
        /// @brief read a simple tech file
        /// @param the file name of the simple tech file
        /// @return whether the parsing is successful
        bool parse(const std::string &filename);
    private:
        /// @brief parse information for a masterslice layer
        /// @param the input file stream
        /// @return if parsing is successful
        bool parseMastersliceLayer(std::ifstream &inf);
        /// @brief parse information for a routing layer
        /// @param the input file stream
        /// @return if the parsing is successful
        bool parseRoutingLayer(std::ifstream &inf);
        /// @brief parse information for a cut layer
        /// @param the input file stream
        /// @return if the parsing is successful
        bool parseCutLayer(std::ifstream &inf);
        /// @brief parse information for a via macro
        /// @param the input file stream
        /// @return if the parsing is successful
        bool parseVia(std::ifstream &inf);
        /// @brief processing raw vias
        /// @return if the processing is successful
        bool processRawVias();
        /// @brief add one via to the macro db
        /// @param one unprocessed via
        void processOneVia(const LefRawFixedVia &rawVia);
    private:
        TechDatabase &_techDB; ///< Reference to the technology database
        MacroDatabase &_macroDB; ///< Reference to the macro databse
        std::vector<LefRawFixedVia> _rawFixedViaArray; ///< Read unprocessed via macros
};

PROJECT_NAMESPACE_END

#endif //AROUTER_PARSE_SIMPLE_TECH_H_
