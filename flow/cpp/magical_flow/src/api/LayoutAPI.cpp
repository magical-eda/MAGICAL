/**
 * @file LayoutAPI.cpp
 * @brief The Python interface for the classes defined in Layout.h
 * @author Keren Zhu
 * @date 07/03/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "db/Layout.h"

namespace py = pybind11;

void initLayoutAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::LayoutObject> layoutObject(m, "LayoutObject");
    layoutObject.def(py::init<>());
        //.def_property("layer", &PROJECT_NAMESPACE::LayoutObject::layer, &PROJECT_NAMESPACE::LayoutObject::setLayer);

    py::class_<PROJECT_NAMESPACE::TextLayout>(m , "TextLayout", layoutObject)
        .def(py::init<>())
        .def_property("text", &PROJECT_NAMESPACE::TextLayout::text, &PROJECT_NAMESPACE::TextLayout::setText)
        .def("coord", &PROJECT_NAMESPACE::TextLayout::coord, py::return_value_policy::reference, "The coordinate of the text in the layout");

    py::class_<PROJECT_NAMESPACE::RectLayout>(m , "RectLayout", layoutObject)
        .def(py::init<>())
        .def("rect", py::overload_cast<>(&PROJECT_NAMESPACE::RectLayout::rect), py::return_value_policy::reference, "The rectangle representing the geometry");

    py::class_<PROJECT_NAMESPACE::LayoutLayer>(m , "LayoutLayer", layoutObject)
        .def(py::init<>())
        .def("rect", &PROJECT_NAMESPACE::LayoutLayer::rect, py::return_value_policy::reference, "The rectangle representing the geometry");

    py::class_<PROJECT_NAMESPACE::Layout>(m, "Layout")
        .def(py::init())
        .def("init", &PROJECT_NAMESPACE::Layout::init)
        .def("clear", &PROJECT_NAMESPACE::Layout::clear)
        .def("text", &PROJECT_NAMESPACE::Layout::text, py::return_value_policy::reference)
        .def("numLayers", &PROJECT_NAMESPACE::Layout::numLayers, py::return_value_policy::reference)
        .def("numRects", &PROJECT_NAMESPACE::Layout::numRects, py::return_value_policy::reference)
        .def("boundary", &PROJECT_NAMESPACE::Layout::boundary, py::return_value_policy::reference)
        .def("setBoundary", &PROJECT_NAMESPACE::Layout::setBoundary, py::return_value_policy::reference)
        .def("rect", &PROJECT_NAMESPACE::Layout::rect, py::return_value_policy::reference)
        .def("insertLayout", &PROJECT_NAMESPACE::Layout::insertLayout)
        .def("setRectDatatype", &PROJECT_NAMESPACE::Layout::setRectDatatype)
        .def("insertText", py::overload_cast<PROJECT_NAMESPACE::IndexType, const PROJECT_NAMESPACE::TextLayout &>(&PROJECT_NAMESPACE::Layout::insertText), "Insert a text object in the layout")
        .def("insertText", py::overload_cast<PROJECT_NAMESPACE::IndexType, const std::string &, const PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType> &>
                (&PROJECT_NAMESPACE::Layout::insertText), "Insert a text object in the layout")
        .def("insertText", py::overload_cast<PROJECT_NAMESPACE::IndexType, const std::string &, PROJECT_NAMESPACE::LocType, PROJECT_NAMESPACE::LocType>
                (&PROJECT_NAMESPACE::Layout::insertText), "Insert a text object in the layout")
        .def("insertRect", py::overload_cast<PROJECT_NAMESPACE::IndexType, const PROJECT_NAMESPACE::RectLayout &>
                (&PROJECT_NAMESPACE::Layout::insertRect), "Insert a rectangle object in the layout")
        .def("insertRect", py::overload_cast<PROJECT_NAMESPACE::IndexType, const PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType> &>
                (&PROJECT_NAMESPACE::Layout::insertRect), "Insert a rectangle object in the layout")
        .def("insertRect", py::overload_cast<PROJECT_NAMESPACE::IndexType, const PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType> &, const PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType> &>
                (&PROJECT_NAMESPACE::Layout::insertRect), "Insert a rectangle object in the layout")
        .def("insertRect", py::overload_cast<PROJECT_NAMESPACE::IndexType, PROJECT_NAMESPACE::LocType, PROJECT_NAMESPACE::LocType, PROJECT_NAMESPACE::LocType, PROJECT_NAMESPACE::LocType>
                (&PROJECT_NAMESPACE::Layout::insertRect), "Insert a rectangle object in the layout");
}
