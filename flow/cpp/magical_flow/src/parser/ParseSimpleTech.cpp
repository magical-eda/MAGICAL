#include "ParseSimpleTech.h"

PROJECT_NAMESPACE_BEGIN

bool ParseSimpleTech::read(const std::string &filename)
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
        _techDB.addNewLayer(gdsLayer, layerName);
    }
    return true;
}

bool ParseSimpleTech::parse(const std::string &filename)
{
    std::ifstream inf(filename.c_str());
    if (!inf.is_open()) 
    {
        ERR("Simple tech parser::%s: cannot open file: %s \n", __FUNCTION__ , filename.c_str());
        Assert(false);
        return false;
    }
    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::stringstream ss(lineStr);
        std::string token;
        ss >> token;
        if (token == "DBU")
        {
            RealType dbu = 1000;
            ss >> dbu;
            auto &units = _techDB.units();
            units.setDbu(static_cast<IntType>(dbu));
        }
        else if (token == "MANUFACTURINGGRID")
        {
            RealType mgrid = 0.0001;
            ss >> mgrid;
        }
        else if (token == "LAYER")
        {
            ss >> token;
            if (token == "MASTERSLICE")
            {
                if (!parseMastersliceLayer(inf))
                {
                    return false;
                }
            }
            else if (token == "ROUTING")
            {
                if (!parseRoutingLayer(inf))
                {
                    return false;
                }
            }
            else if (token == "CUT")
            {
                if (!parseCutLayer(inf))
                {
                    return false;
                }
            }
            else
            {
                ERR("Simple tech parse::%s: syntax error. Token %s\n", __FUNCTION__, token.c_str());
                return false;
            }
        }
        else if (token == "VIA")
        {
            if (!parseVia(inf))
            {
                return false;
            }
        }
        else
        {
            ERR("Simple tech parse::%s: syntax error. Token %s\n", __FUNCTION__, token.c_str());
            return false;
        }
    }
    if (!this->finish())
    {
        return false;
    }
    return true;
}

bool ParseSimpleTech::parseMastersliceLayer(std::ifstream &inf)
{
    std::string name;
    IntType techLayer = 0;

    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::stringstream ss(lineStr);
        std::string token;
        ss >> token;
        if (token == "ENDLAYER")
        {
            // Wrap up everything and return
            _techLayers.emplace_back(name, techLayer);
            return true;
        }
        else if (token == "NAME")
        {
            ss >> name;
        }
        else if (token == "TECHLAYER")
        {
            ss >> techLayer;
        }
        else
        {
            ERR("Simple tech parse::%s: syntax error. Token %s \n", __FUNCTION__, token.c_str());
            return false;
        }
    }
    ERR("Simple tech parse::%s: syntax error. No ENDLAYER? \n", __FUNCTION__);
    return false;
}

bool ParseSimpleTech::parseRoutingLayer(std::ifstream &inf)
{
    IntType techLayer = 0;
    std::string name = "";
    bool hasDirection = false;
    std::string direction = "";
    RealType spacing = 0.0;
    bool hasWidth = false;
    RealType width = 0.0;

    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::stringstream ss(lineStr);
        std::string token;
        RealType floatToken;
        ss >> token;
        if (token == "ENDLAYER")
        {
            _techLayers.emplace_back(name, techLayer);
            return true;
        }
        else if (token == "NAME")
        {
            ss >> token;
            name = token;
        }
        else if (token == "DIRECTION")
        {
            ss >> token;
            hasDirection = true;
            direction = token;
        }
        else if (token == "SPACING")
        {
            ss >> floatToken;
            spacing = floatToken;
        }
        else if (token == "WIDTH")
        {
            ss >> floatToken;
            hasWidth = true;
            width = floatToken;
        }
        else if (token == "TECHLAYER")
        {
            ss >> techLayer;
        }
        else
        {
            ERR("Simple tech parse::%s: syntax error. Token %s \n", __FUNCTION__, token.c_str());
            return false;
        }
    }
    ERR("Simple tech parse::%s: syntax error. No ENDLAYER? \n", __FUNCTION__);
    return false;
}

bool ParseSimpleTech::parseCutLayer(std::ifstream &inf)
{
    IntType techLayer = 0;
    std::string name = "";
    RealType spacing = 0.0;
    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::stringstream ss(lineStr);
        std::string token;
        RealType floatToken;
        ss >> token;
        if (token == "ENDLAYER")
        {
            _techLayers.emplace_back(name, techLayer);
            return true;
        }
        else if (token == "NAME")
        {
            ss >> token;
            name = token;
        }
        else if (token == "SPACING")
        {
            // Use one spacing to overwrite all the spacing requirements
            ss >> floatToken;
            spacing = floatToken;
        }
        else if (token == "TECHLAYER")
        {
            ss >> techLayer;
        }
        else
        {
            ERR("Simple tech parse::%s: syntax error. Token %s \n", __FUNCTION__, token.c_str());
            return false;
        }
    }
    ERR("Simple tech parse::%s: syntax error. No ENDLAYER? \n", __FUNCTION__);
    return false;
}

bool ParseSimpleTech::parseVia(std::ifstream &inf)
{
    return true;
}

bool ParseSimpleTech::finish()
{
    // Tech layers
    auto sortLayer = [&] (const TechLayer &lhs, const TechLayer &rhs)
    {
        return lhs.techLayer < rhs.techLayer;
    };
    std::sort(_techLayers.begin(), _techLayers.end(), sortLayer); // Sort by acesending layer id
    for (IndexType idx = 0; idx < _techLayers.size(); ++idx)
    {
        IndexType returnIdx = _techDB.addNewLayer(_techLayers.at(idx).techLayer, _techLayers.at(idx).name);
        Assert(returnIdx == idx);
    }
    return true;
}


PROJECT_NAMESPACE_END
