/**
 * @file   dbCir.cpp
 * @brief  The Top-Level Database - Circuit
 * @author Hao Chen
 * @date   09/23/2019
 *
 **/

#include "dbCir.hpp"
#include "src/writer/wrGds.hpp"

PROJECT_NAMESPACE_START

//////////////////////////////////
//  Funcs                       //
//////////////////////////////////
void CirDB::buildSpatial() {
  buildSpatialPins();
  buildSpatialBlks();
  initSpatialRoutedWires();
}

void CirDB::buildSpatialPins() {
  Vector_t<Vector_t<spatial::b_value<Int_t, UInt_t>>> vvShapes;
  vvShapes.resize(_lef.numLayers());
  UInt_t i, j, layerIdx;
  const Pin* cpPin;
  const Box<Int_t>* cpBox;
  Cir_ForEachPinC((*this), cpPin, i) {
    Pin_ForEachLayerIdx((*cpPin), layerIdx) {
      Pin_ForEachLayerBoxC((*cpPin), layerIdx, cpBox, j) {
        vvShapes[layerIdx].emplace_back(spatial::b_box<Int_t>(cpBox->bl(), cpBox->tr()), i);
      }
    }
  }
  _vSpatialPins.resize(_lef.numLayers());
  Cir_ForEachLayerIdx((*this), layerIdx) {
    _vSpatialPins[layerIdx] = SpatialMap<Int_t, UInt_t>(vvShapes[layerIdx]);
  }
}

void CirDB::buildSpatialBlks() {
  Vector_t<Vector_t<spatial::b_value<Int_t, UInt_t>>> vvShapes;
  vvShapes.resize(_lef.numLayers());
  _vSpatialBlks.resize(_lef.numLayers());
  UInt_t i, layerIdx;
  const Blk* cpBlk;
  Cir_ForEachLayerIdx((*this), layerIdx) {
    Cir_ForEachLayerBlkC((*this), layerIdx, cpBlk, i) {
      vvShapes[layerIdx].emplace_back(spatial::b_box<Int_t>(cpBlk->bl(), cpBlk->tr()), cpBlk->idx());
    }
    _vSpatialBlks[layerIdx] = SpatialMap<Int_t, UInt_t>(vvShapes[layerIdx]);
  }
}

void CirDB::buildSpatialNetGuides() {
  _vvSpatialNetGuides.resize(_vNets.size());
  UInt_t i;
  const Net* cpNet;
  Cir_ForEachNetC((*this), cpNet, i) {
    const Vector_t<Pair_t<Box<Int_t>, Int_t>>& vGuides = cpNet->vGuides();
    _vvSpatialNetGuides[i].resize(_lef.numLayers());
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vGuides) {
      _vvSpatialNetGuides[i][pair.second].insert(pair.first);
    }
  }
}

void CirDB::initSpatialRoutedWires() {
  _vSpatialRoutedWires.resize(_lef.numLayers());
}

void CirDB::addSpatialRoutedWire(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  assert(u.z() == v.z());
  const UInt_t layerIdx = u.z();
  assert(layerIdx >= 0 and layerIdx < _vSpatialRoutedWires.size());
  assert(_lef.bRoutingLayer(layerIdx) or _lef.bMastersliceLayer(layerIdx));
  const Pair_t<LefLayerType, UInt_t>& layerPair = _lef.layerPair(layerIdx);
  const LefRoutingLayer& routingLayer = _lef.routingLayer(layerPair.second);
  const Int_t halfWidth = routingLayer.minWidth() / 2;
  const Int_t xl = std::min(u.x(), v.x()) - halfWidth;
  const Int_t xh = std::max(u.x(), v.x()) + halfWidth;
  const Int_t yl = std::min(u.y(), v.y()) - halfWidth;
  const Int_t yh = std::max(u.y(), v.y()) + halfWidth;
  const Point<Int_t> min_corner(xl, yl);
  const Point<Int_t> max_corner(xh, yh);
  _vSpatialRoutedWires[layerIdx].insert(min_corner, max_corner, netIdx);
}

void CirDB::addSpatialRoutedWire(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& box) {
  _vSpatialRoutedWires[layerIdx].insert(box, netIdx);
}

