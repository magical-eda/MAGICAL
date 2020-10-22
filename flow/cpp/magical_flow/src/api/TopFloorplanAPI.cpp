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
      .def("initProblem", &PROJECT_NAMESPACE::TopFloorplanProblem::initProblem, "Init the problem")
      .def("setPinResourceLen", &PROJECT_NAMESPACE::TopFloorplanProblem::setPinResourceLen, "Set which length of yLen will create unit of pin resource");
  py::class_<PROJECT_NAMESPACE::TopFloorplanProblemResult>(m, "TopFloorplanProblemResult")
      .def(py::init<>())
      .def("pinAssignSol", &PROJECT_NAMESPACE::TopFloorplanProblemResult::pinAssignSol, "Get the pin assignment")
      .def("targetModuleHeight", &PROJECT_NAMESPACE::TopFloorplanProblemResult::targetModuleHeight, "Get the target module height");
  py::class_<PROJECT_NAMESPACE::IlpTopFloorplanProblem>(m, "IlpTopFloorplanProblem")
      .def(py::init<const PROJECT_NAMESPACE::TopFloorplanProblem &>())
      .def("solve", &PROJECT_NAMESPACE::IlpTopFloorplanProblem::solve, "Solve the problem")
      .def("writeOut", &PROJECT_NAMESPACE::IlpTopFloorplanProblem::writeOut, "Write out the solution");
  m.def("applyFpSolution", &PROJECT_NAMESPACE::FP::applyFpSolution,  "apply floorplan solution to design db");

}
