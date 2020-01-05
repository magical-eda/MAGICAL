/**
 * @file   lefVia.cpp
 * @brief  Technology configuration - Lef Via
 * @author Hao Chen
 * @date   09/16/2019
 *
 **/

#include "lefVia.hpp"

PROJECT_NAMESPACE_START

void LefVia::setName(const String_t& n) {
  _name = n;
}

void LefVia::setDefault() {
  _bDefault = true;
}

void LefVia::setResistance(const Int_t r) {
  _resistance = r;
}

void LefVia::setLayerIdx(const UInt_t i, const UInt_t v) {
  assert(0 <= i and i <= 2);
  _layerIndices[i] = v;
}

void LefVia::setLayerName(const UInt_t i, const String_t& n) {
  assert(0 <= i and i <= 2);
  _layerNames[i] = n;
}

void LefVia::addBox(const UInt_t i, const Box<Int_t>& b) {
  assert(0 <= i and i <= 2);
  _vBoxes[i].emplace_back(b);
}

void LefVia::logInfo() const {
  FILE* fout = stderr;
  fprintf(fout, "VIA %s\n", _name.c_str());
  fprintf(fout, "    RESISTANCE %d\n", _resistance);
  fprintf(fout, "  LAYER %s\n", _layerNames[0].c_str());
  for (UInt_t i = 0; i < _vBoxes[0].size(); ++i) {
    Box<Int_t> box = _vBoxes[0][i];
    fprintf(fout, "    RECT %d %d %d %d\n", box.xl(), box.yl(), box.xh(), box.yh());
  }
  fprintf(fout, "  LAYER %s\n", _layerNames[1].c_str());
  for (UInt_t i = 0; i < _vBoxes[1].size(); ++i) {
    Box<Int_t> box = _vBoxes[1][i];
    fprintf(fout, "    RECT %d %d %d %d\n", box.xl(), box.yl(), box.xh(), box.yh());
  }
  fprintf(fout, "  LAYER %s\n", _layerNames[2].c_str());
  for (UInt_t i = 0; i < _vBoxes[2].size(); ++i) {
    Box<Int_t> box = _vBoxes[2][i];
    fprintf(fout, "    RECT %d %d %d %d\n", box.xl(), box.yl(), box.xh(), box.yh());
  }
  fprintf(fout, "END %s\n\n", _name.c_str());
}

PROJECT_NAMESPACE_END

