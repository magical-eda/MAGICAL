#include "LefReader.h"
#include <algorithm>

PROJECT_NAMESPACE_BEGIN


// Override functions

void LefReader::lef_version_cbk(const std::string &v) 
{
    _techDB.setVersion(v);
}

void LefReader::lef_version_cbk(double v) 
{

}

void LefReader::lef_dividerchar_cbk(const std::string &v) 
{
    _techDB.setDividChar(v);
}

void LefReader::lef_casesensitive_cbk(int v) 
{

}

void LefReader::lef_nowireextension_cbk(const std::string &v) 
{

}

void LefReader::lef_manufacturing_cbk(double v) 
{
    _techDB.setManufacturingGrid(v);
}

void LefReader::lef_useminspacing_cbk(lefiUseMinSpacing const &v) 
{

}

void LefReader::lef_clearancemeasure_cbk(const std::string &v) 
{
    _techDB.setClearanceMeasure(v);
}

void LefReader::lef_units_cbk(lefiUnits const &v) 
{
    _techDB.units() = LefUnit();
    auto &units = _techDB.units();
    if (v.hasDatabase())
    {
        units.hasDatabase = true;
        units.datarbaseName = v.databaseName();
        units.databaseNumber = v.databaseNumber();
        //units.databaseNumber = 200;
        //WRN("%s: Hard code database number to 200 \n", __FUNCTION__);
    }
    if (v.hasCapacitance())
    {
        units.hasCapacitance = true;
        units.capacitance = v.capacitance();
    }
    if (v.hasResistance())
    {
        units.hasResistance = true;
        units.resistance = v.resistance();
    }
    if (v.hasTime())
    {
        units.hasTime = true;
        units.time = v.time();
    }
    if (v.hasPower())
    {
        units.hasPower = true;
        units.power = v.power();
    }
    if (v.hasCurrent())
    {
        units.hasCurrent = true;
        units.current = v.current();
    }
    if (v.hasVoltage())
    {
        units.hasVoltage = true;
        units.voltage = v.voltage();
    }
    if (v.hasFrequency())
    {
        units.hasFrequency = true;
        units.frequency = v.frequency();
    }
}

void LefReader::lef_busbitchars_cbk(const std::string &v) 
{
    _techDB.setBusBitChars(v);
}

void LefReader::lef_layer_cbk(lefiLayer &v) 
{
    if (v.hasType())
    {
        if (std::string(v.type()) == "CUT")
        {
            IndexType cutIdx = parseCutLayer(v);
            _techDB.mapLayerStr2Idx()[v.name()] = _techDB.layerTypeVec().size();
            _techDB.layerTypeVec().emplace_back(std::make_pair(0, cutIdx));
            return;
        }
        if (std::string(v.type()) == "ROUTING")
        {
            IndexType routingIdx = parseRoutingLayer(v);
            _techDB.mapLayerStr2Idx()[v.name()] = _techDB.layerTypeVec().size();
            _techDB.mapLayerStr2AccessIdx()[v.name()] = _techDB.accessLayerTypeVec().size();
            _techDB.layerTypeVec().emplace_back(std::make_pair(1, routingIdx));
            //DBG("%s: access %d %s \n", __FUNCTION__, _techDB.accessLayerTypeVec().size(), v.name());
            _techDB.accessLayerTypeVec().emplace_back(std::make_pair(1, routingIdx));
            return;
        }
        if (std::string(v.type()) == "MASTERSLICE")
        {
            IndexType masterIdx = parseMastersliceLayer(v);
            _techDB.mapLayerStr2Idx()[v.name()] = _techDB.layerTypeVec().size();
            _techDB.mapLayerStr2AccessIdx()[v.name()] = _techDB.accessLayerTypeVec().size();
            _techDB.layerTypeVec().emplace_back(std::make_pair(2, masterIdx));
            //DBG("%s: access %d %s \n", __FUNCTION__, _techDB.accessLayerTypeVec().size(), v.name());
            _techDB.accessLayerTypeVec().emplace_back(std::make_pair(2, masterIdx));
            return;
        }
        if (std::string(v.type()) == "OVERLAP")
        {

            _techDB.mapLayerStr2Idx()[v.name()] = _techDB.layerTypeVec().size();
            _techDB.layerTypeVec().emplace_back(std::make_pair(3, 0));
            INF("%s: ignore OVERLAP layer %s. Not influence routing\n", __FUNCTION__, v.name());
            return;
        }
        if (std::string(v.type()) == "IMPLANT")
        {
            INF("%s: ignore IMPLANT layer %s. Not influence routing\n", __FUNCTION__, v.name());
            return;
        }
        AssertMsg(false, "%s: unknown layer type %s \n", __FUNCTION__, v.type());
    }
    else
    {
        AssertMsg(false, "%s: layer does not have type!\n", __FUNCTION__);
    }
}

