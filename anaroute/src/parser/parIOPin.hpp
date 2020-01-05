/**
 * @file   parIOPin.hpp
 * @brief  Parser - IO Pin file
 * @author Hao Chen
 * @date   11/25/2019
 *
 **/

#ifndef _PAR_IOPIN_HPP_
#define _PAR_IOPIN_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class IOPinReader {
 public:
  IOPinReader(CirDB& c)
    : _cir(c) {}
  ~IOPinReader() {}

  void parse(const String_t& filename);

 private:
  CirDB& _cir;
};

PROJECT_NAMESPACE_END

#endif /// _PAR_IOPIN_HPP_
