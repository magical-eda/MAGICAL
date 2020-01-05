/**
 * @file IdeaPlaceEx.cpp
 * @brief The pybind11 interface for the core placement engine api
 * @author Keren Zhu
 * @date 12/16/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "main/IdeaPlaceEx.h"

namespace py = pybind11;
void initIdeaPlaceExAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::IdeaPlaceEx>(m , "IdeaPlaceEx")
        .def(py::init<>())
        .def("solve", &PROJECT_NAMESPACE::IdeaPlaceEx::solve, "Solve the problem")
        .def("readTechSimpleFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readTechSimpleFile)
        .def("readPinFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readPinFile)
        .def("readConnectionFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readConnectionFile)
        .def("readNetWgtFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readNetWgtFile)
        .def("readSymFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readSymFile)
        .def("readGdsLayout", &PROJECT_NAMESPACE::IdeaPlaceEx::readGdsLayout)
        .def("addGdsLayer", &PROJECT_NAMESPACE::IdeaPlaceEx::addGdsLayer, py::arg("cellIdx") = PROJECT_NAMESPACE::INDEX_TYPE_MAX)
        .def("finishAddingGdsLayer", &PROJECT_NAMESPACE::IdeaPlaceEx::finishAddingGdsLayer)
        .def("allocateCell", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateCell)
        .def("setCellName", &PROJECT_NAMESPACE::IdeaPlaceEx::setCellName)
        .def("allocatePin", &PROJECT_NAMESPACE::IdeaPlaceEx::allocatePin)
        .def("addPinShape", &PROJECT_NAMESPACE::IdeaPlaceEx::addPinShape)
        .def("allocateNet", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateNet)
        .def("addPinToNet", &PROJECT_NAMESPACE::IdeaPlaceEx::addPinToNet)
        .def("setNetWgt", &PROJECT_NAMESPACE::IdeaPlaceEx::setNetWgt)
        .def("allocateSymGrp", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateSymGrp)
        .def("addSymPair", &PROJECT_NAMESPACE::IdeaPlaceEx::addSymPair)
        .def("addSelfSym", &PROJECT_NAMESPACE::IdeaPlaceEx::addSelfSym)
        .def("addCellShape", &PROJECT_NAMESPACE::IdeaPlaceEx::addCellShape)
        .def("numCells", &PROJECT_NAMESPACE::IdeaPlaceEx::numCells)
        .def("cellIdxName", &PROJECT_NAMESPACE::IdeaPlaceEx::cellIdxName)
        .def("cellName", &PROJECT_NAMESPACE::IdeaPlaceEx::cellName)
        .def("pinIdx", &PROJECT_NAMESPACE::IdeaPlaceEx::pinIdx)
        .def("xCellLoc", &PROJECT_NAMESPACE::IdeaPlaceEx::xCellLoc)
        .def("yCellLoc", &PROJECT_NAMESPACE::IdeaPlaceEx::yCellLoc);
}
