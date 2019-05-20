/*! @file db/Bias.cpp
    @brief Bias implementation.
    @author Mingjie Liu
    @date 12/11/2018
*/

#include "db/Bias.h"

PROJECT_NAMESPACE_BEGIN

void Bias::init()
{
    _netlist.getInstNetConn(_bias, _netId);
    _netlist.fltrInstNetConnPinType(_bias, _netId, PinType::GATE);
    _driver = _bias;
    _netlist.fltrInstMosType(_driver, MosType::DIODE);
    if (valid())
    {
        _netlist.fltrInstType(_bias, _netlist.inst(_driver[0]).type());
    }
}

PROJECT_NAMESPACE_END
