/**
 * @file ParseSimpleTech.h
 * @brief Parsing simple simple tech file
 * @author Keren Zhu
 * @date 07/05/2019
 */

#ifndef MAGICAL_FLOW_SIMPLE_TECH_H_
#define MAGICAL_FLOW_SIMPLE_TECH_H_

#include "global/global.h"
#include "db/TechDB.h"

PROJECT_NAMESPACE_BEGIN

struct TechLayer
{
    TechLayer() = default;
    TechLayer(const std::string &name_, IndexType techLayer_) : name(name_), techLayer(techLayer_) {}
    std::string name;
    IndexType techLayer;
};

/// @class PROJECT_NAMESPACE::ParseSimpleTech
/// @brief parser for simple tech files
class ParseSimpleTech
{
    public:
        /// @brief constructor
        /// @param first: technology database reference for the routing flow
        /// @param second: macro databse reference for the routing flow
        explicit ParseSimpleTech(TechDB &techDB) : _techDB(techDB) {}
        /// @brief read a simple tech file
        /// @param the file name of the simple tech file
        /// @return whether the parsing is successful
        bool parse(const std::string &filename);
        /// @brief read a simple tech file, no idea what parse reads
        /// @param the file name of the simple tech file
        /// @return whether the parsing is successful
        bool read(const std::string &filename);
    private:
        /// @brief finish up the parsing
        bool finish();
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
    private:
        TechDB &_techDB; ///< Reference to the technology database
        std::vector<TechLayer> _techLayers; ///< For recording the techlayer IDs
};

PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_SIMPLE_TECH_H_
