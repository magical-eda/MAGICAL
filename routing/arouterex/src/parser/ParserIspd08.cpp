#include "ParserIspd08.h"

PROJECT_NAMESPACE_BEGIN

bool ParserIspd08::read()
{
    _benchmark = 0;
    // Read in the benchmark file
    if (!readFile())
    {
        return false;
    }
    // Translate the database
    if (!trans2DB())
    {
        return false;
    }
    return true;
}

bool ParserIspd08::readPinShapeBenchmark()
{
    _benchmark = 1;
    // Read in the benchmark file
    if (!readFilePinShapeBenchmark())
    {
        return false;
    }
    // Translate the database
    if (!trans2DB())
    {
        return false;
    }
    return true;
}

bool ParserIspd08::readFilePinShapeBenchmark()
{
    std::ifstream inf(_fileName.c_str());
    if (!inf.is_open()) 
    {
        ERR("ISPDParser::%s: cannot open file: %s \n", __FUNCTION__ , _fileName.c_str());
        Assert(false);
        return false;
    }
    std::string anoynmouseStr;
    std::string token;
    IntType intToken = 0;
    std::getanoynmouse(inf, anoynmouseStr);
    std::stringstream ss(anoynmouseStr);
    IntType dieXLo = INT_TYPE_MAX;
    IntType dieYLo = INT_TYPE_MAX;
    IntType dieXHi = INT_TYPE_MIN;
    IntType dieYHi = INT_TYPE_MIN;
    // Die boundary
#if 0
    ss >> token;
    Assert(!std::strcmp(token.c_str(), "die"));
    ss >> intToken;
    _rawDB.setXLF(intToken * _scale);
    ss >> intToken;
    _rawDB.setYLF(intToken * _scale);
    ss >> intToken;
    _rawDB.setXHi(intToken * _scale);
    ss >> intToken;
    _rawDB.setYHi(intToken * _scale);
    // Net number
    std::getanoynmouse(inf, anoynmouseStr);
    ss = std::stringstream(anoynmouseStr);
#endif
    ss >> token;
    Assert(!std::strcmp(token.c_str(), "num"));
    ss >> token;
    Assert(!std::strcmp(token.c_str(), "net"));
    ss >> intToken;
    _rawDB.setNetSize(intToken);

    // Read each net
    for (IndexType netIdx = 0; netIdx < _rawDB.netSize(); ++netIdx)
    {
        std::getanoynmouse(inf, anoynmouseStr);
        ss = std::stringstream(anoynmouseStr);
        ss >> token;
        std::string netName = std::string(token);
        IndexType netIdxx;
        ss >> netIdxx;
        IndexType pinSize;
        ss >> pinSize;
        IndexType minimumWidth;
        ss >> minimumWidth;
        ISPDRawDb::ispdNet tempNet = ISPDRawDb::ispdNet(netName, netIdxx, pinSize, minimumWidth * _scale);
        for (IndexType pinIdx = 0; pinIdx < pinSize; ++pinIdx) 
        {
            std::getanoynmouse(inf, anoynmouseStr);
            ss = std::stringstream(anoynmouseStr);
            IndexType layer;
            ss >> layer;
            tempNet._pinArray.emplace_back(ISPDRawDb::ispdPin(0, 0, layer));
            bool isX = true;
            IntType x = 0;
            IntType y = 0;
            while (ss >> intToken)
            {
                if (isX) // read the x for a point
                {
                    x = intToken;
                    isX = false; // The next is y
                    if (x < dieXLo)
                    {
                        dieXLo = x;
                    }
                    if (x > dieXHi)
                    {
                        dieXHi = x;
                    }
                }
                else 
                {
                    // Reading y for a point
                    y = intToken;
                    isX = true; // The next one is x
                    tempNet._pinArray.back()._polygonPts.emplace_back(XY<LocType> (x * _scale, y * _scale));
                    if (y < dieYLo)
                    {
                        dieYLo = y;
                    }
                    if (y > dieYHi)
                    {
                        dieYHi = y;
                    }
                }
            }
        }
        _rawDB.addNet(tempNet);
    }
    // Set die boundary
    _rawDB.setXLF(dieXLo * _scale - 500);
    _rawDB.setYLF(dieYLo * _scale - 500);
    _rawDB.setXHi(dieXHi * _scale +  500);
    _rawDB.setYHi(dieYHi * _scale + 500);
    INF("%s: Choose design boundary: %d %d %d %d \n", __FUNCTION__, _rawDB.xLF(), _rawDB.yLF(), _rawDB.xHi(), _rawDB.yHi());
    return true;
}