void LefReader::lef_maxstackvia_cbk(lefiMaxStackVia const &v) 
{

}

void LefReader::lef_via_cbk(lefiVia const &v) 
{
    if (v.hasGenerated())
    {
    }
    else
    {
        parseFixedVia(v);
    }
}

void LefReader::lef_viarule_cbk(lefiViaRule const &v) 
{

}

void LefReader::lef_spacing_cbk(lefiSpacing const &v) 
{
    // NDR?
}

void LefReader::lef_irdrop_cbk(lefiIRDrop const &v) 
{

}

void LefReader::lef_minfeature_cbk(lefiMinFeature const &v) 
{

}

void LefReader::lef_dielectric_cbk(double v) 
{

}

void LefReader::lef_nondefault_cbk(lefiNonDefault const &v) 
{

}


void LefReader::lef_site_cbk(lefiSite const &v)
{
    //TODO:
}

void LefReader::lef_macrobegin_cbk(std::string const &v) 
{
    //TODO
}

void LefReader::lef_macro_cbk(lefiMacro const &v) 
{
    // TODO
}

void LefReader::lef_pin_cbk(lefiPin const &v) 
{
    //TODO:
}

void LefReader::lef_obstruction_cbk(lefiObstruction const &v) 
{
    //TODO:
}

void LefReader::lef_density_cbk(lefiDensity const &v) 
{
}

void LefReader::lef_timing_cbk(lefiTiming const &v) 
{
}

void LefReader::lef_array_cbk(lefiArray const &v) 
{
}

void LefReader::lef_prop_cbk(lefiProp const &v) 
{
    // TODO:
}

void LefReader::lef_noisemargin_cbk(lefiNoiseMargin const &v) 
{
}

void LefReader::lef_edgeratethreshold1_cbk(double v) 
{
}

void LefReader::lef_edgeratethreshold2_cbk(double v) 
{
}

void LefReader::lef_edgeratescalefactor_cbk(double v) 
{
}

void LefReader::lef_noisetable_cbk(lefiNoiseTable const &v) 
{
}

void LefReader::lef_correctiontable_cbk(lefiCorrectionTable const &v) 
{
}

void LefReader::lef_inputantenna_cbk(double v) 
{
}

void LefReader::lef_outputantenna_cbk(double v) 
{
}

void LefReader::lef_inoutantenna_cbk(double v) 
{
}

void LefReader::lef_antennainput_cbk(double v) 
{
}

void LefReader::lef_antennaoutput_cbk(double v) 
{
}

void LefReader::lef_antennainout_cbk(double v) 
{
}

void LefReader::lef_extension_cbk(const std::string &v) 
{
}

void LefReader::lef_macro_endcbk(const char *macroName) 
{
    /// TODO
}

// Local defined functions
IndexType LefReader::parseCutLayer(lefiLayer const &v) 
{
    LefLayerCut cut = LefLayerCut();
    cut.name = std::string(v.name());
    cut.width = v.width();
    IntType numSpacing = v.numSpacing();
    for (IntType spacingIdx = 0; spacingIdx < numSpacing; ++ spacingIdx)
    {
        if (v.hasSpacingAdjacent(spacingIdx))
        {
            /// TODO: spacing adjacent
        }
        else if (v.hasSpacingParallelOverlap(spacingIdx))
        {
            cut.spacingParallelOverLap = v.spacing(spacingIdx);
        }
        else if (v.hasSpacingSamenet(spacingIdx))
        {
            cut.spacingSameNet = v.spacing(spacingIdx);
        }
        else
        {
            // Default spacing
            cut.spacing = v.spacing(spacingIdx);
        }
    }
    // Same net
    // Return index of this in the cut layers 
    IndexType cutIdx = _techDB.cutLayers().size();
    _techDB.cutLayers().emplace_back(cut);
    return cutIdx;
}

