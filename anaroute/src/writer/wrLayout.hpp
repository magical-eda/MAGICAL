/**
 * @file   wrLayout.hpp
 * @brief  Output file writer - final layout
 * @author Hao Chen
 * @date   11/21/2019
 *
 **/

#ifndef _WR_LAYOUT_HPP_
#define _WR_LAYOUT_HPP_

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>

#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class LayoutWriter {
 public:
  LayoutWriter(const CirDB& c)
    : _cir(c) {}
  ~LayoutWriter() {}

  void writeGds(const String_t& placementFilename, const String_t& outputFilename, const bool bFlatten);

 private:
  const CirDB& _cir;
  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  typedef GdsParser::GdsDB::GdsDB   GdsDB;
  typedef GdsParser::GdsDB::GdsCell GdsCell;
  typedef boost::polygon::point_data<Int_t> GdsPoint;

  bool      readPlacementGds(const String_t& filename, GdsParser::GdsDB::GdsDB& gdsDB);
  String_t  topCellName(const GdsDB& gdsDB);
  void      addRoutingLayout(GdsCell& gdsCell, const Float_t scale);
  void      addBox2Cell(GdsCell& gdsCell, const Int_t maskIdx, const Box<Int_t>& box, const Float_t scale, const Int_t dataType);
  void      addTxt2Cell(GdsCell& gdsCell, const Int_t maskIdx, const Point<Int_t>& pt, const String_t& txt, const Int_t size, const Float_t scale);

};

PROJECT_NAMESPACE_END

#endif /// _WR_LAYOUT_HPP_
