/**
 * @file TranslateGdsLayer.hpp
 * @brief Translate the layer ID/ datatype to another configuration
 * @author Keren Zhu
 * @date 01/21/2022
 */

#pragma once

#include <map>
#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

class TranslateGdsLayer 
{
    public:
        explicit TranslateGdsLayer() {}
        bool translate(const std::string &layerMappingFile, const std::string &inputGds, const std::string &outputGds)
        {
          if (not parseMappingFile(layerMappingFile)) { return false; };
          if (not translateGds(inputGds, outputGds)) { return false; };
          return true;
        }
    private:
        bool parseMappingFile(const std::string layerMappingFile);
        bool translateGds(const std::string &inputGds, const std::string &outputGds);
        std::map<std::pair<int, int>, std::pair<int, int>> _mapping; // The read layer mapping
        ::GdsParser::GdsDB::GdsDB _gdsDB; ///< The database for the GDS
};


namespace TRANSLATEGDSLAYER
{
    /// @brief write the layout for circuit to GDSII
    /// @param first: circuit graph index
    /// @param second: output file name
    /// @param third: design database
    /// @param fourth: technology database
    inline bool translateGdsLayer(const std::string &layerMappingFile, const std::string &inputGds, const std::string &outputGds)
    {
        return TranslateGdsLayer().translate(layerMappingFile, inputGds, outputGds);
    }
}
PROJECT_NAMESPACE_END
