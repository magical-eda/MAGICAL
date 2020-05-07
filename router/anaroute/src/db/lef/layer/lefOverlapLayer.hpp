/**
 * @file   lefOverlapLayer.hpp
 * @brief  Technology configuration - Lef Overlap layer
 * @author Hao Chen
 * @date   09/12/2019
 *
 **/

#ifndef _DB_LEF_LAYER_OVERLAP_HPP_
#define _DB_LEF_LAYER_OVERLAP_HPP_

#include "lefLayerBase.hpp"

PROJECT_NAMESPACE_START

class LefOverlapLayer {
  friend class LefReader;
 
 public:
  LefOverlapLayer()
    : _name("") {}
  ~LefOverlapLayer() {}
  
  ////////////////////////////////////////
  //   Getter                           //
  ////////////////////////////////////////
  const String_t&   name()    const { return _name; }

 private:
  String_t      _name;
  
  ////////////////////////////////////////
  //   Setter                           //
  ////////////////////////////////////////
  void setName(const String_t& n) { _name = n; }

};


PROJECT_NAMESPACE_END

#endif /// _DB_LEF_LAYER_OVERLAP_HPP_
