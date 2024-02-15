/**
 * @file   drMgr.cpp
 * @brief  Detailed Routing - Manager
 * @author Hao Chen
 * @date   09/30/2019
 *
 **/

#include "drMgr.hpp"
#include "drGridlessRoute.hpp"
#include "drGridRoute.hpp"
#include "drSymmetry.hpp"
#include "drRoutable.hpp"
#include "drPower.hpp"

PROJECT_NAMESPACE_START

bool DrMgr::solve(const bool bGrid, const bool bUseSymFile) {
  fprintf(stderr, "\nDrMgr::%s Start Detailed Routing\n", __func__);
  if (bGrid)
    return runGridRoute(bUseSymFile);
  else
    return runGridlessRoute();
}

bool DrMgr::runGridlessRoute() {
  DrGridlessRoute kernel(_cir, *this, _drcMgr);
  return kernel.solve();
}

bool DrMgr::runGridRoute(const bool bUseSymFile) {

  DrSymmetry sym(_cir);
  sym.solve(bUseSymFile);

  DrRoutable ro(_cir);
  ro.constructRoutables();

  DrGridRoute kernel(_cir, *this, _drcMgr);
  return kernel.solve();
}

/////////////////////////////////////////
//    Private functions                //
/////////////////////////////////////////

PROJECT_NAMESPACE_END
