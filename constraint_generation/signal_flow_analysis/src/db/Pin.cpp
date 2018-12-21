/*! @file db/Pin.cpp
    @brief Net class implementation
    @author Mingjie Liu
    @date 11/24/2018
*/
#include "db/Pin.h"

PROJECT_NAMESPACE_BEGIN

/*! @param type Querry the next search PinType.
    @see #PinType

    The DFS search for symmetry relys on 
    Pin::nextPinType to define the search path 
    direction. For example, if a Mosfet was reached
    through a source then the DFS algorithm would 
    search for connected Inst of the drain.
    Currently supported search paths:
    | Input PinType   | nextPinType   |
    | --------------- | ------------- |
    | SOURCE          | DRAIN         |
    | GATE            | DRAIN         |
    | DRAIN           | SOURCE        |
    | THIS            | THAT          |
    | THAT            | THIS          |
*/
PinType Pin::nextPinType(PinType type) 
{
    if (type == PinType::SOURCE ||
        type == PinType::GATE)
        return PinType::DRAIN;
    if (type == PinType::DRAIN)
        return PinType::SOURCE;
    if (type == PinType::THIS)
        return PinType::THAT;
    if (type == PinType::THAT)
        return PinType::THIS;
    return PinType::OTHER;
}

/*! 
    A pin is said to belong to a passive 
    device if the PinType is PinType::THIS 
    or PinType::THAT.
*/
bool Pin::isPasvDev(PinType type)
{
    return type == PinType::THIS || type == PinType::THAT;
}

PROJECT_NAMESPACE_END

