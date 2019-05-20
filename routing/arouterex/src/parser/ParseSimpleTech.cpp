#include "ParseSimpleTech.h"

PROJECT_NAMESPACE_BEGIN

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
            _techDB.units() = LefUnit();
            auto &units = _techDB.units();
            units.databaseNumber = dbu;
            
            units.hasDatabase = true;
            units.datarbaseName = "db";
        }
        else if (token == "MANUFACTURINGGRID")
        {
            RealType mgrid = 0.0001;
            ss >> mgrid;
            _techDB.setManufacturingGrid(mgrid);
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
    // Convert raw VIAs to macro DB standard
    if (!processRawVias())
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
            LefLayerMasterslice masterslice = LefLayerMasterslice();
            IndexType layerIdx = _techDB.mastersliceLayers().size();
            masterslice.name = name;
            _techDB.mastersliceLayers().emplace_back(masterslice);
            _techDB.mapLayerStr2Idx()[name] = _techDB.layerTypeVec().size();
            _techDB.mapLayerStr2AccessIdx()[name] = _techDB.accessLayerTypeVec().size();
            _techDB.layerTypeVec().emplace_back(std::make_pair(2, layerIdx));
            _techDB.accessLayerTypeVec().emplace_back(std::make_pair(2, layerIdx));
            _techDB.mapLayerStr2LayerMaskId()[name] = techLayer; 
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
    LefLayerRouting route = LefLayerRouting();
    IntType techLayer = 0;

    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::stringstream ss(lineStr);
        std::string token;
        RealType floatToken;
        ss >> token;
        if (token == "ENDLAYER")
        {
            route.pitch = route.spacingTableSpacing.at(0, 0) + route.width;
            IndexType layerIdx = _techDB.routingLayers().size();
            _techDB.routingLayers().emplace_back(route);

            _techDB.mapLayerStr2Idx()[route.name] = _techDB.layerTypeVec().size();
            _techDB.mapLayerStr2AccessIdx()[route.name] = _techDB.accessLayerTypeVec().size();
            _techDB.layerTypeVec().emplace_back(std::make_pair(1, layerIdx));
            _techDB.accessLayerTypeVec().emplace_back(std::make_pair(1, layerIdx));
            _techDB.mapLayerStr2LayerMaskId()[route.name] = techLayer; 
            return true;
        }
        else if (token == "NAME")
        {
            ss >> token;
            route.name = token;
        }
        else if (token == "DIRECTION")
        {
            ss >> token;
            route.hasDirection = true;
            route.direction = token;
        }
        else if (token == "SPACING")
        {
            ss >> floatToken;
            // Use one entry spacing table to store this
            route.numSpacingTable = 1;
            route.spacingTableWidth.resize(1, 0);
            route.spacingTableParallelRunLength.resize(1, 0);
            route.spacingTableSpacing.resize(1, 1);
            route.spacingTableSpacing.at(0, 0) = floatToken;
        }
        else if (token == "WIDTH")
        {
            ss >> floatToken;
            route.hasWidth = true;
            route.width = floatToken;
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
    LefLayerCut cut = LefLayerCut();
    IntType techLayer = 0;
    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::stringstream ss(lineStr);
        std::string token;
        RealType floatToken;
        ss >> token;
        if (token == "ENDLAYER")
        {
            IndexType layerIdx = _techDB.cutLayers().size();
            _techDB.cutLayers().emplace_back(cut);
            _techDB.mapLayerStr2Idx()[cut.name] = _techDB.layerTypeVec().size();
            _techDB.layerTypeVec().emplace_back(std::make_pair(0, layerIdx));
            _techDB.mapLayerStr2LayerMaskId()[cut.name] = techLayer; 
            return true;
        }
        else if (token == "NAME")
        {
            ss >> token;
            cut.name = token;
        }
        else if (token == "SPACING")
        {
            // Use one spacing to overwrite all the spacing requirements
            ss >> floatToken;
            cut.spacingParallelOverLap = floatToken;
            cut.spacing = floatToken;
            cut.spacingSameNet = floatToken;
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
    LefRawFixedVia via;
    std::string token;
    RealType xLo, yLo, xHi, yHi;
    std::string lineStr;
    std::getline(inf, lineStr);
    std::stringstream ss(lineStr);
    ss >> token;
    if (token != "NAME")
    {
        ERR("Simple tech parse::%s: syntax error. Token %s \n", __FUNCTION__, token.c_str());
        return false;
    }
    ss >>  token;
    via.name = token;

    std::getline(inf, lineStr);
    ss = std::stringstream(lineStr);
    ss >> token;
    via.bottomLayer = token;
    ss >> xLo; ss >> yLo; ss >> xHi; ss >> yHi;
    via.bottomLayerRect = Box<RealType>(xLo, yLo, xHi, yHi);

    std::getline(inf, lineStr);
    ss = std::stringstream(lineStr);
    ss >> token;
    via.cutLayer = token;
    via.numCutRect = 1; // Only have one cut 
    via.cutLayerRectArray.resize(1);
    ss >> xLo; ss >> yLo; ss >> xHi; ss >> yHi;
    via.cutLayerRectArray.at(0) = Box<RealType>(xLo, yLo, xHi, yHi);

    std::getline(inf, lineStr);
    ss = std::stringstream(lineStr);
    ss >> token;
    via.topLayer = token;
    ss >> xLo; ss >> yLo; ss >> xHi; ss >> yHi;
    via.topLayerRect = Box<RealType>(xLo, yLo, xHi, yHi);

    std::getline(inf, lineStr);
    ss = std::stringstream(lineStr);
    ss >> token;
    if (token == "ENDVIA")
    {
        _rawFixedViaArray.emplace_back(via);
        return true;
    }

    ERR("Simple tech parse::%s: syntax error. No ENDVIA? \n", __FUNCTION__);
    return false;
}

bool ParseSimpleTech::processRawVias()
{
    // Fixed VIA
    _macroDB.fixedViaVec2D().resize(_techDB.accessLayerTypeVec().size());
    for (const auto &rawVia : _rawFixedViaArray)
    {
        processOneVia(rawVia);
    }
    // Sort Vias to put the default vias in the end
    auto func = [&](LefFixedVia via1, LefFixedVia via2)
    {
        if (via1.isDefault())
            return true;
        else if (via2.isDefault())
            return false;
        else
            return true;
    };
    for (auto &fixedViaVec : _macroDB.fixedViaVec2D())
    {
        std::stable_sort(fixedViaVec.begin(), fixedViaVec.end(), func);
        for (auto &fixedVia : fixedViaVec)
        {
            fixedVia.offSetShape();
            fixedVia.findBoundingBox();
        }
    }
    return true;
}

void ParseSimpleTech::processOneVia(const LefRawFixedVia &rawVia)
{
    LefFixedVia via;
    via.setName(rawVia.name);
    via.setIsDefault(true);

    // Get the correct index based on name
    via.setAccessLayerIdx(_techDB.mapLayerStr2AccessIdx()[rawVia.bottomLayer]);
    via.setBottomLayerIdx(_techDB.mapLayerStr2Idx()[rawVia.bottomLayer]);
    via.setCutLayerIdx(_techDB.mapLayerStr2Idx()[rawVia.cutLayer]);
    via.setTopLayerIdx(_techDB.mapLayerStr2Idx()[rawVia.topLayer]);

    // Convert rect unit from um to databse unit
    LocType xLo;
    LocType yLo;
    LocType xHi;
    LocType yHi;

    // Bottom
    xLo = static_cast<LocType>(rawVia.bottomLayerRect.xLo() * _techDB.units().databaseNumber);
    yLo = static_cast<LocType>(rawVia.bottomLayerRect.yLo() * _techDB.units().databaseNumber);
    xHi = static_cast<LocType>(rawVia.bottomLayerRect.xHi() * _techDB.units().databaseNumber);
    yHi = static_cast<LocType>(rawVia.bottomLayerRect.yHi() * _techDB.units().databaseNumber);
    via.bottomRect() = Box<LocType>(xLo, yLo, xHi, yHi);

    // Cut
    via.setNumCutRect(rawVia.numCutRect);
    via.cutRectArray().resize(0);
    via.cutRectArray().reserve(rawVia.numCutRect);
    for (IndexType cutIdx = 0; cutIdx < via.numCutRect(); ++cutIdx)
    {
        xLo = static_cast<LocType>(rawVia.cutLayerRectArray.at(cutIdx).xLo() * _techDB.units().databaseNumber);
        yLo = static_cast<LocType>(rawVia.cutLayerRectArray.at(cutIdx).yLo() * _techDB.units().databaseNumber);
        xHi = static_cast<LocType>(rawVia.cutLayerRectArray.at(cutIdx).xHi() * _techDB.units().databaseNumber);
        yHi = static_cast<LocType>(rawVia.cutLayerRectArray.at(cutIdx).yHi() * _techDB.units().databaseNumber);
        via.cutRectArray().emplace_back(Box<LocType>(xLo, yLo, xHi, yHi));
    }

    // Top
    xLo = static_cast<LocType>(rawVia.topLayerRect.xLo() * _techDB.units().databaseNumber);
    yLo = static_cast<LocType>(rawVia.topLayerRect.yLo() * _techDB.units().databaseNumber);
    xHi = static_cast<LocType>(rawVia.topLayerRect.xHi() * _techDB.units().databaseNumber);
    yHi = static_cast<LocType>(rawVia.topLayerRect.yHi() * _techDB.units().databaseNumber);
    via.topRect() = Box<LocType>(xLo, yLo, xHi, yHi);

    // push_back to the vector
    _macroDB.fixedViaVec2D().at(via.accessLayerIdx()).emplace_back(via);
}

PROJECT_NAMESPACE_END
