/**
 * @file   grAstarNode.hpp
 * @brief  Global Routing - Astar Node
 * @author Hao Chen
 * @date   10/09/2019
 *
 **/

#ifndef _GR_ASTAR_NODE_HPP_
#define _GR_ASTAR_NODE_HPP_

#include "src/global/global.hpp"
#include "src/geo/point3d.hpp"

PROJECT_NAMESPACE_START


class GrAstarNode {
 public:
  GrAstarNode(const Point3d<Int_t>& co = Point3d<Int_t>(),
              const Int_t g0 = MAX_INT, const Int_t g1 = MAX_INT,
              const Int_t f0 = MAX_INT, const Int_t f1 = MAX_INT,
              const Int_t b0 = MAX_INT, const Int_t b1 = MAX_INT,
              const bool e0 = false, const bool e1 = false,
              GrAstarNode* p0 = nullptr, GrAstarNode* p1 = nullptr)
    : _coord(co),
      _costG{g0, g1},
      _costF{f0, f1},
      _bendCnt{b0, b1},
      _bExplored{e0, e1},
      _pParent{p0, p1} {}

  ~GrAstarNode() {}
  
  /////////////////////////////////////////
  //    Getters                          //
  /////////////////////////////////////////
  const Point3d<Int_t>&           coord()                       const { return _coord; }
  Int_t                           costG(const UInt_t i)         const { return _costG[i]; }
  Int_t                           costF(const UInt_t i)         const { return _costF[i]; }
  Int_t                           bendCnt(const UInt_t i)       const { return _bendCnt[i]; }
  bool                            bExplored(const UInt_t i)     const { return _bExplored[i]; }
  GrAstarNode*                    pParent(const UInt_t i)       const { return _pParent[i]; }
  Vector_t<GrAstarNode*>&         vpNeighbors()                       { return _vpNeighbors; }
  const Vector_t<GrAstarNode*>&   vpNeighbors()                 const { return _vpNeighbors; }
  
  /////////////////////////////////////////
  //    Setters                          //
  /////////////////////////////////////////
  void setCoord(const Point3d<Int_t>& co) { _coord = co; }
  void setCostG(const UInt_t i, const Int_t c) { _costG[i] = c; }
  void setCostF(const UInt_t i, const Int_t c) { _costF[i] = c; }
  void setBendCnt(const UInt_t i, const Int_t c) { _bendCnt[i] = c; }
  void setExplored(const UInt_t i, const bool b) { _bExplored[i] = b; }
  void setParent(const UInt_t i, GrAstarNode* p) { _pParent[i] = p; }
  void addNeighbor(GrAstarNode* n) { _vpNeighbors.emplace_back(n); }
  void setNeighbors(const Vector_t<GrAstarNode*>& v) { _vpNeighbors = v; }
  void reset() {
    _costG[0] = MAX_INT;
    _costG[1] = MAX_INT;
    _costF[0] = MAX_INT;
    _costF[1] = MAX_INT;
    _bendCnt[0] = MAX_INT;
    _bendCnt[1] = MAX_INT;
    _bExplored[0] = false;
    _bExplored[1] = false;
    _pParent[0] = nullptr;
    _pParent[1] = nullptr;
  }

 private:
  Point3d<Int_t>          _coord;
  Int_t                   _costG[2];
  Int_t                   _costF[2];
  Int_t                   _bendCnt[2];
  bool                    _bExplored[2];
  GrAstarNode*            _pParent[2];
  Vector_t<GrAstarNode*>  _vpNeighbors;
};

struct GrAstarNodeCmp0 {
  bool operator () (const GrAstarNode& n1, const GrAstarNode& n2) {
    if (n1.costF(0) != n2.costF(0))
      return n1.costF(0) > n2.costF(0);
    else if (n1.bendCnt(0) != n2.bendCnt(0))
      return n1.bendCnt(0) > n2.bendCnt(0);
    else if (n1.costG(0) != n2.costG(0))
      return n1.costG(0) < n2.costG(0);
    else
      return false;
  }
  bool operator () (const GrAstarNode* pN1, const GrAstarNode* pN2) {
    if (pN1->costF(0) != pN2->costF(0))
      return pN1->costF(0) > pN2->costF(0);
    else if (pN1->bendCnt(0) != pN2->bendCnt(0))
      return pN1->bendCnt(0) > pN2->bendCnt(0);
    else if (pN1->costG(0) != pN2->costG(0))
      return pN1->costG(0) < pN2->costG(0);
    else
      return false;
  }
};

struct GrAstarNodeCmp1 {
  bool operator () (const GrAstarNode& n1, const GrAstarNode& n2) {
    if (n1.costF(1) != n2.costF(1))
      return n1.costF(1) > n2.costF(1);
    else if (n1.bendCnt(1) != n2.bendCnt(1))
      return n1.bendCnt(1) > n2.bendCnt(1);
    else if (n1.costG(1) != n2.costG(1))
      return n1.costG(1) < n2.costG(1);
    else
      return false;
  }
  bool operator () (const GrAstarNode* pN1, const GrAstarNode* pN2) {
    if (pN1->costF(1) != pN2->costF(1))
      return pN1->costF(1) > pN2->costF(1);
    else if (pN1->bendCnt(1) != pN2->bendCnt(1))
      return pN1->bendCnt(1) > pN2->bendCnt(1);
    else if (pN1->costG(1) != pN2->costG(1))
      return pN1->costG(1) < pN2->costG(1);
    else
      return false;
  }
};

PROJECT_NAMESPACE_END

#endif /// _GR_ASTAR_NODE_HPP_
