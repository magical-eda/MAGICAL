/**
 * @file   drNetNodeWire.hpp
 * @brief  Net Element - Node wire
 * @author Hao Chen
 * @date   09/21/2019
 *
 **/

#ifndef _DB_DR_NET_NODE_WIRE_HPP_
#define _DB_DR_NET_NODE_WIRE_HPP_

#include "netBase.hpp"

PROJECT_NAMESPACE_START

class NetNodeWire {
 public:
  NetNodeWire()
    : _extension(0), _halfWidth(0) {}
  ~NetNodeWire() {}
 
  // getter
  Int_t extension() const { return _extension; }
  Int_t halfWidth() const { return _halfWidth; }

  // setter
  void setExtension(const Int_t e) { _extension = e; }
  void setHalfWidth(const Int_t w) { _halfWidth = w; }

 private:
  Int_t   _extension;
  Int_t   _halfWidth;
};

PROJECT_NAMESPACE_END

#endif /// _DB_DR_NET_NODE_WIRE_HPP_
