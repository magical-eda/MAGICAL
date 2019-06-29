/**
 * @file GraphComponentsAPI.cpp
 * @brief The Python interface for the classes defined in GraphComponents.h
 * @author Keren Zhu
 * @date 06/22/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "db/GraphComponents.h"

namespace py = pybind11;

void initGraphComponentsAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::CktNode>(m , "CktNode")
        .def(py::init<>())
        .def_property("graphIdx", &PROJECT_NAMESPACE::CktNode::subgraphIdx, &PROJECT_NAMESPACE::CktNode::setSubgraphIdx)
        .def("appendPinIdx", &PROJECT_NAMESPACE::CktNode::appendPinIdx)
        .def("numPins", &PROJECT_NAMESPACE::CktNode::numPins)
        .def("pinIdx", &PROJECT_NAMESPACE::CktNode::pinIdx)
        .def_property("isImpl", &PROJECT_NAMESPACE::CktNode::isImpl, &PROJECT_NAMESPACE::CktNode::setIsImpl)
        .def("isLeaf", &PROJECT_NAMESPACE::CktNode::isLeaf)
        .def("offset", &PROJECT_NAMESPACE::CktNode::offset, py::return_value_policy::reference)
        .def_property("implType", &PROJECT_NAMESPACE::CktNode::implType, &PROJECT_NAMESPACE::CktNode::setImplType)
        .def("setOrient", &PROJECT_NAMESPACE::CktNode::setOrient)
        .def("orient", &PROJECT_NAMESPACE::CktNode::orient, py::return_value_policy::reference);

    py::class_<PROJECT_NAMESPACE::Net>(m, "Net")
        .def(py::init<>())
        .def("appendPinIdx", &PROJECT_NAMESPACE::Net::appendPinIdx)
        .def("numPins", &PROJECT_NAMESPACE::Net::numPins)
        .def("pinIdx", &PROJECT_NAMESPACE::Net::pinIdx);

    py::class_<PROJECT_NAMESPACE::Pin>(m, "Pin")
        .def(py::init())
        .def_property("nodeIdx", &::PROJECT_NAMESPACE::Pin::nodeIdx, &PROJECT_NAMESPACE::Pin::setNodeIdx)
        .def_property("intPinIdx", &PROJECT_NAMESPACE::Pin::intPinIdx, &PROJECT_NAMESPACE::Pin::setIntPinIdx)
        .def("numNets", &PROJECT_NAMESPACE::Pin::numNets)
        .def("netIdx", &PROJECT_NAMESPACE::Pin::netIdx)
        .def("appendNetIdx", &PROJECT_NAMESPACE::Pin::appendNetIdx);
}
