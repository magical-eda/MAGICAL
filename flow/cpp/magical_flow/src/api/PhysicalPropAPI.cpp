/**
 * @file PhysicalPropAPI.cpp
 * @brief The Python interface for the classes defined in PhysicalProp
 * @author Keren Zhu
 * @date 06/21/2019
 */

#include <pybind11/pybind11.h>
#include "db/PhysicalProp.h"

namespace py = pybind11;

void initPhysicalPropAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::PhyPropDB>(m, "PhyProbDB")
        .def(py::init<>())
        .def("nch", &PROJECT_NAMESPACE::PhyPropDB::nch, py::return_value_policy::reference, "Get a nch device property")
        .def("allocateNch", &PROJECT_NAMESPACE::PhyPropDB::allocateNch, "Allocate a new nch device")
        .def("pch", &PROJECT_NAMESPACE::PhyPropDB::pch, py::return_value_policy::reference, "Get a pch device property")
        .def("allocatePch", &PROJECT_NAMESPACE::PhyPropDB::allocatePch, "Allocate a new pch device")
        .def("rppolywom", &PROJECT_NAMESPACE::PhyPropDB::rppolywom, py::return_value_policy::reference, "Get a rppolywom device property")
        .def("allocateRppolywom", &PROJECT_NAMESPACE::PhyPropDB::allocateRppolywom, "Get a rppolywom device property")
        .def("cfmom", &PROJECT_NAMESPACE::PhyPropDB::cfmom, py::return_value_policy::reference, "Get a Cfmom device property")
        .def("allocateCfmom", &PROJECT_NAMESPACE::PhyPropDB::allocateCfmom, "Allocate a cfmom devices");

    py::class_<PROJECT_NAMESPACE::MosProp>(m , "MosProp")
        .def(py::init<>())
        .def_property("length", &PROJECT_NAMESPACE::MosProp::length, &PROJECT_NAMESPACE::MosProp::setLength)
        .def("lengthValid", &PROJECT_NAMESPACE::MosProp::lengthValid)
        .def_property("width", &PROJECT_NAMESPACE::MosProp::width, &PROJECT_NAMESPACE::MosProp::setWidth)
        .def("widthValid", &PROJECT_NAMESPACE::MosProp::widthValid);
    
    py::class_<PROJECT_NAMESPACE::NchProp, PROJECT_NAMESPACE::MosProp>(m , "NchProp")
        .def(py::init<>())
        .def_property("mult", &PROJECT_NAMESPACE::NchProp::mult, &PROJECT_NAMESPACE::NchProp::setMult)
        .def("multValid", &PROJECT_NAMESPACE::NchProp::multValid)
        .def_property("numFingers", &PROJECT_NAMESPACE::NchProp::numFingers, &PROJECT_NAMESPACE::NchProp::setNumFingers)
        .def("numFingersValid", &PROJECT_NAMESPACE::NchProp::numFingersValid)
        .def_property("sd", &PROJECT_NAMESPACE::NchProp::sd, &PROJECT_NAMESPACE::NchProp::setSd)
        .def("sdValid", &PROJECT_NAMESPACE::NchProp::sdValid)
        .def_property("ad", &PROJECT_NAMESPACE::NchProp::ad, &PROJECT_NAMESPACE::NchProp::setAd)
        .def("adValid", &PROJECT_NAMESPACE::NchProp::adValid)
        .def_property("ass", &PROJECT_NAMESPACE::NchProp::as, &PROJECT_NAMESPACE::NchProp::setAs) // as is a keyword in Python. Use ass instead
        .def("asValid", &PROJECT_NAMESPACE::NchProp::asValid)
        .def_property("pd", &PROJECT_NAMESPACE::NchProp::pd, &PROJECT_NAMESPACE::NchProp::setPd)
        .def("pdValid", &PROJECT_NAMESPACE::NchProp::pdValid)
        .def_property("ps", &PROJECT_NAMESPACE::NchProp::ps, &PROJECT_NAMESPACE::NchProp::setPs)
        .def("psValid", &PROJECT_NAMESPACE::NchProp::psValid)
        .def_property("nrd", &PROJECT_NAMESPACE::NchProp::nrd, &PROJECT_NAMESPACE::NchProp::setNrd)
        .def("nrdValid", &PROJECT_NAMESPACE::NchProp::nrdValid)
        .def_property("nrs", &PROJECT_NAMESPACE::NchProp::nrs, &PROJECT_NAMESPACE::NchProp::setNrs)
        .def("nrsValid", &PROJECT_NAMESPACE::NchProp::nrsValid)
        .def_property("sa", &PROJECT_NAMESPACE::NchProp::sa, &PROJECT_NAMESPACE::NchProp::setSa)
        .def("saValid", &PROJECT_NAMESPACE::NchProp::saValid)
        .def_property("sb", &PROJECT_NAMESPACE::NchProp::sb, &PROJECT_NAMESPACE::NchProp::setSb)
        .def("sbValid", &PROJECT_NAMESPACE::NchProp::sbValid);

    py::class_<PROJECT_NAMESPACE::Nch25ud18macProp, PROJECT_NAMESPACE::NchProp>(m , "Nch25ud18macProp")
        .def(py::init<>())
        .def_property("sa1", &PROJECT_NAMESPACE::Nch25ud18macProp::sa1, &PROJECT_NAMESPACE::Nch25ud18macProp::setSa1)
        .def("sa1Valid", &PROJECT_NAMESPACE::Nch25ud18macProp::sa1Valid)
        .def_property("sa2", &PROJECT_NAMESPACE::Nch25ud18macProp::sa2, &PROJECT_NAMESPACE::Nch25ud18macProp::setSa2)
        .def("sa2Valid", &PROJECT_NAMESPACE::Nch25ud18macProp::sa2Valid)
        .def_property("sa3", &PROJECT_NAMESPACE::Nch25ud18macProp::sa3, &PROJECT_NAMESPACE::Nch25ud18macProp::setSa3)
        .def("sa3Valid", &PROJECT_NAMESPACE::Nch25ud18macProp::sa3Valid)
        .def_property("sb1", &PROJECT_NAMESPACE::Nch25ud18macProp::sb1, &PROJECT_NAMESPACE::Nch25ud18macProp::setSb1)
        .def("sb1Valid", &PROJECT_NAMESPACE::Nch25ud18macProp::sb1Valid)
        .def_property("sb2", &PROJECT_NAMESPACE::Nch25ud18macProp::sb2, &PROJECT_NAMESPACE::Nch25ud18macProp::setSb2)
        .def("sb2Valid", &PROJECT_NAMESPACE::Nch25ud18macProp::sb2Valid)
        .def_property("sb3", &PROJECT_NAMESPACE::Nch25ud18macProp::sb3, &PROJECT_NAMESPACE::Nch25ud18macProp::setSb3)
        .def("sb3Valid", &PROJECT_NAMESPACE::Nch25ud18macProp::sb3Valid);

    py::class_<PROJECT_NAMESPACE::PchProp, PROJECT_NAMESPACE::MosProp>(m , "PchProp")
        .def(py::init<>())
        .def_property("mult", &PROJECT_NAMESPACE::PchProp::mult, &PROJECT_NAMESPACE::PchProp::setMult)
        .def("multValid", &PROJECT_NAMESPACE::PchProp::multValid)
        .def_property("numFingers", &PROJECT_NAMESPACE::PchProp::numFingers, &PROJECT_NAMESPACE::PchProp::setNumFingers)
        .def("numFingersValid", &PROJECT_NAMESPACE::PchProp::numFingersValid)
        .def_property("sd", &PROJECT_NAMESPACE::PchProp::sd, &PROJECT_NAMESPACE::PchProp::setSd)
        .def("sdValid", &PROJECT_NAMESPACE::PchProp::sdValid)
        .def_property("ad", &PROJECT_NAMESPACE::PchProp::ad, &PROJECT_NAMESPACE::PchProp::setAd)
        .def("adValid", &PROJECT_NAMESPACE::PchProp::adValid)
        .def_property("ass", &PROJECT_NAMESPACE::PchProp::as, &PROJECT_NAMESPACE::PchProp::setAs) // as is a keyword in Python. Use ass instead
        .def("asValid", &PROJECT_NAMESPACE::PchProp::asValid)
        .def_property("pd", &PROJECT_NAMESPACE::PchProp::pd, &PROJECT_NAMESPACE::PchProp::setPd)
        .def("pdValid", &PROJECT_NAMESPACE::PchProp::pdValid)
        .def_property("ps", &PROJECT_NAMESPACE::PchProp::ps, &PROJECT_NAMESPACE::PchProp::setPs)
        .def("psValid", &PROJECT_NAMESPACE::PchProp::psValid)
        .def_property("nrd", &PROJECT_NAMESPACE::PchProp::nrd, &PROJECT_NAMESPACE::PchProp::setNrd)
        .def("nrdValid", &PROJECT_NAMESPACE::PchProp::nrdValid)
        .def_property("nrs", &PROJECT_NAMESPACE::PchProp::nrs, &PROJECT_NAMESPACE::PchProp::setNrs)
        .def("nrsValid", &PROJECT_NAMESPACE::PchProp::nrsValid)
        .def_property("sa", &PROJECT_NAMESPACE::PchProp::sa, &PROJECT_NAMESPACE::PchProp::setSa)
        .def("saValid", &PROJECT_NAMESPACE::PchProp::saValid)
        .def_property("sb", &PROJECT_NAMESPACE::PchProp::sb, &PROJECT_NAMESPACE::PchProp::setSb)
        .def("sbValid", &PROJECT_NAMESPACE::PchProp::sbValid);

    py::class_<PROJECT_NAMESPACE::RppolywoMProp>(m, "RppolywoMProp")
        .def(py::init<>())
        .def_property("lr", &PROJECT_NAMESPACE::RppolywoMProp::lr, &PROJECT_NAMESPACE::RppolywoMProp::setLr)
        .def("lrValid", &PROJECT_NAMESPACE::RppolywoMProp::lrValid)
        .def_property("wr", &PROJECT_NAMESPACE::RppolywoMProp::wr, &PROJECT_NAMESPACE::RppolywoMProp::setWr)
        .def("wrValid", &PROJECT_NAMESPACE::RppolywoMProp::wrValid)
        .def_property("multi", &PROJECT_NAMESPACE::RppolywoMProp::multi, &PROJECT_NAMESPACE::RppolywoMProp::setMulti)
        .def("multiValid", &PROJECT_NAMESPACE::RppolywoMProp::multiValid)
        .def_property("m", &PROJECT_NAMESPACE::RppolywoMProp::m, &PROJECT_NAMESPACE::RppolywoMProp::setM)
        .def("mValid", &PROJECT_NAMESPACE::RppolywoMProp::mValid);

    py::class_<PROJECT_NAMESPACE::CfmomProp>(m, "CfmomProp")
        .def(py::init<>())
        .def_property("nr", &PROJECT_NAMESPACE::CfmomProp::nr, &PROJECT_NAMESPACE::CfmomProp::setNr)
        .def("nrValid", &PROJECT_NAMESPACE::CfmomProp::nrValid)
        .def_property("lr", &PROJECT_NAMESPACE::CfmomProp::lr, &PROJECT_NAMESPACE::CfmomProp::setLr)
        .def("lrValid", &PROJECT_NAMESPACE::CfmomProp::lrValid)
        .def_property("w", &PROJECT_NAMESPACE::CfmomProp::w, &PROJECT_NAMESPACE::CfmomProp::setW)
        .def("wValid", &PROJECT_NAMESPACE::CfmomProp::wValid)
        .def_property("s", &PROJECT_NAMESPACE::CfmomProp::s, &PROJECT_NAMESPACE::CfmomProp::setS)
        .def("sValid", &PROJECT_NAMESPACE::CfmomProp::sValid)
        .def_property("stm", &PROJECT_NAMESPACE::CfmomProp::stm, &PROJECT_NAMESPACE::CfmomProp::setStm)
        .def("stmValid", &PROJECT_NAMESPACE::CfmomProp::stmValid)
        .def_property("spm", &PROJECT_NAMESPACE::CfmomProp::spm, &PROJECT_NAMESPACE::CfmomProp::setSpm)
        .def("spmValid", &PROJECT_NAMESPACE::CfmomProp::spmValid)
        .def_property("multi", &PROJECT_NAMESPACE::CfmomProp::multi, &PROJECT_NAMESPACE::CfmomProp::setMulti)
        .def("multiValid", &PROJECT_NAMESPACE::CfmomProp::multiValid)
        .def_property("ftip", &PROJECT_NAMESPACE::CfmomProp::ftip, &PROJECT_NAMESPACE::CfmomProp::setFtip)
        .def("ftipValid", &PROJECT_NAMESPACE::CfmomProp::ftipValid);

}
