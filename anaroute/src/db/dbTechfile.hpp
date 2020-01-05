/**
 * @file   dbTechfile.hpp
 * @brief  Database - tech file
 * @author Hao Chen
 * @date   09/23/2019
 *
 **/

#ifndef _DB_TECH_FILE_HPP_
#define _DB_TECH_FILE_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

class TechfileDB {
  friend class TechfileReader;
 public:
  TechfileDB() {}
  ~TechfileDB() {}
  
  //////////////////////////////////
  //  Getter                      //
  //////////////////////////////////
  const UMap_t<String_t, UInt_t>&  mStr2LayerMaskIdx()                 const { return _mStr2LayerMaskIdx; }
  UInt_t                           str2LayerMaskIdx(const String_t& n) const { return _mStr2LayerMaskIdx.at(n); }

 private:
  UMap_t<String_t, UInt_t>  _mStr2LayerMaskIdx; // layer name -> layer idx specified in tsmc techfile
  
  //////////////////////////////////
  //  Private Setter              //
  //////////////////////////////////
  void addStr2LayerMaxIdx(const String_t& n, const UInt_t i);
};

PROJECT_NAMESPACE_END

#endif /// _DB_TECH_FILE_HPP_
