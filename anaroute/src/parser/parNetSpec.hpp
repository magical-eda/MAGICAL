/**
 * @file   parNetSpec.hpp
 * @brief  Net Spec parser
 * @author Hao Chen
 * @date   04/06/2020
 *
 **/

#ifndef _PAR_NETSPEC_HPP_
#define _PAR_NETSPEC_HPP_

#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class NetSpecReader {
 public:
  NetSpecReader(CirDB& c)
    : _cir(c) {}
  ~NetSpecReader() {}

  void parse(const String_t& filename);

 private:
  CirDB& _cir;
};

PROJECT_NAMESPACE_END

#endif /// _PAR_NETSPEC_HPP_
