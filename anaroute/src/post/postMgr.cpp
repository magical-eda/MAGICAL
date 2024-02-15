/**
 * @file   postMgr.cpp
 * @brief  Post DR processing - Manager
 * @author Hao Chen
 * @date   02/15/2020
 *
 **/

#include "postMgr.hpp"
#include "src/geo/segment.hpp"

PROJECT_NAMESPACE_START

void PostMgr::solve() {
  patchJogs();
}

void PostMgr::patchJogs() {
  _cir.resizeVVMaskWires(_cir.lef().numLayers());
  _cir.resizeVVPatchWires(_cir.lef().numLayers());
  
  Vector_t<Vector_t<Polygon<Int_t>>> vvPolygons;
  
  //initPolygons(vvPolygons);
  //patchConcaveJogs(vvPolygons);
  //addMetal2Net();

  while (1) {
    initPolygons(vvPolygons);
    
    bool hasJog = false;
    if (patchConcaveJogs(vvPolygons)) {
      addMetal2Net();
      hasJog = true;
      continue;
    }

    if (patchConvexJogs(vvPolygons)) {
      addMetal2Net();
      hasJog = true;
      continue;
    }
    if (!hasJog)
      break;
  }
}

void PostMgr::initPolygons(Vector_t<Vector_t<Polygon<Int_t>>>& vvPolygons) {
  vvPolygons.clear();
  vvPolygons.resize(_cir.lef().numLayers());
  // separate shapes of each layer
  Vector_t<Vector_t<Box<Int_t>>> vvBoxes;
  vvBoxes.resize(_cir.lef().numLayers());
  // add boxes (pin shapes and routed wires)
  UInt_t i, j;
  const Net* cpNet;
  const Blk* cpBlk;
  const Pair_t<Box<Int_t>, Int_t>* cpWire;

  Cir_ForEachBlk(_cir, cpBlk, i) {
    assert(_cir.lef().bRoutingLayer(cpBlk->layerIdx()));
    if (!cpBlk->bLVS())
      vvBoxes[cpBlk->layerIdx()].emplace_back(cpBlk->box());
  }
  Cir_ForEachNet(_cir, cpNet, i) {
    Net_ForEachRoutedWire((*cpNet), cpWire, j) {
      const auto& box = cpWire->first;
      const Int_t layerIdx = cpWire->second;
      // don't need to add cut layer shapes
      if (_cir.lef().bRoutingLayer(layerIdx)) {
        vvBoxes[layerIdx].emplace_back(box);
      }
    }
  }
  for (i = 0; i < vvBoxes.size(); ++i) {
    const auto& vBoxes = vvBoxes.at(i);
    if (vBoxes.empty())
      continue;
    assert(_cir.lef().bRoutingLayer(i));
    const auto& layerPair = _cir.lef().layerPair(i);
    const auto& layer = _cir.lef().routingLayer(layerPair.second);
    // if no constraint
    if (layer.numMinSteps() == 0)
      continue;

    // FIXME: only handle our PDK condition currently
    assert(layer.numMinSteps() == 1);

    geo::box2Polygon<Int_t>(vBoxes, vvPolygons[i]);
  }
}

