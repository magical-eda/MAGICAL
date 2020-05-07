/**
 * @file   drGridlessRoute.hpp
 * @brief  Detailed Routing - Gridless Routing
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/

#ifndef _DR_GRIDLESS_ROUTE_HPP_
#define _DR_GRIDLESS_ROUTE_HPP_

#include "drMgr.hpp"
#include "src/ds/hash.hpp"
#include "drAstarNode.hpp"

PROJECT_NAMESPACE_START

class DrGridlessRoute {
  friend class DrAstar;
 public:
  DrGridlessRoute(CirDB& c, DrMgr& dr, DrcMgr& drc)
    : _cir(c), _drMgr(dr), _drcMgr(drc) {}
  ~DrGridlessRoute() {
    //for (auto& v : _vAllAstarNodesMap) {
      //for (auto& m : v) {
        //delete m.second;
      //}
    //}
  }

  void solve();

 private:
  CirDB&  _cir;
  DrMgr&  _drMgr;
  DrcMgr& _drcMgr;
  

  // for Astar
  //Vector_t<DenseHashMap<Point<Int_t>, DrAstarNode*, Point<Int_t>::hasher>>  _vAllAstarNodesMap;

  /////////////////////////////////////////
  //    Private structs                  //
  /////////////////////////////////////////
  // for net comparison in pqueue
  struct Net_Cmp {
    bool operator() (const Net* pn1, const Net* pn2) {
      Int_t c1 = w_pin_cnt  * pn1->numPins() +
                 w_sym      * pn1->hasSymNet() +
                 w_selfSym  * pn1->bSelfSym() +
                 w_fail_cnt * pn1->drFailCnt();
      Int_t c2 = w_pin_cnt  * pn2->numPins() +
                 w_sym      * pn2->hasSymNet() +
                 w_selfSym  * pn2->bSelfSym() +
                 w_fail_cnt * pn2->drFailCnt();
      return c1 < c2;
    }
    Int_t w_pin_cnt = 1;
    Int_t w_sym = 100;
    Int_t w_selfSym = 1;
    Int_t w_fail_cnt = 100;
  };
  struct RR_Param {
    UInt_t numFrozen;
  } _rrParams;
  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  bool routeSingleNet(Net& n);
  void ripupSingleNet(Net& n);
  void ripupAllNets();
  void visualize();
  
};

PROJECT_NAMESPACE_END

#endif /// _DR_GRIDLESS_ROUTE_HPP_

