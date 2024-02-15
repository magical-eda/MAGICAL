/**
 * @file   parPower.hpp
 * @brief  Parser - Power net reader
 * @author Hao Chen
 * @date   03/11/2020
 *
 **/

#ifndef _PAR_POWER_HPP_
#define _PAR_POWER_HPP_

#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class PowerNetReader {
 public:
  PowerNetReader(CirDB& c)
    : _cir(c) {}
  ~PowerNetReader() {}

  void parse(const String_t& filename);

 private:
  CirDB& _cir;
};

PROJECT_NAMESPACE_END

#endif /// _PAR_POWER_HPP_
