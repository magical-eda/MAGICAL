/**
 * @file   grMgr.hpp
 * @brief  Global Routing - Manager
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/

#ifndef _GR_MGR_HPP_
#define _GR_MGR_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"


PROJECT_NAMESPACE_START

class GrMgr {
 public:
  GrMgr(CirDB& c)
    : _cir(c) {}
  ~GrMgr() {}

  void solve();
  void runGridRoute();
  void extendResults();

 private:
  CirDB& _cir; 
};

PROJECT_NAMESPACE_END

#endif /// _GR_MGR_HPP_

