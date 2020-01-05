/**
 * @file   parSymNet.hpp
 * @brief  Parser - Symmetric net reader
 * @author Hao Chen
 * @date   09/29/2019
 *
 **/

#ifndef _PAR_SYMNET_HPP_
#define _PAR_SYMNET_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class SymNetReader {
 public:
  SymNetReader(CirDB& c)
    : _cir(c) {}
  ~SymNetReader() {}

  void parse(const String_t& filename);

 private:
  CirDB& _cir;
  
  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
};

PROJECT_NAMESPACE_END

#endif /// _PAR_SYMNET_HPP_
