/**
 * @file CktGraphAPI.cpp
 * @brief The Python interface for the CktGraph
 * @author Keren Zhu
 * @date 06/25/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "db/CktGraph.h"

namespace py = pybind11;

void initCktGraphAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::CktGraph>(m , "CktGraph")
        .def(py::init<>())
        .def("allocateNode", &PROJECT_NAMESPACE::CktGraph::allocateNode)
        .def("numNodes", &PROJECT_NAMESPACE::CktGraph::numNodes)
        .def("node", &PROJECT_NAMESPACE::CktGraph::node, py::return_value_policy::reference)
        .def("allocatePin", &PROJECT_NAMESPACE::CktGraph::allocatePin)
        .def("numPins", &PROJECT_NAMESPACE::CktGraph::numPins)
        .def("pin", &PROJECT_NAMESPACE::CktGraph::pin, py::return_value_policy::reference)
        .def("allocateNet", &PROJECT_NAMESPACE::CktGraph::allocateNet)
        .def("numNets", &PROJECT_NAMESPACE::CktGraph::numNets)
        .def("net", &PROJECT_NAMESPACE::CktGraph::net, py::return_value_policy::reference)
        .def_property("name", &PROJECT_NAMESPACE::CktGraph::name, &PROJECT_NAMESPACE::CktGraph::setName)
        .def("layout", &PROJECT_NAMESPACE::CktGraph::layout, py::return_value_policy::reference)
        .def_property("implType", &PROJECT_NAMESPACE::CktGraph::implType, &PROJECT_NAMESPACE::CktGraph::setImplType)
        .def_property("implIdx", &PROJECT_NAMESPACE::CktGraph::implIdx, &PROJECT_NAMESPACE::CktGraph::setImplIdx)
        .def("GdsData", &::MAGICAL_FLOW::CktGraph::gdsData, py::return_value_policy::reference);
}
