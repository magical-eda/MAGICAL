/**
 * @file global.h
 * @brief The header file intended to be included globally in the project
 * @author Keren Zhu
 * @date 09/30/2019
 */

#ifndef IDEAPLACE_GLOBAL_H_
#define IDEAPLACE_GLOBAL_H_

// Add all global/common header files here

#include "type.h"
#include "constant.h"
#include "parameter.h"
#include "util/MsgPrinter.h"
#include "util/Assert.h"
#include "util/kLibBase.h"
#include "util/StopWatch.hpp"

PROJECT_NAMESPACE_BEGIN

// Function aliases
static const auto &INF = MsgPrinter::inf;
static const auto &WRN = MsgPrinter::wrn;
static const auto &ERR = MsgPrinter::err;
static const auto &DBG = MsgPrinter::dbg;
static const auto &WATCH_QUICK_START = klib::StopWatchMgr::quickStart;
static const auto &WATCH_QUICK_END = klib::StopWatchMgr::quickEnd;
static const auto &WATCH_CREATE_NEW = klib::StopWatchMgr::createNewStopWatch;;
static const auto &WATCH_LOOK_RECORD_TIME = klib::StopWatchMgr::time;

PROJECT_NAMESPACE_END

#endif // IDEAPLACE_GLOBAL_H_
