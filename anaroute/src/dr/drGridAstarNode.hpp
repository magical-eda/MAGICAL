/**
 * @file   drGridAstarNode.hpp
 * @brief  Detailed Routing - Grid-Based A* Node
 * @author Hao Chen
 * @date   02/05/2020
 *
 **/

#ifndef _DR_GRID_ASTAR_NODE_HPP_
#define _DR_GRID_ASTAR_NODE_HPP_

#include "src/geo/point3d.hpp"

PROJECT_NAMESPACE_START

class DrGridAstarNode {
 public:
  DrGridAstarNode(const Point3d<Int_t>& co = Point3d<Int_t>(),
                  const Int_t g = MAX_INT,
                  const Int_t f = MAX_INT,
                  const Int_t b = MAX_INT,
                  const bool  e = false,
                  DrGridAstarNode* p = nullptr,
                  const Int_t d = MAX_INT)
    : _coord(co),
      _costG(g),
      _costF(f),
      _bendCnt(b),
      _bExplored(e),
      _pParent(p),
      _dist2Tar(d)
  {}
  ~DrGridAstarNode() {}
  
  /////////////////////////////////////////
  //    Getters                          //
  /////////////////////////////////////////
  const Point3d<Int_t>&               coord()       const { return _coord; }
  Int_t                               costG()       const { return _costG; }
  Int_t                               costF()       const { return _costF; }
  Int_t                               bendCnt()     const { return _bendCnt; }
  bool                                bExplored()   const { return _bExplored; }
  DrGridAstarNode*                    pParent()     const { return _pParent; }
  Int_t                               dist2Tar()    const { return _dist2Tar; }
  Vector_t<DrGridAstarNode*>&         vpNeighbors()       { return _vpNeighbors; }
  const Vector_t<DrGridAstarNode*>&   vpNeighbors() const { return _vpNeighbors; }

  /////////////////////////////////////////
  //    Setters                          //
  /////////////////////////////////////////
  void setCoord(const Point3d<Int_t>& co) { _coord = co; }
  void setCostG(const Int_t c) { _costG = c; }
  void setCostF(const Int_t c) { _costF = c; }
  void setBendCnt(const Int_t c) { _bendCnt = c; }
  void setExplored(const bool b) { _bExplored = b; }
  void setParent(DrGridAstarNode* p) { _pParent = p; }
  void setDist2Tar(const Int_t d) { _dist2Tar = d; }
  void addNeighbor(DrGridAstarNode* n) { _vpNeighbors.emplace_back(n); }
  void setNeighbors(const Vector_t<DrGridAstarNode*>& v) { _vpNeighbors = v; }
  void reset() {
    _costG = MAX_INT;
    _costF = MAX_INT;
    _bendCnt = MAX_INT;
    _bExplored = false;
    _pParent = nullptr;
    _dist2Tar = MAX_INT;
  }

 private:
  Point3d<Int_t>              _coord;
  Int_t                       _costG;
  Int_t                       _costF;
  Int_t                       _bendCnt;
  
  bool                        _bExplored;
  DrGridAstarNode*            _pParent;
  Int_t                       _dist2Tar;
  Vector_t<DrGridAstarNode*>  _vpNeighbors;
};

struct DrGridAstarNodeCmp {
  bool operator () (const DrGridAstarNode& n1, const DrGridAstarNode& n2) {
    if (n1.costF() != n2.costF())
      return n1.costF() > n2.costF();
    else if (n1.bendCnt() != n2.bendCnt())
      return n1.bendCnt() > n2.bendCnt();
    //else if (n1.costG() != n2.costG())
      //return n1.costG() > n2.costG();
    else
      return false;
  }
  bool operator () (const DrGridAstarNode* pN1, const DrGridAstarNode* pN2) {
    if (pN1->costF() != pN2->costF())
      return pN1->costF() > pN2->costF();
    else if (pN1->bendCnt() != pN2->bendCnt())
      return pN1->bendCnt() > pN2->bendCnt();
    //else if (pN1->costG() != pN2->costG())
      //return pN1->costG() > pN2->costG();
    else
      return false;
  }
};
PROJECT_NAMESPACE_END

#endif /// _DR_GRID_ASTAR_NODE_HPP_
