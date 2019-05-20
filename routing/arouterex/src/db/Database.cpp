#include "Database.h"
#include <algorithm> ///std::max
#include "writer/gdsii/WriteGds.h"

PROJECT_NAMESPACE_BEGIN

/*------------------------------*/ 
/* Parse                        */
/*------------------------------*/ 
void Database::parse(IntType argc, char** argv)
{
    _progArgs = parseProgArgsCMD(argc, argv);
}

void Database::overrideGlobalRoutingGrid()
{
    LocType trackWidth = _techDB.micro2DBU(_techDB.routingLayers().at(0).pitch);
    LocType gridWidthDefault = GLOBAL_GRID_DEFAULT_TRACK * trackWidth;
    auto dieLength = _grDB.dieUR() - _grDB.dieLL(); // xy represent the length of the boundary of the design
    // Calculate the number of grids needed if using the default grid width
    IntType numXGrids = dieLength.x() / gridWidthDefault; 
    IntType numYGrids = dieLength.y() / gridWidthDefault;
    // If exceed the parameter of max grids, expand the grid length correspondingly
    if (numXGrids > GLOBAL_GRID_MAX_NUM)
    {
        _grDB.gridMap().setGridWidth( dieLength.x() / GLOBAL_GRID_MAX_NUM);
    }
    else
    {
        // Default is okay, save it
        _grDB.gridMap().setGridWidth(gridWidthDefault);
    }
    if (numYGrids > GLOBAL_GRID_MAX_NUM)
    {
        _grDB.gridMap().setGridHeight( dieLength.y() / GLOBAL_GRID_MAX_NUM);
    }
    else
    {
        // Default is okay, save it
        _grDB.gridMap().setGridHeight(gridWidthDefault);
    }
}

bool Database::overrideGlobalSettingWithLef()
{

    // Override the width and height of the global routing grid
    this->overrideGlobalRoutingGrid();
    // Grid width and height of the global routing grid
    LocType gridWidth = _grDB.gridMap().gridWidth();
    LocType gridHeight = _grDB.gridMap().gridHeight();
    WRN("%s: for now only allow routing on M1- M%d \n", __FUNCTION__, HIGHEST_LAYER - 1); ///TODO: Handle all the layers
    IndexType numLayers = HIGHEST_LAYER;
    //IndexType numLayers = _techDB.accessLayerTypeVec().size(); /// Include P0, exclude AP
    // Calculate the number of grids based on overrideGlobalRoutingGrid()
    auto dieLength = _grDB.dieUR() - _grDB.dieLL();
    IntType numXGrids = dieLength.x() / gridWidth + 1;
    IntType numYGrids = dieLength.y() / gridHeight + 1;
    if (this->progArgs().isRouteGuideSet())
    {
        numXGrids = _routeGuide.numX();
        numYGrids = _routeGuide.numX();
        gridWidth = dieLength.x() / numXGrids; 
        gridHeight = dieLength.y() / numYGrids;
        _grDB.gridMap().setGridWidth(gridWidth);
        _grDB.gridMap().setGridHeight(gridHeight);

    }
    // Init the grid map for the global routing base
    _grDB.gridMap() = GridMapGR(numXGrids, numYGrids, numLayers);
    // init the caps of masterslice layer of 3D gridmap to have capacity all 0
    _grDB.gridMap().initHCap(0, 0);
    _grDB.gridMap().initVCap(0, 0);
    _grDB.gridMap().initViaCap(0, 0);

    IntType totalHCap = 0;
    IntType totalVCap = 0;
    // Recalculate the edge capacities for all metal layers
    for (IndexType layer = 1; layer < numLayers; ++layer) // 0: masterslice numlayers-1: AP
    {
        const auto &lefLayer = _techDB.routingLayers().at(layer - 1);
        LocType pitchX, pitchY;
        if (lefLayer.hasXYPitch)
        {
            pitchX = _techDB.micro2DBU(lefLayer.pitchX);
            pitchY = _techDB.micro2DBU(lefLayer.pitchY);
        }
        else
        {
            pitchX = _techDB.micro2DBU(lefLayer.pitch);
            pitchY = pitchX;
        }
        LocType hCap = 0; LocType vCap = 0;
        if (lefLayer.direction == "HORIZONTAL" || lefLayer.direction == "BOTH")
        {
            hCap = std::max(static_cast<LocType>(gridHeight * GLOBAL_CAPACITY_RATIO / pitchY) , 1);
        }
        if (lefLayer.direction == "VERTICAL" || lefLayer.direction == "BOTH")
        {
            vCap = std::max(static_cast<LocType>(gridWidth * GLOBAL_CAPACITY_RATIO / pitchX), 1);
        }
        _grDB.gridMap().initHCap(layer, hCap);
        _grDB.gridMap().initVCap(layer, vCap);
        totalHCap += hCap;
        totalVCap += vCap;
        // Set via to be infinity
        _grDB.gridMap().initViaCap(layer - 1, INT_TYPE_MAX);
    }

    // Set edge distances
    _grDB.gridMap().setGridWidth(gridWidth);
    _grDB.gridMap().setGridHeight(gridHeight);

    // Init map 2D
    _grDB.gridMap().init2D();
    /// TODO: also add blockage to the global routing
    // Recalculate the global routing index
    recalculateGridIdx();
    return true;
}

