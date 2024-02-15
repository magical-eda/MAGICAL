/**
 * @file   parPower.cpp
 * @brief  Parser - Power net reader
 * @author Hao Chen
 * @date   03/11/2020
 *
 **/

#include <fstream>

#include "parPower.hpp"


PROJECT_NAMESPACE_START

void PowerNetReader::parse(const String_t& filename) {
  assert(_cir.lef().version() > 0);
  std::ifstream ifs(filename.c_str(), std::ifstream::in);
  
  if (!ifs.is_open()) {
    fprintf(stderr, "PowerNetReader::%s ERROR: Cannot open file `%s`\n", __func__, filename.c_str());
    exit(0);
  }

  String_t netName;

  while (ifs >> netName) {
    Net& net = _cir.net(netName);
    net.setPower();
    UInt_t i, pinIdx;
    Net_ForEachPinIdx(net, pinIdx, i) {
      Pin& pin = _cir.pin(pinIdx);
      pin.setPower();
    }
  }
  ifs.close();
}

PROJECT_NAMESPACE_END
