/**
 * @file GlobalAPI.cpp
 * @brief The Python interface for the classes defined in global.h
 * @author Keren Zhu, Mingjie Liu
 * @date 06/24/2019
 */

#include <pybind11/pybind11.h>
#include "global/global.h"

namespace py = pybind11;

void initGlobalAPI(py::module &m)
{
    // See https://stackoverflow.com/questions/47893832/pybind11-global-level-enum
    py::enum_<PROJECT_NAMESPACE::OriType>(m, "OriType")
        .value("OriTypeN", PROJECT_NAMESPACE::OriType::N)
        .value("OriTypeS", PROJECT_NAMESPACE::OriType::S)
        .value("OriTypeW", PROJECT_NAMESPACE::OriType::W)
        .value("OriTypeE", PROJECT_NAMESPACE::OriType::E)
        .value("OriTypeFN", PROJECT_NAMESPACE::OriType::FN)
        .value("OriTypeFS", PROJECT_NAMESPACE::OriType::FS)
        .value("OriTypeFW", PROJECT_NAMESPACE::OriType::FW)
        .value("OriTypeFE", PROJECT_NAMESPACE::OriType::FE)
        .export_values();

    py::enum_<PROJECT_NAMESPACE::ImplType>(m, "ImplType")
        .value("ImplTypeUNSET", PROJECT_NAMESPACE::ImplType::UNSET)
        .value("ImplTypePCELL_Pch", PROJECT_NAMESPACE::ImplType::PCELL_Pch)
        .value("ImplTypePCELL_Nch", PROJECT_NAMESPACE::ImplType::PCELL_Nch)
//        .value("ImplTypePCELL_Nch25ud18mac", PROJECT_NAMESPACE::ImplType::PCELL_Nch25ud18mac)
        .value("ImplTypePCELL_Res", PROJECT_NAMESPACE::ImplType::PCELL_Res)
        .value("ImplTypePCELL_Cap", PROJECT_NAMESPACE::ImplType::PCELL_Cap)
        .export_values();
    
    py::enum_<PROJECT_NAMESPACE::PinType>(m, "PinType")
        .value("UNSET", PROJECT_NAMESPACE::PinType::UNSET)
        .value("PSUB", PROJECT_NAMESPACE::PinType::PSUB)
        .value("NWELL", PROJECT_NAMESPACE::PinType::NWELL)
        .export_values();
 
    m.def("orientConv", &PROJECT_NAMESPACE::MfUtil::orientConv, "convert coordinates under different offset and orientation",
            py::arg_v("coord", PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType>(0,0), "XYLoc(0, 0)"), 
            py::arg_v("orient", PROJECT_NAMESPACE::OriType::N, "OriType::N"), 
            py::arg_v("offset", PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType>(0, 0), "XYLoc(0, 0)"), 
            py::arg_v("bbox", PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>(0,0,0,0)), "BoxLoc(0, 0)");

    m.def("isImplTypeDevice", &PROJECT_NAMESPACE::MfUtil::isImplTypeDevice, "Determine whether the implementation type is a device or subckt");
}
