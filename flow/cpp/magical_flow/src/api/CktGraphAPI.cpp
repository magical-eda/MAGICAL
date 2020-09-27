/**
 * @file CktGraphAPI.cpp
 * @brief The Python interface for the CktGraph
 * @author Keren Zhu, Mingjie Liu
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
        .def("setTechDB", &PROJECT_NAMESPACE::CktGraph::setTechDB)
        .def("allocateNode", &PROJECT_NAMESPACE::CktGraph::allocateNode)
        .def("numNodes", &PROJECT_NAMESPACE::CktGraph::numNodes)
        .def("node", &PROJECT_NAMESPACE::CktGraph::node, py::return_value_policy::reference)
        .def("allocatePin", &PROJECT_NAMESPACE::CktGraph::allocatePin)
        .def("resizeNodes", &PROJECT_NAMESPACE::CktGraph::resizeNodeArray, "Resize the node vector in this circuit")
        .def("numPins", &PROJECT_NAMESPACE::CktGraph::numPins)
        .def("pin", &PROJECT_NAMESPACE::CktGraph::pin, py::return_value_policy::reference)
        .def("allocateNet", &PROJECT_NAMESPACE::CktGraph::allocateNet)
        .def("allocatePsub", &PROJECT_NAMESPACE::CktGraph::allocatePsub)
        .def("addPsubIdx", &PROJECT_NAMESPACE::CktGraph::addPsubIdx)
        .def("allocateNwell", &PROJECT_NAMESPACE::CktGraph::allocateNwell)
        .def("addNwellIdx", &PROJECT_NAMESPACE::CktGraph::addNwellIdx)
        .def("numNets", &PROJECT_NAMESPACE::CktGraph::numNets)
        .def("numPsubs", &PROJECT_NAMESPACE::CktGraph::numPsubs)
        .def("numNwells", &PROJECT_NAMESPACE::CktGraph::numNwells)
        .def("net", &PROJECT_NAMESPACE::CktGraph::net, py::return_value_policy::reference)
        .def("psub", &PROJECT_NAMESPACE::CktGraph::psub, py::return_value_policy::reference)
        .def("nwell", &PROJECT_NAMESPACE::CktGraph::nwell, py::return_value_policy::reference)
        .def_property("name", &PROJECT_NAMESPACE::CktGraph::name, &PROJECT_NAMESPACE::CktGraph::setName)
        .def("layout", &PROJECT_NAMESPACE::CktGraph::layout, py::return_value_policy::reference)
        .def("parseGDS", &PROJECT_NAMESPACE::CktGraph::parseGDS, py::return_value_policy::reference)
        .def_property("implType", &PROJECT_NAMESPACE::CktGraph::implType, &PROJECT_NAMESPACE::CktGraph::setImplType) 
        .def_property("implIdx", &PROJECT_NAMESPACE::CktGraph::implIdx, &PROJECT_NAMESPACE::CktGraph::setImplIdx)
        .def_property("isImpl", &PROJECT_NAMESPACE::CktGraph::isImpl, &PROJECT_NAMESPACE::CktGraph::setIsImpl)
        .def("GdsData", &::MAGICAL_FLOW::CktGraph::gdsData, py::return_value_policy::reference)
        .def("gdsData", &::MAGICAL_FLOW::CktGraph::gdsData, py::return_value_policy::reference);
}