void CirDB::addSpatialRoutedVia(const UInt_t netIdx, const UInt_t viaIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  assert(u.x() == v.x() and u.y() == v.y());
  const Int_t x = u.x();
  const Int_t y = u.y();
  const Int_t botLayerIdx = std::min(u.z(), v.z()); 
  const Int_t cutLayerIdx = botLayerIdx + 1;
  const Int_t topLayerIdx = std::max(u.z(), v.z());
  assert(topLayerIdx - botLayerIdx == 2); // routing-routing, or master-routing

  const LefVia& via = _lef.via(viaIdx);
  for (const Box<Int_t>& box : via.vBotBoxes()) {
    Box<Int_t> shift_box(box);
    shift_box.shift(x, y);
    _vSpatialRoutedWires[botLayerIdx].insert(shift_box, netIdx);
  }
  for (const Box<Int_t>& box : via.vCutBoxes()) {
    Box<Int_t> shift_box(box);
    shift_box.shift(x, y);
    _vSpatialRoutedWires[cutLayerIdx].insert(shift_box, netIdx);
  }
  for (const Box<Int_t>& box : via.vTopBoxes()) {
    Box<Int_t> shift_box(box);
    shift_box.shift(x, y);
    _vSpatialRoutedWires[topLayerIdx].insert(shift_box, netIdx);
  }
  
}

bool CirDB::removeSpatialRoutedWire(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  assert(u.z() == v.z());
  const UInt_t layerIdx = u.z();
  assert(layerIdx >= 0 and layerIdx < _vSpatialRoutedWires.size());
  assert(_lef.bRoutingLayer(layerIdx) or _lef.bMastersliceLayer(layerIdx));
  const Pair_t<LefLayerType, UInt_t>& layerPair = _lef.layerPair(layerIdx);
  const LefRoutingLayer& routingLayer = _lef.routingLayer(layerPair.second);
  const Int_t halfWidth = routingLayer.minWidth() / 2;
  const Int_t xl = std::min(u.x(), v.x()) - halfWidth;
  const Int_t xh = std::max(u.x(), v.x()) + halfWidth;
  const Int_t yl = std::min(u.y(), v.y()) - halfWidth;
  const Int_t yh = std::max(u.y(), v.y()) + halfWidth;
  const Point<Int_t> min_corner(xl, yl);
  const Point<Int_t> max_corner(xh, yh);
  return _vSpatialRoutedWires[layerIdx].erase(min_corner, max_corner, netIdx);
}

bool CirDB::removeSpatialRoutedWire(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& box) {
  return _vSpatialRoutedWires[layerIdx].erase(box, netIdx);
}

bool CirDB::removeSpatialRoutedVia(const UInt_t netIdx, const UInt_t viaIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  assert(u.x() == v.x() and u.y() == v.y());
  const Int_t x = u.x();
  const Int_t y = u.y();
  const Int_t botLayerIdx = std::min(u.z(), v.z()); 
  const Int_t cutLayerIdx = botLayerIdx + 1;
  const Int_t topLayerIdx = std::max(u.z(), v.z());
  assert(topLayerIdx - botLayerIdx == 2); // routing-routing, or master-routing

  const LefVia& via = _lef.via(viaIdx);
  bool ret = true;
  for (const Box<Int_t>& box : via.vBotBoxes()) {
    Box<Int_t> shift_box(box);
    shift_box.shift(x, y);
    ret &= _vSpatialRoutedWires[botLayerIdx].erase(shift_box, netIdx);
  }
  for (const Box<Int_t>& box : via.vCutBoxes()) {
    Box<Int_t> shift_box(box);
    shift_box.shift(x, y);
    ret &= _vSpatialRoutedWires[cutLayerIdx].erase(shift_box, netIdx);
  }
  for (const Box<Int_t>& box : via.vTopBoxes()) {
    Box<Int_t> shift_box(box);
    shift_box.shift(x, y);
    ret &= _vSpatialRoutedWires[topLayerIdx].erase(shift_box, netIdx);
  }
  return ret;
}

bool CirDB::querySpatialPin(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, Vector_t<UInt_t>& vPinIndices) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialPins.size());
  _vSpatialPins[layerIdx].query(bl, tr, vPinIndices);
  return !vPinIndices.empty();
}

bool CirDB::querySpatialPin(const UInt_t layerIdx, const Box<Int_t>& box, Vector_t<UInt_t>& vPinIndices) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialPins.size());
  _vSpatialPins[layerIdx].query(box, vPinIndices);
  return !vPinIndices.empty();
}

bool CirDB::querySpatialBlk(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, Vector_t<UInt_t>& vBlkIndices) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialBlks.size());
  _vSpatialBlks[layerIdx].query(bl, tr, vBlkIndices);
  return !vBlkIndices.empty();
}

bool CirDB::querySpatialBlk(const UInt_t layerIdx, const Box<Int_t>& box, Vector_t<UInt_t>& vBlkIndices) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialBlks.size());
  _vSpatialBlks[layerIdx].query(box, vBlkIndices);
  return !vBlkIndices.empty();
}

