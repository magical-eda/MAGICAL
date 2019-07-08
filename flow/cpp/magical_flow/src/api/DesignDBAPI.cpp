/**
 * @file DesignDBAPI.cpp
 * @brief The Python interface for the classes DesignDB
 * @author Keren Zhu
 * @date 06/27/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "db/DesignDB.h"

namespace py = pybind11;

void initDesignDBAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::DesignDB>(m , "DesignDB")
        .def(py::init<>())
        .def("numCkts", &PROJECT_NAMESPACE::DesignDB::numCkts)
        .def("subCkt", &PROJECT_NAMESPACE::DesignDB::subCkt, py::return_value_policy::reference)
        .def("rootCktIdx", &PROJECT_NAMESPACE::DesignDB::rootCktIdx)
        .def("allocateCkt", &PROJECT_NAMESPACE::DesignDB::allocateCkt)
        .def("findRootCkt", &PROJECT_NAMESPACE::DesignDB::findRootCkt)
        .def("phyPropDB", &PROJECT_NAMESPACE::DesignDB::phyPropDB, py::return_value_policy::reference, "Get physical property DB");
}
