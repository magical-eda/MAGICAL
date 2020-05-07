/**
 * @file   dbPin.cpp
 * @brief  Circuit Element - Pin
 * @author Hao Chen
 * @date   09/23/2019
 *
 **/

#include "dbPin.hpp"

PROJECT_NAMESPACE_START

void Pin::setName(const String_t& n) {
  _name = n;
}

void Pin::setNetName(const String_t& n) {
  _netName = n;
}

void Pin::setNetIdx(const UInt_t i) {
  _netIdx = i;
}

void Pin::setIdx(const UInt_t i) {
  _idx = i;
}

void Pin::setMinLayerIdx(const UInt_t i) {
  _minLayerIdx = i;
}

void Pin::setMaxLayerIdx(const UInt_t i) {
  _maxLayerIdx = i;
}

void Pin::addBox(const UInt_t layerIdx, const Box<Int_t>& box) {
  assert(layerIdx >= 0 and layerIdx < _vvBoxes.size());
  _vvBoxes[layerIdx].emplace_back(box);
  _minLayerIdx = std::min(_minLayerIdx, layerIdx);
  _maxLayerIdx = std::max(_maxLayerIdx, layerIdx);
}

void Pin::setLayerBoxes(const UInt_t layerIdx, const Vector_t<Box<Int_t>>& vBoxes) {
  assert(layerIdx >= 0 and layerIdx < _vvBoxes.size());
  _vvBoxes[layerIdx] = vBoxes;
  _minLayerIdx = std::min(_minLayerIdx, layerIdx);
  _maxLayerIdx = std::max(_maxLayerIdx, layerIdx);
}

void Pin::resizeLayerBoxes(const UInt_t i) {
  _vvBoxes.resize(i);
}

void Pin::printInfo() const {

}

PROJECT_NAMESPACE_END
