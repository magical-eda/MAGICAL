/**
 * @file   drcMgr.cpp
 * @brief  Design Rule Checking - Manager
 * @author Hao Chen
 * @date   10/23/2019
 *
 **/

#include "drcMgr.hpp"
#include "src/geo/box2polygon.hpp"

using namespace std;

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
  Vector_t<Box<Int_t>> vBoxes;
  _cir.querySpatialRoutedWire(layerIdx, b, vNetIndices, vBoxes);
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
  Vector_t<Box<Int_t>> vBoxes;
  _cir.querySpatialRoutedWire(layerIdx, b, vNetIndices, vBoxes);
  for (const UInt_t idx : vNetIndices) {
    if (idx != netIdx)
      return false;
  }
  return true;
}

// min area
bool DrcMgr::checkWireMinArea(const UInt_t layerIdx, const Vector_t<Box<Int_t>>& vBoxes) const {
  if (vBoxes.empty()) {
    return true;
  }
  Int_t totalArea = vBoxes[0].area();
  Int_t overlapArea = 0;
  for (Int_t i = 0; i < (Int_t)vBoxes.size() - 1; ++i) {
    const auto& b1 = vBoxes[i];
    const auto& b2 = vBoxes[i + 1];
    totalArea += b2.area();
    overlapArea += Box<Int_t>::overlapArea(b1, b2);
  }
  totalArea -= overlapArea;
  assert(_cir.lef().bRoutingLayer(layerIdx));
  const auto& layerPair = _cir.lef().layerPair(layerIdx);
  const auto& layer = _cir.lef().routingLayer(layerPair.second);
  return totalArea >= layer.minArea();
}

// spacing
bool DrcMgr::checkWireRoutingLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b, const Int_t prl) const {
  assert(_cir.lef().bRoutingLayer(layerIdx));
  //const auto& layerPair = _cir.lef().layerPair(layerIdx);
  //const auto& layer = _cir.lef().routingLayer(layerPair.second);
  
  const Int_t wireWidth = std::min(b.width(), b.height());
  // FIXME: power
  Int_t prlSpacing = _cir.lef().prlSpacing(layerIdx, wireWidth, prl);
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
    else {
      // same net
      //UInt_t i;
      //const Box<Int_t>* cpBox;
      //Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
        //const Box<Int_t>& box = *cpBox;
        //if (Box<Int_t>::bConnect(box, checkBox)) {
          //if (!Box<Int_t>::bConnect(box, b))
            //return false;
        //}
      //}
    }
  }
  // check other net's wires
  Vector_t<UInt_t> vNetIndices;
  Vector_t<Box<Int_t>> vBoxes;
  _cir.querySpatialRoutedWire(layerIdx, checkBox, vNetIndices, vBoxes);
  assert(vNetIndices.size() == vBoxes.size());
  for (Int_t i = 0; i < (Int_t)vNetIndices.size(); ++i) {
    const UInt_t idx = vNetIndices[i];
    if (idx != netIdx)
      return false;
    else {
      // same net
      //const auto& box = vBoxes[i];
      //if (!Box<Int_t>::bConnect(box, b))
        //return false;
    }
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
    else {
      const UInt_t blkNetIdx = _cir.pin(blk.pinIdx()).netIdx();
      if (netIdx != blkNetIdx)
        return false;
      //else {
        //if (!Box<Int_t>::bConnect(blk.box(), b))
          //return false;
      //}
    }
  }
  return true;
}

bool DrcMgr::checkWireCutLayerSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {
  assert(_cir.lef().bCutLayer(layerIdx));
  const auto& layerPair = _cir.lef().layerPair(layerIdx);
  const auto& layer = _cir.lef().cutLayer(layerPair.second);
  const Int_t spacing = layer.spacing();
  
  Box<Int_t> checkBox(b);
  checkBox.expand(spacing - 1);
  
  // no pin in cut layers
  // check other net's wire (via)
  Vector_t<UInt_t> vNetIndices;
  Vector_t<Box<Int_t>> vBoxes;
  _cir.querySpatialRoutedWire(layerIdx, checkBox, vNetIndices, vBoxes);
  for (const UInt_t idx : vNetIndices) {
    if (idx != netIdx)
      return false;
  }
  return true;
}

bool DrcMgr::checkWireEolSpacing(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& b) const {

  return true;
}


