/**
 * @file api.cpp
 * @brief The top level cpp for initialize the pybind module
 * @author Keren Zhu
 * @date 06/22/2019
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

void initPhysicalPropAPI(py::module &);
void initGraphComponentsAPI(py::module &);

PYBIND11_MODULE(magicalFlow, m)
{
    initPhysicalPropAPI(m);
    initGraphComponentsAPI(m);
}