bool PostMgr::patchConcaveJogs(const Vector_t<Vector_t<Polygon<Int_t>>>& vvPolygons) {
  bool hasJog = false;
  for (UInt_t i = 0; i < vvPolygons.size(); ++i) {
    if (vvPolygons[i].empty())
      continue;
    assert(_cir.lef().bRoutingLayer(i));
    const auto& layerPair = _cir.lef().layerPair(i);
    const auto& layer = _cir.lef().routingLayer(layerPair.second);
    // if no constraint
    if (layer.numMinSteps() == 0)
      continue;

    // FIXME: only handle our PDK condition currently
    assert(layer.numMinSteps() == 1);
    const auto& vPolygons = vvPolygons[i];
    for (const auto& polygon : vPolygons) {
      const auto& ring = polygon.outer();
      for (UInt_t j = 1; j < ring.size(); ++j) {
        const auto& pt0 = ring[j - 1];
        const auto& pt1 = ring[j];
        const auto& pt2 = j + 1 == ring.size() ? ring[0] : ring[j + 1];
        Segment<Int_t> edge1(pt0, pt1);
        Segment<Int_t> edge2(pt1, pt2);
        assert((edge1.bHorizontal() and edge2.bVertical())
               or (edge1.bVertical() and edge2.bHorizontal()));
        if (edge1.length() < layer.minStep(0)
            and edge2.length() < layer.minStep(0)) {
          Box<Int_t> box(std::min({pt0.x(), pt1.x(), pt2.x()}),
                         std::min({pt0.y(), pt1.y(), pt2.y()}),
                         std::max({pt0.x(), pt1.x(), pt2.x()}),
                         std::max({pt0.y(), pt1.y(), pt2.y()}));
          JogOrient_t orient;
          if (counterClockwise(pt0, pt1, pt2, orient)) { // concave
            fprintf(stderr, "PostMgr::%s Jog-%d  %d (%d %d) (%d %d) (%d %d) concave\n",
                    __func__, ++_cnt, i, pt0.x(), pt0.y(), pt1.x(), pt1.y(), pt2.x(), pt2.y());
            _cir.addPatchWire(box, i);
            hasJog = true;
            assert(orient != JogOrient_t::INVALID);
          }
        }
      }

    }
  }
  return hasJog;
}

bool PostMgr::patchConvexJogs(const Vector_t<Vector_t<Polygon<Int_t>>>& vvPolygons) {
  bool hasJog = false;
  for (UInt_t i = 0; i < vvPolygons.size(); ++i) {
    if (vvPolygons[i].empty())
      continue;
    assert(_cir.lef().bRoutingLayer(i));
    const auto& layerPair = _cir.lef().layerPair(i);
    const auto& layer = _cir.lef().routingLayer(layerPair.second);
    // if no constraint
    if (layer.numMinSteps() == 0)
      continue;

    // FIXME: only handle our PDK condition currently
    assert(layer.numMinSteps() == 1);
    const auto& vPolygons = vvPolygons[i];
    for (const auto& polygon : vPolygons) {
      const auto& ring = polygon.outer();
      for (UInt_t j = 1; j < ring.size(); ++j) {
        const auto& pt0 = ring[j - 1];
        const auto& pt1 = ring[j];
        const auto& pt2 = j + 1 == ring.size() ? ring[0] : ring[j + 1];
        Segment<Int_t> edge1(pt0, pt1);
        Segment<Int_t> edge2(pt1, pt2);
        assert((edge1.bHorizontal() and edge2.bVertical())
               or (edge1.bVertical() and edge2.bHorizontal()));
        if (edge1.length() < layer.minStep(0)
            and edge2.length() < layer.minStep(0)) {
          
          Box<Int_t> box(std::min({pt0.x(), pt1.x(), pt2.x()}),
                         std::min({pt0.y(), pt1.y(), pt2.y()}),
                         std::max({pt0.x(), pt1.x(), pt2.x()}),
                         std::max({pt0.y(), pt1.y(), pt2.y()}));
          JogOrient_t orient;
          if (clockwise(pt0, pt1, pt2, orient)) { // convex
            fprintf(stderr, "PostMgr::%s  Jog-%d  %d (%d %d) (%d %d) (%d %d) convex\n",
                    __func__, ++_cnt, i, pt0.x(), pt0.y(), pt1.x(), pt1.y(), pt2.x(), pt2.y());
            hasJog = true;
            //_cir.addMaskWire(box, i);
            switch(orient) {
              case JogOrient_t::NE: box.setYH(box.yh() + layer.minStep(0)); break;
              case JogOrient_t::SE: box.setYL(box.yl() - layer.minStep(0)); break;
              case JogOrient_t::SW: box.setYL(box.yl() - layer.minStep(0)); break;  
              case JogOrient_t::NW: box.setYH(box.yh() + layer.minStep(0)); break;
              default: assert(false);
            }
            _cir.addPatchWire(box, i);
          }
        }
      }

    }
  }
  return hasJog;
}

