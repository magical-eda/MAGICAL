/**
 * @file   parTech.hpp
 * @brief  Parser - Tsmc techfile (techlayer only)
 * @author Hao Chen
 * @date   09/23/2019
 *
 **/

#ifndef _PAR_TECH_HPP_
#define _PAR_TECH_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class TechfileReader {
 public:
  TechfileReader(CirDB& c)
    : _cir(c) {}
  ~TechfileReader() {}

  void parse(const String_t& filename);

 private:
  CirDB& _cir;
  
  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  void readTechLayers(FILE* fin, char* buf, const UInt_t bufSize);
};

PROJECT_NAMESPACE_END

#endif /// _PAR_TECH_HPP_
