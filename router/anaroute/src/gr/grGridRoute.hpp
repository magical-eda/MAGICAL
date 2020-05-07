/**
 * @file   grGridRoute.hpp
 * @brief  Global Routing - Grid based Routing
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/

#ifndef _GR_GRID_ROUTE_HPP_
#define _GR_GRID_ROUTE_HPP_

#include "src/geo/point3d.hpp"
#include "grMgr.hpp"
#include "grGridMap3d.hpp"

PROJECT_NAMESPACE_START

class GrGridRoute {
  friend class GrAstar;
 public:
  GrGridRoute(GrMgr& mgr, CirDB& c)
    : _grMgr(mgr), _cir(c) {}
  ~GrGridRoute() {}
 
  void solve();

 private:
  GrMgr&      _grMgr;
  CirDB&      _cir;
  // for explicit grid construction method
  GrGridMap3d _gridMap;
  Vector_t<Vector_t<Point3d<Int_t>>> _vvNetPinLocs; // (x,y,z) of pins of each net in the gridMap
  /////////////////////////////////////////
  //    Private structs                  //
  /////////////////////////////////////////
  // for net comparison in pqueue
  struct Net_Cmp {
    bool operator() (const Net* pn1, const Net* pn2) {
      Int_t c1 = w_pin_cnt  * pn1->numPins() +
                 w_sym      * pn1->hasSymNet() +
                 w_selfSym  * pn1->bSelfSym() +
                 w_fail_cnt * pn1->grFailCnt();
      Int_t c2 = w_pin_cnt  * pn2->numPins() +
                 w_sym      * pn2->hasSymNet() +
                 w_selfSym  * pn2->bSelfSym() +
                 w_fail_cnt * pn2->grFailCnt();
      return c1 < c2;
    }
    Int_t w_pin_cnt = 1;
    Int_t w_sym = 1;
    Int_t w_selfSym = 1;
    Int_t w_fail_cnt = 1;
  };
  struct Grid_Param {
    UInt_t grid_x_scale = 8;
    UInt_t grid_y_scale = 8;
  } _param;
  // for rip up and reroute
  struct RR_Param {
    UInt_t numFrozen        = 1;
    UInt_t numMaxFails      = 3;
    UInt_t normalNetWeight  = 1;
    UInt_t symNetWeight     = 1;
    UInt_t selfSymNetWeight = 1;
  } _rrParams;

  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  // explicit grid method
  void initGrids(const Int_t scaleX, const Int_t scaleY); // dim = (scaleX * stepX), (scaleY * stepY)
  void markBlockedGrids();
  // incremental grid method
  void initSteps();
  // Path search kernel
  bool routeSingleNet(Net& n, const Int_t netWeight);
  bool routeSelfSymNet(Net& n, const Int_t netWeight);
  bool routeSymNet(Net& n, const Int_t netWeight);
};

PROJECT_NAMESPACE_END

#endif /// _GR_GRID_ROUTE_HPP_

