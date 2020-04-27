/**
 * @file api.cpp
 * @brief The top level cpp for initialize the pybind module
 * @author Keren Zhu
 * @date 06/22/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include "global/global.h"

namespace py = pybind11;

void initUtilAPI(py::module &);
void initGlobalAPI(py::module &);
void initLayoutAPI(py::module &);
void initPhysicalPropAPI(py::module &);
void initGraphComponentsAPI(py::module &);
void initCktGraphAPI(py::module &);
void initDesignDBAPI(py::module &);
void initParseAPI(py::module &);
void initWriterAPI(py::module &);
void initTechDbAPI(py::module &);
void initCSFlowAPI(py::module &);

PYBIND11_MAKE_OPAQUE(std::vector<PROJECT_NAMESPACE::IndexType>);

PYBIND11_MODULE(magicalFlow, m)
{
    initUtilAPI(m);
    initGlobalAPI(m);
    initLayoutAPI(m);
    initPhysicalPropAPI(m);
    initGraphComponentsAPI(m);
    initCktGraphAPI(m);
    initDesignDBAPI(m);
    initParseAPI(m);
    initWriterAPI(m);
    initTechDbAPI(m);
    initCSFlowAPI(m);
}
