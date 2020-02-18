/**
 * @file GraphComponentsAPI.cpp
 * @brief The Python interface for the classes defined in GraphComponents.h
 * @author Keren Zhu, Mingjie Liu
 * @date 06/22/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "db/GraphComponents.h"

namespace py = pybind11;

void initGraphComponentsAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::GdsData>(m, "GdsData")
        .def(py::init<>())
        .def_property("gdsFile", &PROJECT_NAMESPACE::GdsData::gdsFile, &PROJECT_NAMESPACE::GdsData::setGdsFile)
        .def("bbox", &PROJECT_NAMESPACE::GdsData::bbox, py::return_value_policy::reference)
        .def("setBBox", &PROJECT_NAMESPACE::GdsData::setBBox);

    py::class_<PROJECT_NAMESPACE::CktNode>(m , "CktNode")
        .def(py::init<>())
        .def_property("graphIdx", &PROJECT_NAMESPACE::CktNode::subgraphIdx, &PROJECT_NAMESPACE::CktNode::setSubgraphIdx)
        .def("appendPinIdx", &PROJECT_NAMESPACE::CktNode::appendPinIdx)
        .def("numPins", &PROJECT_NAMESPACE::CktNode::numPins)
        .def("pinIdx", &PROJECT_NAMESPACE::CktNode::pinIdx)
        .def_property("isImpl", &PROJECT_NAMESPACE::CktNode::isImpl, &PROJECT_NAMESPACE::CktNode::setIsImpl)
        .def_property("flipVertFlag", &PROJECT_NAMESPACE::CktNode::flipVertFlag, &PROJECT_NAMESPACE::CktNode::setFlipVertFlag)
        .def("isLeaf", &PROJECT_NAMESPACE::CktNode::isLeaf)
        .def("offset", &PROJECT_NAMESPACE::CktNode::offset, py::return_value_policy::reference)
        .def("setOffset", &PROJECT_NAMESPACE::CktNode::setOffset)
        .def_property("implType", &PROJECT_NAMESPACE::CktNode::implType, &PROJECT_NAMESPACE::CktNode::setImplType)
        .def_property("refName", &PROJECT_NAMESPACE::CktNode::refName, &PROJECT_NAMESPACE::CktNode::setRefName)
        .def_property("name", &PROJECT_NAMESPACE::CktNode::name, &PROJECT_NAMESPACE::CktNode::setName)
        .def("setOrient", &PROJECT_NAMESPACE::CktNode::setOrient)
        .def("orient", &PROJECT_NAMESPACE::CktNode::orient, py::return_value_policy::reference);

    py::class_<PROJECT_NAMESPACE::Net>(m, "Net")
        .def(py::init<>())
        .def("appendPinIdx", &PROJECT_NAMESPACE::Net::appendPinIdx)
        .def("appendSubIdx", &PROJECT_NAMESPACE::Net::appendSubIdx)
        .def("numPins", &PROJECT_NAMESPACE::Net::numPins)
        .def("numSubs", &PROJECT_NAMESPACE::Net::numSubs)
        .def_property("name", &PROJECT_NAMESPACE::Net::name, &PROJECT_NAMESPACE::Net::setName)
        .def_property("ioPos", &PROJECT_NAMESPACE::Net::ioPos, &PROJECT_NAMESPACE::Net::setIoPos)
        .def("isIo", &PROJECT_NAMESPACE::Net::isIo)
        .def("isSub", &PROJECT_NAMESPACE::Net::isSub)
        .def("pinIdx", &PROJECT_NAMESPACE::Net::pinIdx)
        .def("setIoShape", &PROJECT_NAMESPACE::Net::setIoShape)
        .def("ioShape", &PROJECT_NAMESPACE::Net::ioShape, py::return_value_policy::reference)
        .def_property("ioLayer", &PROJECT_NAMESPACE::Net::ioLayer, &PROJECT_NAMESPACE::Net::setIoLayer);

    py::class_<PROJECT_NAMESPACE::Pin>(m, "Pin")
        .def(py::init())
        .def_property("nodeIdx", &::PROJECT_NAMESPACE::Pin::nodeIdx, &PROJECT_NAMESPACE::Pin::setNodeIdx)
        .def_property("intNetIdx", &PROJECT_NAMESPACE::Pin::intNetIdx, &PROJECT_NAMESPACE::Pin::setIntNetIdx)
        .def_property("netIdx", &PROJECT_NAMESPACE::Pin::netIdx, &PROJECT_NAMESPACE::Pin::setNetIdx)
        .def_property("pinType", &PROJECT_NAMESPACE::Pin::pinType, &PROJECT_NAMESPACE::Pin::setPinType)
        .def_property("valid", &PROJECT_NAMESPACE::Pin::valid, &PROJECT_NAMESPACE::Pin::setValid)
        .def("isConnected", &PROJECT_NAMESPACE::Pin::isConnected)
        .def("numLayoutRects", &PROJECT_NAMESPACE::Pin::numLayoutRects, "the number of rectangles in Layout")
        .def("layoutRectIdx", &PROJECT_NAMESPACE::Pin::layoutRectIdx, "get one layout rectangle index of this pin")
        .def("addLayoutRectIdx", &PROJECT_NAMESPACE::Pin::addLayoutRectIdx, "add a new rectangle index in _layoutRectIdx");
        
}
