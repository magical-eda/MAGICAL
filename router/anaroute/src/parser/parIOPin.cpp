/**
 * @file   parIOPin.cpp
 * @brief  Parser - IO Pin file
 * @author Hao Chen
 * @date   11/25/2019
 *
 **/

#include "parIOPin.hpp"
#include "src/util/util.hpp"

PROJECT_NAMESPACE_START

void IOPinReader::parse(const String_t& filename) {

  FILE* fin = fopen(filename.c_str(), "r");
  if (!fin) {
    fprintf(stderr, "IOPinReader::%s ERROR: Cannot open file %s!\n", __func__, filename.c_str());
    return;
  }
  constexpr Int_t bufSize = 200;
  char buf[bufSize];
  while (fgets(buf, bufSize, fin)) {
    String_t netName(buf);
    util::trimString(netName);
    Int_t netIdx = _cir.str2NetIdx(netName);
    Net& net = _cir.net(netIdx);
    net.setIOPort();
  }

  fclose(fin);

}

PROJECT_NAMESPACE_END
