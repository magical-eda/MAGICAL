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
  bool checkWireMinArea(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  // spacing
  bool checkWireRoutingLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  bool checkWireCutLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  bool checkWireEolSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const;
  
  /////////////////////////////////////////
  //    Net level checking               //
  /////////////////////////////////////////

 private:
  CirDB& _cir;
  const Vector_t<SpatialMap<Int_t, UInt_t>>&  _vSpatialPins;
  const Vector_t<SpatialMap<Int_t, UInt_t>>&  _vSpatialBlks;
  const Vector_t<SpatialMap<Int_t, UInt_t>>&  _vSpatialRoutedWires;

};

PROJECT_NAMESPACE_END

#endif /// _DRC_MGR_HPP_
