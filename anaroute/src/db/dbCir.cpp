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

void CirDB::addSpatialOD(const Box<Int_t> &box)
{
    _spatialOD.insert(box);
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

void CirDB::addSpatialRoutedVia(const Int_t netIdx, const Int_t x, const Int_t y, const LefVia& via) {
  for (auto box : via.vBotBoxes()) {
    box.shift(x, y);
    _vSpatialRoutedWires[via.botLayerIdx()].insert(box, netIdx);
  }
  for (auto box : via.vCutBoxes()) {
    box.shift(x, y);
    _vSpatialRoutedWires[via.cutLayerIdx()].insert(box, netIdx);
  }
  for (auto box : via.vTopBoxes()) {
    box.shift(x, y);
    _vSpatialRoutedWires[via.topLayerIdx()].insert(box, netIdx);
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

bool CirDB::removeSpatialRoutedVia(const Int_t netIdx, const Int_t x, const Int_t y, const LefVia& via) {
  bool ret = true;
  for (auto box : via.vBotBoxes()) {
    box.shift(x, y);
    ret &= _vSpatialRoutedWires[via.botLayerIdx()].erase(box, netIdx);
  }
  for (auto box : via.vCutBoxes()) {
    box.shift(x, y);
    ret &= _vSpatialRoutedWires[via.cutLayerIdx()].erase(box, netIdx);
  }
  for (auto box : via.vTopBoxes()) {
    box.shift(x, y);
    ret &= _vSpatialRoutedWires[via.topLayerIdx()].erase(box, netIdx);
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

bool CirDB::querySpatialRoutedWire(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, Vector_t<UInt_t>& vNetIndices, Vector_t<Box<Int_t>>& vWires) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialPins.size());
  Vector_t<Pair_t<Box<Int_t>, UInt_t>> vRet;
  _vSpatialRoutedWires[layerIdx].queryBoth(bl, tr, vRet);
  vNetIndices.reserve(vRet.size());
  vWires.reserve(vRet.size());
  for (const auto& pair : vRet) {
    vNetIndices.emplace_back(pair.second);
    vWires.emplace_back(pair.first);
  }
  return !vRet.empty();
}

bool CirDB::querySpatialRoutedWire(const UInt_t layerIdx, const Box<Int_t>& box, Vector_t<UInt_t>& vNetIndices, Vector_t<Box<Int_t>>& vWires) {
  assert(layerIdx >= 0 and layerIdx < _vSpatialPins.size());
  Vector_t<Pair_t<Box<Int_t>, UInt_t>> vRet;
  _vSpatialRoutedWires[layerIdx].queryBoth(box, vRet);
  vNetIndices.reserve(vRet.size());
  vWires.reserve(vRet.size());
  for (const auto& pair : vRet) {
    vNetIndices.emplace_back(pair.second);
    vWires.emplace_back(pair.first);
  }
  return !vRet.empty();
}

bool CirDB::existSpatialRoutedWire(const UInt_t layerIdx, const Point<Int_t>& bl,const Point<Int_t>& tr) {
  return _vSpatialRoutedWires[layerIdx].exist(bl, tr);
}

bool CirDB::existSpatialRoutedWire(const UInt_t layerIdx, const Box<Int_t>& box) {
  return _vSpatialRoutedWires[layerIdx].exist(box);
}

bool CirDB::existSpatialRoutedWireNet(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, const UInt_t netIdx) {
  Vector_t<UInt_t> vNetIndices;
  Vector_t<Box<Int_t>> vWires;
  querySpatialRoutedWire(layerIdx, bl, tr, vNetIndices, vWires);
  for (const UInt_t idx : vNetIndices) {
    if (idx == netIdx)
      return true;
  }
  return false;
}

bool CirDB::existSpatialRoutedWireNet(const UInt_t layerIdx, const Box<Int_t>& box, const UInt_t netIdx) {
  Vector_t<UInt_t> vNetIndices;
  Vector_t<Box<Int_t>> vWires;
  querySpatialRoutedWire(layerIdx, box, vNetIndices, vWires);
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
              //or Box<Int_t>::bCover(blkBox, box)) {
            blk.setDummy();
            //std::cerr << blkBox << std::endl;
          }
          blk.setPinIdx(pPin->idx());
        }
      }
    }
  }
}

