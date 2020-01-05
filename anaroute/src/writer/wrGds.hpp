/**
 * @file   wrGds.hpp
 * @brief  Output file writer - Gds format
 * @author Hao Chen
 * @date   10/22/2019
 *
 **/

#ifndef _WRITER_GDS_HPP_
#define _WRITER_GDS_HPP_

#include "src/global/global.hpp"
#include "src/geo/box.hpp"

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
//#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>

PROJECT_NAMESPACE_START

enum class GdsDataType : Byte_t {
  NODATA   = 0,
  BITARRAY = 1,
  INT2     = 2,
  INT4     = 3,
  REAL4    = 4,
  REAL8    = 5,
  ASCII    = 6
};

// see http://www.artwork.com/gdsii/gdsii/page4.htm for the end cap path types
enum class GdsPathEndCapType : Byte_t {
  FLUSH  = 0,
  HALF_ROUND_EXTENSION = 1,
  HALF_WIDTH_EXTENSION = 2,
  CUSTOM = 4
};

class GdsWriter {
 public:
  GdsWriter(const String_t& filename)
    : _gw(filename.c_str()), _bActive(false) {}
  ~GdsWriter() {}

  //////////////////////////////
  /// Basics
  //////////////////////////////
  Int_t gdsDataType2Int(GdsDataType dataType);
  Int_t gdsPathEndCapType2Int(GdsPathEndCapType pathEnd);

  //////////////////////////////
  /// Writing function routines
  //////////////////////////////
  bool initWriter();
  void createLib(const String_t& libName, const Float_t dbu_uu = 0.001, const Float_t dbu_m = 1e-09);
  void endLib();
  void writeCellBgn(const String_t& cellName);
  void writeCellEnd();


  ////////////////////////////////
  /// Write shapes, cells etc.
  ////////////////////////////////
  /// Write a cell/structure
  void writeCellRef(const String_t& cellName, const Point<Int_t>& loc, const Float_t mag = 1.0, const Float_t angle = 0.0);

  /// Write an rectangle
  void writeRectangle(const Box<Int_t>& box, const Int_t layerIdx = 1, const Int_t dataType = 0);
  void writeRectangle(const Box<Int_t>& box, const Int_t layerIdx = 1, const GdsDataType dataType = GdsDataType::NODATA);

  /// Write an text
  void writeText(const String_t& text, const Int_t x, const Int_t y, const Int_t layerIdx, const Int_t size);

  /// write an path
  void writePath(const Vector_t<Int_t>& vPts, const Int_t width, const Int_t layerIdx = 1, const Int_t dataType = 0, const Int_t endCapType = 0);
  void writePath(const Vector_t<Int_t>& vPts, const Int_t width, const Int_t layerIdx = 1, const GdsDataType dataType = GdsDataType::NODATA, const GdsPathEndCapType endCapType= GdsPathEndCapType::FLUSH);


 private:
  GdsParser::GdsWriter _gw;
  bool _bActive;
};

namespace GetSRefNameActionDetails {
  template<typename ObjectType>
    inline void getSName(String_t& name, GdsParser::GdsRecords::EnumType type, ObjectType* object) {}

  template<>
    inline void getSName(String_t& name, GdsParser::GdsRecords::EnumType type, GdsParser::GdsDB::GdsCellReference* object) {
      name = object->refCell();
    }
}

struct GetSRefNameAction {
  GetSRefNameAction(String_t& name)
    : _name(name) {}
  template<typename ObjectType>
    void operator()(GdsParser::GdsRecords::EnumType type, ObjectType* object) {
      GetSRefNameActionDetails::getSName(_name, type, object);
    }

  String_t message() const {
    return "GetSRefNameAction";
  }


  String_t& _name; ///< The cell reference name
};


PROJECT_NAMESPACE_END

#endif /// _WRITER_GDS_HPP_
