/**
 * @file   drViaSelector.hpp
 * @brief  Detailed Routing - Via Selector
 * @author Hao Chen
 * @date   10/27/2019
 *
 **/

#ifndef _DR_VIA_SELECTOR_HPP_
#define _DR_VIA_SELECTOR_HPP_

#include "src/global/global.hpp"
#include "src/geo/point3d.hpp"
#include "src/drc/drcMgr.hpp"

PROJECT_NAMESPACE_START

class DrViaSelector {
 public:
  DrViaSelector(CirDB& c, DrcMgr& drc)
    : _cir(c), _drcMgr(drc) {}
  ~DrViaSelector() {}

  UInt_t selectViaIdx(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  UInt_t selectViaIdxSym(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v, const Int_t symAxisX);

 private:
  CirDB&    _cir;
  DrcMgr&   _drcMgr;
};

PROJECT_NAMESPACE_END

#endif /// _DR_VIA_SELECTOR_HPP_
