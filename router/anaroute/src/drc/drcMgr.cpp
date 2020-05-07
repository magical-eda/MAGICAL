/**
 * @file   drcMgr.cpp
 * @brief  Design Rule Checking - Manager
 * @author Hao Chen
 * @date   10/23/2019
 *
 **/

#include "drcMgr.hpp"

PROJECT_NAMESPACE_START

/////////////////////////////////////////
//    Wire level checking              //
/////////////////////////////////////////
// short
bool DrcMgr::checkWireRoutingLayerShort(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {
  // check pin
  Vector_t<UInt_t> vPinIndices;
  _cir.querySpatialPin(layerIdx, b, vPinIndices);
  for (const UInt_t pinIdx : vPinIndices) {
    const Pin& pin = _cir.pin(pinIdx);
    if (pin.netIdx() != netIdx)
      return false;
  }
  // check other net's wires
  Vector_t<UInt_t> vNetIndices;
  _cir.querySpatialRoutedWire(layerIdx, b, vNetIndices);
  for (const UInt_t idx : vNetIndices) {
    if (idx != netIdx)
      return false;
  }
  // check blk
  Vector_t<UInt_t> vBlkIndices;
  _cir.querySpatialBlk(layerIdx, b, vBlkIndices);
  for (const UInt_t idx : vBlkIndices) {
    const Blk& blk = _cir.blk(idx);
    if (!blk.bDummy())
      return false;
  }
  return true;
}

bool DrcMgr::checkWireCutLayerShort(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {
  // no pin in cut layers
  // check other net's wire (via)
  Vector_t<UInt_t> vNetIndices;
  _cir.querySpatialRoutedWire(layerIdx, b, vNetIndices);
  for (const UInt_t idx : vNetIndices) {
    if (idx != netIdx)
      return false;
  }
  return true;
}

// min area
bool DrcMgr::checkWireMinArea(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {
  // TODO
  return true;
}

// spacing
bool DrcMgr::checkWireRoutingLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {
  const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(layerIdx);
  assert(layerPair.first == LefLayerType::ROUTING);
  const LefRoutingLayer& layer = _cir.lef().routingLayer(layerPair.second);
  const Int_t prlSpacing = layer.spacingTable().table.size() ?
                           layer.spacingTable().table[0].second[0] :
                           layer.spacing(0);
  //const Int_t eolSpacing = layer.numEolSpacings() ? layer.eolSpacing(0) : 0;
  //const Int_t eolWithin = layer.numEolSpacings() ? layer.eolWithin(0) : 0;
  
  Box<Int_t> checkBox(b);
  checkBox.expand(prlSpacing - 1);
  
  // check pin
  Vector_t<UInt_t> vPinIndices;
  _cir.querySpatialPin(layerIdx, checkBox, vPinIndices);
  for (const UInt_t pinIdx : vPinIndices) {
    const Pin& pin = _cir.pin(pinIdx);
    if (pin.netIdx() != netIdx)
      return false;
  }
  // check other net's wires
  Vector_t<UInt_t> vNetIndices;
  _cir.querySpatialRoutedWire(layerIdx, checkBox, vNetIndices);
  for (const UInt_t idx : vNetIndices) {
    if (idx != netIdx)
      return false;
  }
  // check blk
  Vector_t<UInt_t> vBlkIndices;
  _cir.querySpatialBlk(layerIdx, checkBox, vBlkIndices);
  for (const UInt_t idx : vBlkIndices) {
    const Blk& blk = _cir.blk(idx);
    //if (!blk.bDummy()) {
      //return false;
    //}
    if (blk.pinIdx() == MAX_UINT)
      return false;
    else if (netIdx != _cir.pin(blk.pinIdx()).netIdx())
      return false;
  }
  return true;
}

bool DrcMgr::checkWireCutLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {
  const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(layerIdx);
  assert(layerPair.first == LefLayerType::CUT);
  const LefCutLayer& layer = _cir.lef().cutLayer(layerPair.second);
  const Int_t spacing = layer.spacing();
  
  Box<Int_t> checkBox(b);
  checkBox.expand(spacing - 1);
  
  // no pin in cut layers
  // check other net's wire (via)
  Vector_t<UInt_t> vNetIndices;
  _cir.querySpatialRoutedWire(layerIdx, checkBox, vNetIndices);
  for (const UInt_t idx : vNetIndices) {
    if (idx != netIdx)
      return false;
  }
  return true;
}

bool DrcMgr::checkWireEolSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {

  return true;
}

PROJECT_NAMESPACE_END
