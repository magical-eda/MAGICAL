/**
 * @file   taMgr.hpp
 * @brief  Track Assignment - Manager
 * @author Hao Chen
 * @date   10/08/2019
 *
 **/

#ifndef _TA_MGR_HPP_
#define _TA_MGR_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class TaMgr {
 public:
  TaMgr(CirDB& c)
    : _cir(c) {}
  ~TaMgr() {}

  void solve();

 private:
  CirDB& _cir;
};

PROJECT_NAMESPACE_END

#endif /// _TA_MGR_HPP_
