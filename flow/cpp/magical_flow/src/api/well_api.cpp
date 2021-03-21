/**
 * @file well_api.cpp
 * @brief Initialize the pybind module related to wells
 * @author Keren Zhu
 * @date 03/16/2021
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include "well/wellgan_util.hpp"

namespace py = pybind11;

void initWellAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::DataWellGAN>(m , "DataWellGAN")
        .def(py::init<PROJECT_NAMESPACE::DesignDB &, const PROJECT_NAMESPACE::IntType>())
        .def("construct", &PROJECT_NAMESPACE::DataWellGAN::construct, "Construct the image for the circuit")
        .def("layoutOffset", &PROJECT_NAMESPACE::DataWellGAN::layoutOffset, "Get the offset of the shapes compared to layout lower left coordinate")
        .def("numPchOdRects", &PROJECT_NAMESPACE::DataWellGAN::numPchOdRects, "Get the number of PMOS OD rectangle shapes")
        .def("odPchRect", &PROJECT_NAMESPACE::DataWellGAN::odPchRect, "Get a OD rectangle shape for PMOS ODs")
        .def("numOtherOdRects", &PROJECT_NAMESPACE::DataWellGAN::numOtherOdRects, "Get the number of \"other\" OD rectangle shapes")
        .def("odOtherRect", &PROJECT_NAMESPACE::DataWellGAN::odOtherRect, "Get a OD rectangle shape for \"other\" ODs")
        ;
};