Int_t CirDB::overlapAreaWithOD(const Box<Int_t> &box) const
{
    Int_t area = 0;
    Vector_t<Box<Int_t>> rects; 
    _spatialOD.query(box, rects);
    for (const auto &rect : rects)
    {
        area += Box<Int_t>::overlapArea(box, rect);
    }
    return area;
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

void CirDB::setGridStep(const Int_t s) {
  _gridStep = s;
}

void CirDB::setGridOffsetX(const Int_t x) {
  _gridOffsetX = x;
}

void CirDB::setGridOffsetY(const Int_t y) {
  _gridOffsetY = y;
}

void CirDB::setSymAxisX(const Int_t x) {
  _symAxisX = x;
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

void CirDB::resizeVVMaskWires(const Int_t s) {
  _vvMaskWires.resize(s);
}

void CirDB::resizeVVPatchWires(const Int_t s) {
  _vvPatchWires.resize(s);
}

void CirDB::addMaskWire(const Box<Int_t>& box, const Int_t layerIdx) {
  _vvMaskWires[layerIdx].emplace_back(box);
}

void CirDB::addPatchWire(const Box<Int_t>& box, const Int_t layerIdx) {
  _vvPatchWires[layerIdx].emplace_back(box);
}

bool CirDB::bSatisfySymCondition(const Net& net, const Int_t symAxisX) const {
  if (!net.hasSymNet())
    return false;
  const Net& symNet = this->net(net.symNetIdx());
  Vector_t<Vector_t<Box<Int_t>>> vvBoxes1(this->lef().numLayers());
  Vector_t<Vector_t<Box<Int_t>>> vvBoxes2(this->lef().numLayers());
  
  // init net1 net2 pin shapes
  auto addPinShapes = [&] (const Net& net, Vector_t<Vector_t<Box<Int_t>>>& vvBoxes) {
    for (const auto idx : net.vPinIndices()) {
      const Pin& pin = this->pin(idx);
      UInt_t i, layerIdx;
      const Box<Int_t>* cpBox;
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
          vvBoxes[layerIdx].emplace_back(*cpBox);
        }
      }
    }
  };
  addPinShapes(net, vvBoxes1);
  addPinShapes(symNet, vvBoxes2);
  // search net sym shapes in symNet
  for (UInt_t i = 0; i < vvBoxes2.size(); ++i) {
    if (vvBoxes1[i].size() != vvBoxes2[i].size())
      return false;
    const auto& vBoxes1 = vvBoxes1[i];
    auto& vBoxes2 = vvBoxes2[i];
    std::sort(vBoxes2.begin(), vBoxes2.end());
    for (const auto& box : vBoxes1) {
      Box<Int_t> symBox;
      symBox.setXL(2 * symAxisX - box.xh());
      symBox.setYL(box.yl());
      symBox.setXH(2 * symAxisX - box.xl());
      symBox.setYH(box.yh());
      if (!std::binary_search(vBoxes2.begin(), vBoxes2.end(), symBox)) {
        return false;
      }
    }
  }
  return true;
}

bool CirDB::bSatisfySelfSymCondition(const Net& net, const Int_t symAxisX) const {
  if (!net.bSelfSym())
    return false;
  Vector_t<Vector_t<Box<Int_t>>> vvBoxes(this->lef().numLayers());
  // init pin shapes
  for (const auto idx : net.vPinIndices()) {
    const Pin& pin = this->pin(idx);
    UInt_t i, layerIdx;
    const Box<Int_t>* cpBox;
    Pin_ForEachLayerIdx(pin, layerIdx) {
      Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
        vvBoxes[layerIdx].emplace_back(*cpBox);
      }
    }
  }
  // make sure there exist a sym box of each box
  for (auto& vBoxes : vvBoxes) {
    std::sort(vBoxes.begin(), vBoxes.end());
    for (const auto& box : vBoxes) {
      Box<Int_t> symBox;
      symBox.setXL(2 * symAxisX - box.xh());
      symBox.setYL(box.yl());
      symBox.setXH(2 * symAxisX - box.xl());
      symBox.setYH(box.yh());
      if (!std::binary_search(vBoxes.begin(), vBoxes.end(), symBox)) {
        return false;
      }
    }
  }
  return true;
}

