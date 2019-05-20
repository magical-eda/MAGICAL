#include "GR2DR.h"

PROJECT_NAMESPACE_BEGIN

bool GR2DR::convert()
{
    _pinDict.clear();
    convertDesign();
    convertNets();
    return true;
}

void GR2DR::convertNets() 
{
    /// First reserve the net array
    _drDB.netArray().resize(_aroute.nets().size());
    /// Also resize the via array
    _drDB.viaSetArray().resize(_aroute.nets().size());
    /// Build the map from net idx defined in input and the index of the net in the vector
    for (IndexType netIdx = 0; netIdx < _aroute.nets().size(); ++netIdx)
    {
        myNet *grNet = _aroute.nets().at(netIdx);
        _benchNetIdx2VecIdx[grNet->m_id] = netIdx;
    }
    /// Convert single net information
    for (IndexType netIdx = 0; netIdx < _aroute.nets().size(); ++netIdx)
    {
        convertNet(netIdx);
        convertRoutingGuide(netIdx);
    }
    /// Convert sym net information
    for (IndexType symIdx = 0; symIdx < _aroute.symNets().size(); ++symIdx)
    {
        convertSymNet(_aroute.symNets().at(symIdx));
    }
    /// Convert GR branch/2-pin-net-spliting information
    if (READ_GR_BRANCH)
    {
        for (IndexType netIdx = 0; netIdx < _aroute.nets().size(); ++netIdx)
        {
            convertBranch(netIdx);
        }
    }
}

void GR2DR::convertNet(IndexType netIdx)
{
    /// Symmtry information should be handled in a function for converting symNet
    myNet *grNet = _aroute.nets().at(netIdx);
    NetDR &drNet = _drDB.netArray().at(netIdx);
    /// Assert this netIdx is the same as the record in myNet
    //AssertMsg(grNet->m_id == static_cast<IntType>(netIdx), "NetIdx in converting is not corresponding to the myNet->m_id. It's possible, use the _benchNetIdx2VecIdx to handle this\n");
    /// net name
    drNet.setNetName(grNet->m_name);
    /// net idx in benchmark
    drNet.setBenchIdx(grNet->m_id);
    /// pins
    drNet.pinArray().clear();
#ifdef DEBUG_GR2DR
    DBG("Pin size %u \n", grNet->_rawPinArray.size());
#endif
    /// trim the overlapped pins 
    for (const auto &ispdPin : grNet->_rawPinArray)
    {
        Assert(ispdPin.layer > 0);
#ifdef TRIM_OVERLAP_PIN
        auto pinXYZ = XYZ<LocType, IndexType>(ispdPin.x, ispdPin.y, ispdPin.layer);
        auto pinIter = _pinDict.find(pinXYZ);
        if (pinIter != _pinDict.end())
        {
            continue;
        }
        else
        {
            _pinDict.insert(pinXYZ);
        }
#endif
        drNet.pinArray().emplace_back(Pin(XY<LocType>(ispdPin.x, ispdPin.y), static_cast<IndexType>(ispdPin.layer)));
#ifdef DEBUG_GR2DR
        DBG("Add pin %s\n", drNet.pinArray().back().toStr().c_str());
#endif
    }
}

