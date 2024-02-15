/**
 * @file ConstGenAPI.cpp
 * @brief The pybind11 interface for the constraint generation
 * @author Mingjie Liu
 * @date 03/18/2020
 */

#include <pybind11/pybind11.h>
#include "main/ConstGen.h"

namespace py = pybind11;
void initConstGenAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::ConstGen>(m , "ConstGen")
        .def(py::init<>())
        .def("addNet", &PROJECT_NAMESPACE::ConstGen::addNet, "Add net to netlist.")
        .def("addInst", &PROJECT_NAMESPACE::ConstGen::addInst, "Add instance to netlist.")
        .def("addInstPin", &PROJECT_NAMESPACE::ConstGen::addInstPin, "Add instance pin.")
        .def("dumpResult", &PROJECT_NAMESPACE::ConstGen::dumpResult, "Dump result to file.");
}
