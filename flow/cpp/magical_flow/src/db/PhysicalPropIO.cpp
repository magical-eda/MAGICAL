#include <pybind11/pybind11.h>
#include "PhysicalProp.h"

namespace py = pybind11;

PYBIND11_MODULE(magicalFlow, m)
{
    py::class_<PROJECT_NAMESPACE::MosProp>(m , "MosProp")
        .def(py::init<>())
        .def_property("_length", &PROJECT_NAMESPACE::MosProp::length, &PROJECT_NAMESPACE::MosProp::setLength)
        .def_property("_width", &PROJECT_NAMESPACE::MosProp::width, &PROJECT_NAMESPACE::MosProp::setWidth);
}
