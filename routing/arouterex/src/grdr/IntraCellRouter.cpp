#include "IntraCellRouter.h"
#include "util/ManSegment.h"

PROJECT_NAMESPACE_BEGIN

bool IntraCellRouter::intraCellRouteCMOS(IndexType gateTermIdx, IndexType drainTermIdx, IndexType sourceTermIdx)
{
    auto &gate = _db.terminal(gateTermIdx);
    auto &drain = _db.terminal(drainTermIdx);
    auto &source = _db.terminal(sourceTermIdx);
    Assert(gate.pinArray().size() > 0);
    Assert(drain.pinArray().size() > 0);
    Assert(source.pinArray().size() > 0);
    // Assume drain and source are on M1 and gate is on PO
    // Get the spacing for M1
    WRN("%s: use min spacing for intra cell generation, should use EOL? \n", __FUNCTION__);
    LocType m1Spacing = std::max(_instr.metalMinSpacing(1), _instr.cutConservativeSpacing(1)); // Pick the larger one: metal or cut spacing requirement

    // sort the pins by xlo
    auto func = [&] (IndexType lhs, IndexType rhs)
    {
        return _db.grDB().pin(lhs).shapeRects().front().xLo() < _db.grDB().pin(rhs).shapeRects().front().xLo();
    };
    std::sort(source.pinArray().begin(), source.pinArray().end(), func);
    std::sort(drain.pinArray().begin(), drain.pinArray().end(), func);
    std::sort(gate.pinArray().begin(), gate.pinArray().end(), func);

    // Source: make it go up
    Assert(_db.grDB().pin(source.pinArray().front()).shapeRects().size() == 1 );
    const auto & beginRectSource = _db.grDB().pin(source.pinArray().front()).shapeRects().front();
    const auto & endRectSource = _db.grDB().pin(source.pinArray().back()).shapeRects().front();
    Assert(beginRectSource.yLo() == endRectSource.yLo());
    Assert(beginRectSource.yHi() == endRectSource.yHi());
    Assert(beginRectSource.xLen() == endRectSource.xLen());
    LocType beginX = beginRectSource.xLo();
    LocType endX = endRectSource.xHi();
    LocType rectYHi = beginRectSource.yHi();
    LocType segmentYLo = beginRectSource.yHi() + m1Spacing; //The y coordinate for the horizontal segment 
    LocType segmentWidth = beginRectSource.xLen();
    // Add the horizontal segment (m1? maybe M2)
    source.addIntraBox(Box<LocType>(beginX, segmentYLo, endX, segmentYLo + segmentWidth), 2, true, true);
    source.setPrimarySearchPointShape(0);
    // primary search point set to be the center
    source.setPrimarySearchPoint(NetNodeLoc(source.intraCellRoute().back().box().center(), 1));
    for (IndexType pinIdx : source.pinArray())
    {
        const auto & rect = _db.grDB().pin(pinIdx).shapeRects().front();
        beginX = rect.xLo();
        endX = rect.xHi();
        source.addIntraBox(Box<LocType>(beginX, rectYHi, endX, segmentYLo), 2, false, true);
    }

    // Gate: make it above the source
    Assert(_db.grDB().pin(gate.pinArray().front()).shapeRects().size() == 1 );
    const auto & beginRectGate = _db.grDB().pin(gate.pinArray().front()).shapeRects().front();
    const auto & endRectGate = _db.grDB().pin(gate.pinArray().back()).shapeRects().front();
    Assert(beginRectGate.yLo() == endRectGate.yLo());
    Assert(beginRectGate.yHi() == endRectGate.yHi());
    Assert(beginRectGate.xLen() == endRectGate.xLen());
    segmentYLo = segmentYLo + segmentWidth + m1Spacing; // Determined by the location of the source segment
    beginX = beginRectGate.xLo();
    endX = endRectGate.xHi();
    rectYHi = beginRectGate.yHi();
    segmentWidth = beginRectGate.xLen();
    // Add the horizontal segment (PO)
    gate.addIntraBox(Box<LocType>(beginX, segmentYLo, endX, segmentYLo + segmentWidth), 0, true, true);
    // primary search point set to be the center
    gate.setPrimarySearchPoint(NetNodeLoc(gate.intraCellRoute().back().box().center(), 0));
    source.setPrimarySearchPointShape(0);
    for (IndexType pinIdx : gate.pinArray())
    {
        const auto & rect = _db.grDB().pin(pinIdx).shapeRects().front();
        beginX = rect.xLo();
        endX = rect.xHi();
        gate.addIntraBox(Box<LocType>(beginX, rectYHi, endX, segmentYLo), 0, false, true);
    }
    
    // Drain: make it go down
    Assert(_db.grDB().pin(drain.pinArray().front()).shapeRects().size() == 1 );
    const auto & beginRectDrain = _db.grDB().pin(drain.pinArray().front()).shapeRects().front();
    const auto & endRectDrain = _db.grDB().pin(drain.pinArray().back()).shapeRects().front();
    Assert(beginRectDrain.yLo() == endRectDrain.yLo());
    Assert(beginRectDrain.yHi() == endRectDrain.yHi());
    Assert(beginRectDrain.xLen() == endRectDrain.xLen());
    beginX = beginRectDrain.xLo();
    endX = endRectDrain.xHi();
    LocType rectYLo = beginRectDrain.yLo();
    LocType segmentYHi = rectYLo - m1Spacing;
    segmentWidth = beginRectDrain.xLen();
    // Add the horizontal segment (m1? maybe M2)
    drain.addIntraBox(Box<LocType>(beginX, segmentYHi - segmentWidth, endX, segmentYHi), 2, true, true);
    // primary search point set to be the center
    drain.setPrimarySearchPoint(NetNodeLoc(drain.intraCellRoute().back().box().center(), 1));
    source.setPrimarySearchPointShape(0);
    for (IndexType pinIdx : drain.pinArray())
    {
        const auto & rect = _db.grDB().pin(pinIdx).shapeRects().front();
        beginX = rect.xLo();
        endX = rect.xHi();
        drain.addIntraBox(Box<LocType>(beginX, segmentYHi, endX, rect.yLo()), 2, false, true);
    }

    // Process the pin shape and so on

    return true;
}

