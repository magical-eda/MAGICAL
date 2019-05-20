#include "TechfileReader.h"

PROJECT_NAMESPACE_BEGIN

namespace TechfileReader 
{

bool TechfileReader::read(const std::string &fileName)
{
    
    std::ifstream inf(fileName.c_str());
    if (!inf.is_open()) 
    {
        ERR("%s: cannot open file: %s \n", __FUNCTION__, fileName.c_str());
        return false;
    }
    
    std::string lineStr;
    
    /// Get line until reach the part we need
    while(getline(inf, lineStr))
    {
        if (lineStr == "techLayers(")
        {
            /// techLayers and their ids
            if (!processTechLayers(inf))
            {
                return false;
            }
        }
    }

    return true;
}


bool TechfileReader::processTechLayers(std::ifstream &inf)
{
    std::string lineStr;
    while(getline(inf, lineStr))
    {
        /// end of techLayers
        if (lineStr == ")")
        {
            return true;
        }

        /// jump empty line
        if (lineStr == "")
        {
            continue;
        }

        std::istringstream ss(lineStr);
        std::string token;
        ss >> token;
        /// commented
        if (token.at(0) == ';')
        {
            continue;
        }
#ifdef DEBUG_INPUT
        Assert(token == "(");
#endif
        std::string techLayerStr = "";
        ss >> techLayerStr;
        IntType layerId = 0;
        ss >> layerId;
        std::string techLayerAbbv = "";
        ss >> techLayerAbbv;
#ifdef DEBUG_INPUT
        DBG("%s: read techLayer %s %d %s \n", __FUNCTION__, techLayerStr.c_str(), layerId, techLayerAbbv.c_str());
        ss >> token;
        Assert(token == ")");
#endif
        _techDB.mapLayerStr2LayerMaskId()[techLayerStr] = layerId; 
    }
    return true;
}

bool readTechfile(const std::string &fileName, TechDatabase &techDB)
{
    TechfileReader reader = TechfileReader(techDB);
    return reader.read(fileName);
}

} /// namespace TechfileReader
PROJECT_NAMESPACE_END
