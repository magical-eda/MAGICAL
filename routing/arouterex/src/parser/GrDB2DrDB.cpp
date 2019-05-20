#include "GrDB2DrDB.h"

PROJECT_NAMESPACE_BEGIN

/*------------------------------*/ 
/* v2 to v2                     */
/*------------------------------*/ 

using V2V2 = GrDB_v2_to_DrDB_v2_converter;

bool V2V2::convert()
{
    // Init the pin location set to check the pin location overlapping
    _pinLocSet.clear();
    if (!convertDesign()) { return false; }
    if (!convertNets()) { return false; }
    if (GR_2D)
    {
        if (!convertRoutingPin2D()) { return false; }
    }
    else
    {
        if (!convertRoutingPin()) { return false; }
    }
    return true;
}


bool V2V2::convertDesign()
{
    /// NOTE: The design parameters, eg. min spacing and min width, specified in some format of benchmarks, are overrided by the LEF file
    IndexType layerSize = _grDB.gridMap().numLayers(); // include the P0 and metals
    //IndexType numGridX = _grDB.gridMap().numX();
    //IndexType numGridY = _grDB.gridMap().numY();
    LocType dieXLo = _grDB.dieLL().x();
    LocType dieYLo = _grDB.dieLL().y();
    LocType gridWidth = _grDB.gridMap().gridWidth();
    LocType gridHeight = _grDB.gridMap().gridHeight();
    LocType dieXHi = _grDB.dieUR().x();
    LocType dieYHi = _grDB.dieUR().y();
    // Init the convertor of grid cell indices and LocType
    Box<LocType> dieBox(dieXLo, dieYLo, dieXHi, dieYHi);

    // Init rough grid
    _db.drDB().roughGrid() = RoughGrid(dieBox, gridWidth, gridHeight, layerSize);

    // Add the layers to the grid
    for (IndexType layer = 0; layer < layerSize; ++layer)
    {
        RouteDirType routeType;
        LocType minSpacing = -1;
        LocType minWidth = -1;
        auto &layerTypePair = _db.techDB().accessLayerTypeVec().at(layer);
        // layerTypePair.first : 1 ROUTING 2 MASTERSLICE
        Assert(layerTypePair.first == 1 || layerTypePair.first == 2);
        if (layerTypePair.first == 2)
        {
            // MASTERSLICE
            routeType = RouteDirType::FORBID;
            // Use the M1 routing layer to set the default minSpacing and minWidth
            minSpacing = _db.techDB().micro2DBU(_db.techDB().routingLayers().at(0).spacingTableSpacing.at(0, 0));
            minWidth = _db.techDB().micro2DBU(_db.techDB().routingLayers().at(0).width);
        }
        else 
        {
            // Routing layer
            auto &routingLayer = _db.techDB().routingLayers().at(layerTypePair.second);
            minWidth = _db.techDB().micro2DBU(routingLayer.width);
            if (routingLayer.hasDirection)
            {
                if (routingLayer.direction == "HORIZONTAL") { routeType = RouteDirType::HORIZONTAL; }
                else if (routingLayer.direction == "VERTICAL") { routeType = RouteDirType::VERTICAL; }
                else 
                { 
                    ERR("%s: Unknown routing direction %s of layer %s \n", __FUNCTION__, routingLayer.direction.c_str(), routingLayer.name.c_str());
                    Assert(0); 
                    return false; 
                }
            }
            else
            {
                routeType = RouteDirType::BOTH;
            }
            if (routingLayer.numSpacingTable > 0)
            {
                minSpacing = _db.techDB().micro2DBU(routingLayer.spacingTableSpacing.at(0, 0));
            }
            else if (routingLayer.numSpacing > 0)
            {
                minSpacing = INT_TYPE_MAX;
                for (auto sp : routingLayer.spacing)
                {
                    if (minSpacing > _db.techDB().micro2DBU(sp))
                    {
                        minSpacing = _db.techDB().micro2DBU(sp);
                    }
                }
            }
            else
            {
                minSpacing = minWidth * 2;
                WRN("%s:min spacing not found \n", __FUNCTION__);
            }
        } // end of routing layer
#ifdef DEBUG_GR2DR
        DBG("%s: Add layer %d min spacing %d min width %d \n", __FUNCTION__, layer,  minSpacing, minWidth);
#endif
        // Add to the rough grid
        _db.drDB().roughGrid().addRouteDir(routeType);
        _db.drDB().roughGrid().addPitch(minSpacing + minWidth);
        //_db.drDB().roughGrid().addTrackOffset(minSpacing + minWidth);
        if (routeType == RouteDirType::HORIZONTAL)
        {
            _db.drDB().roughGrid().addTrackOffset(dieYLo + (minSpacing + minWidth) / 2);
        }
        else
        {
            _db.drDB().roughGrid().addTrackOffset(dieXLo + (minSpacing + minWidth) / 2);
        }
    }
    return true;
}


