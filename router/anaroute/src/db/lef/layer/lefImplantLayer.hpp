/**
 * @file   lefImplantLayer.hpp
 * @brief  Technology configuration - Lef Implant layer
 * @author Hao Chen
 * @date   09/15/2019
 *
 **/

#ifndef _DB_LEF_LAYER_IMPLANT_HPP_
#define _DB_LEF_LAYER_IMPLANT_HPP_

#include "lefLayerBase.hpp"

PROJECT_NAMESPACE_START

class LefImplantLayer {
  friend class LefReader;
 public:
  LefImplantLayer()
    : _name("") {}
  ~LefImplantLayer() {}

  ////////////////////////////////////////
  //   Getter                           //
  ////////////////////////////////////////
  const String_t& name() const { return _name; }


 private:
  String_t _name;
  ////////////////////////////////////////
  //   Setter                           //
  ////////////////////////////////////////
  void setName(const String_t& n) { _name = n; }
};

PROJECT_NAMESPACE_END

#endif /// _DB_LEF_LAYER_INPLANT_HPP_