bool ParserIspd08::readFile() 
{

    std::ifstream inf(_fileName.c_str());
    if (!inf.is_open()) 
    {
        ERR("ISPDParser::readFile(): cannot open file: %s \n", _fileName.c_str());
        Assert(false);
        return false;
    }
    char token [100];
    IntType uintToken = 0;
    // Step 1: parsing overall information grid # # # (x grids, y grids, number of layers)i
    inf >> token;
    Assert(!std::strcmp(token, "grid"));
    inf >> uintToken;
    _rawDB.setXGridSize(uintToken * _scale); 
    inf >> uintToken;
    _rawDB.setYGridSize(uintToken * _scale);
    inf >> uintToken;
    _rawDB.setLayerSize(uintToken);

#ifdef DEBUG_INPUT
    DBG("x %u, y %u, layer %u \n", _rawDB.xGridSize(), _rawDB.yGridSize(), _rawDB.layerSize());
#endif
    // Vertical capacity
    inf >> token;
    Assert(!std::strcmp(token, "vertical"));
    inf >> token;
    Assert(!std::strcmp(token, "capacity"));

    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        inf >> uintToken;
        _rawDB.addVerticalCapacity(uintToken);
    }
#ifdef DEBUG_INPUT
    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        DBG("Vertical capacity %u \n", _rawDB.verticalCapacity().at(layer));
    }
#endif
    // Horizontal capacity
    inf >> token;
    DBG("Token : %s \n", token);
    Assert(!std::strcmp(token, "horizontal"));
    inf >> token;
    Assert(!std::strcmp(token, "capacity"));

    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        inf >> uintToken;
        _rawDB.addHorizontalCapacity(uintToken);
    }
#ifdef DEBUG_INPUT
    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        DBG("Horizontal capacity %u \n", _rawDB.horizontalCapacity().at(layer));
    }
#endif

    // Minimum Width
    inf >> token;
    Assert(!std::strcmp(token, "minimum"));
    inf >> token;
    Assert(!std::strcmp(token, "width"));

    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        inf >> uintToken;
        _rawDB.addMinimumWidth(uintToken * _scale);
    }
#ifdef DEBUG_INPUT
    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        DBG("Minimum Width: %u \n", _rawDB.minimumWidth().at(layer));
    } 
#endif
     // Minimum spacing
    inf >> token;
    Assert(!std::strcmp(token, "minimum"));
    inf >> token;
    Assert(!std::strcmp(token, "spacing"));

    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        inf >> uintToken;
        _rawDB.addMinimumSpacing(uintToken * _scale);
    }
#ifdef DEBUG_INPUT
    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        DBG("Minimum Spacing: %u \n", _rawDB.minimumSpacing().at(layer));
    }   
#endif

     // Via spacing
    inf >> token;
    Assert(!std::strcmp(token, "via"));
    inf >> token;
    Assert(!std::strcmp(token, "spacing"));

    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        inf >> uintToken;
        _rawDB.addViaSpacing(uintToken * _scale);
    }
#ifdef DEBUG_INPUT
    for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer) 
    {
        DBG("Via Spacing: %u \n", _rawDB.viaSpacing().at(layer));
    }
#endif

    // lower_left_x lower_left_y tile_width tile_height 
    inf >> uintToken;
    _rawDB.setXLF(uintToken * _scale);
#ifdef DEBUG_INPUT
    DBG("%u ", _rawDB.xLF());
#endif
    inf >> uintToken;
    _rawDB.setYLF(uintToken * _scale);
#ifdef DEBUG_INPUT
    DBG("%u ", _rawDB.yLF());
#endif
    inf >> uintToken;
    _rawDB.setTileWidth(uintToken * _scale);
#ifdef DEBUG_INPUT
    DBG("%u ", _rawDB.tileWidth());
