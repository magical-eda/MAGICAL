#include "DetailedPreProcessing.h"
#include "util/ManSegment.h"

PROJECT_NAMESPACE_BEGIN

bool DetailedPreProcessing::processPins()
{
#if 0
    // Allocate enough ProcessPin
    _db.drDB().processPinArray().resize(_db.drDB().numPins());
    for (IndexType pinIdx = 0; pinIdx < _db.drDB().numPins(); ++pinIdx)
    {
        if (!this->sliceSearchPoint(pinIdx))
        {
            return false;
        }
    }
#endif
    return true;
}

bool DetailedPreProcessing::sliceSearchPoint(IndexType pinIdx)
{
#if 0
    const auto &pin = _db.drDB().pin(pinIdx);
    auto &ptArray = _db.drDB().processPinArray().at(pinIdx).searchPoints(); //< Store the pts here
    if (pin.shapeRects().size() == 0)
    {
        // No pin shape situation, just add the default location as search point
        ptArray.emplace_back(pin.loc()); 
        return true;
    }
    IndexType routeLayer = pin.layer();
    Assert(routeLayer < _instr.numRouteLayers() - 1);
    RouteDirType higherLayerRouteDir = _instr.routeDirection(routeLayer + 1);
    Assert(higherLayerRouteDir == RouteDirType::HORIZONTAL || higherLayerRouteDir == RouteDirType::VERTICAL);
    LocType lowerTrackWidth = _db.drDB().roughGrid().pitch(routeLayer);
    LocType higherTrackWidth = _db.drDB().roughGrid().pitch(routeLayer + 1);
    //LocType lowerTrackOffset = _db.drDB().roughGrid().trackOffset(routeLayer);
    LocType higherTrackOffset = _db.drDB().roughGrid().trackOffset(routeLayer + 1);
    
    // Slices the shapes by the tracks of the higher layer
    std::vector<ManSegment<LocType>> hitpoints; //< A vertical or horizontal interval
    for (const auto &rect : pin.shapeRects())
    {
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
    }
    // Solve the situtation is no hit point is found
    if (hitpoints.size() == 0)
    {
        WRN("%s: pin shape does not hit a track, add the center points \n", __FUNCTION__);
        for (const auto &rect : pin.shapeRects())
        {
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
        }
    }

    // Select the search points
    // For short hitpoints, pick the center point
    // For long hitpoints, partition into tracks and select the points
    for (const auto & hp : hitpoints)
    {
        if (hp.length() > 2 * lowerTrackWidth)
        {
            // Just find the points of the tracks points
            IndexType times = hp.length() / lowerTrackWidth; 
            for (IndexType idx = 0; idx < times; ++idx)
            {
                LocType pt = hp.begin() + idx * lowerTrackWidth + lowerTrackWidth / 2;
                if (higherLayerRouteDir == RouteDirType::HORIZONTAL)
                {
                    // The segment is horizontal
                    ptArray.emplace_back(XY<LocType>(pt, hp.coordinate()));
                }
                else
                {
                    ptArray.emplace_back(XY<LocType>(hp.coordinate(), pt));
                }
            }
        }
        else
        {
            ptArray.emplace_back(hp.centerXY());
        }
    }
    Assert(ptArray.size() > 0);
#endif
    return true;
}

bool DetailedPreProcessing::addPinToGlobalResult()
{
    for (IndexType netIdx = 0; netIdx < _db.drDB().numNets(); ++netIdx)
    {
        const auto &net = _db.drDB().net(netIdx);
        for (IndexType pinIdx = 0; pinIdx < net.numPins(); ++ pinIdx)
        {
            const auto &pin = _db.drDB().pin(net.pin(pinIdx));
            // Add pin search points to the global routing solution
            _db.drDB().roughGrid().addLocationToGlobalRouting(netIdx, pin.loc(), pin.layer()); 
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