void CirDB::checkNetSymSelfSym() {
  fprintf(stderr, "\n");
  for (UInt_t i = 0; i < this->numNets(); ++i) {
    const auto& net = this->net(i);
    if (net.hasSymNet()) {
      if (!bSatisfySymCondition(net, _symAxisX)) {
        fprintf(stderr, "WARNING: Net %s %s not totally symmetric\n",
                net.name().c_str(),
                this->net(net.symNetIdx()).name().c_str());
      }
    }
    else if (net.bSelfSym()) {
      if (!bSatisfySelfSymCondition(net, _symAxisX)) {
        fprintf(stderr, "WARNING: Net %s not totally self-symmetric\n",
                net.name().c_str());
      }
    }
  }
  fprintf(stderr, "\n");
}

// evaluation
void CirDB::computeNSetAllNetStatistics() {
  for (Net& net : _vNets) {
    if (net.numPins() > 1) {
      if (net.bRouted()) {
        computeNSetNetStatistics(net);
        fprintf(stderr, "Net %s, wl: %d, #via: %d, area: %.2f, degSym: %.2f\n",
                net.name().c_str(),
                net.wireLength(),
                net.viaCnt(),
                net.wireArea(),
                net.degSym());     
      }
      else {
        fprintf(stderr, "Net %s route failed\n", net.name().c_str());
      }
    }
    else {
      fprintf(stderr, "Net %s 1-pin ignored\n", net.name().c_str());
    }
  }
}

void CirDB::computeNSetNetStatistics(Net& net) {
  net.computeNSetWireLength();
  net.computeNSetWireArea();
  computeNSetNetDegSym(net);
} 

void CirDB::computeNSetNetDegSym(Net& net) {
  Vector_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>> vPaths;
  if (net.hasSymNet()) {
    const Net& symNet = this->net(net.symNetIdx());
    vPaths = symNet.vRoutePaths();
  }
  else {
    vPaths = net.vRoutePaths();
  }
  std::sort(vPaths.begin(), vPaths.end());
  Float_t totalLength = 0;
  Float_t symLength = 0;
  for (const auto& pair : net.vRoutePaths()) {
    const auto& u = pair.first;
    const auto& v = pair.second;
    Point3d<Int_t> symU(u); symU.flipX(net.symAxisX());
    Point3d<Int_t> symV(v); symV.flipX(net.symAxisX());
    Pair_t<Point3d<Int_t>, Point3d<Int_t>> symPath(symU, symV);
    if (std::binary_search(vPaths.begin(), vPaths.end(), symPath)) {
      symLength += Point3d<Int_t>::Mdistance(u, v);
    }
    totalLength += Point3d<Int_t>::Mdistance(u, v);
  }
  Float_t degSym = symLength / totalLength;
  net.setDegSym(degSym);
}

void CirDB::computeTotalStatistics() {
  fprintf(stderr, "Total wl: %d, Total #via: %d, "
          "Total Sym wl: %d, Total degSym: %.4f, Avg degSym: %.4f\n\n",
          computeTotalWireLength(),
          computeTotalViaCnt(),
          computeTotalSymWireLength(),
          computeTotalDegSym(),
          computeAvgDegSym());
}

Int_t CirDB::computeTotalWireLength() {
  Int_t wl = 0;
  for (const auto& net : _vNets) {
    wl += net.wireLength();
  }
  return wl;
}

Int_t CirDB::computeTotalSymWireLength() {
  Int_t symLength = 0;
  for (const Net& net : _vNets) {
    Vector_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>> vPaths;
    if (net.hasSymNet()) {
      const Net& symNet = this->net(net.symNetIdx());
      vPaths = symNet.vRoutePaths();
    }
    else {
      vPaths = net.vRoutePaths();
    }
    std::sort(vPaths.begin(), vPaths.end());
    for (const auto& pair : net.vRoutePaths()) {
      const auto& u = pair.first;
      const auto& v = pair.second;
      if (u.z() != v.z())
        continue;
      Point3d<Int_t> symU(u); symU.flipX(net.symAxisX());
      Point3d<Int_t> symV(v); symV.flipX(net.symAxisX());
      Pair_t<Point3d<Int_t>, Point3d<Int_t>> symPath(symU, symV);
      if (std::binary_search(vPaths.begin(), vPaths.end(), symPath)) {
        symLength += Point3d<Int_t>::Mdistance(u, v);
      }
    }
  }
  return symLength;
}

