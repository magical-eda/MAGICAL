/**
 * @file global.h
 * @brief The wrapper for the global-included headers
 * @author Keren Zhu
 * @date 06/14/2019
 */

#ifndef MAGICAL_FLOW_GLOBAL_H_
#define MAGICAL_FLOW_GLOBAL_H_

// Add all global/common header files here

#include "type.h"
#include "constant.h"
#include "parameter.h"
#include "util/MsgPrinter.h"
#include "util/Assert.h"
#include "util/kLibBase.h"

PROJECT_NAMESPACE_BEGIN

// Function aliases
static const auto &INF = MsgPrinter::inf;
static const auto &WRN = MsgPrinter::wrn;
static const auto &ERR = MsgPrinter::err;
static const auto &DBG = MsgPrinter::dbg;

PROJECT_NAMESPACE_END

#endif // MAGICAL_FLOW_GLOBAL_H_