IndexType LefReader::parseRoutingLayer(lefiLayer &v)
{
    LefLayerRouting route = LefLayerRouting();

    route.name = std::string(v.name());
    if (v.hasPitch())
    {
        route.hasPitch = true;
        route.pitch = v.pitch();
    }
    if (v.hasXYPitch())
    {
        route.hasXYPitch = true;
        route.pitchX = v.pitchX();
        route.pitchY = v.pitchY();
    }
    if (v.hasOffset())
    {
        route.hasOffset = true;
        route.offset = v.offset();
    }
    if (v.hasXYOffset())
    {
        route.hasXYOffset = true;
        route.offsetX = v.offsetX();
        route.offsetY = v.offsetY();
    }
    if (v.hasWidth())
    {
        route.hasWidth = true;
        route.width = v.width();
    }
    if (v.hasArea())
    {
        route.hasArea = true;
        route.area = v.area();
    }
    if (v.hasSpacingNumber())
    {
        route.hasSpacingNumber = true;
        route.numSpacing = v.numSpacing();
    }
    // Process spacing information in other subroutine
    for (IntType idx = 0; idx < route.numSpacing; ++idx)
    {
        processRoutingLayerSpacing(route, v, idx);
    }

    if (v.hasDirection())
    {
        route.hasDirection = true;
        route.direction = std::string(v.direction());
    }
    if (v.hasMinwidth())
    {
        route.hasMinWidth = true;
        route.minWidth = v.minwidth();
    }
    if (v.hasMaxwidth())
    {
        route.hasMaxWidth = true;
        route.maxWidth = v.maxwidth();
    }

    //spacing table
    route.numSpacingTable = v.numSpacingTable();
    
    for (IntType idx = 0; idx < route.numSpacingTable; ++idx)
    {
        const auto &spTable = v.spacingTable(idx);
        if (spTable->isParallel())
        {
            processRoutingLayerSpacingTableParallel(route, spTable);
        }
        else if (spTable->isInfluence())
        {
            AssertMsg(false, "%s: expect no influence \n", __FUNCTION__);
        }
        // Spacingtable can only be influence, parallel, and twoWidth
        else 
        {
            for (IntType twoWidthIdx = 0; twoWidthIdx < spTable->twoWidths()->numWidth(); ++twoWidthIdx)
            {
                DBG("%s: layer %s twoWidth %d/%d: %d\n", __FUNCTION__, v.name(), twoWidthIdx, spTable->twoWidths()->numWidth(), spTable->twoWidths()->width(twoWidthIdx));
            }
        }
        // twoWidth
    }
    
    //Return the index of the input in the routing layers
    IndexType routingIdx = _techDB.routingLayers().size();
    _techDB.routingLayers().emplace_back(route);
    return routingIdx;
}

void LefReader::processRoutingLayerSpacing(LefLayerRouting &route, lefiLayer const &v, IntType spacingIdx)
{
    if (v.hasSpacingName(spacingIdx))
    {
    }
    else if (v.hasSpacingLayerStack(spacingIdx))
    {
    }
    else if (v.hasSpacingAdjacent(spacingIdx))
    {
    }
    else if (v.hasSpacingCenterToCenter(spacingIdx))
    {
    }
    // add others later... if not forever...
    else if (v.hasSpacingEndOfanoynmouse(spacingIdx))
    {
        route.eolSpacing.emplace_back(EOLSpacing(v.spacing(spacingIdx), v.spacingEolWidth(spacingIdx), v.spacingEolWithin(spacingIdx)));
    }
    // others...
    else 
    {
        // raw spacing...
        route.spacing.emplace_back(v.spacing(spacingIdx));
    }
}

void LefReader::processRoutingLayerSpacingTableParallel(LefLayerRouting &route, lefiSpacingTable const *v)
{
    const lefiParallel *para = v->parallel();
    for (IntType lenIdx = 0; lenIdx < para->numLength(); ++lenIdx)
    {
        route.spacingTableParallelRunLength.emplace_back(para->length(lenIdx));
    }
    for (IntType widthIdx = 0; widthIdx < para->numWidth(); ++widthIdx)
    {
        route.spacingTableWidth.emplace_back(para->width(widthIdx));
    }
    route.spacingTableSpacing.resize(para->numLength(), para->numWidth());
    for (IntType lenIdx = 0; lenIdx < para->numLength(); ++lenIdx)
    {
        for (IntType widthIdx = 0; widthIdx < para->numWidth(); ++widthIdx)
        {
            route.spacingTableSpacing.at(lenIdx, widthIdx) = para->widthSpacing(widthIdx, lenIdx);
        }
    }
}

IndexType LefReader::parseMastersliceLayer(const lefiLayer &v)
{
    LefLayerMasterslice masterslice = LefLayerMasterslice();
    
    masterslice.name = v.name();
    IndexType masterIdx = _techDB.mastersliceLayers().size();
    _techDB.mastersliceLayers().emplace_back(masterslice);
    return masterIdx;
}

//Via

