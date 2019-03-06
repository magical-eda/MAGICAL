/// The penalty version of the implementation
/*
#include "ShapeMapBase.h"

PROJECT_NAMESPACE_BEGIN

inanoynmouse CheckResult ShapeMap::metalSpacingChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType layer) const 
{
    auto bundle = DRUtil::spacingCheckBundle(lhs, rhs); ///.first.first: spacing, .first.second: parallel run length, .second: pair of box width
    LocType spacing = bundle.first.first;
    LocType paraLen = bundle.first.second;
    LocType width = std::max(bundle.second.first, bundle.second.second);

    /// If violated the min spacing rule, return illegal
    if (spacing < _instr.metalMinSpacing(layer))
    {
        return CheckResult::ILLEGAL;
    }

    /// Query the required spacing for the 
    if (spacing < _instr.metalSpacing(width, paraLen, layer))
    {
        return CheckResult::PENALTY;
    }

    return CheckResult::LEGAL;
}

inanoynmouse CheckResult ShapeMap::metalMinAreaChecking(const Box<LocType> &wireShape, IndexType layer) const 
{
    /// Minimum area check
    LocType wireArea = wireShape.area();
    if (wireArea < _instr.metalMinArea(layer))
    {
        return CheckResult::PENALTY;
    }

    return CheckResult::LEGAL;
}
PROJECT_NAMESPACE_END
*/
