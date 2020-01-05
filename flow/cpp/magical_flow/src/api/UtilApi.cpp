/**
 * @file GlobalAPI.cpp
 * @brief The Python interface for utilities
 * @author Keren Zhu
 * @date 06/24/2019
 */


#include <pybind11/pybind11.h>
#include "util/XY.h"
#include "global/global.h"

namespace py = pybind11;

void initUtilAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType>>(m , "XYLoc")
        .def(py::init<>())
        .def_property("x", &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType>::x, &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType>::setX)
        .def_property("y", &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType>::y, &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::LocType>::setY);

    py::class_<PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::IndexType>>(m , "XYIdx")
        .def(py::init<>())
        .def_property("x", &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::IndexType>::x, &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::IndexType>::setX)
        .def_property("y", &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::IndexType>::y, &PROJECT_NAMESPACE::XY<PROJECT_NAMESPACE::IndexType>::setY);

    py::class_<PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>>(m, "BoxLoc")
        .def(py::init<>())
        .def_property("xLo", &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::xLo, &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::setXLo)
        .def_property("yLo", &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::yLo, &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::setYLo)
        .def_property("xHi", &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::xHi, &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::setXHi)
        .def_property("yHi", &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::yHi, &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::setYHi)
        .def("xLen", &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::xLen)
        .def("yLen", &PROJECT_NAMESPACE::Box<PROJECT_NAMESPACE::LocType>::yLen);
}
