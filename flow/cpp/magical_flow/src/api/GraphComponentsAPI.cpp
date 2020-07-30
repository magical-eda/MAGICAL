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
        .def_property("ioLayer", &PROJECT_NAMESPACE::Net::ioLayer, &PROJECT_NAMESPACE::Net::setIoLayer)
        .def("addIoPin", &PROJECT_NAMESPACE::Net::addIoPin, " Add a new io pin shape to the net")
        .def("numIoPins", &PROJECT_NAMESPACE::Net::numIoPins, "Get the number of IO pin shapes in this net")
        .def("ioPinShape", &PROJECT_NAMESPACE::Net::ioPinShape, py::return_value_policy::reference, "Similar to ioShape(), but instead of returnning the first io pin shape, this is returning by the index of io pin shape")
        .def("ioPinMetalLayer", &PROJECT_NAMESPACE::Net::ioPinMetalLayer, "Similar to .ioLayer, but instead of returnning the first io pin shape, this is returning by the index of io pin shape")
        .def("isIoPowerStripe", &PROJECT_NAMESPACE::Net::isIoPowerStripe, "Get whether the io shape is a power stripe")
        .def("markIoPowerStripe", &PROJECT_NAMESPACE::Net::markIoPowerStripe, "Mark a io shape as power stripe")
        .def("markLastIoPowerStripe", &PROJECT_NAMESPACE::Net::markLastIoPowerStripe, "Mark the last io shape as power stripe")
        .def("isPower", &PROJECT_NAMESPACE::Net::isPower, "Get whether this net is a power net")
        .def("isVdd", &PROJECT_NAMESPACE::Net::isVdd, "Get whether this net is a vdd net")
        .def("isVss", &PROJECT_NAMESPACE::Net::isVss, "Get whether this net is a vss net")
        .def("markVddFlag", &PROJECT_NAMESPACE::Net::markVddFlag, "Mark this net as vdd net")
        .def("revokeVddFlag", &PROJECT_NAMESPACE::Net::revokeVddFlag, "Remove the vdd flag from this net")
        .def("markVssFlag", &PROJECT_NAMESPACE::Net::markVssFlag, "Mark this net as vss net")
        .def("revokeVssFlag", &PROJECT_NAMESPACE::Net::revokeVssFlag, "Remove the vss flag from this net")
        .def("isDigital", &PROJECT_NAMESPACE::Net::isDigital, "Get whether this net is a digital net")
        .def("isAnalog", &PROJECT_NAMESPACE::Net::isAnalog, "Get whether this net is a analog net")
        .def("markDigitalFlag", &PROJECT_NAMESPACE::Net::markDigitalFlag, "Mark this net as digital net")
        .def("markAnalogFlag", &PROJECT_NAMESPACE::Net::markAnalogFlag, "Mark this net as analog net")
        ;

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