bool IntraCellRouter::intraCellRouteDEFAULT(IndexType terminalIdx)
{
    auto &term = _db.terminal(terminalIdx);
    Assert(term.numPins() == 1 );
    auto &pin = _db.grDB().pin(term.pin(0)); // should be only one pin in the terminal
    IndexType shapeIdx = pin.defaultShapeIdx();
    // Flag the shape in the pin as has been included in terminal
    pin.setIncludeTermFlag(shapeIdx, true);
    // add this shape in terminal
    term.addIntraBox(pin.shapeRects().at(shapeIdx), DRUtil::routeLayer2AllLayer(pin.layer()), true, false);
    // Set the search point
    // Align with grid
    /// TODO: replace hard-coded number
    term.setPrimarySearchPoint(NetNodeLoc(_instr.alignGrid(pin.loc()), pin.layer()));
    term.setPrimarySearchPointShape(shapeIdx);
    // Add all other pin shapes
    for (IndexType idx = 0; idx < pin.shapeRects().size(); ++idx)
    {
        if (idx == shapeIdx)
        {
            break;
        }
        term.addIntraBox(pin.shapeRects().at(idx), DRUtil::routeLayer2AllLayer(pin.layer()), true, false);
    }
    // Slice the search points
    for (IndexType idx = 0; idx < term.numIntraShapes(); ++idx)
    {
        slicePinShape(terminalIdx, idx);
    }
    return true;
}