bool DrcMgr::checkViaSpacing(const UInt_t netIdx, const Int_t x, const Int_t y, const LefVia& via) const {
  // check bot boxes
  for (auto box : via.vBotBoxes()) {
    box.shift(x, y);
    if (!checkWireRoutingLayerSpacing(netIdx, via.botLayerIdx(), box))
      return false;
  }
  // check cut boxes
  for (auto box : via.vCutBoxes()) {
    box.shift(x, y);
    if (!checkWireCutLayerSpacing(netIdx, via.cutLayerIdx(), box))
      return false;
  }
  // check top boxes
  for (auto box : via.vTopBoxes()) {
    box.shift(x, y);
    if (!checkWireRoutingLayerSpacing(netIdx, via.topLayerIdx(), box))
      return false;
  }
  return true;
}

void DrcMgr::addNetShapesBFS(const Int_t netIdx, Vector_t<Vector_t<Box<Int_t>>>& vvBoxes) const {
  UInt_t i;
  const Blk* cpBlk;
  Queue_t<UInt_t> qBlkIndices;
  Cir_ForEachBlk(_cir, cpBlk, i) {
    if (cpBlk->bConnect2Pin()) {
      const Int_t blkNetIdx = _cir.pin(cpBlk->pinIdx()).netIdx();
      if (blkNetIdx == netIdx)
        qBlkIndices.emplace(cpBlk->idx());
    }
  }
  Set_t<UInt_t> exploredSet;
  while (!qBlkIndices.empty()) {
    const Blk& blk = _cir.blk(qBlkIndices.front());
    qBlkIndices.pop();
    exploredSet.emplace(blk.idx());
    vvBoxes[blk.layerIdx()].emplace_back(blk.box());
    Vector_t<UInt_t> vBlkIndices;
    _cir.querySpatialBlk(blk.layerIdx(), blk.box(), vBlkIndices);
    for (const UInt_t idx : vBlkIndices) {
      if (exploredSet.find(idx) == exploredSet.end())
        qBlkIndices.emplace(idx);
    }
  }
}

