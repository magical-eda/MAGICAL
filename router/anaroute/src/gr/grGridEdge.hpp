/**
 * @file   grGridEdge.hpp
 * @brief  Global Routing Structure - GridEdge
 * @author Hao Chen
 * @date   10/10/2019
 *
 **/

#ifndef _GR_GRID_EDGE_HPP_
#define _GR_GRID_EDGE_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

class GrGridEdge {
 public:
  GrGridEdge()
    : _maxCap(0), _ocpCap(0) {}
  ~GrGridEdge() {}
  
  /////////////////////////////////////////
  //    Getters                          //
  /////////////////////////////////////////
  UInt_t                  maxCap()      const { return _maxCap; }
  UInt_t                  ocpCap()      const { return _ocpCap; }
  UInt_t                  restCap()     const { return _maxCap - _ocpCap; }
  USet_t<UInt_t>&         sNetIndices()       { return _sNetIndices; }
  const USet_t<UInt_t>&   sNetIndices() const { return _sNetIndices; }
  bool                    bOverflow()   const { return _ocpCap > _maxCap; }
  /////////////////////////////////////////
  //    Setters                          //
  /////////////////////////////////////////
  void setMaxCap(const UInt_t c) {
    _maxCap = c;
  }
  void addNetIdx(const UInt_t i, const UInt_t c) {
    _sNetIndices.emplace(i);
    _ocpCap += c;
  }
  void removeNetIdx(const UInt_t i, const UInt_t c) {
    bool bExist = _sNetIndices.erase(i);
    assert(bExist);
    _ocpCap -= c;
  }
  void clear() {
    _sNetIndices.clear();
    _ocpCap = 0;
  }

 private:
  UInt_t             _maxCap;
  UInt_t             _ocpCap;
  USet_t<UInt_t>     _sNetIndices;
};

PROJECT_NAMESPACE_END

#endif /// _GR_GRID_EDGE_HPP_
