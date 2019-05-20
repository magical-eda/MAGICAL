#include "DetailedRouter.h"
#include "grdr/NetSpliter.h"

PROJECT_NAMESPACE_BEGIN

void DetailedRouter::testSearchGraph()
{
    addPinAccessFromAllLayers();
    _graphConstructor.constructNetGraph(0);
    AStarDRRouter astar = AStarDRRouter(_graphConstructor, _db);
    DBG("X/Y Size: %u %u \n", drDB().grid3D().gridVec2D(0).gridVec2D().xSize(),  drDB().grid3D().gridVec2D(0).gridVec2D().ySize());
    XY<IndexType> drFrom =drDB().locToDrXYZ( drDB().netArray().at(0).pinArray().at(0).loc(), drDB().netArray().at(0).pinArray().at(0).layer()).xy();
    XY<IndexType> drTo =drDB().locToDrXYZ( drDB().netArray().at(0).pinArray().at(1).loc(), drDB().netArray().at(0).pinArray().at(1).layer()).xy();
    astar.routeSingleTwoPinNet(_graphConstructor.xyidx2Node(drFrom, drDB().netArray().at(0).pinArray().at(0).layer()),_graphConstructor.xyidx2Node(drTo, drDB().netArray().at(0).pinArray().at(1).layer()) );
    //astar.routeSingleTwoPinNet(_graphConstructor.xyidx2Node(drFrom, 5),_graphConstructor.xyidx2Node(drTo, 5) );
    const auto & result = astar.results();
    for (const auto &node : result)
    {
        DBG("%s \n", _graphConstructor.node2Str(node).c_str());
    }
}

void DetailedRouter::testRouting()
{
    addPinAccessFromAllLayers();
    AStarDRRouter astar = AStarDRRouter(_graphConstructor, _db);
    NetSpliter twoPinSpliter;
    twoPinSpliter.setPins(drDB().netArray().at(0).pinArray());
    twoPinSpliter.solveEndToEnd();
    drDB().netArray().at(0).branches() = twoPinSpliter.results();
    relaxPinNeighborLayer(0);
    astar.routeAndAssignNet(0);

    for (const auto &node : drDB().netArray().at(0).drNodes())
    {
        DBG("%s \n", node.toStr().c_str());
    }
    drawNetDRNode(0);
}

////////////////
/// Init
////////////////
void DetailedRouter::occupyTracksAroundPins(IntType blockSize)
{
    for (IndexType netIdx = 0; netIdx < drDB().netArray().size(); ++netIdx)
    {
        const auto &net = drDB().netArray().at(netIdx);
        for (IndexType pinIdx = 0; pinIdx < net.pinArray().size(); ++pinIdx)
        {
            const auto &pin = net.pinArray().at(pinIdx);
            XYZ<IndexType> pinDRIdx = drDB().locToDrXYZ(pin.loc(), pin.layer());
            for (IntType rowIdx = static_cast<IntType>(pinDRIdx.x()) - blockSize; rowIdx <= static_cast<IntType>(pinDRIdx.x()) + blockSize; ++ rowIdx)
            {
                for (IntType colIdx = static_cast<IntType>(pinDRIdx.y()) - blockSize; colIdx <= static_cast<IntType>(pinDRIdx.y()) + blockSize; ++ colIdx)
                {
                     for (IntType layerIdx = static_cast<IntType>(pinDRIdx.z()) - blockSize; layerIdx <= static_cast<IntType>(pinDRIdx.z()) + blockSize; ++ layerIdx)
                     {
                         if (rowIdx >= 0 && colIdx >= 0 && layerIdx >= 0)
                         {
                             if (layerIdx < static_cast<IntType>(drDB().grid3D().layerSize()))
                             {
                                 if (rowIdx < static_cast<IntType>(drDB().grid3D().at(layerIdx).xSize()) && colIdx < static_cast<IntType>(drDB().grid3D().at(layerIdx).ySize()))
                                 {
                                     if (!drDB().grid3D().at(layerIdx).gridCell(XY<IndexType>(rowIdx, colIdx)).isOccupied())
                                     {
                                        drDB().grid3D().at(layerIdx).gridCell(XY<IndexType>(rowIdx, colIdx)).occupy(netIdx);
                                     }
                                 }
                             }
                         }
                     }
                }
            }
        }
    }
}

