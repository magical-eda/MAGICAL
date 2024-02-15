/**
 * @file   global.hpp
 * @brief  Global utils across the project
 * @author Hao Chen
 * @date   09/01/2019
 *
 **/

#ifndef ANAROUTE_GLOBAL_HPP_
#define ANAROUTE_GLOBAL_HPP_

//#define NDEBUG

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include <numeric>
#include <algorithm>

#include "namespace.hpp"
#include "type.hpp"
#include "param.hpp"
#include "src/util/MsgPrinter.h"
#include "src/util/Assert.h"

PROJECT_NAMESPACE_START

// Function aliases
static const auto &INF = MsgPrinter::inf;
static const auto &WRN = MsgPrinter::wrn;
static const auto &ERR = MsgPrinter::err;
static const auto &DBG = MsgPrinter::dbg;

PROJECT_NAMESPACE_END

#endif /// ANAROUTE_GLOBAL_HPP_
