/**
 * @file   drPower.hpp
 * @brief  Power Ground special nets routing
 * @author Hao Chen
 * @date   03/03/2020
 *
 **/

#ifndef _DR_POWER_HPP_
#define _DR_POWER_HPP_

#include "drMgr.hpp"

PROJECT_NAMESPACE_START

class DrPower {
 public:
  DrPower(CirDB& c)
    : _cir(c) {}
  ~DrPower() {}

  void solve();

 private:
  CirDB&  _cir;
};

PROJECT_NAMESPACE_END

#endif /// _DR_POWER_HPP_
