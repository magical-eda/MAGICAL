/**
 * @file   drcMgr.hpp
 * @brief  Design Rule Checking - Manager
 * @author Hao Chen
 * @date   10/23/2019
 *
 **/

#ifndef _DRC_MGR_HPP_
#define _DRC_MGR_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"
#include "src/geo/box.hpp"
#include "src/geo/segment.hpp"
#include "src/geo/spatial.hpp"
#include "src/geo/spatial3d.hpp"

PROJECT_NAMESPACE_START

class DrcMgr {
 public:
  DrcMgr(CirDB& c)
    : _cir(c), _vSpatialPins(c.vSpatialPins()), _vSpatialBlks(c.vSpatialBlks()),
      _vSpatialRoutedWires(c.vSpatialRoutedWires()) {}
  ~DrcMgr() {}

  // return true: no violation ; false : violation

  /////////////////////////////////////////
  //    Wire level checking              //
  /////////////////////////////////////////
  // short
  bool checkWireRoutingLayerShort(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  bool checkWireCutLayerShort(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  // min area
  // FIXME
  bool checkWireMinArea(const UInt_t layerIdx, const Vector_t<Box<Int_t>>& vBoxes) const;
  // spacing
  bool checkWireRoutingLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b, const Int_t prl = 0) const;
  bool checkWireCutLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  bool checkWireEolSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  bool checkViaSpacing(const UInt_t netIdx, const Int_t x, const Int_t y, const LefVia& via) const;

  /////////////////////////////////////////
  //    Net level checking               //
  /////////////////////////////////////////
  bool checkSameNetRoutingLayerSpacing(const UInt_t netIdx) const;

 private:
  CirDB& _cir;
  const Vector_t<SpatialMap<Int_t, UInt_t>>&  _vSpatialPins;
  const Vector_t<SpatialMap<Int_t, UInt_t>>&  _vSpatialBlks;
  const Vector_t<SpatialMap<Int_t, UInt_t>>&  _vSpatialRoutedWires;

  void addNetShapesBFS(const Int_t netIdx, Vector_t<Vector_t<Box<Int_t>>>& vvBoxes) const;
  bool bCanPatch(const Int_t layerIdx, const Segment<Int_t>& s1, const Segment<Int_t>& s2) const;
  

};

PROJECT_NAMESPACE_END

#endif /// _DRC_MGR_HPP_
