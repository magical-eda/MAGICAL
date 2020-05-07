/**
 * @file   grGridCell.hpp
 * @brief  Global Routing Structure - GridCell
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/


#ifndef _GR_GRID_CELL_HPP_
#define _GR_GRID_CELL_HPP_

#include "src/global/global.hpp"
#include "src/geo/box.hpp"

PROJECT_NAMESPACE_START

class GrGridCell {
 public:
  GrGridCell()
    : _z(MAX_UINT), _width(0), _height(0), _bValid(true) {}
  ~GrGridCell() {}

  /////////////////////////////////////////
  //    Getters                          //
  /////////////////////////////////////////
  const Box<Int_t>&         box()                   const { return _box; }
  Point<Int_t>              center()                const { return _box.center(); }
  Point<Int_t>              bl()                    const { return _box.bl(); }
  Point<Int_t>              tr()                    const { return _box.tr(); }
  Int_t                     z()                     const { return _z; }
  Int_t                     width()                 const { return _width; }
  Int_t                     height()                const { return _height; }
  UInt_t                    pinIdx(const UInt_t i)  const { return _vPinIndices[i]; }
  UInt_t                    numPins()               const { return _vPinIndices.size(); }
  const Vector_t<UInt_t>&   vPinIndices()           const { return _vPinIndices; }
  bool                      bValid()                const { return _bValid; }

  /////////////////////////////////////////
  //    Setters                          //
  /////////////////////////////////////////
  void setBox(const Box<Int_t>& b)                            { _box = b; }
  void setBox(const Point<Int_t>& bl, const Point<Int_t>& tr) { _box = Box<Int_t>(bl, tr); }
  void setZ(const UInt_t i)                                   { _z = i; }
  void setWidth(const Int_t w)                                { _width = w; }
  void setHeight(const Int_t h)                               { _height = h; }
  void addPinIdx(const UInt_t i)                              { _vPinIndices.emplace_back(i); }
  void setValid()                                             { _bValid = true; }
  void setInvalid()                                           { _bValid = false; }

 private:
  Box<Int_t>        _box;
  UInt_t            _z;
  Int_t             _width;
  Int_t             _height;
  Vector_t<UInt_t>  _vPinIndices; // pins located in this grid
  bool              _bValid;
};

PROJECT_NAMESPACE_END

#endif /// _GR_GRID_CELL_HPP_
