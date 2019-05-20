#include "Pin.h"

PROJECT_NAMESPACE_BEGIN

bool PinPolygon::calculateBBox()
{
    if (_rect.size() > 0)
    {
        _bbox = _rect.at(0);
    }
    for (const auto &rect : _rect)
    {
        _bbox.unionBox(rect);
    }
    return true;
}

bool PinPolygon::pickLoc()
{
    /// Find the rectangle shape that is most closed to the center of pin bounding box
    XY<LocType> bboxCenter = _bbox.center();
    LocType minDist = LOC_TYPE_MAX; //< The max distance from the bbox center to the shape center
    IndexType chosenIdx = 0; //< The index of the rectangle that choosed
    for (IndexType idx = 0; idx < _rect.size(); ++idx)
    {
        LocType currentDist = ::klib::manhattanDistance<LocType>(bboxCenter, _rect.at(idx).center());
        if (currentDist < minDist)
        {
            minDist = currentDist; 
            chosenIdx = idx;
        }
    }
    /// Use the rectangle center point as the default location
    _defaultLoc = _rect.at(chosenIdx).center();
    _defaultLocShapeIdx = chosenIdx;
    return true;
}

PROJECT_NAMESPACE_END
