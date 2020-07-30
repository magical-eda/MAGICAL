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

    py::class_<PROJECT_NAMESPACE::TechDB>(m, "TechDB")
        .def(py::init())
        .def("units", &PROJECT_NAMESPACE::TechDB::units, py::return_value_policy::reference, "Get units for techDB")
        .def("numLayers", &PROJECT_NAMESPACE::TechDB::numLayers, "Get the number of layers")
        .def("dbLayerToPdk", &PROJECT_NAMESPACE::TechDB::dbLayerToPdk, "Convert db layer index to pdk layer ID")
        .def("pdkLayerToDb", &PROJECT_NAMESPACE::TechDB::pdkLayerToDb, "Convert PDK layer ID to db layer index")
        .def("layerNameToIdx", &PROJECT_NAMESPACE::TechDB::layerNameToIdx, "Convert layer name to db layer index");
}
