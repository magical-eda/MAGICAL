/**
 * @file api.cpp
 * @brief The top level cpp for initialize the pybind module
 * @author Mingjie Liu
 * @date 03/18/2020
 */

#include <pybind11/pybind11.h>
#include "global/global.h"

namespace py = pybind11;

void initConstGenAPI(py::module &);
void initGlobalAPI(py::module &);
void initDefParseAPI(py::module &);

PYBIND11_MODULE(ConstGenPy, m)
{
    initGlobalAPI(m);
    initConstGenAPI(m);
    initDefParseAPI(m);
}