bool CirDB::existSpatialPin(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr) {
  Vector_t<UInt_t> vTmp;
  return querySpatialPin(layerIdx, bl, tr, vTmp);
}

bool CirDB::existSpatialPin(const UInt_t layerIdx, const Box<Int_t>& box) {
  Vector_t<UInt_t> vTmp;
  return querySpatialPin(layerIdx, box, vTmp);
}

bool CirDB::existSpatialBlk(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr) {
  Vector_t<UInt_t> vTmp;
  return querySpatialBlk(layerIdx, bl, tr, vTmp);
}

bool CirDB::existSpatialBlk(const UInt_t layerIdx, const Box<Int_t>& box) {
  Vector_t<UInt_t> vTmp;
  return querySpatialBlk(layerIdx, box, vTmp);
}

bool CirDB::querySpatialRoutedWire(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, Vector_t<UInt_t>& vNetIndices) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialPins.size());
  _vSpatialRoutedWires[layerIdx].query(bl, tr, vNetIndices);
  return !vNetIndices.empty();
}

bool CirDB::querySpatialRoutedWire(const UInt_t layerIdx, const Box<Int_t>& box, Vector_t<UInt_t>& vNetIndices) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialPins.size());
  _vSpatialRoutedWires[layerIdx].query(box, vNetIndices);
  return !vNetIndices.empty();
}

bool CirDB::existSpatialRoutedWire(const UInt_t layerIdx, const Point<Int_t>& bl,const Point<Int_t>& tr) {
  Vector_t<UInt_t> vTmp;
  return querySpatialRoutedWire(layerIdx, bl, tr, vTmp);
}

bool CirDB::existSpatialRoutedWire(const UInt_t layerIdx, const Box<Int_t>& box) {
  Vector_t<UInt_t> vTmp;
  return querySpatialRoutedWire(layerIdx, box, vTmp);
}

bool CirDB::existSpatialRoutedWireNet(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, const UInt_t netIdx) {
  Vector_t<UInt_t> vNetIndices;
  querySpatialRoutedWire(layerIdx, bl, tr, vNetIndices);
  for (const UInt_t idx : vNetIndices) {
    if (idx == netIdx)
      return true;
  }
  return false;
}

bool CirDB::existSpatialRoutedWireNet(const UInt_t layerIdx, const Box<Int_t>& box, const UInt_t netIdx) {
  Vector_t<UInt_t> vNetIndices;
  querySpatialRoutedWire(layerIdx, box, vNetIndices);
  for (const UInt_t idx : vNetIndices) {
    if (idx == netIdx)
      return true;
  }
  return false;
}

void CirDB::markBlks() {
  UInt_t i, j, layerIdx;
  Pin* pPin;
  Box<Int_t>* pBox;
  Cir_ForEachPin((*this), pPin, i) {
    Pin_ForEachLayerIdx((*pPin), layerIdx) {
      Pin_ForEachLayerBox((*pPin), layerIdx, pBox, j) {
        const Box<Int_t>& box = *pBox;
        Vector_t<UInt_t> vBlkIndices;
        querySpatialBlk(layerIdx, box, vBlkIndices);
        for (const UInt_t idx : vBlkIndices) {
          Blk& blk = _vBlks[idx];
          const Box<Int_t>& blkBox = blk.box();
          if (Box<Int_t>::bCover(box, blkBox)) {
            blk.setDummy();
          }
          blk.setPinIdx(pPin->idx());
        }
      }
    }
  }
}



//////////////////////////////////
//  Private Setter              //
//////////////////////////////////
void CirDB::setXL(const Int_t x) {
  _xl = x;
}

void CirDB::setYL(const Int_t y) {
  _yl = y;
}

void CirDB::setXH(const Int_t x) {
  _xh = x;
}

void CirDB::setYH(const Int_t y) {
  _yh = y;
}

void CirDB::addPin(const Pin& p) {
  assert(_vvPinIndices.size() > 0);
  _mStr2PinIdx[p.name()] = _vPins.size();
  UInt_t layerIdx;
  Pin_ForEachLayerIdx(p, layerIdx) {
    _vvPinIndices[layerIdx].emplace_back(_vPins.size());
  }
  _vPins.emplace_back(p);
}

void CirDB::addNet(const Net& n) {
  _mStr2NetIdx[n.name()] = _vNets.size();
  _vNets.emplace_back(n);
}

void CirDB::addBlk(const UInt_t i, const Blk& b) {
  assert(_vvBlkIndices.size() > 0);
  _vvBlkIndices[i].emplace_back(_vBlks.size());
  _vBlks.emplace_back(b);
}

void CirDB::resizeVVPinIndices(const UInt_t i) {
  _vvPinIndices.resize(i);
}