#endif
    inf >> uintToken;
    _rawDB.setTileHeight(uintToken * _scale);
#ifdef DEBUG_INPUT
    DBG("%u \n", _rawDB.tileHeight());
#endif

    // Net
    inf >> token;
    Assert(!std::strcmp(token, "num"));
    inf >> token;
    Assert(!std::strcmp(token, "net"));

    inf >> uintToken;
    _rawDB.setNetSize(uintToken);

    for (IndexType netIdx = 0; netIdx < _rawDB.netSize(); ++netIdx) 
    {
        inf >> token;
        std::string netName = std::string(token);
        IndexType netIdxx;
        inf >> netIdxx;
        IndexType pinSize;
        inf >> pinSize;
        IndexType minimumWidth;
        inf >> minimumWidth;
        ISPDRawDb::ispdNet tempNet = ISPDRawDb::ispdNet(netName, netIdxx, pinSize, minimumWidth * _scale);
        for (IndexType pinIdx = 0; pinIdx < pinSize; ++pinIdx) 
        {
            IndexType x;
            inf >> x;
            IndexType y;
            inf >> y;
            IndexType layer;
            inf >> layer;
            tempNet._pinArray.emplace_back(ISPDRawDb::ispdPin(x * _scale, y * _scale, layer));
        }
        _rawDB.addNet(std::move(tempNet));
    }

#ifdef DEBUG_INPUT
    for (IndexType netIdx = 0; netIdx < _rawDB.netSize(); ++netIdx) 
    {
        const auto &net = _rawDB.netArray().at(netIdx);
        DBG("net %s, %u, %u, %u\n", net._netName.c_str(), net._netIdx, net._pinSize, net._minimumWidth);
        for (IndexType pinIdx = 0; pinIdx < net._pinSize; ++pinIdx) 
        {
            const auto &pin = net._pinArray.at(pinIdx);
            DBG("pin %u, %u, %u\n", pin._x, pin._y, pin._layer);
        }
    }
#endif

    // Sym Nets
    IndexType symSize = 0;
    IndexType adjustSize = 0;
    inf >> token;
    if (!strcmp(token, "net")) 
    {
        // Existing sym nets
        inf >> token;
        Assert(!strcmp(token, "sym"));
        inf >> symSize;
    }
    else 
    {
        adjustSize = atoi(token); // If not existing sym net, the current token store the number of capacity adjustments
    }

    _rawDB.setSymSize(symSize);
    for (IndexType symIdx = 0; symIdx < symSize; ++symIdx) 
    {
        inf >> token;
        std::string netName1 = std::string(token);
        inf >> token;
        std::string netName2 = std::string(token);
        _rawDB.addSymNet(ISPDRawDb::ispdSymNet(netName1, netName2));
    }

#ifdef DEBUG_INPUT
   DBG("Sym net %u\n",_rawDB.symSize());
   for (IndexType symIdx = 0; symIdx < _rawDB.symSize(); ++symIdx) 
   {
        const auto &symNet = _rawDB.symNetArray().at(symIdx);
        DBG("%s %s\n", symNet._net1.c_str(), symNet._net2.c_str());
   }
#endif

   // Capacity adjustment
   
   // Check whether existing sym net and correct the token if necessary
   if (symSize != 0) 
   {
       inf >> adjustSize;
   }

   _rawDB.setCapSize(adjustSize);

   for (IndexType adjIdx = 0; adjIdx < adjustSize; ++adjIdx) 
   {
        IndexType x1; 
        inf >> x1;
        IndexType y1;
        inf >> y1;
        IndexType layer1;
        inf >>layer1;
        IndexType x2;
        inf >> x2;
        IndexType y2;
        inf >> y2;
        IndexType layer2;
        inf >> layer2;
        IndexType cap;
        inf >> cap;
        _rawDB.addCapAdjust(ISPDRawDb::ispdCapAdjust(x1 * _scale, y1 * _scale, layer1, x2 * _scale, y2 * _scale, layer2, cap)); 
   }