bool DrcMgr::checkSameNetRoutingLayerSpacing(const UInt_t netIdx) const {
  const Net& net = _cir.net(netIdx);
  Vector_t<Vector_t<Box<Int_t>>> vvBoxes(_cir.lef().numLayers());
  Vector_t<Vector_t<Polygon<Int_t>>> vvPolygons(_cir.lef().numLayers());
  
  // construct net polygon
  UInt_t i, j;
  //UInt_t pinIdx, layerIdx;
  //const Box<Int_t>* cpBox;
  const Pair_t<Box<Int_t>, Int_t>* cpWire;
  //const Blk* cpBlk;
  //Cir_ForEachBlk(_cir, cpBlk, i) {
    //if (cpBlk->bConnect2Pin()) {
      //const UInt_t blkNetIdx = _cir.pin(cpBlk->pinIdx()).netIdx();
      //if (blkNetIdx == netIdx)
        //vvBoxes[cpBlk->layerIdx()].emplace_back(cpBlk->box());
    //}
  //}
  //Net_ForEachPinIdx(net, pinIdx, i) {
    //const auto& pin = _cir.pin(pinIdx);
    //Pin_ForEachLayerIdx(pin, layerIdx) {
      //Pin_ForEachLayerBox(pin, layerIdx, cpBox, j) {
        //vvBoxes[layerIdx].emplace_back(*cpBox);
      //}
    //}
  //}
  addNetShapesBFS(netIdx, vvBoxes);
  Net_ForEachRoutedWire(net, cpWire, i) {
    const auto& box = cpWire->first;
    const Int_t layerIdx = cpWire->second;
    vvBoxes[layerIdx].emplace_back(box);
  }
  for (i = 0; i < vvBoxes.size(); ++i) {
    const auto& vBoxes = vvBoxes.at(i);
    if (vBoxes.empty())
      continue;
    if (_cir.lef().bCutLayer(i))
      continue;
    assert(_cir.lef().bRoutingLayer(i));
    geo::box2Polygon<Int_t>(vBoxes, vvPolygons[i]);
    const auto& vPolygons = vvPolygons.at(i);
    
    const Int_t spacing = _cir.lef().prlSpacing(i, net.minWidth());
    
    SpatialMap<Int_t, Segment<Int_t>> spatialSegs;
    for (const auto& polygon : vPolygons) {
      const auto& ring = polygon.outer();
      for (j = 0; j < ring.size(); ++j) {
        const auto& pt0 = ring[j];
        const auto& pt1 = j + 1 == ring.size() ? ring[0] : ring[j + 1];
        Box<Int_t> box(std::min(pt0.x(), pt1.x()),
                       std::min(pt0.y(), pt1.y()),
                       std::max(pt0.x(), pt1.x()),
                       std::max(pt0.y(), pt1.y()));
        Segment<Int_t> seg(pt0, pt1);
        spatialSegs.insert(box, seg);
      }
    }
    for (const auto& polygon : vPolygons) {
      const auto& ring = polygon.outer();
      for (j = 0; j < ring.size(); ++j) {
        const auto& pt0 = ring[j];
        const auto& pt1 = j + 1 == ring.size() ? ring[0] : ring[j + 1];
        Box<Int_t> box(std::min(pt0.x(), pt1.x()),
                       std::min(pt0.y(), pt1.y()),
                       std::max(pt0.x(), pt1.x()),
                       std::max(pt0.y(), pt1.y()));
        Segment<Int_t> seg(pt0, pt1);
        // FIXME: PRL and power
        //cerr << "!!!!!!!!!!!!!!!!!!! " << net.name() << " " << net.minWidth() << " " << spacing << endl;
        Box<Int_t> checkBox(box);
        if (seg.bHorizontal()) {
          assert(seg.xl() != seg.xh() and seg.yl() == seg.yh());
          checkBox.shrinkX(1);
          checkBox.expandY(spacing - 1);
          Vector_t<Segment<Int_t>> vSegs;
          spatialSegs.query(checkBox, vSegs);
          for (const auto& qs : vSegs) {
            if (qs.bVertical())
              continue;
            if (!Segment<Int_t>::bConnect(qs, seg)
                and !bCanPatch(i, qs, seg)) {
              Segment<Int_t> centerConnectLine(qs.center(), seg.center());
              const auto& checkPt = centerConnectLine.center();
                //cerr << qs << seg << endl;
                //for (const auto& polygon : vPolygons) {
                  //const auto& ring = polygon.outer();
                  //for (j = 0; j < ring.size(); ++j) {
                    //const auto& pt0 = ring[j];
                    //const auto& pt1 = j + 1 == ring.size() ? ring[0] : ring[j + 1];
                    //cerr << pt0 << pt1 << endl;
                  //}
                  //cerr << endl;
                //}
                //exit(0);
              if (!boost::polygon::contains(polygon, checkPt))
                return false;
            }
          }
        }
        else {
          assert(seg.bVertical());
          assert(seg.xl() == seg.xh() and seg.yl() != seg.yh());
          checkBox.shrinkY(1);
          checkBox.expandX(spacing - 1);
          Vector_t<Segment<Int_t>> vSegs;
          spatialSegs.query(checkBox, vSegs);
          for (const auto& qs : vSegs) {
            if (qs.bHorizontal())
              continue;
            if (!Segment<Int_t>::bConnect(qs, seg)
                and !bCanPatch(i, qs, seg)) {
              Segment<Int_t> centerConnectLine(qs.center(), seg.center());
              const auto& checkPt = centerConnectLine.center();
                //cerr << qs << seg << endl;
                //for (const auto& polygon : vPolygons) {
                  //const auto& ring = polygon.outer();
                  //for (j = 0; j < ring.size(); ++j) {
                    //const auto& pt0 = ring[j];
                    //const auto& pt1 = j + 1 == ring.size() ? ring[0] : ring[j + 1];
                    //cerr << pt0 << pt1 << endl;
                  //}
                  //cerr << endl;
                //}
                //exit(0);
              if (!boost::polygon::contains(polygon, checkPt))
                return false;
            }
          }
        }
      }
    }
  }
  
  return true; 
}

bool DrcMgr::bCanPatch(const Int_t layerIdx, const Segment<Int_t>& s1, const Segment<Int_t>& s2) const {
  assert(s1.bHorizontal() == s2.bHorizontal());
  assert(_cir.lef().bRoutingLayer(layerIdx));
  const auto& layerPair = _cir.lef().layerPair(layerIdx);
  const auto& layer = _cir.lef().routingLayer(layerPair.second);
  const Int_t minStep = layer.minStep(0);
  Int_t dist = 0;
  if (s1.bHorizontal()) {
    dist = std::abs(s1.yl() - s2.yl());
  }
  else {
    dist = std::abs(s1.xl() - s2.xl());
  }
  if (dist <= minStep) {
    if (s1.length() <= minStep or s2.length() <= minStep) {
      if (s1.bHorizontal()) {
        if (s1.xl() == s2.xl() or s1.xh() == s2.xh())
          return true;
      }
      else {
        if (s1.yl() == s2.yl() or s1.yh() == s2.yh())
          return true;
      }
    }
  }
  return false;
}

PROJECT_NAMESPACE_END