void LefReader::parseFixedVia(const lefiVia &v)
{
    LefRawFixedVia via;
    via.name = v.name();
    if (v.hasDefault())
    {
        via.isDefault = true;
    }
    if (v.hasResistance())
    {
        via.hasResistance = true;
        via.resistance = v.resistance();
    }

    // bottom, cut, top
    AssertMsg(v.numLayers() == 3, "%s: VIA %s does not exactly have three layers, which is the default assumption. Instead, having %d \n", __FUNCTION__, v.name(), v.numLayers());

    RealType xLo = 0;
    RealType yLo = 0;
    RealType xHi = 0;
    RealType yHi = 0;

    // Bottom
    via.bottomLayer = v.layerName(0);
    AssertMsg(v.numRects(0) == 1, "%s: VIA %s bottom layer does not have exactly one rectangle! \n", __FUNCTION__, v.name());
    AssertMsg(v.numPolygons(0) == 0, "%s, VIA %s bottom layer has polygons! \n", __FUNCTION__, v.name());
    xLo = v.xl(0, 0);
    yLo = v.yl(0, 0);
    xHi = v.xh(0, 0);
    yHi = v.yh(0, 0);
    via.bottomLayerRect = Box<RealType>(xLo, yLo, xHi, yHi);

    // CUT
    via.cutLayer = v.layerName(1);
    AssertMsg(v.numRects(1) > 0, "%s: VIA %s cut layer does not have rectangle! \n", __FUNCTION__, v.name());
    AssertMsg(v.numPolygons(1) == 0, "%s, VIA %s cut layer has polygons! \n", __FUNCTION__, v.name());
    IntType numCut = v.numRects(1);
    via.numCutRect = static_cast<IndexType>(numCut);
    via.cutLayerRectArray.resize(0);
    via.cutLayerRectArray.reserve(numCut);
    for (IntType cutRectIdx = 0; cutRectIdx < numCut; ++cutRectIdx)
    {
        xLo = v.xl(1, cutRectIdx);
        yLo = v.yl(1, cutRectIdx);
        xHi = v.xh(1, cutRectIdx);
        yHi = v.yh(1, cutRectIdx);
        via.cutLayerRectArray.emplace_back(Box<RealType>(xLo, yLo, xHi, yHi));
    }
    
    // top
    via.topLayer = v.layerName(2);
    AssertMsg(v.numRects(2) == 1, "%s: VIA %s top layer does not have exactly one rectangle! \n", __FUNCTION__, v.name());
    AssertMsg(v.numPolygons(2) == 0, "%s, VIA %s top layer has polygons! \n", __FUNCTION__, v.name());
    xLo = v.xl(2, 0);
    yLo = v.yl(2, 0);
    xHi = v.xh(2, 0);
    yHi = v.yh(2, 0);
    via.topLayerRect = Box<RealType>(xLo, yLo, xHi, yHi);

    _rawFixedViaArray.emplace_back(via);
}

// Post processing

// Wrapper function
void LefReader::postProcessing()
{
    // Fixed VIA
    _macroDB.fixedViaVec2D().resize(_techDB.accessLayerTypeVec().size());
    for (const auto &rawVia : _rawFixedViaArray)
    {
        postProcessFixedVia(rawVia);
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
}

void LefReader::postProcessFixedVia(const LefRawFixedVia &rawVia)
{
    LefFixedVia via;
    via.setName(rawVia.name);
    via.setIsDefault(rawVia.isDefault);
    via.setHasResistance(rawVia.hasResistance);
    via.setResistance(::klib::round2Int(RESISTANCE_PRECISION * rawVia.resistance));

    // Get the correct index based on name
    AssertMsg(_techDB.mapLayerStr2AccessIdx().find(rawVia.bottomLayer) != _techDB.mapLayerStr2AccessIdx().end(), "%s: via %s bottom %s top %s \n", __FUNCTION__, rawVia.name.c_str(), rawVia.bottomLayer.c_str(), rawVia.topLayer.c_str());
    AssertMsg(_techDB.mapLayerStr2AccessIdx().find(rawVia.topLayer) != _techDB.mapLayerStr2AccessIdx().end() || \
            _techDB.layerTypeVec()[_techDB.mapLayerStr2Idx()[rawVia.topLayer]].first == 3, "%s: via %s bottom %s top %s \n", __FUNCTION__, rawVia.name.c_str(), rawVia.bottomLayer.c_str(), rawVia.topLayer.c_str());
    AssertMsg(_techDB.mapLayerStr2AccessIdx()[rawVia.topLayer] - _techDB.mapLayerStr2AccessIdx()[rawVia.bottomLayer] == 1, "%s: via %s bottom %s top %s \n", __FUNCTION__, rawVia.name.c_str(), rawVia.bottomLayer.c_str(), rawVia.topLayer.c_str());
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

// Read function
namespace LefReaderNameSpace {
void readLef(std::string const &fileName, TechDatabase &techDB, MacroDatabase &macroDB)
{
    LefReader reader = LefReader(techDB, macroDB);
    LefParser::read(reader, fileName.c_str());

    reader.postProcessing();
}
}
PROJECT_NAMESPACE_END
