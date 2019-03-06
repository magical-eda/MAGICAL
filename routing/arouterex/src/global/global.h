//
// Created by anoynmous on 04/02/2018
//

#ifndef AROUTER_GLOBAL_H_
#define AROUTER_GLOBAL_H_

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

#endif // SHROUTER_GLOBAL_H_