void DetailedRouter::addPinAccessFromAllLayers()
{
    /// Just add all the pins to grCell and let the SearchGraphConstructor to add edges between layers
    for (auto &net : drDB().netArray())
    {
        for (const auto &pin : net.pinArray())
        {
            XY<IndexType> pinLoc = drDB().grIdx2Loc().loc2XYIdx(pin.loc());
            auto grCellIter = net.grid2GridGR().find(XYZ<IndexType>(pinLoc, pin.layer()));
            if (grCellIter == net.grid2GridGR().end())
            {
                /// Not exist -> add
                for (IndexType layer = 0; layer < drDB().grid3D().layerSize() - 1; ++ layer)
                {
                    net.grid2GridGR()[XYZ<IndexType>(pinLoc, layer)].insert(XYZ<IndexType>(pinLoc, layer + 1));
                }
            }
        }
    }
}


//////////////////////////
/// Routing flow
//////////////////////////

bool DetailedRouter::directRouteFlow()
{
    bool success = true;
    /// init 
    addPinAccessFromAllLayers();
    occupyTracksAroundPins(PIN_NEIGHBOR_OCCUPTY_SIZE);

    /// Build the augment list graph
    std::vector<XY<IndexType>> sizeXY;
    for (IndexType layerIdx = 0; layerIdx < drDB().grid3D().layerSize(); ++layerIdx)
    {
        sizeXY.emplace_back(XY<IndexType>(drDB().grid3D().at(layerIdx).xSize(),drDB().grid3D().at(layerIdx).ySize()));
    }
    //_astar.initAugListGraphXYZ(sizeXY);

    for (IndexType netIdx = 0; netIdx < drDB().netArray().size(); ++netIdx)
    {
        INF("%s: route net %d \n" ,__PRETTY_FUNCTION__, netIdx );
        if(!routeNet(netIdx))
        {
            ERR("%s :detailed routing failed for net %d \n", __FUNCTION__, netIdx);
            success = false;
        }
    }
    return success;
}

bool DetailedRouter::solveAStarSequential()
{
    return DetailedRoutingAStarSequential(_db).runKernel();
}
//////////////////////////
/// Rout,ng subroutine
//////////////////////////
bool DetailedRouter::routeNet(IndexType netIdx)
{
    _fixedViaSelector.init();
    _astar.reset();
    _astar.initFixedViaSelector(_fixedViaSelector);
    /// Split into 2 pin nets (here is simple sequential dividing)
    if (!READ_GR_BRANCH)
    {
        NetSpliter twoPinSpliter;
        twoPinSpliter.setPins(drDB().netArray().at(netIdx).pinArray());
        twoPinSpliter.solveEndToEnd();
        drDB().netArray().at(netIdx).branches() = twoPinSpliter.results();
    }

    /// Relatex the gr cell grid cell for pin access
    relaxPinNeighborLayer(netIdx);

    /// A* search and assign the result
    return _astar.routeAndAssignNet(netIdx);
}

void DetailedRouter::relaxPinNeighborLayer(IndexType netIdx)
{
    const auto &net = drDB().netArray().at(netIdx);
    for (const auto &pin : net.pinArray())
    {
        XY<IndexType> planeIdx = drDB().grIdx2Loc().loc2XYIdx(pin.loc());        
        if (pin.layer() > 0)
        {
            _graphConstructor.addGRGrid(XYZ<IndexType>(planeIdx, pin.layer() - 1));
        }
        if (pin.layer() < drDB().grid3D().layerSize() - 1)
        {
            _graphConstructor.addGRGrid(XYZ<IndexType>(planeIdx, pin.layer() + 1));
        }
    }
}

PROJECT_NAMESPACE_END
