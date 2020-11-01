/**
 * @file CktGraphAPI.cpp
 * @brief The Python interface for the CktGraph
 * @author Keren Zhu, Mingjie Liu
 * @date 06/25/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "db/CktGraph.h"

namespace py = pybind11;

void initCktGraphAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::FloorplanData>(m , "FloorplanData")
        .def(py::init<>())
        .def("clearBoundary", &PROJECT_NAMESPACE::FloorplanData::clearBoundary, "Clear the boundary planning")
        .def("clearNetAssignment", &PROJECT_NAMESPACE::FloorplanData::clearNetAssignment, "Clear the IO pin assignments")
        .def("isBoundarySet", &PROJECT_NAMESPACE::FloorplanData::isBoundarySet, "Get whether the boundary is set")
        .def("isNetAssignmentSet", &PROJECT_NAMESPACE::FloorplanData::isNetAssignmentSet, "Get whether the IO pin assignments are set")
        .def("netAssignment", &PROJECT_NAMESPACE::FloorplanData::netAssignment, "Get the assignment status for a net's IO pin, 0->left, 1->right, -1->unset")
        .def("netExternalBBox", &PROJECT_NAMESPACE::FloorplanData::netExternalBBox, "Get the external bounding box of the net. Be sure to check whether it is valid")
        .def("boundary", &PROJECT_NAMESPACE::FloorplanData::boundary, "Get the boundary set for the circuit from the floorplan");
    py::class_<PROJECT_NAMESPACE::CktGraph>(m , "CktGraph")
        .def(py::init<>())
        .def("setTechDB", &PROJECT_NAMESPACE::CktGraph::setTechDB)
        .def("allocateNode", &PROJECT_NAMESPACE::CktGraph::allocateNode)
        .def("numNodes", &PROJECT_NAMESPACE::CktGraph::numNodes)
        .def("node", &PROJECT_NAMESPACE::CktGraph::node, py::return_value_policy::reference)
        .def("allocatePin", &PROJECT_NAMESPACE::CktGraph::allocatePin)
        .def("resizeNodes", &PROJECT_NAMESPACE::CktGraph::resizeNodeArray, "Resize the node vector in this circuit")
        .def("numPins", &PROJECT_NAMESPACE::CktGraph::numPins)
        .def("pin", &PROJECT_NAMESPACE::CktGraph::pin, py::return_value_policy::reference)
        .def("resizePins", &PROJECT_NAMESPACE::CktGraph::resizePinArray, "Resize the pin vector in this circuit")
        .def("allocatePsub", &PROJECT_NAMESPACE::CktGraph::allocatePsub)
        .def("addPsubIdx", &PROJECT_NAMESPACE::CktGraph::addPsubIdx)
        .def("allocateNwell", &PROJECT_NAMESPACE::CktGraph::allocateNwell)
        .def("addNwellIdx", &PROJECT_NAMESPACE::CktGraph::addNwellIdx)
        .def("numPsubs", &PROJECT_NAMESPACE::CktGraph::numPsubs)
        .def("numNwells", &PROJECT_NAMESPACE::CktGraph::numNwells)
        .def("net", &PROJECT_NAMESPACE::CktGraph::net, py::return_value_policy::reference)
        .def("numNets", &PROJECT_NAMESPACE::CktGraph::numNets)
        .def("allocateNet", &PROJECT_NAMESPACE::CktGraph::allocateNet)
        .def("resizeNets", &PROJECT_NAMESPACE::CktGraph::resizeNetArray, "Resize the net vector in this circuit")
        .def("psub", &PROJECT_NAMESPACE::CktGraph::psub, py::return_value_policy::reference)
        .def("nwell", &PROJECT_NAMESPACE::CktGraph::nwell, py::return_value_policy::reference)
        .def_property("name", &PROJECT_NAMESPACE::CktGraph::name, &PROJECT_NAMESPACE::CktGraph::setName)
        .def("refName", &PROJECT_NAMESPACE::CktGraph::refName, "Return the reference name of the circuit")
        .def("layout", &PROJECT_NAMESPACE::CktGraph::layout, py::return_value_policy::reference)
        .def("parseGDS", &PROJECT_NAMESPACE::CktGraph::parseGDS, py::return_value_policy::reference)
        .def_property("implType", &PROJECT_NAMESPACE::CktGraph::implType, &PROJECT_NAMESPACE::CktGraph::setImplType) 
        .def_property("implIdx", &PROJECT_NAMESPACE::CktGraph::implIdx, &PROJECT_NAMESPACE::CktGraph::setImplIdx)
        .def_property("isImpl", &PROJECT_NAMESPACE::CktGraph::isImpl, &PROJECT_NAMESPACE::CktGraph::setIsImpl)
        .def("backup", &PROJECT_NAMESPACE::CktGraph::backup, "back up the circuit graph")
        .def("restore", &PROJECT_NAMESPACE::CktGraph::restore, "retore the backed up tcircuit graph")
        .def("GdsData", &PROJECT_NAMESPACE::CktGraph::gdsData, py::return_value_policy::reference)
        .def("gdsData", &PROJECT_NAMESPACE::CktGraph::gdsData, py::return_value_policy::reference)
        .def("fpData", &PROJECT_NAMESPACE::CktGraph::fpData, "Get the floorplan results for the circuit");
}