void GR2DR::convertRoutingGuide(IndexType netIdx)
{
    myNet *grNet = _aroute.nets().at(netIdx);
    NetDR &drNet = _drDB.netArray().at(netIdx);
    /// The global routing solution. The pair.first is the 2D node-to-node edges, pair.second is the layer
    if (grNet->m_route == NULL)
    {
        ERR("%s: empty m_route! \n", __FUNCTION__);
        return;
    }
    const std::vector< std::pair<myEdge*, IntType> > &grEdges = grNet->m_route->m_3dedges;
    if (grEdges.size() == 0)
    {
        WRN("%s: empty routing guide \n",__FUNCTION__);
        return;
    }
    /// The routing guide stored in the NetDR: map the global grid cells to the grid cells connecting to it
    auto &grNodeMap = drNet.grid2GridGR();
    /// Key: 2D grid celss
    /// Element: pair of uint of <minLayer, maxLayer>, so that it is possible to check how many layers have added edges between edges
    std::map<XY<IndexType>, std::pair<IndexType, IndexType>> grLayerMap;

#ifdef DEBUG_GR2DR
    std::cout<<"3d edge size: "<<grEdges.size()<<std::endl;
#endif
    for (const auto &edge3D : grEdges)
    {
        const auto &edge = edge3D.first; /// myEdge*
        Assert(edge->m_layerIDs[edge3D.second] >= 1);
        /// Derong's layer starts from 1
        IndexType layer = static_cast<IndexType>(edge->m_layerIDs[edge3D.second]) ;
        XY<IndexType> xy1 = _drDB.grIdx2Loc().loc2XYIdx(XY<LocType>(edge->x1(), edge->y1()));
        XY<IndexType> xy2 = _drDB.grIdx2Loc().loc2XYIdx(XY<LocType>(edge->x2(), edge->y2()));
        XYZ<IndexType> gc1 = XYZ<IndexType>(xy1, layer);
        XYZ<IndexType> gc2 = XYZ<IndexType>(xy2, layer); 
        grNodeMap[gc1].insert(gc2);
        grNodeMap[gc2].insert(gc1);

#ifdef DEBUG_GR2DR
        std::cout<<gc1.toStr()<<" "<<gc2.toStr()<<std::endl;

#endif

        /// If gc1 and gc2 are recorded in grLayerMap
        if (grLayerMap.find(gc1.xy()) == grLayerMap.end())
        {
            grLayerMap[gc1.xy()] = std::make_pair(layer, layer);
        }
        else
        {
            if (grLayerMap[gc1.xy()].first > layer)
            {
                grLayerMap[gc1.xy()].first = layer;
            }
            if (grLayerMap[gc1.xy()].second < layer)
            {
                grLayerMap[gc1.xy()].second = layer;
            }
        }

        if (grLayerMap.find(gc2.xy()) == grLayerMap.end())
        {
            grLayerMap[gc2.xy()] = std::make_pair(layer, layer);
        }
        else
        {
            if (grLayerMap[gc2.xy()].first > layer)
            {
                grLayerMap[gc2.xy()].first = layer;
            }
            if (grLayerMap[gc2.xy()].second < layer)
            {
                grLayerMap[gc2.xy()].second = layer;
            }
        }
        /// Finish updating the grLayerMap
    } /// end of for (const auto &edge3D : grEdges)
    
    /// Add connections between layers.
    for (const auto &edgePair : grLayerMap) 
    {
        XY<IndexType> grid2D = edgePair.first;
        IndexType minLayer = edgePair.second.first;
        IndexType maxLayer = edgePair.second.second;
        for (IndexType lowerLayer = minLayer; lowerLayer < maxLayer; ++lowerLayer)
        {
            grNodeMap[XYZ<IndexType>(grid2D, lowerLayer)].insert(XYZ<IndexType>(grid2D, lowerLayer + 1));
            grNodeMap[XYZ<IndexType>(grid2D, lowerLayer + 1)].insert(XYZ<IndexType>(grid2D, lowerLayer));
        }
    }
}

void GR2DR::convertSymNet(mySymNet *grSymNet)
{
    /// The index of net defined in benchmark
    IntType rawIdx1 = grSymNet->m_net1->m_id;
    IntType rawIdx2 = grSymNet->m_net2->m_id;

    /// The index of net in the vector
    IndexType vecIdx1 = _benchNetIdx2VecIdx[rawIdx1];
    IndexType vecIdx2 = _benchNetIdx2VecIdx[rawIdx2];

    /// Set the correct information in the net
    auto &net1 = _drDB.netArray().at(vecIdx1);
    auto &net2 = _drDB.netArray().at(vecIdx2);

    net1.setIsSymmetry(true);
    net1.setSymNetIdx(vecIdx2);
    if (grSymNet->m_isHor)
    {
        net1.setSymType(SymType::HORIZONTAL);
    }
    else
    {
        net1.setSymType(SymType::VERTICAL);
    }


    net2.setIsSymmetry(true);
    net2.setSymNetIdx(vecIdx1);
    if (grSymNet->m_isHor)
    {
        net2.setSymType(SymType::HORIZONTAL);
    }
    else
    {
        net2.setSymType(SymType::VERTICAL);
    }

}

