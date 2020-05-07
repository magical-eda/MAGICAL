/**
 * @file   lefRoutingLayer.cpp
 * @brief  Technology configuration - Lef Routing layer
 * @author Hao Chen
 * @date   09/12/2019
 *
 **/

#include "lefRoutingLayer.hpp"

PROJECT_NAMESPACE_START

void LefRoutingLayer::setName(const String_t& n) {
  _name = n;
}

void LefRoutingLayer::setRouteDir(const String_t& d) {
  _routeDirStr = d;
  if (d == "HORIZONTAL") _routeDir = RouteDir::HORIZONTAL;
  else if (d == "VERTICAL") _routeDir = RouteDir::VERTICAL;
  else assert(false);
}

void LefRoutingLayer::setMinArea(const Int_t a) {
  _minArea = a;
}

void LefRoutingLayer::setMinWidth(const Int_t w) {
  _minWidth = w;
}

void LefRoutingLayer::setMaxWidth(const Int_t w) {
  _maxWidth = w;
}

void LefRoutingLayer::setDefaultWidth(const Int_t w) {
  _defaultWidth = w;
}

void LefRoutingLayer::setPitch(const Int_t p) {
  _pitch = p;
}

void LefRoutingLayer::setPitchX(const Int_t p) {
  _pitchX = p;
}

void LefRoutingLayer::setPitchY(const Int_t p) {
  _pitchY = p;
}

void LefRoutingLayer::setOffset(const Int_t o) {
  _offset = o;
}

void LefRoutingLayer::setOffsetX(const Int_t o) {
  _offsetX = o;
}

void LefRoutingLayer::setOffsetY(const Int_t o) {
  _offsetY = o;
}

void LefRoutingLayer::addSpacing(const Int_t s) {
  _vSpacings.emplace_back(s);
}

void LefRoutingLayer::addEolSpacing(const Int_t s, const Int_t d, const Int_t w) {
  _vEolSpacings.emplace_back(s, d, w);
}

void LefRoutingLayer::addParallelRunLength(const Int_t p) {
  _spacingTable.vParallelRunLength.emplace_back(p);
}

void LefRoutingLayer::addSpacingTableWidth(const Int_t w) {
  _spacingTable.table.emplace_back(w, Vector_t<Int_t>());
}

void LefRoutingLayer::addSpacingTableWidthSpacing(const UInt_t idx, const Int_t s) {
  _spacingTable.table[idx].second.emplace_back(s);
}

void LefRoutingLayer::logInfo() const {
  FILE* fout = stderr;
  fprintf(fout,"LAYER %s\n", _name.c_str());
  fprintf(fout,"  TYPE ROUTING\n");
  fprintf(fout,"  DIRECTION %s\n", _routeDirStr.c_str());
  fprintf(fout,"  PITCH %d PITCH_X %d PITCH_Y %d\n", _pitch, _pitchX, _pitchY);
  fprintf(fout,"  OFFSET %d OFFSET_X %d OFFSET_Y %d\n", _offset, _offsetX, _offsetY);
  if (_vSpacings.size()) {
    fprintf(fout,"  SPACING\n");
    fprintf(fout,"    ");
    for (UInt_t i = 0; i < _vSpacings.size(); ++i)
      fprintf(fout,"%d ", _vSpacings[i]);
    fprintf(fout,"\n");
  }
  if (_vEolSpacings.size()) {
    fprintf(fout,"  EOL\n");
    fprintf(fout,"    ");
    for (UInt_t i = 0; i < _vEolSpacings.size(); ++i)
      fprintf(fout,"SPACING %d WIDTH %d WITHIN %d\n", _vEolSpacings[i].eolSpacing(),
                                                _vEolSpacings[i].eolWidth(),
                                                _vEolSpacings[i].eolWithin());
  }
  if (_spacingTable.vParallelRunLength.size()) {
    fprintf(fout,"SPACINGTABLE\n");
      fprintf(fout,"PARALLELRUNLENGTH");
    for (UInt_t i = 0; i < _spacingTable.vParallelRunLength.size(); ++i) {
      fprintf(fout,"  %d", _spacingTable.vParallelRunLength[i]);
    }
    fprintf(fout,"\n");
    for (UInt_t i = 0; i < _spacingTable.table.size(); ++i) {
      fprintf(fout,"WIDTH %d", _spacingTable.table[i].first);
      for (UInt_t j = 0; j < _spacingTable.table[i].second.size(); ++j) {
        fprintf(fout,"  %d", _spacingTable.table[i].second[j]);
      }
      fprintf(fout,"\n");
    }
  }
  fprintf(fout,"END %s\n", _name.c_str());
}

PROJECT_NAMESPACE_END
