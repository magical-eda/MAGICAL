/**
 * @file   drNetBase.hpp
 * @brief  Net Element - Basics
 * @author Hao Chen
 * @date   09/21/2019
 *
 **/

#ifndef _DB_DR_NET_BASE_HPP_
#define _DB_DR_NET_BASE_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

enum NetNodeType : Byte_t {
  TERMINAL = 0,
  MEDIUM = 1
};

PROJECT_NAMESPACE_END

#endif /// _DB_DR_NET_BASE_HPP_
