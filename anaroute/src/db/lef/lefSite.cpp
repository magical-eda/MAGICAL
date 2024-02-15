/**
 * @file   lefSite.cpp
 * @brief  Technology configuration - Lef Site
 * @author Hao Chen
 * @date   09/15/2019
 *
 **/

#include "lefSite.hpp"

PROJECT_NAMESPACE_START

void LefSite::setName(const String_t& n) {
  _name = n;
}

void LefSite::setClassType(const String_t& t) {
  _classTypeStr = t;
  if (t == "PAD") {
    _classType = LefSiteClassType::PAD;
  }
  else if (t == "CORE") {
    _classType = LefSiteClassType::CORE;
  }
  else {
    fprintf(stderr, "LefSite::%s Undefined Class Type\n", __func__);
    assert(false);
  }
}

void LefSite::setXSymmetry() {
  _bXSymmetry = true;
}

void LefSite::setYSymmetry() {
  _bYSymmetry = true;
}

void LefSite::setR90Symmetry() {
  _bR90Symmetry = true;
}

void LefSite::setSizeX(const Int_t s) {
  _sizeX = s;
}

void LefSite::setSizeY(const Int_t s) {
  _sizeY = s;
}

void LefSite::addRowPattern(const String_t& n, const String_t& o) {
  _rowPattern.addPattern(n, o);
}

void LefSite::logInfo() const {
  FILE* fout = stderr;
  fprintf(fout, "SITE %s\n", _name.c_str());
  fprintf(fout, "  SIZE %d BY %d\n", _sizeX, _sizeY);
  fprintf(fout, "  CLASS %s\n", _classTypeStr.c_str());
  if (_bXSymmetry or _bYSymmetry or _bR90Symmetry)
    fprintf(fout, "  SYMMETRY");
  if (_bXSymmetry)
    fprintf(fout, " X");
  if (_bYSymmetry)
    fprintf(fout, " Y");
  if (_bR90Symmetry)
    fprintf(fout, " R90");
  fprintf(fout, "\n");
  if (_rowPattern.numSites()) {
    fprintf(fout, "  ROWPATTERN");
    for (UInt_t i = 0; i < _rowPattern.numSites(); ++i) {
      fprintf(fout, " %s %s", _rowPattern.siteName(i).c_str(), _rowPattern.orientStr(i).c_str());
    }
    fprintf(fout, "\n");
  }
  fprintf(fout, "END %s\n\n", _name.c_str());
}

PROJECT_NAMESPACE_END
