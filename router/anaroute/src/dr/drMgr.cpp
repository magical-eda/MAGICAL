/**
 * @file   drMgr.cpp
 * @brief  Detailed Routing - Manager
 * @author Hao Chen
 * @date   09/30/2019
 *
 **/

#include "drMgr.hpp"
#include "drGridlessRoute.hpp"

PROJECT_NAMESPACE_START

void DrMgr::solve() {
  fprintf(stderr, "DrMgr::%s Start Detailed Routing\n", __func__);
  runGridlessRoute();
}

void DrMgr::runGridlessRoute() {
  DrGridlessRoute kernel(_cir, *this, _drcMgr);
  kernel.solve();
}

/////////////////////////////////////////
//    Private functions                //
/////////////////////////////////////////

PROJECT_NAMESPACE_END
