/**
 * @file   lefLayerBase.hpp
 * @brief  Technology configuration - Lef layer basics
 * @author Hao Chen
 * @date   09/12/2019
 *
 **/

#ifndef _DB_LEF_LAYER_BASE_HPP_
#define _DB_LEF_LAYER_BASE_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

enum class LefLayerType : Byte_t {
  IMPLANT     = 0,
  MASTERSLICE = 1,
  CUT         = 2,
  ROUTING     = 3,
  OVERLAP     = 4,
  UNKNOWN     = 5
};

PROJECT_NAMESPACE_END



#endif /// _DB_LEF_LAYER_BASE_HPP_

