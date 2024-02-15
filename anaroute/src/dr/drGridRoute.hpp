/**
 * @file   drGridRoute.hpp
 * @brief  Detailed Routing - Grid Based Routing
 * @author Hao Chen
 * @date   02/01/2020
 *
 **/

#ifndef _DR_GRID_ROUTE_HPP_
#define _DR_GRID_ROUTE_HPP_

#include "drMgr.hpp"
#include "src/geo/spatial.hpp"
#include "src/ds/pqueue.hpp"

PROJECT_NAMESPACE_START

class DrGridRoute {
  friend class DrGridAstar;
 public:
  DrGridRoute(CirDB& c, DrMgr& dr, DrcMgr& drc)
    : _cir(c), _drMgr(dr), _drc(drc),
      _vSpatialHistoryMaps(c.lef().numLayers()) {}
  ~DrGridRoute() {}

  bool solve();
  bool solveDR(auto& pq, const bool bPower);
  void checkFailed();

 private:
  CirDB&    _cir;
  DrMgr&    _drMgr;
  DrcMgr&   _drc;
  
  Vector_t<SpatialMap<Int_t, Int_t>> _vSpatialHistoryMaps;

  /////////////////////////////////////////
  //    Private structs                  //
  /////////////////////////////////////////
  struct SignalNetCmp {
    bool operator() (const Net* pn1, const Net* pn2) {
      if (pn1->drFailCnt() != pn2->drFailCnt()) {
        return pn1->drFailCnt() < pn2->drFailCnt();
      }
      else if (pn1->bSelfSym() != pn2->bSelfSym()) {
        return pn1->bSelfSym() < pn2->bSelfSym();
      }
      else if (pn1->hasSymNet() != pn2->hasSymNet()) {
        return pn1->hasSymNet() < pn2->hasSymNet();
      }
      else if (pn1->bbox().hpwl() != pn2->bbox().hpwl()) {
        return pn1->bbox().hpwl() > pn2->bbox().hpwl();
      }
      else if (pn1->numPins() != pn2->numPins()) {
        return pn1->numPins() < pn2->numPins();
      }
      return true;
    }
  };

  struct PowerNetCmp {
    bool operator() (const Net* pn1, const Net* pn2) {
      if (pn1->drFailCnt() != pn2->drFailCnt()) {
        return pn1->drFailCnt() < pn2->drFailCnt();
      }
      else if (pn1->bSelfSym() != pn2->bSelfSym()) {
        return pn1->bSelfSym() < pn2->bSelfSym();
      }
      else if (pn1->bbox().hpwl() != pn2->bbox().hpwl()) {
        return pn1->bbox().hpwl() > pn2->bbox().hpwl();
      }
      else if (pn1->numPins() != pn2->numPins()) {
        return pn1->numPins() < pn2->numPins();
      }
      return true;
    }
  };

  struct Param {
    Int_t maxSymTry = 5;
    Int_t maxSelfSymTry = 5;
    Int_t maxIteration = 15;
    Int_t maxIteration2 = 20;
  } _param;
  
  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  void addUnroutedNetsToPQ(auto& pq, const bool bPower);

  void checkSymSelfSym(const Net& net, const Routable& ro, bool& bSym, bool& bSelfSym);

  bool runNRR(auto& pq, const bool bPower, const Int_t maxIteration);

  bool routeSingleNet(Net& net, const bool bStrictDRC);

  bool checkDRC(const bool bPower);
  bool checkSingleNetDRC(const Net& net);
  
  void ripupSingleNet(Net& net);
  
  void addWireHistoryCost(const Int_t cost, const Int_t layerIdx, const Box<Int_t>& wire);
  void addViaHistoryCost(const Int_t cost, const Int_t x, const Int_t y, const LefVia& via);

};

PROJECT_NAMESPACE_END

#endif /// _DR_GRID_ROUTE_HPP_
