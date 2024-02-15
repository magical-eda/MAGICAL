/**
 * @file   parTech.cpp
 * @brief  Parser - Tsmc techfile (techlayer only)
 * @author Hao Chen
 * @date   09/23/2019
 *
 **/

#include "parTech.hpp"
#include "src/util/util.hpp"

PROJECT_NAMESPACE_START

void TechfileReader::parse(const String_t& filename) {
  FILE* fin = fopen(filename.c_str(), "r");
  if (!fin) {
    fprintf(stderr, "TechfileReader::%s ERROR: Cannot open file %s!!!\n",__func__, filename.c_str());
    exit(0);
  }
  const UInt_t bufSize = 200;
  char buf[bufSize];
  while (fgets(buf, bufSize, fin)) {
    if (strcmp(buf, "techLayers(\n") == 0) {
      readTechLayers(fin, buf, bufSize);
    }
    else {

    }
  }

  fclose(fin);
}

void TechfileReader::readTechLayers(FILE* fin, char* buf, const UInt_t bufSize) {
  Vector_t<String_t> vTokens;
  while (fgets(buf, bufSize, fin)) {
    if (strcmp(buf, "\n") == 0) {
      continue;
    }
    else if (strcmp(buf, ")\n") == 0) {
      return;
    }
    util::splitString(buf, " ", vTokens);
    if (vTokens[0][0] == ';') {
      continue;
    }
    char layerName[bufSize];
    UInt_t layerIdx;
    sscanf(buf, " ( %s %u %*s )\n", layerName, &layerIdx);
    _cir.tech().addStr2LayerMaxIdx(String_t(layerName), layerIdx);
  }
}

PROJECT_NAMESPACE_END