#ifdef DEBUG_INPUT
    DBG("cap ajust %u\n", _rawDB.capAdjustSize());
    for (IndexType adjIdx = 0; adjIdx < _rawDB.capAdjustSize(); ++adjIdx) 
    {
        const auto &cap = _rawDB.capAdjustArray().at(adjIdx);
       DBG("%u, %u, %u, %u, %u, %u, %u \n", cap._x1, cap._y1, cap._layer1, cap._x2, cap._y2, cap._layer2, cap._adjustment); 
       AssertMsg(cap._layer1 == cap._layer2, "Check the benchmark cap ajustment");
    }
#endif

    return true;
}

bool ParserIspd08::trans2DB()
{
    auto &grDB = _db.grDB();
    if (_benchmark == 0)
    {
        // Lower left corner of the design
        grDB.dieLL() = XY<LocType>(_rawDB.xLF(), _rawDB.yLF());

        // tile width/height of the global routing grid
        grDB.setGridWidth(_rawDB.tileWidth());
        grDB.setGridHeight(_rawDB.tileHeight());


        // metal/via width/spacing
        WRN("%s: ignore min width/spacing from ISPD'08 benchmarks\n", __FUNCTION__);

        // Init the grid map for the global routing base
        grDB.gridMap() = GridMapGR(_rawDB.xGridSize(), _rawDB.yGridSize(), _rawDB.layerSize() + 1);

        // init the caps of masterslice layer of 3D gridmap to have capacity all 0
        grDB.gridMap().initHCap(0, 0);
        grDB.gridMap().initVCap(0, 0);
        grDB.gridMap().initViaCap(0, 0);

        IntType totalHCap = 0;
        IntType totalVCap = 0;
        for (IndexType layer = 0; layer < _rawDB.layerSize(); ++layer)
        {
            totalHCap += _rawDB.horizontalCapacity().at(layer);
            totalVCap += _rawDB.verticalCapacity().at(layer);
            // Set the caps for the layer in 3D grid map
            grDB.gridMap().initHCap(layer + 1, _rawDB.horizontalCapacity().at(layer));
            grDB.gridMap().initVCap(layer + 1, _rawDB.verticalCapacity().at(layer));
            if (layer != _rawDB.layerSize() - 1)
            {
                grDB.gridMap().initViaCap(layer + 1, INT_TYPE_MAX); // the benchmark format does not specify this number
            }
        }
        // Also add the access from POLY
        grDB.gridMap().initViaCap(0, INT_TYPE_MAX);

        // Set edge distances
        _db.grDB().gridMap().setGridWidth(_rawDB.tileWidth());
        _db.grDB().gridMap().setGridHeight(_rawDB.tileHeight());

        // Set die upper right corner
        LocType dieXHi = _rawDB.xGridSize() * _rawDB.tileWidth();
        LocType dieYHi = _rawDB.yGridSize() * _rawDB.tileHeight();
        _db.grDB().dieUR() = XY<LocType> (dieXHi, dieYHi);


    } // if _benchmark == 0
    if (_benchmark == 1)
    {
        grDB.dieLL() = XY<LocType> (_rawDB.xLF(), _rawDB.yLF());
        grDB.dieUR() = XY<LocType> (_rawDB.xHi(), _rawDB.yHi());
    }
    // reserve net vector
    grDB.nets().reserve(_rawDB.netSize());

    // Build a map with [netName] = netIdx
    std::unordered_map<std::string, IndexType> netNameMap;
    for (IndexType netIdx = 0; netIdx < _rawDB.netSize(); ++netIdx)
    {
        netNameMap[_rawDB.netArray().at(netIdx)._netName] = netIdx;
    }


    // convert pin information
    for (IndexType netIdx = 0; netIdx < _rawDB.netSize(); ++netIdx)
    {
        const auto &net = _rawDB.netArray().at(netIdx);
        NetGR temp;
        temp.setName(net._netName);
        temp.setBenchmarkNetIdx(net._netIdx);
        // process the pins for the net
        for (const auto &pin : net._pinArray)
        {
            IndexType pinIdx = grDB.phyPinArray().size();
            Pin tempPin = Pin();
            // Layer is for both benchmarks
            tempPin.setLayer(pin._layer);
            if (_benchmark == 0)
            {
                IndexType xGridIdx = std::floor(pin._x - _rawDB.xLF()) / _rawDB.tileWidth();
                IndexType yGridIdx = std::floor(pin._y - _rawDB.yLF()) / _rawDB.tileHeight();
                tempPin.setLoc(XY<LocType>(pin._x, pin._y));
                tempPin.setGridIdx(XY<IndexType>(xGridIdx, yGridIdx));
            }
            // Pin shape poly
            for (const auto &polyPt : pin._polygonPts)
            {
                tempPin.addPinShapePoint(polyPt);
            }
            tempPin.init();
            grDB.phyPinArray().emplace_back(tempPin);
            temp.physPinArray().emplace_back(pinIdx);
        }
        grDB.nets().emplace_back(temp);
    }

    if (_benchmark == 0)
    {
        
        // find the sym nets
        for (const auto &symNet : _rawDB.symNetArray())
        {
            IndexType netIdx1 = netNameMap.at(symNet._net2);
            IndexType netIdx2 = netNameMap.at(symNet._net1);
            // Add SymNet objects
            IndexType symNetIdx = _db.numSymNets();
            _db.symNetArray().emplace_back(SymNet(netIdx1, netIdx2));
            // update the NetGR objects
            _db.grDB().nets().at(netIdx1).setSymNetIdx(symNetIdx);
            _db.grDB().nets().at(netIdx2).setSymNetIdx(symNetIdx);
        }

        // Routing blockages
        
        for (const auto &adj : _rawDB.capAdjustArray()) 
        {
            Assert(adj._layer1 == adj._layer2);
            //Assert(adj._adjustment == 0);
            IndexType layer = adj._layer1; 
            //IndexType layerCap = klib::greaterOne(_rawDb.horizontalCapacity().at(layer), _rawDb.verticalCapacity().at(layer));
            //AssertMsg(layerCap != 0, "layer: %u, layer cap %u %u \n", layer, _rawDb.horizontalCapacity().at(layer), _rawDb.verticalCapacity().at(layer));
            //IndexType reducedCap = adj._adjustment * PERCENT_ACC / layerCap;
            IntType reducedCap = adj._adjustment;

            // Calculate the index of grids that need to be adjusted
            IndexType xIdxLo = std::min((adj._x1 - _rawDB.xLF())/ _rawDB.tileWidth(), (adj._x2 - _rawDB.xLF()) / _rawDB.tileWidth());
            IndexType yIdxLo = std::min((adj._y1 - _rawDB.yLF())/ _rawDB.tileHeight(), (adj._y2 - _rawDB.yLF()) / _rawDB.tileHeight());
            IndexType xIdxHi = std::max((adj._x1 - _rawDB.xLF())/ _rawDB.tileWidth(), (adj._x2 - _rawDB.xLF()) / _rawDB.tileWidth());
            IndexType yIdxHi = std::max((adj._y1 - _rawDB.yLF())/ _rawDB.tileHeight(), (adj._y2 - _rawDB.yLF()) / _rawDB.tileHeight());

            for (IndexType xIdx = xIdxLo; xIdx < xIdxHi; ++xIdx)
            {
                for (IndexType yIdx = yIdxLo; yIdx < yIdxHi; ++yIdx)
                {
                    // The horizontal edge
                    auto &hEdge = grDB.gridEdge(XYZ<IndexType>(xIdx, yIdx, layer), XYZ<IndexType>(xIdx + 1, yIdx, layer));
                    IntType hReduced = hEdge.totalCap() > reducedCap ? \
                                       hEdge.totalCap() - reducedCap : \
                                       0;
                    hEdge.setTotalCap(hEdge.totalCap() - hReduced);
                    // The vertical edge
                    auto &vEdge = grDB.gridEdge(XYZ<IndexType>(xIdx, yIdx, layer), XYZ<IndexType>(xIdx, yIdx + 1, layer));
                    IntType vReduced = vEdge.totalCap() > reducedCap ? \
                                       vEdge.totalCap() - reducedCap : \
                                       0;
                    vEdge.setTotalCap(vEdge.totalCap() - vReduced);

                }
            }

        }
        
        // Init map 2D
        grDB.gridMap().init2D();

    } // if benchmark == 0
    INF("Parsing ISPD'08 Benchmark completed \n");
    return true;
}

PROJECT_NAMESPACE_END