bool V2V2::convertNets()
{
    // First reserve the net array
    _drDB.nets().clear();
    _drDB.nets().reserve(_grDB.nets().size());
    // Convert single net information
    for (IndexType netIdx = 0; netIdx < _grDB.nets().size(); ++netIdx)
    {
        if (!convertSingleNet(netIdx)) { return false; }
    }

    return true;
}

bool V2V2::convertSingleNet(IndexType netIdx)
{
#ifdef DEBUG_GR2DR
    DBG("%s: coverting net %d \n", __FUNCTION__, netIdx);
#endif
    auto & grNet = _grDB.nets().at(netIdx);
    NetDR drNet = NetDR();    
    // net name
    //drNet.setNetName(grNet.name());
    // net index in benchmark
    //drNet.setBenchIdx(grNet.benchNetIdx());
    // pins
    drNet.pins().clear();
    std::vector<IndexType> netPinIdxVec; //< the pin idx (in GR db) of this net. Used to track the pin idx for converting the MST 
    for (IndexType physicalPinIdx : grNet.physPinArray())
    {
        netPinIdxVec.emplace_back(physicalPinIdx);
    }
    // Terminal indices
    drNet.terminalArray() = grNet.terminalArray();
    // Add the pin to drNet and also check the overlapping
    for (IndexType pinIdx : netPinIdxVec)
    {
        auto &pin = _grDB.pin(pinIdx);
#if 0
        auto pinXYZ = XYZ<LocType, IndexType>(pin.loc(), pin.layer());
        auto pinIter = _pinLocSet.find(pinXYZ);
        if (pinIter != _pinLocSet.end())
        {
            ERR("%s: pins are overlapping! \n", __FUNCTION__);
            return false;
        }
        else
        {
            _pinLocSet.insert(pinXYZ);
        }
#endif
        drNet.pins().emplace_back(pinIdx);
        _db.drDB().pins().emplace_back(pin);
    }

    // Set the symmetric net
    drNet.setSymNetIdx(grNet.symNetIdx());
    // Set the self symmetric nets
    drNet.setSelfSymIdx(grNet.selfSymNetIdx());

#if 0
    // Convert the 2-pin net splittings from the global router to the detailed router
    if (_convertSubNet)
    {
        for (IndexType subNetIdx : grNet.subNets())
        {
            auto &subNet = _grDB.subNets().at(subNetIdx);
            IndexType pinGrIdx1 = subNet.sourcePinIdx();
            IndexType pinGrIdx2 = subNet.targetPinIdx();
            IndexType pinIdx1 = INDEX_TYPE_MAX; //< The index of the pin in the NetGR->pins
            IndexType pinIdx2 = INDEX_TYPE_MAX;
            for (IndexType idx = 0; idx < netPinIdxVec.size(); ++idx)
            {
                if (netPinIdxVec.at(idx) == pinGrIdx1) 
                {
                    pinIdx1 = idx;
                }
                if (netPinIdxVec.at(idx) == pinGrIdx2)
                {
                    pinIdx2 = idx;
                }
            }
            Assert(pinIdx1 != INDEX_TYPE_MAX);
            Assert(pinIdx2 != INDEX_TYPE_MAX);
            drNet.branches().emplace_back(std::make_pair(pinIdx1, pinIdx2));
        }
    }
    
#endif

    // Converting routing guide
    if (!GR_2D)
    {
        if (!convertRoutingGuide(netIdx, drNet))
        {
            return false;
        }
    }
    else
    {
        if (!convertRoutingGuide2D(netIdx))
        {
            return false;
        }
    }
    // Set whether waive gr
    drNet.setWaiveGr(grNet.grWaived());

    // push_back this net to db
    _drDB.nets().emplace_back(drNet);
    
    return true;
}

