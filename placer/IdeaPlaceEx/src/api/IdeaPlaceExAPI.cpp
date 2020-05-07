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
        .def("alignToGrid", &PROJECT_NAMESPACE::IdeaPlaceEx::alignToGrid, "Align the placement to grid")
        .def("readTechSimpleFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readTechSimpleFile, "Internal usage: Read in the techsimple file")
        .def("readPinFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readPinFile, "Internal usage: Read in the .pin file")
        .def("readConnectionFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readConnectionFile, "Internal usage: Read in the .connection file")
        .def("readNetWgtFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readNetWgtFile, "Internal usage: Read in the .netwgt file")
        .def("readSymFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readSymFile, "Internal usage: Read in the .sym file")
        .def("readGdsLayout", &PROJECT_NAMESPACE::IdeaPlaceEx::readGdsLayout, "Internal Usage: Read in a gds for cell, the name of the GDS cell need to match the cell name")
        .def("addGdsLayer", &PROJECT_NAMESPACE::IdeaPlaceEx::addGdsLayer, py::arg("cellIdx") = PROJECT_NAMESPACE::INDEX_TYPE_MAX, "Add a gds to a cell")
        .def("finishAddingGdsLayer", &PROJECT_NAMESPACE::IdeaPlaceEx::finishAddingGdsLayer, "Finish the gds layer adding, trigger a init function")
        .def("allocateCell", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateCell, "Allocate a new cell, return the index of the cell")
        .def("setCellName", &PROJECT_NAMESPACE::IdeaPlaceEx::setCellName, "Set the name of a cell")
        .def("allocatePin", &PROJECT_NAMESPACE::IdeaPlaceEx::allocatePin, "Allocate a new pin. Return the index of the pin")
        .def("addPinShape", &PROJECT_NAMESPACE::IdeaPlaceEx::addPinShape, "Add a shape to a pin")
        .def("allocateNet", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateNet, "Allocate a new net. Return the index of the net")
        .def("addPinToNet", &PROJECT_NAMESPACE::IdeaPlaceEx::addPinToNet, "Add a pin to a net")
        .def("setNetWgt", &PROJECT_NAMESPACE::IdeaPlaceEx::setNetWgt, "Set the weight of a net")
        .def("allocateSymGrp", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateSymGrp, "Allocate a new symmetric group")
        .def("addSymPair", &PROJECT_NAMESPACE::IdeaPlaceEx::addSymPair, "Add a new symmetric pair to a symmetric group")
        .def("addSelfSym", &PROJECT_NAMESPACE::IdeaPlaceEx::addSelfSym, "Add a self-symmetric constraint to a symmetric group")
        .def("addCellShape", &PROJECT_NAMESPACE::IdeaPlaceEx::addCellShape, "Add a shape to a cell")
        .def("numCells", &PROJECT_NAMESPACE::IdeaPlaceEx::numCells, "Get the number of cells")
        .def("cellIdxName", &PROJECT_NAMESPACE::IdeaPlaceEx::cellIdxName, "Get the index based on cell name")
        .def("cellName", &PROJECT_NAMESPACE::IdeaPlaceEx::cellName, "Get the cell name")
        .def("pinIdx", &PROJECT_NAMESPACE::IdeaPlaceEx::pinIdx, "Get the index based on pin name")
        .def("openVirtualPinAssignment", &PROJECT_NAMESPACE::IdeaPlaceEx::openVirtualPinAssignment, "Open the virtual pin assignment functionality")
        .def("closeVirtualPinAssignment", &PROJECT_NAMESPACE::IdeaPlaceEx::closeVirtualPinAssignment, "Close the virtual pin assignment functionality")
        .def("xCellLoc", &PROJECT_NAMESPACE::IdeaPlaceEx::xCellLoc, "Get x coordinate of a cell location")
        .def("yCellLoc", &PROJECT_NAMESPACE::IdeaPlaceEx::yCellLoc, "Get y coordinate of a cell location");
}
