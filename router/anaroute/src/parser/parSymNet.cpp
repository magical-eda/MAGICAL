/**
 * @file   parSymNet.cpp
 * @brief  Parser - Symmetric net reader
 * @author Hao Chen
 * @date   09/29/2019
 *
 **/

#include "parSymNet.hpp"
#include "src/util/util.hpp"

PROJECT_NAMESPACE_START

void SymNetReader::parse(const String_t& filename) {
  FILE* fin = fopen(filename.c_str(), "r");
  
  if (!fin) {
    fprintf(stderr, "SymNetReader::%s ERROR: Cannot open file %s!!!\n", __func__, filename.c_str());
    exit(0);
  }
  const UInt_t bufSize = 200;
  char buf[bufSize];
  Vector_t<String_t> vTokens;

  while (fgets(buf, bufSize, fin)) {
    util::splitString(buf, " ", vTokens);
    if (vTokens.size() == 1) { // self symmetric net
      if (_cir.hasNet(vTokens[0])) {
        _cir.net(vTokens[0]).setSelfSym();
      }
      else {
        fprintf(stderr, "SymNetReader::%s WARNING: Invalid net name %s\n", __func__, vTokens[0].c_str());
      }
    }
    else {
      if (_cir.hasNet(vTokens[0]) and _cir.hasNet(vTokens[1])) {
        assert(vTokens.size() == 2);
        Net& n0 = _cir.net(vTokens[0]);
        Net& n1 = _cir.net(vTokens[1]);
        n0.setSymNetIdx(n1.idx());
        n1.setSymNetIdx(n0.idx());
      }
      else {
        if (!_cir.hasNet(vTokens[0])) {
          fprintf(stderr, "SymNetReader::%s WARNING: Invalid net name %s\n", __func__, vTokens[0].c_str());
        }
        if (!_cir.hasNet(vTokens[1])) {
          fprintf(stderr, "SymNetReader::%s WARNING: Invalid net name %s\n", __func__, vTokens[1].c_str());
        }
      }
    }
  }

  fclose(fin);
}

PROJECT_NAMESPACE_END
