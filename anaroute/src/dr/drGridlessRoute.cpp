/**
 * @file   drGridlessRoute.cpp
 * @brief  Detailed Routing - Gridless Routing
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/

#include "drGridlessRoute.hpp"
#include "drAstar.hpp"
#include "src/ds/pqueue.hpp"
#include "src/writer/wrGds.hpp"

PROJECT_NAMESPACE_START

void DrGridlessRoute::solve() {
  
  //_vAllAstarNodesMap.resize(_cir.lef().numLayers());
  //for (auto& m : _vAllAstarNodesMap)
    //m.set_empty_key(Point<Int_t>(-1, -1));

  PairingHeap<Net*, Net_Cmp> pq;
  UInt_t i;
  Net* pNet;
  Cir_ForEachNet(_cir, pNet, i) {
    // ignore dangling nets
    if (pNet->numPins() > 1) {
      //if (pNet->hasSymNet() and pNet->idx() > pNet->symNetIdx()) {
        //continue;
      //}
      //if (pNet->name() != "outp" and pNet->name() != "outm")
      pq.push(pNet);
    }
  }
  // rip up and reroute
  Queue_t<Net*> qFrozenNet;
  while (!pq.empty() or !qFrozenNet.empty()) {
    while (qFrozenNet.size() > _rrParams.numFrozen) {
      pq.push(qFrozenNet.front());
      qFrozenNet.pop();
    }
    if (pq.empty()) {
      pq.push(qFrozenNet.front());
      qFrozenNet.pop();
    }
    pNet = pq.top();
    pq.pop();
    if (!routeSingleNet(*pNet)) {
      pNet->addDrFail();
      //qFrozenNet.push(pNet);
      //ripupSingleNet(*pNet);
      ripupAllNets();
      pq.clear();
      Cir_ForEachNet(_cir, pNet, i) {
        if (pNet->numPins() > 1) {
          pq.push(pNet);
        }
      }
    }
  }

  // debug
  //visualize();
}

bool DrGridlessRoute::routeSingleNet(Net& n) {
  DrAstar astarKernel(_cir, n, *this);
  return astarKernel.runKernel();
}

void DrGridlessRoute::ripupSingleNet(Net& n) {
  auto& vWires = n.vWires();
  //assert(vWires.size() > 0);
  for (const auto& pair : vWires) {
    const Box<Int_t>& wire = pair.first;
    const Int_t layerIdx = pair.second;
    _cir.removeSpatialRoutedWire(n.idx(), layerIdx, wire);
  }
  vWires.clear();
  n.setRouted(false);
}

void DrGridlessRoute::ripupAllNets() {
  UInt_t i;
  Net* pNet;
  Cir_ForEachNet(_cir, pNet, i) {
    ripupSingleNet(*pNet);
  }
}

void DrGridlessRoute::visualize() {
  GdsWriter gw("a.gds");
  gw.initWriter();
  gw.createLib("TOP", 2000, 1e-6/2000);
  gw.writeCellBgn("INTERCONNECTION");

  UInt_t i, j, k, pinIdx, layerIdx;
  const Net* cpNet;
  const Blk* cpBlk;
  const Box<Int_t>* cpBox;
  
  Cir_ForEachNet(_cir, cpNet, i) {
    Net_ForEachPinIdx((*cpNet), pinIdx, j) {
      const Pin& pin = _cir.pin(pinIdx);
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, k) {
          gw.writeRectangle(*cpBox, layerIdx + 100 * (i + 1), 0);
        }
      }
      const auto& vWires = cpNet->vWires();
      for (const auto& p : vWires) {
        gw.writeRectangle(p.first, p.second, 0);
      }
    }
  }

  Cir_ForEachBlk(_cir, cpBlk, i) {
    gw.writeRectangle(cpBlk->box(), cpBlk->layerIdx() + 10000, 0);
  }



  gw.writeCellEnd();
  gw.endLib();
}

PROJECT_NAMESPACE_END