bool IntraCellRouter::slicePinShape(IndexType terminalIdx, IndexType shapeIdx)
{
    auto &term = _db.terminal(terminalIdx);
    //auto &ptArray = _db.drDB().processPinArray().at(pinIdx).searchPoints(); //< Store the pts here
    IndexType routeLayer = DRUtil::allLayer2RouteLayer(term.intraShape(term.primarySearchPointShape()).layer());
    AssertMsg(routeLayer < _instr.numRouteLayers() - 1, "Layer %d exceed the number of layers %d \n", routeLayer, _instr.numRouteLayers());
    RouteDirType higherLayerRouteDir = _instr.routeDirection(routeLayer + 1);
    Assert(higherLayerRouteDir == RouteDirType::HORIZONTAL || higherLayerRouteDir == RouteDirType::VERTICAL);
    LocType lowerTrackWidth = _instr.metalMinSpacing(routeLayer) + _instr.metalMinWidth(routeLayer);
    //LocType higherTrackWidth = _instr.metalMinSpacing(routeLayer + 1) + _instr.metalMinWidth(routeLayer + 1);
    //LocType lowerTrackOffset = _db.drDB().roughGrid().trackOffset(routeLayer);
    /*
    LocType higherTrackOffset = 0;
    if (higherLayerRouteDir == RouteDirType::HORIZONTAL)
    {
        higherTrackOffset = (_db.grDB().dieLL().y() + (higherTrackWidth) / 2);
    }
    else
    {
        higherTrackOffset = (_db.grDB().dieLL().x() + (higherTrackWidth) / 2);
    }
    */
    
    // Slices the shapes by the tracks of the higher layer
    std::vector<ManSegment<LocType>> hitpoints; //< A vertical or horizontal interval
    auto rect = term.intraCellRoute().at(shapeIdx).box();
/*
    if (higherLayerRouteDir == RouteDirType::HORIZONTAL)
    {
        LocType startY = ((rect.yLo() - higherTrackOffset) / higherTrackWidth + 1)  *  higherTrackWidth + higherTrackOffset;
        for (LocType sliceY = startY; sliceY < rect.yHi(); sliceY += higherTrackWidth)
        {
            hitpoints.emplace_back(ManSegment<LocType>(XY<LocType>(rect.xLo(), sliceY), XY<LocType>(rect.xHi(), sliceY)));
        }
    }
    else
    {
        //Vertical
        LocType startX = ((rect.xLo() - higherTrackOffset) / higherTrackWidth + 1)  *  higherTrackWidth + higherTrackOffset;
        for (LocType sliceX = startX; sliceX < rect.xHi(); sliceX += higherTrackWidth)
        {
            hitpoints.emplace_back(ManSegment<LocType>(XY<LocType>(sliceX, rect.yLo()), XY<LocType>(sliceX, rect.yHi())));
        }
    }
*/
    // Solve the situtation is no hit point is found
    if (hitpoints.size() == 0)
    {
        //WRN("%s: pin shape does not hit a track, add the center points \n", __FUNCTION__);
        //for (const auto &rect : pin.shapeRects())
        //{
            if (higherLayerRouteDir == RouteDirType::HORIZONTAL)
            {
                LocType centerY = ( rect.yLo() + rect.yHi() ) / 2;
                hitpoints.emplace_back(ManSegment<LocType>(XY<LocType>(rect.xLo(), centerY), XY<LocType>(rect.xHi(), centerY)));
            }
            else
            {
                LocType centerX = ( rect.xLo() + rect.xHi() ) / 2;
                hitpoints.emplace_back(ManSegment<LocType>(XY<LocType>(centerX, rect.yLo()), XY<LocType>(centerX, rect.yHi())));
            }
        //}
    }

    // Select the search points
    // For short hitpoints, pick the center point
    // For long hitpoints, partition into tracks and select the points
    IndexType allLayer = term.primarySearchPoint().layer();
    for (const auto & hp : hitpoints)
    {
        if (hp.length() > 5 * lowerTrackWidth)
        {
            // Just find the points of the tracks points
            IndexType times = hp.length() / lowerTrackWidth; 
            for (IndexType idx = 0; idx < times; ++idx)
            {
                LocType pt = hp.begin() + idx * lowerTrackWidth + lowerTrackWidth / 2;
                if (higherLayerRouteDir == RouteDirType::HORIZONTAL)
                {
                    // The segment is horizontal
                    term.addOtherSearchPoint(NetNodeLoc(_instr.alignGrid(XY<LocType>(pt, hp.coordinate())), allLayer), term.primarySearchPointShape());
                }
                else
                {
                    term.addOtherSearchPoint(NetNodeLoc(_instr.alignGrid(XY<LocType>(hp.coordinate(), pt)), allLayer), term.primarySearchPointShape());
                }
            }
        }
        else
        {
            term.addOtherSearchPoint(NetNodeLoc(_instr.alignGrid(XY<LocType>(hp.centerXY())), allLayer), term.primarySearchPointShape());
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
