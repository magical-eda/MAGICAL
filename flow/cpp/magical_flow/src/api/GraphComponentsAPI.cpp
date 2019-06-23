/**
 * @file GraphComponentsAPI.cpp
 * @brief The Python interface for the classes defined in GraphComponents.h
 * @author Keren Zhu
 * @date 06/22/2019
 */

#include <pybind11/pybind11.h>
#include "db/GraphComponents.h"

namespace py = pybind11;

void initGraphComponentsAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::CktNode>(m , "CktNode")
        .def(py::init<>())
        .def_property("graphIdx", &PROJECT_NAMESPACE::CktNode::subgraphIdx, &PROJECT_NAMESPACE::CktNode::setSubgraphIdx)
        .def("pinIdxArray", &PROJECT_NAMESPACE::CktNode::pinIdxArray, py::return_value_policy::reference);
}
