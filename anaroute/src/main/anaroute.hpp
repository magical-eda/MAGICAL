/**
 * @file   anaroute.hpp
 * @brief  Analog Router
 * @author Hao Chen
 * @date   09/01/2019
 *
 **/

#ifndef _ANAROUTE_CORE_HPP_
#define _ANAROUTE_CORE_HPP_

#include "src/global/global.hpp"
#include "include/cmdline.h"

PROJECT_NAMESPACE_START

class Anaroute {
public:
  Anaroute(int argc, char** argv);
  ~Anaroute() {}
    
private:
  cmdline::parser _args;

  void parseArgs(int argc, char** argv);
};

PROJECT_NAMESPACE_END

#endif /// _ANAROUTE_CORE_HPP_