bool V2V2::convertRoutingPin2D()
{
    if (SKIP_GLOBAL_ROUTING)
    {
        return true;
    }
    // Add terminal/pin to the guide
    for (IndexType xIdx = 0 ; xIdx < _grDB.gridMap().numX(); ++xIdx)
    {
        for (IndexType yIdx = 0; yIdx < _grDB.gridMap().numY(); ++yIdx)
        {
            for (IndexType zIdx = 0; zIdx < _grDB.gridMap().numLayers(); ++zIdx)
            {
                const auto grGrid = _grDB.gridMap().gridCell(XYZ<IndexType>(xIdx, yIdx, zIdx));
                for (IndexType termIdx : grGrid.terminals())
                {
                    IndexType netIdx = _db.terminal(termIdx).netIdx();
                    for (IndexType layer = 0; layer < HIGHEST_LAYER; ++layer)
                    {
                        _db.drDB().roughGrid().at(xIdx, yIdx, layer).addGlobalRoutingGuide(netIdx);
                    }
                    // Do not penaltize detour near the pins
                }
            }
        }
    }
    return true;
}

bool V2V2::convertRoutingPin()
{
    if (SKIP_GLOBAL_ROUTING)
    {
        return true;
    }
    // Add terminal/pin to the guide
    for (IndexType xIdx = 0 ; xIdx < _grDB.gridMap().numX(); ++xIdx)
    {
        for (IndexType yIdx = 0; yIdx < _grDB.gridMap().numY(); ++yIdx)
        {
            for (IndexType zIdx = 0; zIdx < _grDB.gridMap().numLayers(); ++zIdx)
            {
                const auto grGrid = _grDB.gridMap().gridCell(XYZ<IndexType>(xIdx, yIdx, zIdx));
                for (IndexType termIdx : grGrid.terminals())
                {
                    IndexType netIdx = _db.terminal(termIdx).netIdx();
                    _db.drDB().roughGrid().at(xIdx, yIdx, zIdx).addGlobalRoutingGuide(netIdx);
                    // Do not penaltize detour near the pins
                    if (zIdx > 0)
                    {
                        _db.drDB().roughGrid().at(xIdx, yIdx, zIdx - 1).addGlobalRoutingGuide(netIdx);
                    }
                    if (zIdx > 1)
                    {
                        _db.drDB().roughGrid().at(xIdx, yIdx, zIdx - 2).addGlobalRoutingPenalty(netIdx);
                    }
                    if (zIdx < _grDB.gridMap().numLayers() - 1)
                    {
                        _db.drDB().roughGrid().at(xIdx, yIdx, zIdx + 1).addGlobalRoutingGuide(netIdx);
                    }
                    if (zIdx < _grDB.gridMap().numLayers() - 2)
                    {
                        _db.drDB().roughGrid().at(xIdx, yIdx, zIdx + 2).addGlobalRoutingPenalty(netIdx);
                    }
                }
            }
        }
    }
    return true;
}

bool V2V2::convertRoutingGuide2D(IndexType netIdx)
{
    if (SKIP_GLOBAL_ROUTING)
    {
        return true;
    }
    auto &edgeMap = _grDB.edgeMap().at(netIdx);
    if (edgeMap.size() == 0)
    {
        WRN("%s: net %s has no global routing edges \n", __FUNCTION__, _grDB.nets().at(netIdx).name().c_str());
    }
    // The routing guide stored in the NetDR: map the global grid cells to the grid cells connecting to it
    // Shouldn't be issue with missing the inter-layer edges
    for (auto &intervalPair : edgeMap)
    {
        auto &node1 = intervalPair.first.begin();
        auto &node2 = intervalPair.first.end();
        for (IntType layer = 0; layer < HIGHEST_LAYER; ++layer)
        {
            _db.drDB().roughGrid().at(node1.xy(), layer).addGlobalRoutingGuide(netIdx);
            _db.drDB().roughGrid().at(node2.xy(), layer).addGlobalRoutingGuide(netIdx);
            addPenaltyNearyby(XYZ<IndexType>(node1.xy(), layer), netIdx);
            addPenaltyNearyby(XYZ<IndexType>(node2.xy(), layer), netIdx);
        }
    }
    _db.drDB().roughGrid().init();
    return true;
}