void Database::recalculateGridIdx()
{
    LocType gridWidth = _grDB.gridMap().gridWidth();
    LocType gridHeight = _grDB.gridMap().gridHeight();
    for (auto &pin : _grDB.phyPinArray())
    {
        IndexType xGridIdx = std::floor(pin.ptX() - _grDB.dieLL().x()) / gridWidth;
        IndexType yGridIdx = std::floor(pin.ptY() - _grDB.dieLL().y()) / gridHeight;
        pin.setGridIdx(XY<IndexType>(xGridIdx, yGridIdx));
    }
}

void Database::calculateHpwls()
{
    _hpwls.resize(_grDB.nets().size());
    for (IndexType netIdx = 0; netIdx < _grDB.nets().size(); ++netIdx)
    {
        const auto &net = _grDB.nets().at(netIdx);
        if (net.numPhyPins() == 0)
        {
            _hpwls.at(netIdx) = 0;
            continue;
        }
        XY<LocType> ll = XY<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX);
        XY<LocType> ur = XY<LocType>(LOC_TYPE_MIN, LOC_TYPE_MIN);
        for (IndexType pinIdx : net.physPinArray())
        {
            const auto &pin = _grDB.pin(pinIdx);
            // Test the default location of pin
            if (pin.loc().x() < ll.x())
            {
                ll.setX(pin.loc().x());
            }
            if (pin.loc().y() < ll.y())
            {
                ll.setY(pin.loc().y());
            }
            if (pin.loc().x() > ur.x())
            {
                ur.setX(pin.loc().x());
            }
            if (pin.loc().y() > ur.y())
            {
                ur.setY(pin.loc().y());
            }
            // Test on all the pin shapes
            for (const auto &rect : pin.shapeRects())
            {
                if (rect.xLo() < ll.x())
                {
                    ll.setX(rect.xLo());
                }
                if (rect.yLo() < ll.y())
                {
                    ll.setY(rect.yLo());
                }
                if (rect.xHi() > ur.x())
                {
                    ur.setX(rect.xHi());
                }
                if (rect.yHi() > ur.y())
                {
                    ur.setY(rect.yHi());
                }
            }
        }
        // Calculate the hpwl for the net
        _hpwls.at(netIdx) = (ur.x() - ll.x() ) + (ur.y() - ll.y());
    }
}


bool Database::drainAllPins(const std::string &filename)
{
    auto _wg = std::make_shared<WriteGds>(filename);
    //_wg = std::shared_ptr<WriteGds>(new WriteGds(outputFile));
    if (!_wg->initWriter())
    {
        return false;
    }
    if (!_wg->createLib("TOP", _techDB.units().databaseNumber, 1e-6/_techDB.units().databaseNumber))
    {
        return false;
    }
    if (!_wg->writeCellBgn("INTERCONNECTION"))
    {
        return false;
    }
    for (IndexType termIdx = 0; termIdx < _terminalArray.size(); ++termIdx)
   {
        const auto &terminal = _terminalArray.at(termIdx);
        for (IndexType pinIdx : terminal.pinArray())
        {
            const auto &pin = _grDB.phyPinArray().at(pinIdx);
            for (IndexType idx = 0; idx <  pin.shapeRects().size(); ++idx)
            {
                const auto &rect = pin.shapeRects().at(idx);
                if (pin.isIncludeTerm(idx))
                {
                    continue;
                }
                IndexType accessLayerIdx = DRUtil::routeLayer2AllLayer(pin.layer());
                _wg->writeRectangle(rect, _techDB.accessLayerMaskId(accessLayerIdx), 0);
            }
        }
        for (const auto &shape : terminal.intraCellRoute())
        {
            IndexType accessLayerIdx = shape.layer();
            _wg->writeRectangle(shape.box(), _techDB.accessLayerMaskId(accessLayerIdx), 0);
        }
    }
    
    if (!_wg->writeCellEnd())
    {
        return false;
    }
    if (!_wg->endLib())
    {
        return false;
    }
    DBG("%s: debug rtree shapes saved in %s \n", __FUNCTION__, filename.c_str());
    return true;
}

PROJECT_NAMESPACE_END
