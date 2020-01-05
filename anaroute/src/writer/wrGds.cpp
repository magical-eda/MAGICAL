/**
 * @file   wrGds.cpp
 * @brief  Output file writer - Gds format
 * @author Hao Chen
 * @date   10/22/2019
 *
 **/

#include "wrGds.hpp"

PROJECT_NAMESPACE_START

Int_t GdsWriter::gdsDataType2Int(GdsDataType dataType) {
  switch(dataType) {
    case GdsDataType::NODATA : return 0; break;
    case GdsDataType::BITARRAY : return 1; break;
    case GdsDataType::INT2 : return 2; break;
    case GdsDataType::INT4 : return 3; break;
    case GdsDataType::REAL4 : return 4; break;
    case GdsDataType::REAL8 : return 5; break;
    case GdsDataType::ASCII : return 6; break;
    default : return 0; break;
  }
}

Int_t GdsWriter::gdsPathEndCapType2Int(GdsPathEndCapType pathEnd) {
  switch(pathEnd) {
    case GdsPathEndCapType::FLUSH : return 0; break;
    case GdsPathEndCapType::HALF_ROUND_EXTENSION : return 1; break;
    case GdsPathEndCapType::HALF_WIDTH_EXTENSION : return 2; break;
    case GdsPathEndCapType::CUSTOM : return 4; break;
    default : return 0; break;
  }
}

//////////////////////////////
/// Writing function routines
//////////////////////////////
bool GdsWriter::initWriter() {
  if (_bActive) {
    fprintf(stderr, "GdsWriter::%s ERROR: Try initing a gdsWriter while there is already one in active!\n", __func__);
    return false;
  }
  _bActive = true;
  return true;
}

void GdsWriter::createLib(const String_t& libName, const Float_t dbu_uu, const Float_t dbu_m) {
  _gw.create_lib(libName.c_str(), dbu_uu, dbu_m);
}

void GdsWriter::endLib() {
  _gw.gds_write_endlib();
  _bActive = false;
}

void GdsWriter::writeCellBgn(const String_t& cellName) {
  _gw.gds_write_bgnstr();
  _gw.gds_write_strname(cellName.c_str());
}

void GdsWriter::writeCellEnd() {
  _gw.gds_write_endstr();
}

////////////////////////////////
/// Write shapes, cells etc.
////////////////////////////////
/// Write a cell/structure
void GdsWriter::writeCellRef(const String_t& cellName, const Point<Int_t>& loc, const Float_t mag, const Float_t angle) {
  _gw.gds_write_sref();
  _gw.gds_write_sname(cellName.c_str());
  int x[1], y[1];
  x[0] = loc.x();
  y[0] = loc.y();
  _gw.gds_write_xy(x, y, 1);
  _gw.gds_write_mag(mag);
  _gw.gds_write_angle(angle);
  _gw.gds_write_endel();
}

/// Write an rectangle
void GdsWriter::writeRectangle(const Box<Int_t>& box, const Int_t layerIdx, const Int_t dataType) {
  _gw.gds_write_boundary();
  _gw.gds_write_layer(static_cast<short>(layerIdx));
  _gw.gds_write_datatype(static_cast<short>(dataType));

  int x[5], y[5];
  x[0] = box.xl(); y[0] = box.yl();
  x[1] = box.xh(); y[1] = box.yl();
  x[2] = box.xh(); y[2] = box.yh();
  x[3] = box.xl(); y[3] = box.yh();
  x[4] = box.xl(); y[4] = box.yl();

  _gw.gds_write_xy(x, y, 5);
  _gw.gds_write_endel();

}

void GdsWriter::writeRectangle(const Box<Int_t>& box, const Int_t layerIdx, const GdsDataType dataType) {
  writeRectangle(box, layerIdx, gdsDataType2Int(dataType));
}

/// Write an text
void GdsWriter::writeText(const String_t& text, const Int_t x, const Int_t y, const Int_t layerIdx, const Int_t size) {
  _gw.gds_create_text(text.c_str(), x, y, layerIdx, size);
}

/// write an path
void GdsWriter::writePath(const Vector_t<Int_t>& vPts, const Int_t width, const Int_t layerIdx, const Int_t dataType, const Int_t endCapType) {
  Vector_t<int> vXs;
  Vector_t<int> vYs;
  vXs.reserve(vPts.size());
  vYs.reserve(vPts.size());
  for (const Point<Int_t>& p : vPts) {
    vXs.emplace_back(p.x());
    vYs.emplace_back(p.y());
  }

  _gw.gds_write_path();
  _gw.gds_write_layer(layerIdx);
  _gw.gds_write_datatype(dataType);
  _gw.gds_write_pathtype(endCapType);
  _gw.gds_write_xy(&vXs[0], &vYs[0], vPts.size());
  _gw.gds_write_endel();
}

void GdsWriter::writePath(const Vector_t<Int_t>& vPts, const Int_t width, const Int_t layerIdx, const GdsDataType dataType, const GdsPathEndCapType endCapType) {
  writePath(vPts, width, layerIdx, gdsDataType2Int(dataType), gdsPathEndCapType2Int(endCapType));
}

PROJECT_NAMESPACE_END
