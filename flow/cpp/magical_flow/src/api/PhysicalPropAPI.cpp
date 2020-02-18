/**
 * @file PhysicalPropAPI.cpp
 * @brief The Python interface for the classes defined in PhysicalProp
 * @author Keren Zhu, Mingjie Liu
 * @date 06/21/2019
 */

#include <pybind11/pybind11.h>
#include "db/PhysicalProp.h"

namespace py = pybind11;

void initPhysicalPropAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::PhyPropDB>(m, "PhyPropDB")
        .def(py::init<>())
        .def("nch", &PROJECT_NAMESPACE::PhyPropDB::nch, py::return_value_policy::reference, "Get a nch device property")
        .def("allocateNch", &PROJECT_NAMESPACE::PhyPropDB::allocateNch, "Allocate a new nch device")
        .def("pch", &PROJECT_NAMESPACE::PhyPropDB::pch, py::return_value_policy::reference, "Get a pch device property")
        .def("allocatePch", &PROJECT_NAMESPACE::PhyPropDB::allocatePch, "Allocate a new pch device")
        .def("resistor", &PROJECT_NAMESPACE::PhyPropDB::resister, py::return_value_policy::reference, "Get a resistor device property")
        .def("allocateRes", &PROJECT_NAMESPACE::PhyPropDB::allocateRes, "Get a resistor device property")
        .def("capacitor", &PROJECT_NAMESPACE::PhyPropDB::capacitor, py::return_value_policy::reference, "Get a capacitor device property")
        .def("allocateCap", &PROJECT_NAMESPACE::PhyPropDB::allocateCap, "Allocate a capacitor devices");

    py::class_<PROJECT_NAMESPACE::MosProp>(m , "MosProp")
        .def(py::init<>())
        .def_property("length", &PROJECT_NAMESPACE::MosProp::length, &PROJECT_NAMESPACE::MosProp::setLength)
        .def("lengthValid", &PROJECT_NAMESPACE::MosProp::lengthValid)
        .def_property("width", &PROJECT_NAMESPACE::MosProp::width, &PROJECT_NAMESPACE::MosProp::setWidth)
        .def("widthValid", &PROJECT_NAMESPACE::MosProp::widthValid)
        .def_property("mult", &PROJECT_NAMESPACE::MosProp::mult, &PROJECT_NAMESPACE::NchProp::setMult)
        .def_property("numFingers", &PROJECT_NAMESPACE::MosProp::numFingers, &PROJECT_NAMESPACE::NchProp::setNumFingers)
        .def_property("attr", &PROJECT_NAMESPACE::MosProp::attr, &PROJECT_NAMESPACE::MosProp::setAttr)
        .def_property("pinConType", &PROJECT_NAMESPACE::MosProp::pinConType, &PROJECT_NAMESPACE::MosProp::setPinConType)
        .def("numBulkCon", &PROJECT_NAMESPACE::MosProp::numBulkCon)
        .def("bulkCon", &PROJECT_NAMESPACE::MosProp::bulkCon)
        .def("appendBulkCon", &PROJECT_NAMESPACE::MosProp::appendBulkCon);
    
    py::class_<PROJECT_NAMESPACE::NchProp, PROJECT_NAMESPACE::MosProp>(m , "NchProp")
        .def(py::init<>());

    py::class_<PROJECT_NAMESPACE::PchProp, PROJECT_NAMESPACE::MosProp>(m , "PchProp")
        .def(py::init<>());

    py::class_<PROJECT_NAMESPACE::ResProp>(m, "ResProp")
        .def(py::init<>())
        .def_property("lr", &PROJECT_NAMESPACE::ResProp::lr, &PROJECT_NAMESPACE::ResProp::setLr)
        .def("lrValid", &PROJECT_NAMESPACE::ResProp::lrValid)
        .def_property("wr", &PROJECT_NAMESPACE::ResProp::wr, &PROJECT_NAMESPACE::ResProp::setWr)
        .def("wrValid", &PROJECT_NAMESPACE::ResProp::wrValid)
        .def_property("series", &PROJECT_NAMESPACE::ResProp::series, &PROJECT_NAMESPACE::ResProp::setSeries)
        .def_property("parallel", &PROJECT_NAMESPACE::ResProp::parallel, &PROJECT_NAMESPACE::ResProp::setParallel)
        .def_property("segNum", &PROJECT_NAMESPACE::ResProp::segNum, &PROJECT_NAMESPACE::ResProp::setSegNum)
        .def_property("segSpace", &PROJECT_NAMESPACE::ResProp::segSpace, &PROJECT_NAMESPACE::ResProp::setSegSpace)
        .def_property("attr", &PROJECT_NAMESPACE::ResProp::attr, &PROJECT_NAMESPACE::ResProp::setAttr);
//        .def_property("multi", &PROJECT_NAMESPACE::ResProp::multi, &PROJECT_NAMESPACE::ResProp::setMulti)
//        .def("multiValid", &PROJECT_NAMESPACE::ResProp::multiValid)
//        .def_property("m", &PROJECT_NAMESPACE::ResProp::m, &PROJECT_NAMESPACE::ResProp::setM)
//        .def("mValid", &PROJECT_NAMESPACE::ResProp::mValid);

    py::class_<PROJECT_NAMESPACE::CapProp>(m, "CapProp")
        .def(py::init<>())
        .def_property("numFingers", &PROJECT_NAMESPACE::CapProp::numFingers, &PROJECT_NAMESPACE::CapProp::setNumFingers)
        .def_property("lr", &PROJECT_NAMESPACE::CapProp::lr, &PROJECT_NAMESPACE::CapProp::setLr)
        .def("lrValid", &PROJECT_NAMESPACE::CapProp::lrValid)
        .def_property("w", &PROJECT_NAMESPACE::CapProp::w, &PROJECT_NAMESPACE::CapProp::setW)
        .def("wValid", &PROJECT_NAMESPACE::CapProp::wValid)
        .def_property("spacing", &PROJECT_NAMESPACE::CapProp::spacing, &PROJECT_NAMESPACE::CapProp::setSpacing)
        .def("spacingValid", &PROJECT_NAMESPACE::CapProp::spacingValid)
        .def_property("stm", &PROJECT_NAMESPACE::CapProp::stm, &PROJECT_NAMESPACE::CapProp::setStm)
        .def("stmValid", &PROJECT_NAMESPACE::CapProp::stmValid)
        .def_property("spm", &PROJECT_NAMESPACE::CapProp::spm, &PROJECT_NAMESPACE::CapProp::setSpm)
        .def("spmValid", &PROJECT_NAMESPACE::CapProp::spmValid)
        .def_property("multi", &PROJECT_NAMESPACE::CapProp::multi, &PROJECT_NAMESPACE::CapProp::setMulti)
//        .def("multiValid", &PROJECT_NAMESPACE::CapProp::multiValid)
        .def_property("ftip", &PROJECT_NAMESPACE::CapProp::ftip, &PROJECT_NAMESPACE::CapProp::setFtip)
        .def("ftipValid", &PROJECT_NAMESPACE::CapProp::ftipValid)
        .def_property("attr", &PROJECT_NAMESPACE::CapProp::attr, &PROJECT_NAMESPACE::CapProp::setAttr);

}
