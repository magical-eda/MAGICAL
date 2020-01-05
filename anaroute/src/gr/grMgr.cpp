/**
 * @file   grMgr.cpp
 * @brief  Global Routing - Manager
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/


#include "grMgr.hpp"
#include "grGridRoute.hpp"

PROJECT_NAMESPACE_START

void GrMgr::solve() {
  runGridRoute();
  extendResults();
}

void GrMgr::runGridRoute() {
  GrGridRoute kernel(*this, _cir);
  kernel.solve();
}

void GrMgr::extendResults() {
  UInt_t i;
  Net* pNet;
  Cir_ForEachNet(_cir, pNet, i) {
    Vector_t<Pair_t<Box<Int_t>, Int_t>> vAddedGuides;
    Vector_t<Pair_t<Box<Int_t>, Int_t>>& vGuides = pNet->vGuides();
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vGuides) {
      const Box<Int_t>& box = pair.first;
      const Int_t layerIdx = pair.second;
      assert(_cir.lef().bRoutingLayer(layerIdx));
      if (layerIdx > (Int_t)_cir.lef().routingLayerIdx2LayerIdx(0)) {
        vAddedGuides.emplace_back(box, layerIdx - 2);
      }
      if (layerIdx < (Int_t)_cir.lef().routingLayerIdx2LayerIdx(_cir.lef().numRoutingLayers() - 1)) {
        vAddedGuides.emplace_back(box, layerIdx + 2);
      }
    }
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vAddedGuides) {
      vGuides.emplace_back(pair);
    }
  }
}

PROJECT_NAMESPACE_END
