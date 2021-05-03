/**
 * @file TechDbAPI.cpp
 * @brief The Python interface for TechDB
 * @author Keren Zhu
 * @date 07/06/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "db/TechDB.h"

namespace py = pybind11;

void initTechDbAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::TechUnit>(m, "TechUnit")
        .def(py::init())
        .def_property("dbu", &PROJECT_NAMESPACE::TechUnit::dbu, &PROJECT_NAMESPACE::TechUnit::setDbu);
    py::class_<PROJECT_NAMESPACE::SameLayerSpacingRule>(m, "SameLayerSpacingRule")
      .def(py::init<PROJECT_NAMESPACE::IndexType, PROJECT_NAMESPACE::LocType>())
      .def("layerIdx", &PROJECT_NAMESPACE::SameLayerSpacingRule::layerIdx)
      .def("spacing", &PROJECT_NAMESPACE::SameLayerSpacingRule::spacing)
      ;
    py::class_<PROJECT_NAMESPACE::InterLayerSpacingRule>(m, "InterLayerSpacingRule")
      .def(py::init<PROJECT_NAMESPACE::IndexType, PROJECT_NAMESPACE::IndexType, PROJECT_NAMESPACE::LocType>())
      .def("layer1", &PROJECT_NAMESPACE::InterLayerSpacingRule::layer1)
      .def("layer2", &PROJECT_NAMESPACE::InterLayerSpacingRule::layer2)
      .def("spacing", &PROJECT_NAMESPACE::InterLayerSpacingRule::spacing)
      ;

    py::class_<PROJECT_NAMESPACE::TechDB>(m, "TechDB")
        .def(py::init())
        .def("units", &PROJECT_NAMESPACE::TechDB::units, py::return_value_policy::reference, "Get units for techDB")
        .def("numLayers", &PROJECT_NAMESPACE::TechDB::numLayers, "Get the number of layers")
        .def("dbLayerToPdk", &PROJECT_NAMESPACE::TechDB::dbLayerToPdk, "Convert db layer index to pdk layer ID")
        .def("pdkLayerToDb", &PROJECT_NAMESPACE::TechDB::pdkLayerToDb, "Convert PDK layer ID to db layer index")
        .def("layerNameToIdx", &PROJECT_NAMESPACE::TechDB::layerNameToIdx, "Convert layer name to db layer index")
        .def("addSameLayerSpacingRule", &PROJECT_NAMESPACE::TechDB::addSameLayerSpacingRule, "Add spacing rule for same layer")
        .def("numSameLayerSpacingRules", &PROJECT_NAMESPACE::TechDB::numSameLayerSpacingRules, "Get the number of same layer spacing rules")
        .def("sameLayerSpacingRule", &PROJECT_NAMESPACE::TechDB::sameLayerSpacingRule, "Get a same layer spacing rule")
        .def("addInterLayerSpacingRule", &PROJECT_NAMESPACE::TechDB::addInterLayerSpacingRule, "Add spacing rule for different layer")
        .def("numInterLayerSpacingRules", &PROJECT_NAMESPACE::TechDB::numInterLayerSpacingRules, "Get the number of inter layer spacing rules")
        .def("interLayerSpacingRule", &PROJECT_NAMESPACE::TechDB::interLayerSpacingRule, "Get a inter layer spacing rule")
        .def("setNwellLayerName", &PROJECT_NAMESPACE::TechDB::setNwellLayerName, "Set the layer name for N-well")
        .def("isNwellLayerSet", &PROJECT_NAMESPACE::TechDB::isNwellLayerSet, "Get whether the N-well layer is specified")
        .def("nwellLayerIdx", &PROJECT_NAMESPACE::TechDB::nwellLayerIdx, "Get the layer index for the N-well layer")
        ;
}
