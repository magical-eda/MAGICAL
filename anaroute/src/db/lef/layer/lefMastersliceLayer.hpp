/**
 * @file   lefMastersliceLayer.hpp
 * @brief  Technology configuration - Lef Masterslice layer
 * @author Hao Chen
 * @date   09/12/2019
 *
 **/

#ifndef _DB_LEF_LAYER_MASTERSLICE_HPP_
#define _DB_LEF_LAYER_MASTERSLICE_HPP_

#include "lefLayerBase.hpp"

PROJECT_NAMESPACE_START

class LefMastersliceLayer {
  friend class LefReader;
 
 public:
  LefMastersliceLayer()
    : _name("") {}
  ~LefMastersliceLayer() {}
  
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

#endif /// _DB_LEF_LAYER_MASTERSLICE_HPP_