bool V2V2::convertRoutingGuide(IndexType netIdx, NetDR &drNet)
{
    if (SKIP_GLOBAL_ROUTING)
    {
        return true;
    }
    auto &edgeMap = _grDB.edgeMap().at(netIdx);
    if (edgeMap.size() == 0)
    {
        WRN("%s: net %s has no global routing edges \n", __FUNCTION__, _grDB.nets().at(netIdx).name().c_str());
    }
    // The routing guide stored in the NetDR: map the global grid cells to the grid cells connecting to it
    // Shouldn't be issue with missing the inter-layer edges
    for (auto &intervalPair : edgeMap)
    {
        auto &node1 = intervalPair.first.begin();
        auto &node2 = intervalPair.first.end();
        _db.drDB().roughGrid().at(node1.xy(), node1.z()).addGlobalRoutingGuide(netIdx);
        _db.drDB().roughGrid().at(node2.xy(), node2.z()).addGlobalRoutingGuide(netIdx);
    }
    //_db.drDB().roughGrid().init();
    // Add penaly list which is allowed to be detoured
    for (auto &intervalPair : edgeMap)
    {
        auto &node1 = intervalPair.first.begin();
        auto &node2 = intervalPair.first.end();
        addPenaltyNearyby(node1, netIdx);
        addPenaltyNearyby(node2, netIdx);
    }
    _db.drDB().roughGrid().init();
    return true;
}

void V2V2::addPenaltyNearyby(const XYZ<IndexType> &node, IndexType netIdx)
{
    if (node.z() > 0)
    {
        _db.drDB().roughGrid().at(node.xy(), node.z() - 1).addGlobalRoutingPenalty(netIdx);
    }
    if (node.z() < _grDB.gridMap().numLayers() - 1)
    {
        _db.drDB().roughGrid().at(node.xy(), node.z() + 1).addGlobalRoutingPenalty(netIdx);
    }
    if (node.x() > 0)
    {
        _db.drDB().roughGrid().at(node.x() - 1, node.y(), node.z()).addGlobalRoutingPenalty(netIdx);
    }
    if (node.x() < _grDB.gridMap().numX() - 1)
    {
        _db.drDB().roughGrid().at(node.x() + 1, node.y(), node.z()).addGlobalRoutingPenalty(netIdx);
    }
    if (node.y() > 0)
    {
        _db.drDB().roughGrid().at(node.x(), node.y() - 1, node.z()).addGlobalRoutingPenalty(netIdx);
    }
    if (node.y() < _grDB.gridMap().numY() - 1)
    {
        _db.drDB().roughGrid().at(node.x(), node.y() + 1, node.z()).addGlobalRoutingPenalty(netIdx);
    }
    if (node.x() > 1)
    {
        _db.drDB().roughGrid().at(node.x() - 2, node.y(), node.z()).addGlobalRoutingPenalty(netIdx);
    }
    if (node.x() < _grDB.gridMap().numX() - 2)
    {
        _db.drDB().roughGrid().at(node.x() + 2, node.y(), node.z()).addGlobalRoutingPenalty(netIdx);
    }
    if (node.y() > 1)
    {
        _db.drDB().roughGrid().at(node.x(), node.y() - 2, node.z()).addGlobalRoutingPenalty(netIdx);
    }
    if (node.y() < _grDB.gridMap().numY() - 2)
    {
        _db.drDB().roughGrid().at(node.x(), node.y() + 2, node.z()).addGlobalRoutingPenalty(netIdx);
    }
}
PROJECT_NAMESPACE_END
