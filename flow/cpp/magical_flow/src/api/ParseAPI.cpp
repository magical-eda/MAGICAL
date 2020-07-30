/**
 * @file ParseAPI.cpp
 * @brief The Python interface for all parsing function
 * @author Keren Zhu
 * @date 07/06/2019
 */

#include <pybind11/pybind11.h>
#include "global/global.h"
#include "db/TechDB.h"

namespace py = pybind11;


void initParseAPI(py::module &m)
{
    m.def("parseSimpleTechFile", &PROJECT_NAMESPACE::PARSE::parseSimpleTechFile, "Parse simple tech file");
}
