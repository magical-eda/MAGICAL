/**
 * @file   drViaSelector.cpp
 * @brief  Detailed Routing - Via Selector
 * @author Hao Chen
 * @date   10/27/2019
 *
 **/

#include "drViaSelector.hpp"

PROJECT_NAMESPACE_START

UInt_t DrViaSelector::selectViaIdx(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  const Int_t zl = std::min(u.z(), v.z());
  const Int_t zc = zl + 1;
  const Int_t zh = std::max(u.z(), v.z());
  assert(zh - zl == 2);
  assert(u.x() == v.x() and u.y() == v.y());
  const Int_t x = u.x();
  const Int_t y = u.y();
  UInt_t i, j, ret = MAX_INT;
  //UInt_t viaIdx;
  //Lef_ForEachLayerViaIdx(_cir.lef(), zc, viaIdx, i) {
  UInt_t maxIdx = _cir.lef().numVias();
  for (i = 0; i < maxIdx; ++i) {
    bool bViolate = false;
    const LefVia& lefVia = _cir.lef().via(i);
    if ((Int_t)_cir.lef().str2LayerIdx(lefVia.cutLayerName()) != zc)
      continue;
    for (j = 0; j < lefVia.numBotBoxes(); ++j) {
      Box<Int_t> box(lefVia.botBox(j));
      box.shift(x, y);
      //if (!_drcMgr.checkWireRoutingLayerShort(netIdx, zl, box)) {
        //bViolate = true;
        //break;
      //}
      if (!_drcMgr.checkWireRoutingLayerSpacing(netIdx, zl, box)) {
        bViolate = true;
        break;
      }
      if (!_drcMgr.checkWireEolSpacing(netIdx, zl, box)) {
        bViolate = true;
        break;
      }
    }
    if (bViolate)
      continue;
    for (j = 0; j < lefVia.numCutBoxes(); ++j) {
      Box<Int_t> box(lefVia.cutBox(j));
      box.shift(x, y);
      //if (!_drcMgr.checkWireCutLayerShort(netIdx, zc, box)) {
        //bViolate = true;
        //break;
      //}
      if (!_drcMgr.checkWireCutLayerSpacing(netIdx, zc, box)) {
        bViolate = true;
        break;
      }
    }
    if (bViolate)
      continue;
    for (j = 0; j < lefVia.numTopBoxes(); ++j) {
      Box<Int_t> box(lefVia.topBox(j));
      box.shift(x, y);
      //if (!_drcMgr.checkWireRoutingLayerShort(netIdx, zh, box)) {
        //bViolate = true;
        //break;
      //}
      if (!_drcMgr.checkWireRoutingLayerSpacing(netIdx, zh, box)) {
        bViolate = true;
        break;
      }
      if (!_drcMgr.checkWireEolSpacing(netIdx, zh, box)) {
        bViolate = true;
        break;
      }
    }
    if (bViolate)
      continue;
    ret = i;
    break;
  }
  return ret;
}

UInt_t DrViaSelector::selectViaIdxSym(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v, const Int_t symAxisX) {
  assert(_cir.net(netIdx).hasSymNet());
  const UInt_t symNetIdx = _cir.net(netIdx).symNetIdx();
  const Int_t zl = std::min(u.z(), v.z());
  const Int_t zc = zl + 1;
  const Int_t zh = std::max(u.z(), v.z());
  assert(zh - zl == 2);
  assert(u.x() == v.x() and u.y() == v.y());
  const Int_t x = u.x();
  const Int_t y = u.y();
  UInt_t i, j, ret = MAX_INT;
  //UInt_t viaIdx;
  //Lef_ForEachLayerViaIdx(_cir.lef(), zc, viaIdx, i) {
  UInt_t maxIdx = _cir.lef().numVias();
  for (i = 0; i < maxIdx; ++i) {
    bool bViolate = false;
    const LefVia& lefVia = _cir.lef().via(i);
    if ((Int_t)_cir.lef().str2LayerIdx(lefVia.cutLayerName()) != zc)
      continue;
    for (j = 0; j < lefVia.numBotBoxes(); ++j) {
      Box<Int_t> box(lefVia.botBox(j));
      box.shift(x, y);
      Box<Int_t> symBox(box);
      symBox.flipX(symAxisX);
      //if (!_drcMgr.checkWireRoutingLayerShort(netIdx, zl, box)) {
        //bViolate = true;
        //break;
      //}
      if (!_drcMgr.checkWireRoutingLayerSpacing(netIdx, zl, box) or
          !_drcMgr.checkWireRoutingLayerSpacing(symNetIdx, zl, symBox)) {
        bViolate = true;
        break;
      }
      if (!_drcMgr.checkWireEolSpacing(netIdx, zl, box) or
          !_drcMgr.checkWireEolSpacing(symNetIdx, zl, symBox)) {
        bViolate = true;
        break;
      }
    }
    if (bViolate)
      continue;
    for (j = 0; j < lefVia.numCutBoxes(); ++j) {
      Box<Int_t> box(lefVia.cutBox(j));
      box.shift(x, y);
      Box<Int_t> symBox(box);
      symBox.flipX(symAxisX);
      //if (!_drcMgr.checkWireCutLayerShort(netIdx, zc, box)) {
        //bViolate = true;
        //break;
      //}
      if (!_drcMgr.checkWireCutLayerSpacing(netIdx, zc, box) or
          !_drcMgr.checkWireCutLayerSpacing(symNetIdx, zc, symBox)) {
        bViolate = true;
        break;
      }
    }
    if (bViolate)
      continue;
    for (j = 0; j < lefVia.numTopBoxes(); ++j) {
      Box<Int_t> box(lefVia.topBox(j));
      box.shift(x, y);
      Box<Int_t> symBox(box);
      symBox.flipX(symAxisX);
      //if (!_drcMgr.checkWireRoutingLayerShort(netIdx, zh, box)) {
        //bViolate = true;
        //break;
      //}
      if (!_drcMgr.checkWireRoutingLayerSpacing(netIdx, zh, box) or
          !_drcMgr.checkWireRoutingLayerSpacing(symNetIdx, zh, symBox)) {
        bViolate = true;
        break;
      }
      if (!_drcMgr.checkWireEolSpacing(netIdx, zh, box) or
          !_drcMgr.checkWireEolSpacing(symNetIdx, zh, symBox)) {
        bViolate = true;
        break;
      }
    }
    if (bViolate)
      continue;
    ret = i;
    break;
  }
  return ret;
}
PROJECT_NAMESPACE_END
