/**
 * @file ConstGenAPI.cpp
 * @brief The pybind11 interface for the constraint generation
 * @author Mingjie Liu
 * @date 03/18/2020
 */

#include <pybind11/pybind11.h>
#include "main/DefParse.h"

namespace py = pybind11;
void initDefParseAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::DefParse>(m , "DefParse")
        .def(py::init<>())
        .def("readDefFile", &PROJECT_NAMESPACE::DefParse::readDefFile, "Read def file.")
        .def("numPins", &PROJECT_NAMESPACE::DefParse::numPins, "Number of pins.")
        .def("pinName", &PROJECT_NAMESPACE::DefParse::pinName, "Pin name.")
        .def("pinLayer", &PROJECT_NAMESPACE::DefParse::pinLayer, "Pin Layer.")
        .def("pinXLo", &PROJECT_NAMESPACE::DefParse::pinXLo, "Pin XLo.")
        .def("pinYLo", &PROJECT_NAMESPACE::DefParse::pinYLo, "Pin YLo.")
        .def("pinXHi", &PROJECT_NAMESPACE::DefParse::pinXHi, "Pin XHi.")
        .def("pinYHi", &PROJECT_NAMESPACE::DefParse::pinYHi, "Pin YHi.");
}