Int_t CirDB::computeTotalViaCnt() {
  Int_t viaCnt = 0;
  for (const auto& net : _vNets) {
    viaCnt += net.viaCnt();
  }
  return viaCnt;
}

Float_t CirDB::computeTotalWireArea() {
  Float_t area = 0;
  for (const auto& net : _vNets) {
    area += net.wireArea();
  }
  return area;
}

Float_t CirDB::computeTotalDegSym() {
  Float_t totalLength = 0;
  Float_t symLength = 0;
  for (const Net& net : _vNets) {
    Vector_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>> vPaths;
    if (net.hasSymNet()) {
      const Net& symNet = this->net(net.symNetIdx());
      vPaths = symNet.vRoutePaths();
    }
    else {
      vPaths = net.vRoutePaths();
    }
    std::sort(vPaths.begin(), vPaths.end());
    for (const auto& pair : net.vRoutePaths()) {
      const auto& u = pair.first;
      const auto& v = pair.second;
      Point3d<Int_t> symU(u); symU.flipX(net.symAxisX());
      Point3d<Int_t> symV(v); symV.flipX(net.symAxisX());
      Pair_t<Point3d<Int_t>, Point3d<Int_t>> symPath(symU, symV);
      if (std::binary_search(vPaths.begin(), vPaths.end(), symPath)) {
        symLength += Point3d<Int_t>::Mdistance(u, v);
      }
      totalLength += Point3d<Int_t>::Mdistance(u, v);
    }
  }
  return symLength / totalLength;
}

Float_t CirDB::computeAvgDegSym() {
  Float_t degSym = 0;
  Int_t cnt = 0;
  for (const auto& net : _vNets) {
    if (net.numPins() > 1 and net.bRouted()) {
      ++cnt;
      degSym += net.degSym();
    }
  }
  return degSym / cnt;
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
  std::string gdsName = this->_name  + "_init.gds";
  GdsWriter gw(gdsName);
  gw.initWriter();
  gw.createLib("TOP", 2000, 1e-6/2000);
  gw.writeCellBgn("INTERCONNECTION");

  UInt_t i, j, k, pinIdx, layerIdx;
  const Net* cpNet;
  const Blk* cpBlk;
  const Box<Int_t>* cpBox;
  const AcsPt* cpPt;

  Cir_ForEachNet((*this), cpNet, i) {
    Net_ForEachPinIdx((*cpNet), pinIdx, j) {
      const Pin& pin = this->pin(pinIdx);
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, k) {
          gw.writeRectangle(*cpBox, layerIdx + 100 * (i + 1), 0);
        }
      }

      Pin_ForEachAcsPt(pin, cpPt, k) {
        const auto& acsPt = cpPt->gridPt();
        Box<Int_t> box(acsPt.x() - 5, acsPt.y() - 5,
                       acsPt.x() + 5, acsPt.y() + 5);
        Point3d<Int_t> pt(acsPt);
        switch (cpPt->dir()) {
          case AcsPt::DirType::NORTH: pt.setY(pt.y() + _gridStep); break;
          case AcsPt::DirType::SOUTH: pt.setY(pt.y() - _gridStep); break;
          case AcsPt::DirType::EAST: pt.setX(pt.x() + _gridStep); break;
          case AcsPt::DirType::WEST: pt.setX(pt.x() - _gridStep); break;
          case AcsPt::DirType::UP: assert(false);
          case AcsPt::DirType::DOWN: assert(false);
          default: assert(false);
        }
        Box<Int_t> box2(pt.x() - 5, pt.y() - 5,
                        pt.x() + 5, pt.y() + 5);
        Box<Int_t> box3(std::min(acsPt.x(), pt.x()) - 2, std::min(acsPt.y(), pt.y()) - 2,
                        std::max(acsPt.x(), pt.x()) + 2, std::max(acsPt.y(), pt.y()) + 2);
        gw.writeRectangle(box, 20000, 0);
        gw.writeRectangle(box2, 30000, 0);
        gw.writeRectangle(box3, 30000, 0);
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