void GR2DR::convertBranch(IndexType netIdx)
{
    std::unordered_map<XY<LocType>, IndexType> locToPinIdxMap;
    /// record the pin location and pin idx in the map
    auto &drNet = _drDB.netArray().at(netIdx);
    /// clear the drNet branch
    drNet.branches().clear();
    for (IndexType pinIdx = 0; pinIdx < drNet.pinArray().size(); ++pinIdx)
    {
        const auto &pin = drNet.pinArray().at(pinIdx);
        locToPinIdxMap[pin.loc()] = netIdx;
#ifdef DEBUG_GR2DR
        DBG("%s: Net %u dr pin: %s \n", __FUNCTION__, netIdx, pin.loc().toStr().c_str());
#endif
    }
    /// Loop over the branches of the GR and assign the corresponding pin idx
    
    auto &grNet = _aroute.nets().at(netIdx);
    for (IndexType branchIdx = 0; branchIdx < grNet->m_branches.size(); ++branchIdx)
    {
        auto nodeFrom = grNet->m_branches.at(branchIdx)->m_from;
        auto nodeTo = grNet->m_branches.at(branchIdx)->m_to;
        auto pinFrom = grNet->m_mapNode2Pin[std::make_pair(nodeFrom->x(), nodeFrom->y())];
        auto pinTo = grNet->m_mapNode2Pin[std::make_pair(nodeTo->x(), nodeTo->y())];
#ifdef DEBUG_GR2DR
        DBG("%s: gr branch: %s to %s \n", __FUNCTION__, XY<LocType>(pinFrom->x(), pinFrom->y()).toStr().c_str(), XY<LocType>(pinTo->x(), pinTo->y()).toStr().c_str());
#endif
        auto fromIter = locToPinIdxMap.find(XY<LocType>(pinFrom->x(), pinFrom->y()));
        auto toIter = locToPinIdxMap.find(XY<LocType>(pinTo->x(), pinTo->y()));
        if (fromIter != locToPinIdxMap.end() && toIter != locToPinIdxMap.end())
        {
            drNet.branches().emplace_back(fromIter->second, toIter->second);
        }
    }
#ifdef DEBUG_GR2DR
    /// Test whether all pins are assigned to a branch
    std::vector<bool> assignArray;
    assignArray.resize(drNet.pinArray().size(), false);
    for (auto &branch : drNet.branches())
    {
        assignArray.at(branch.first) = true;
        assignArray.at(branch.second) = true;
    }
    for (IndexType assignIdx = 0;  assignIdx < assignArray.size(); ++ assignIdx)
    {
        bool assign = assignArray.at(assignIdx);
        AssertMsg(assign, "%s: Net %u, pin %u not assigned!\n", __FUNCTION__, netIdx, assignIdx);
    }
#endif
}

void GR2DR::convertDesign()
{
    IndexType layerSize = _aroute.layerSize() - 1;
   // _drDB.grid3D().resizeLayer(layerSize);
    /// Init gridCell-location conversion
    _drDB.grIdx2Loc() = GRCell2DRMap(_aroute.xGridsSize(), _aroute.yGridsSize(), static_cast<LocType>(_aroute.dieXLo()), static_cast<LocType>(_aroute.dieYLo()), static_cast<LocType>(_aroute.tileWidth()), static_cast<LocType>(_aroute.tileHeight()));
    /// Init each layer of Grid2D
    LocType dieXLo = _drDB.grIdx2Loc().dieXLo();
    LocType dieYLo = _drDB.grIdx2Loc().dieYLo();
    LocType dieXHi = _drDB.grIdx2Loc().dieXHi();
    LocType dieYHi = _drDB.grIdx2Loc().dieYHi();
    Box<LocType> dieBox = Box<LocType>(dieXLo, dieYLo, dieXHi, dieYHi);

    /// Add one layer of forbidden layer as PO
    _drDB.grid3D().emplaceLayer(dieBox, _aroute.minWidths().back(), _aroute.minSpaces().back(), RouteDirType::FORBID);
    
    /// Add the remaining layers
    for (IndexType layer = 0; layer < layerSize; ++layer)
    {
        /// TODO: why minWidths and minSpaces are the same?
#ifdef DEBUG_GR2DR
        DBG("%s: minWidth %d minSpaces %d \n", __FUNCTION__, _aroute.minWidths().at(layer + 1), _aroute.minSpaces().at(layer + 1));
#endif
        ///initLayer(IndexType layerIdx, const Box<LocType> &dieBox, IndexType metalWidth, LocType metalSpacing, RouteDirType routeDir);
        RouteDirType routeType;
        if (_aroute.hCaps().at(layer + 1) == 0 && _aroute.vCaps().at(layer + 1) == 0)
        {
            routeType = RouteDirType::FORBID;
        }
        else if (_aroute.hCaps().at(layer + 1) != 0 && _aroute.vCaps().at(layer + 1) != 0)
        {
            routeType = RouteDirType::BOTH;
        }
        else if (_aroute.hCaps().at(layer + 1) != 0)
        {
            routeType = RouteDirType::HORIZONTAL;
        }
        else
        {
            routeType = RouteDirType::VERTICAL;
        }
        _drDB.grid3D().emplaceLayer(dieBox, _aroute.minWidths().at(layer + 1), _aroute.minSpaces().at(layer + 1), routeType);
        //_drDB.grid3D().initLayer(layer, dieBox, _aroute.minWidths().at(layer + 1), _aroute.minSpaces().at(layer + 1), routeType);
        //DBG("Layer %u trackWidth %u \n", layer, _drDB.grid3D().at(layer).trackWidth());
        //std::cout<<"Layer "<<layer<<" trackWidth "<<_drDB.grid3D().at(layer).trackWidth()<<std::endl;
    }


}

PROJECT_NAMESPACE_END
