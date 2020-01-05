/**
 * @file   lefCutLayer.cpp
 * @brief  Technology configuration - Lef Cut layer
 * @author Hao Chen
 * @date   09/12/2019
 *
 **/

#include "lefCutLayer.hpp"

PROJECT_NAMESPACE_START

void LefCutLayer::setName(const String_t& n) {
  _name = n;
}

void LefCutLayer::setMinWidth(const Int_t w) {
  _minWidth = w;
}

void LefCutLayer::setSpacing(const Int_t s) {
  _spacing = s;
}

void LefCutLayer::setSameNetSpacing(const Int_t s) {
  _sameNetSpacing = s;
}

void LefCutLayer::setParallelLength(const Int_t l) {
  _parallelLength = l;
}

void LefCutLayer::setParallelOverlapSpacing(const Int_t s) {
  _parallelOverlapSpacing = s;
}

void LefCutLayer::setExceotCutWithin(const Int_t w) {
  _exceptCutWithin = w;
}

void LefCutLayer::logInfo() const {
  FILE* fout = stderr;
  fprintf(fout,"LAYER %s\n", _name.c_str());
  fprintf(fout,"  TYPE CUT\n");
  fprintf(fout,"  WIDTH %d\n", _minWidth);
  fprintf(fout,"  SPACING %d\n", _spacing);
  fprintf(fout,"  SPACING %d SAMENET\n", _sameNetSpacing);
  fprintf(fout,"  SPACING %d PARALLELOVERLAP\n", _parallelOverlapSpacing);
  fprintf(fout,"END %s\n", _name.c_str());

}

PROJECT_NAMESPACE_END
