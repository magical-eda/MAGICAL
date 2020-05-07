/**
 * @file   dbBlk.hpp
 * @brief  Circuit Element - Blkage
 * @author Hao Chen
 * @date   09/20/2019
 *
 **/

#ifndef _DB_BLOCK_HPP_
#define _DB_BLOCK_HPP_

#include "src/global/global.hpp"
#include "src/geo/box.hpp"

PROJECT_NAMESPACE_START

class Blk {
  friend class Parser;
  friend class CirDB;
 public:
  Blk()
    : _idx(MAX_UINT), _layerIdx(MAX_UINT), _bDummy(false), _pinIdx(MAX_UINT) {}
  Blk(const UInt_t i, const UInt_t l, const Box<Int_t>& b)
    : _idx(i), _layerIdx(l), _box(b), _bDummy(false), _pinIdx(MAX_UINT) {}
  ~Blk() {}
  
  ////////////////////////////////////////
  //   Getter                           //
  ////////////////////////////////////////
  UInt_t                          idx()                     const { return _idx; }
  UInt_t                          layerIdx()                const { return _layerIdx; }
  const Box<Int_t>&               box()                     const { return _box; }
  Int_t                           xl()                      const { return _box.xl(); } 
  Int_t                           yl()                      const { return _box.yl(); } 
  Int_t                           xh()                      const { return _box.xh(); } 
  Int_t                           yh()                      const { return _box.yh(); }
  const Point<Int_t>&             bl()                      const { return _box.bl(); }
  const Point<Int_t>&             tr()                      const { return _box.tr(); }
  const Point<Int_t>&             min_corner()              const { return _box.min_corner(); }
  const Point<Int_t>&             max_corner()              const { return _box.max_corner(); }
  // connected pin
  UInt_t                          pinIdx()                  const { return _pinIdx; }

  // 
  bool bDummy() const { return _bDummy; }
  void setDummy() { _bDummy = true; }

 private:
  UInt_t                _idx;
  UInt_t                _layerIdx;
  Box<Int_t>            _box;
  bool                  _bDummy;
  UInt_t                _pinIdx;
  
  ////////////////////////////////////////
  //   Setter                           //
  ////////////////////////////////////////
  void setIdx(const UInt_t i) { _idx = i; }
  void setLayerIdx(const UInt_t i) { _layerIdx = i; }
  void setBox(const Box<Int_t>& b) { _box = b; }
  void setPinIdx(const UInt_t i) { _pinIdx = i; }
};

PROJECT_NAMESPACE_END

#endif /// _DB_BLOCK_HPP_
