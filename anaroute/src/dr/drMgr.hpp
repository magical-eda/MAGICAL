/**
 * @file   drMgr.hpp
 * @brief  Detailed Routing - Manager
 * @author Hao Chen
 * @date   09/30/2019
 *
 **/

#ifndef _DR_MGR_HPP_
#define _DR_MGR_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"
#include "src/drc/drcMgr.hpp"

PROJECT_NAMESPACE_START

class DrMgr {
 public:
  DrMgr(CirDB& c, DrcMgr& d)
    : _cir(c), _drcMgr(d) {}
  ~DrMgr() {}

  void solve();
  void runGridlessRoute();

 private:
  CirDB&  _cir;
  DrcMgr& _drcMgr;
  /////////////////////////////////////////
  //    Private structs                  //
  /////////////////////////////////////////
  
  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
};


PROJECT_NAMESPACE_END

#endif /// _DR_MGR_HPP_
