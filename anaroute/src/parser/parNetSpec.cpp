/**
 * @file   parNetSpec.cpp
 * @brief  Net Spec parser
 * @author Hao Chen
 * @date   04/06/2020
 *
 **/

#include <fstream>

#include "parNetSpec.hpp"

PROJECT_NAMESPACE_START

void NetSpecReader::parse(const String_t& filename) {
  assert(_cir.lef().version() > 0);
  std::ifstream ifs(filename.c_str(), std::ifstream::in);
  if (!ifs.is_open()) {
    fprintf(stderr, "NetSpecReader::%s ERROR: Cannot open file `%s`\n", __func__, filename.c_str());
    exit(0);
  }

  String_t buf;
  String_t netName;
  Int_t minWidth, minCuts, row, col;
  bool bPower;

  ifs >> buf;
  while (ifs >> netName >> minWidth >> minCuts >> bPower >> row >> col) {
    Net& net = _cir.net(netName);
    net.setMinWidth(minWidth);
    net.setMinCuts(minCuts);
    net.setPower(bPower);
    net.setNumCutsRow(row);
    net.setNumCutsCol(col);
  }
  ifs.close();
}

PROJECT_NAMESPACE_END
