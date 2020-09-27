/**
 * @file DesignDBAPI.cpp
 * @brief The Python interface for the classes DesignDB
 * @author Keren Zhu, Mingjie Liu
 * @date 06/27/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "db/DesignDB.h"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<std::string>);

void initDesignDBAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::DesignDB>(m , "DesignDB")
        .def(py::init<>())
        .def("numCkts", &PROJECT_NAMESPACE::DesignDB::numCkts)
        .def("subCkt", &PROJECT_NAMESPACE::DesignDB::subCkt, py::return_value_policy::reference)
        .def("resizeSubCkts", &PROJECT_NAMESPACE::DesignDB::resizeSubCkts, "resize the sub circuits")
        .def("rootCktIdx", &PROJECT_NAMESPACE::DesignDB::rootCktIdx)
        .def("allocateCkt", &PROJECT_NAMESPACE::DesignDB::allocateCkt)
        .def("findRootCkt", &PROJECT_NAMESPACE::DesignDB::findRootCkt)
        .def_readwrite("power", &PROJECT_NAMESPACE::DesignDB::power)
        .def_readwrite("ground", &PROJECT_NAMESPACE::DesignDB::power)
        .def("phyPropDB", &PROJECT_NAMESPACE::DesignDB::phyPropDB, py::return_value_policy::reference, "Get physical property DB");
}
