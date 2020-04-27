/**
 * @file CSFlowAPI.cpp
 * @brief The Python interface for CSFlow
 * @author Hao Chen
 * @date 04/01/2020
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "csflow/CSFlow.h"

namespace py = pybind11;

void initCSFlowAPI(py::module &m) {
  py::class_<PROJECT_NAMESPACE::CSFlow>(m, "CSFlow")
    .def(py::init<PROJECT_NAMESPACE::DesignDB&>())
    .def("computeCurrentFlow", &PROJECT_NAMESPACE::CSFlow::computeCurrentFlow)
    .def("computeSignalFlow", &PROJECT_NAMESPACE::CSFlow::computeSignalFlow)
    .def("numCurrentPaths", &PROJECT_NAMESPACE::CSFlow::numCurrentPaths)
    .def("currentPinPath", &PROJECT_NAMESPACE::CSFlow::currentPinPath)
    .def("currentCellPath", &PROJECT_NAMESPACE::CSFlow::currentCellPath)
    .def("currentPinPaths", &PROJECT_NAMESPACE::CSFlow::currentPinPaths)
    .def("currentCellPaths", &PROJECT_NAMESPACE::CSFlow::currentCellPaths);
}
