/**
 * @file   dbNet.cpp
 * @brief  Circuit Element - Detailed Routing Net
 * @author Hao Chen
 * @date   09/21/2019
 *
 **/

#include "dbNet.hpp"

PROJECT_NAMESPACE_START

void Net::setName(const String_t& n) {
  _name = n;
}

void Net::setSelfSym() {
  _bSelfSym = true;
}

void Net::setIOPort() {
  _bIOPort = true;
}

void Net::setSymNetIdx(const UInt_t i) {
  _symNetIdx = i;
}

void Net::setRouted(const bool b) {
  _bRouted = b;
}

void Net::addPinIdx(const UInt_t i) {
  _vPinIndices.emplace_back(i);
}

void Net::addNode(const NetNode& n) {
  // TODO
  _vNodes.emplace_back(n);
}

void Net::addGrFail() {
  ++_grFailCnt;
}

void Net::addDrFail() {
  ++_drFailCnt;
}

void Net::clearGrFail() {
  _grFailCnt = 0;
}

void Net::clearDrFail() {
  _drFailCnt = 0;
}

void Net::setGuides(const Vector_t<Pair_t<Box<Int_t>, Int_t>>& v) {
  _vGuides = v;
}
PROJECT_NAMESPACE_END
