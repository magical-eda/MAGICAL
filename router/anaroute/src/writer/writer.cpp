/**
 * @file   writer.cpp
 * @brief  Output file writer
 * @author Hao Chen
 * @date   11/21/2019
 *
 **/

#include "writer.hpp"

PROJECT_NAMESPACE_START

// result
void Writer::writeLayoutGds(const String_t& placementFilename, const String_t& outputFilename, const bool bFlatten) {
  fprintf(stdout, "Writing Routing GDS Layout %s\n", outputFilename.c_str());
  _layoutw.writeGds(placementFilename, outputFilename, bFlatten);
}

// guide
void Writer::writeGrGuide(const String_t& filename) const {
  fprintf(stdout, "Writing Global Routing Guides %s\n", filename.c_str());
  _grGuidew.write(filename);
}

void Writer::writeGrGuideGds(const String_t& netName, const String_t& filename) const {
  fprintf(stdout, "Writing Global Routing Guides (GDS) %s\n", filename.c_str());
  _grGuidew.writeGds(netName, filename);
}

// ....
void Writer::writeDumb(const String_t& placementFilename, const String_t& outputFilename) {
  fprintf(stdout, "Writing Dumb File %s\n", outputFilename.c_str());
  _dumbw.write(placementFilename, outputFilename);
}

PROJECT_NAMESPACE_END
