/**
 * @file TopFloorplanAPI.cpp
 * @brief The Python interface for Top level floorplan
 * @author Keren Zhu
 * @date 10/07/2020
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "floorplan/top_floorplan.hpp"

namespace py = pybind11;

void initTopFloorplanAPI(py::module &m) {
  py::class_<PROJECT_NAMESPACE::TopFloorplanProblem>(m, "TopFloorplanProblem")
    .def(py::init<>())
    .def("initProblem", &PROJECT_NAMESPACE::TopFloorplanProblem::initProblem, "Init the problem");
  py::class_<PROJECT_NAMESPACE::IlpTopFloorplanProblem>(m, "IlpTopFloorplanProblem")
    .def(py::init<const PROJECT_NAMESPACE::TopFloorplanProblem &>())
    .def("solve", &PROJECT_NAMESPACE::IlpTopFloorplanProblem::solve, "Solve the problem");
}
