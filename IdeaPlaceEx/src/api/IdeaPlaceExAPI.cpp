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
        .def("numThreads", &PROJECT_NAMESPACE::IdeaPlaceEx::setNumThreads, "Set number of threads")
        .def("readTechSimpleFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readTechSimpleFile, "Internal usage: Read in the techsimple file")
        .def("readPinFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readPinFile, "Internal usage: Read in the .pin file")
        .def("readConnectionFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readConnectionFile, "Internal usage: Read in the .connection file")
        .def("readNetWgtFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readNetWgtFile, "Internal usage: Read in the .netwgt file")
        .def("readSymFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readSymFile, "Internal usage: Read in the .sym file")
        .def("readGdsLayout", &PROJECT_NAMESPACE::IdeaPlaceEx::readGdsLayout, "Internal Usage: Read in a gds for cell, the name of the GDS cell need to match the cell name")
        .def("readSymNetFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readSymNetFile, "Internal Usage: Read in a .symnet file")
        .def("readSigpathFile", &PROJECT_NAMESPACE::IdeaPlaceEx::readSigpathFile, "Internal Usage: Read in a .sigpath file")
        .def("addGdsLayer", &PROJECT_NAMESPACE::IdeaPlaceEx::addGdsLayer, py::arg("cellIdx") = PROJECT_NAMESPACE::INDEX_TYPE_MAX, "Add a gds to a cell")
        .def("finishAddingGdsLayer", &PROJECT_NAMESPACE::IdeaPlaceEx::finishAddingGdsLayer, "Finish the gds layer adding, trigger a init function")
        .def("allocateCell", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateCell, "Allocate a new cell, return the index of the cell")
        .def("setCellName", &PROJECT_NAMESPACE::IdeaPlaceEx::setCellName, "Set the name of a cell")
        .def("setCellFlip", &PROJECT_NAMESPACE::IdeaPlaceEx::setCellFlip, "Flip a cell")
        .def("allocatePin", &PROJECT_NAMESPACE::IdeaPlaceEx::allocatePin, "Allocate a new pin. Return the index of the pin")
        .def("addPinShape", &PROJECT_NAMESPACE::IdeaPlaceEx::addPinShape, "Add a shape to a pin")
        .def("setPinName", &PROJECT_NAMESPACE::IdeaPlaceEx::setPinName, "Set the name of a pin")
        .def("allocateNet", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateNet, "Allocate a new net. Return the index of the net")
        .def("setNetName", &PROJECT_NAMESPACE::IdeaPlaceEx::setNetName, "Set the name of a net")
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
        .def("setIoPinBoundaryExtension", &PROJECT_NAMESPACE::IdeaPlaceEx::setIoPinBoundaryExtension, "Set the extension of io pin locations to the boundary of cell placements")
        .def("setIoPinInterval", &PROJECT_NAMESPACE::IdeaPlaceEx::setIoPinInterval, "Set the minimum interval of io pins")
        .def("markIoNet", &PROJECT_NAMESPACE::IdeaPlaceEx::markAsIoNet, "Mark a net as IO net")
        .def("revokeIoNet", &PROJECT_NAMESPACE::IdeaPlaceEx::revokeIoNet, "Revoke IO net flag on a net")
        .def("markAsVddNet", &PROJECT_NAMESPACE::IdeaPlaceEx::markAsVddNet, "Mark a net as VDD")
        .def("markAsVssNet", &PROJECT_NAMESPACE::IdeaPlaceEx::markAsVssNet, "Mark a net as VSS")
        .def("addSymNetPair", &PROJECT_NAMESPACE::IdeaPlaceEx::addSymNetPair, "Add a pair of symmetric nets")
        .def("markSelfSym", &PROJECT_NAMESPACE::IdeaPlaceEx::markSelfSymNet, "Mark a net as self symmetric")
        .def("allocateProximityGroup", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateProximityGroup, "Allocate a new proximity group")
        .def("addCellToProximityGroup", &PROJECT_NAMESPACE::IdeaPlaceEx::addCellToProximityGroup, "Add a cell to a proximity group")
        .def("allocateSignalPath", &PROJECT_NAMESPACE::IdeaPlaceEx::allocateSignalPath, "Allocate a new signal path")
        .def("markSignalPathAsPower", &PROJECT_NAMESPACE::IdeaPlaceEx::markSignalPathAsPower, "Mark a signal path as power")
        .def("addPinToSignalPath", &PROJECT_NAMESPACE::IdeaPlaceEx::addPinToSignalPath, "add a pin to a signal path")
        .def("iopinX", &PROJECT_NAMESPACE::IdeaPlaceEx::iopinX, "Get the x coordinate of a net")
        .def("iopinY", &PROJECT_NAMESPACE::IdeaPlaceEx::iopinY, "Get the y coordinate of a net")
        .def("isIoPinVertical", &PROJECT_NAMESPACE::IdeaPlaceEx::isIopinVertical, "true if io pins are on top or bottom")
        .def("xCellLoc", &PROJECT_NAMESPACE::IdeaPlaceEx::xCellLoc, "Get x coordinate of a cell location")
        .def("yCellLoc", &PROJECT_NAMESPACE::IdeaPlaceEx::yCellLoc, "Get y coordinate of a cell location")
        .def("runtimeIdeaPlaceEx", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeIdeaPlaceEx, "Get the runtime for the Ideaplace")
        .def("runtimeGlobalPlace", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeGlobalPlace, "Get the time used for global placement")
        .def("runtimeGlobalPlaceCalcObj", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeGlobalPlaceCalcObj, "Get the time used for calculating the objectives in global placement")
        .def("runtimeGlobalPlaceCalcGrad", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeGlobalPlaceCalcGrad, "Get the time used for calculating the gradients in global placement")
        .def("runtimeGlobalPlaceOptmKernel", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeGlobalPlaceOptmKernel, "Get the time used for optimizer kernel in the global placement")
        .def("runtimeGlobalPlaceOptimize", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeGlobalPlaceOptimize, "Get the time used for global placement optimize routine")
        .def("runtimeGlobalPlaceUpdateProblem", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeGlobalPlaceUpdateProblem, "Get the time used for updating the problem in gloobal placement")
        .def("runtimeLegalization", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeLegalization, "Get the time used for legalization")
        .def("runtimeDetailedPlacement", &PROJECT_NAMESPACE::IdeaPlaceEx::runtimeDetailedPlacement, "Get the time used for detailed placement")
        ;
}
