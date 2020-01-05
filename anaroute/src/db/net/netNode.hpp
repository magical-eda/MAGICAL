/**
 * @file   drNetNode.hpp
 * @brief  Net Element - Node
 * @author Hao Chen
 * @date   09/21/2019
 *
 **/

#ifndef _DB_DR_NET_NODE_HPP_
#define _DB_DR_NET_NODE_HPP_

#include "netBase.hpp"
#include "netNodeWire.hpp"
#include "netNodeVia.hpp"
#include "src/geo/point3d.hpp"

PROJECT_NAMESPACE_START


class NetNode {
 public:
  NetNode()
    : _type(NetNodeType::MEDIUM), _parent(MAX_UINT) {}
  ~NetNode() {}
  
  // getter
  Point3d<Int_t>&           loc()                         { return _loc; }
  const Point3d<Int_t>&     loc()                   const { return _loc; }
  NetNodeType               type()                  const { return _type; }
  NetNodeWire               wire()                  const { return _wire; }
  NetNodeVia                via()                   const { return _via; }
  UInt_t                    parent()                const { return _parent; }
  UInt_t                    child(const UInt_t i)   const { return _vChilds[i]; }
  Vector_t<UInt_t>&         vChilds()                     { return _vChilds; }
  const Vector_t<UInt_t>&   vChilds()               const { return _vChilds; }
  
  bool                      hasWire()               const { return _parent != MAX_UINT; }
  bool                      hasVia()                const { return _via.bValid(); }
  bool                      hasParent()             const { return _parent != MAX_UINT; }
  bool                      hasChild()              const { return !_vChilds.empty(); }
  
  // setter
  void setLoc(const Point3d<Int_t>& p)                            { _loc = p; }
  void setLoc(const UInt_t x, const UInt_t y, const UInt_t z)     { _loc.setX(x); _loc.setY(y); _loc.setZ(z); }
  void setType(const NetNodeType t)                               { _type = t; }
  void setWire(const NetNodeWire w)                               { _wire = w; }
  void setVia(const NetNodeVia v)                                 { _via = v; }
  void setParent(const UInt_t i)                                  { _parent = i; }
  void addChild(const UInt_t i)                                   { _vChilds.emplace_back(i); }

 private:
  Point3d<Int_t>      _loc;
  NetNodeType         _type;
  NetNodeWire         _wire;
  NetNodeVia          _via;
  UInt_t             _parent;
  Vector_t<UInt_t>   _vChilds;
  
};

PROJECT_NAMESPACE_END

#endif /// _DB_DR_NET_HPP_