void PostMgr::addMetal2Net() {
  // update routed wires in nets
  UInt_t i, j;
  Net* pNet;
  const Pair_t<Box<Int_t>, Int_t>* cpWire;
  Cir_ForEachNet(_cir, pNet, i) {
    Vector_t<Vector_t<Box<Int_t>>> vvWires;
    vvWires.resize(_cir.lef().numLayers());
    Net_ForEachRoutedWire((*pNet), cpWire, j) {
      const auto& box = cpWire->first;
      const Int_t layerIdx = cpWire->second;
      vvWires[layerIdx].emplace_back(box);
    } 
    for (Int_t layerIdx = 0; layerIdx < (Int_t)vvWires.size(); ++layerIdx) {
      auto& vWires = vvWires[layerIdx];
      geo::boxesDiffAssign(vWires, _cir.vMaskWires(layerIdx));
      geo::boxesAddAssign(vWires, _cir.vPatchWires(layerIdx));
    }
    pNet->vWires().clear();
    for (Int_t layerIdx = 0; layerIdx < (Int_t)vvWires.size(); ++layerIdx) {
      for (const auto& box : vvWires[layerIdx]) {
        pNet->vWires().emplace_back(box, layerIdx);
      }
    }
  }
  for (i = 0; i < _cir.lef().numLayers(); ++i) {
    _cir.vMaskWires(i).clear();
    _cir.vPatchWires(i).clear();
  }
}


bool PostMgr::clockwise(const Point<Int_t>& p0, const Point<Int_t>& p1, const Point<Int_t>& p2, JogOrient_t& orient) {
  const Int_t path1_deltaX = p1.x() - p0.x();
  const Int_t path1_deltaY = p1.y() - p0.y();
  const Int_t path2_deltaX = p2.x() - p1.x();
  const Int_t path2_deltaY = p2.y() - p1.y();
  if (path1_deltaX > 0 and path1_deltaY == 0 and path2_deltaX == 0 and path2_deltaY < 0) {
    orient = JogOrient_t::NE;
    return true;
  }
  if (path1_deltaX == 0 and path1_deltaY < 0 and path2_deltaX < 0 and path2_deltaY == 0) {
    orient = JogOrient_t::SE;
    return true;
  }
  if (path1_deltaX < 0 and path1_deltaY == 0 and path2_deltaX == 0 and path2_deltaY > 0) {
    orient = JogOrient_t::SW;
    return true;
  }
  if (path1_deltaX == 0 and path1_deltaY > 0 and path2_deltaX > 0 and path2_deltaY == 0) {
    orient = JogOrient_t::NW;
    return true;
  }
  orient = JogOrient_t::INVALID;
  return false;
}

bool PostMgr::counterClockwise(const Point<Int_t>& p0, const Point<Int_t>& p1, const Point<Int_t>& p2, JogOrient_t& orient) {
  const Int_t path1_deltaX = p1.x() - p0.x();
  const Int_t path1_deltaY = p1.y() - p0.y();
  const Int_t path2_deltaX = p2.x() - p1.x();
  const Int_t path2_deltaY = p2.y() - p1.y();
  if (path1_deltaX == 0 and path1_deltaY < 0 and path2_deltaX > 0 and path2_deltaY == 0) {
    orient = JogOrient_t::NE;
    return true;
  }
  if (path1_deltaX > 0 and path1_deltaY == 0 and path2_deltaX == 0 and path2_deltaY > 0) {
    orient = JogOrient_t::NW;
    return true;
  }
  if (path1_deltaX == 0 and path1_deltaY > 0 and path2_deltaX < 0 and path2_deltaY == 0) {
    orient = JogOrient_t::SW;
    return true;
  }
  if (path1_deltaX < 0 and path1_deltaY == 0 and path2_deltaX == 0 and path2_deltaY < 0) {
    orient = JogOrient_t::SE;
    return true;
  }
  orient = JogOrient_t::INVALID;
  return false;
}
PROJECT_NAMESPACE_END
