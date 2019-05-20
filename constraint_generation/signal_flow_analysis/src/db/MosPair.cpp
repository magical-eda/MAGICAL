/*! @file db/MosPair.cpp
    @brief MosPair implementation.
    @author Mingjie Liu
    @date 11/27/2018
*/

#include "db/MosPair.h"

PROJECT_NAMESPACE_BEGIN

bool MosPair::isEqual(const MosPair & right) const
{
    if (_mosId1 == right.mosId1() && _mosId2 == right.mosId2())
        return true;
    if (_mosId1 == right.mosId2() && _mosId2 == right.mosId1())
        return true;
    return false;
}

PROJECT_NAMESPACE_END
