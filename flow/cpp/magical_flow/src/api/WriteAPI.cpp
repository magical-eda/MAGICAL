/**
 * @file WriterAPI.cpp
 * @brief The Python interface for all writing function
 * @author Keren Zhu
 * @date 07/09/2019
 */

#include <pybind11/pybind11.h>
#include "global/global.h"
#include "writer/GdsWriter.h"

namespace py = pybind11;


void initWriterAPI(py::module &m)
{
    m.def("writeGdsLayout", &PROJECT_NAMESPACE::WRITER::writeGdsLayout, "write the layout for circuit to GDSII");
}