void CirDB::resizeVVBlkIndices(const UInt_t i) {
  _vvBlkIndices.resize(i);
}

UInt_t CirDB::layerIdx2MaskIdx(const UInt_t i) const {
  const Pair_t<LefLayerType, UInt_t>& p = _lef.layerPair(i);
  switch (p.first) {
    case LefLayerType::IMPLANT:     return _tech.str2LayerMaskIdx(_lef.implantLayer(p.second).name());
    case LefLayerType::MASTERSLICE: return _tech.str2LayerMaskIdx(_lef.mastersliceLayer(p.second).name());
    case LefLayerType::CUT:         return _tech.str2LayerMaskIdx(_lef.cutLayer(p.second).name());
    case LefLayerType::ROUTING:     return _tech.str2LayerMaskIdx(_lef.routingLayer(p.second).name());
    case LefLayerType::OVERLAP:     return _tech.str2LayerMaskIdx(_lef.overlapLayer(p.second).name());
    default: assert(false);
  }
  return MAX_UINT;
}

// for debug
void CirDB::printInfo() const {
  FILE* fout = stdout;
  fprintf(fout, "CIRCUIT %s (%d %d %d %d)\n", _name.c_str(), _xl, _yl, _xh, _yh);
  fprintf(fout, "  NUM PINS %lu\n", _vPins.size());
  for (UInt_t i = 0; i < _vPins.size(); ++i) {
    const Pin& pin = _vPins[i];
    fprintf(fout, "    PIN %s\n", pin.name().c_str());
    const auto& vvBoxes = pin.vvBoxes();
    for (UInt_t j = 0; j < vvBoxes.size(); ++j) {
      for (UInt_t k = 0; k < vvBoxes[j].size(); ++k) {
        const auto& box = pin.box(j, k);
        // a bug from placement
        assert(box.xl() % 10 == 0);
        assert(box.yl() % 10 == 0);
        assert(box.xh() % 10 == 0);
        assert(box.yh() % 10 == 0);
        fprintf(fout, "      %d (%d %d %d %d)\n", j,
                                                  box.xl(),
                                                  box.yl(),
                                                  box.xh(),
                                                  box.yh());
      }
    }
  }
  fprintf(fout, "\n  NUM NETS %lu\n", _vNets.size());
  for (UInt_t i = 0; i < _vNets.size(); ++i) {
    const Net& net = _vNets[i];
    fprintf(fout, "    NET %s", net.name().c_str());
    if (net.bSelfSym())
      fprintf(fout, " SELFSYM");
    if (net.hasSymNet())
      fprintf(fout, " SYMNET %s", _vNets[net.symNetIdx()].name().c_str());
    fprintf(fout, "\n");
    for (UInt_t j = 0; j < net.numPins(); ++j) {
      fprintf(fout, "      PIN %u\n", net.pinIdx(j));
    }
  }
  fprintf(fout, "\n  TSMC TECHLAYER %lu\n", _tech.mStr2LayerMaskIdx().size());
  for (const auto& obj : _tech.mStr2LayerMaskIdx()) {
    fprintf(fout, "    LAYER %s %d\n", obj.first.c_str(), obj.second);
  }
  fprintf(fout, "\n  BLOCKS\n");
  for (const auto& blk : _vBlks) {
    // a bug from placement
    assert(blk.xl() % 10 == 0);
    assert(blk.yl() % 10 == 0);
    assert(blk.xh() % 10 == 0);
    assert(blk.yh() % 10 == 0);
    fprintf(fout, "    BLOCK %u (%d %d %d %d)\n", blk.layerIdx(),
                                                  blk.xl(),
                                                  blk.yl(),
                                                  blk.xh(),
                                                  blk.yh());
  }
}

void CirDB::visualize() const {
  GdsWriter gw("init.gds");
  gw.initWriter();
  gw.createLib("TOP", 2000, 1e-6/2000);
  gw.writeCellBgn("INTERCONNECTION");

  UInt_t i, j, k, pinIdx, layerIdx;
  const Net* cpNet;
  const Blk* cpBlk;
  const Box<Int_t>* cpBox;
  
  Cir_ForEachNet((*this), cpNet, i) {
    Net_ForEachPinIdx((*cpNet), pinIdx, j) {
      const Pin& pin = this->pin(pinIdx);
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, k) {
          gw.writeRectangle(*cpBox, layerIdx + 100 * (i + 1), 0);
        }
      }
    }
  }
  
  Cir_ForEachBlk((*this), cpBlk, i) {
    gw.writeRectangle(cpBlk->box(), cpBlk->layerIdx() + 10000, 0);
  }

  gw.writeCellEnd();
  gw.endLib();
}

PROJECT_NAMESPACE_END
