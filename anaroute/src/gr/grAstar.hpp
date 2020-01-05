/**
 * @file   grAstar.hpp
 * @brief  Global Routing - Astar Kernel
 * @author Hao Chen
 * @date   10/09/2019
 *
 **/

#ifndef _GR_ASTAR_HPP_
#define _GR_ASTAR_HPP_

#include "grGridRoute.hpp"
#include "grAstarNode.hpp"
#include "src/ds/disjointSet.hpp"
#include "src/ds/hash.hpp"
#include "src/geo/point3d.hpp"

PROJECT_NAMESPACE_START

class GrAstar {
 public:
  GrAstar(CirDB& c, Net& n, Int_t w, GrGridRoute& g)
    : _cir(c), _net(n), _netWeight(w), _grGridRoute(g),
      _vPinLocs(g._vvNetPinLocs[n.idx()]) {}
  ~GrAstar() {}
  
  bool runKernel();

 private:
  CirDB&                      _cir;
  Net&                        _net;
  Int_t                       _netWeight; // the weight to be added in grid edges
  GrGridRoute&                _grGridRoute;
  Vector_t<Point3d<Int_t>>&   _vPinLocs;
  DisjointSet                 _compDS;
  UMap_t<UInt_t, UInt_t>                                          _mPinLocIdx2CompIdx;
  Vector_t<DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher>>  _vCompDatas;
  Vector_t<Pair_t<UInt_t, UInt_t>>                                _vCompPairs;
  Vector_t<Vector_t<Vector_t<GrAstarNode>>>                       _vvvAstarNodes;
  Vector_t<Vector_t<Point3d<Int_t>>>    _vvGuidePaths;

  // for self sym nets
  Float_t _selfSymAxisX = 0;
  
  // for sym nets
  Float_t _symAxisX = 0;
  Vector_t<Vector_t<Point3d<Int_t>>>    _vvSymGuidePaths;

  struct Param {
    Int_t horCost = 1;
    Int_t verCost = 1;
    Int_t viaCost = 10;
    Int_t factorG = 1;
    Int_t factorH = 1;
    Int_t overflowCost = 10;
  } _param;

  enum class PathDir : Byte_t {
    LEFT      = 0,
    RIGHT     = 1,
    UP        = 2,
    DOWN      = 3,
    VIA_UP    = 4,
    VIA_DOWN  = 5
  };
  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  void    initAstarNodes();
  void    initComps(const Vector_t<Point3d<Int_t>>& vPinLocs);
  void    resetAstarNodes();
  void    splitSubNetMST(Vector_t<Pair_t<UInt_t, UInt_t>>& vCompPairs);
  bool    bSatisfySelfSymCondition();
  void    makeSelfSym();
  bool    bSatisfySymCondition();
  void    makeSym();
  bool    routeSubNet(UInt_t srcIdx, UInt_t tarIdx); 
  UInt_t  mergeComp(const UInt_t srcIdx, const UInt_t tarIdx);
  void    backTrack(const GrAstarNode* pNode, const UInt_t bigCompIdx, const UInt_t srcIdx, const UInt_t tarIdx);
  void    updateGridEdges();
  void    saveGuide2Net();
  // for symnet
  void    genSymNetGuidesFromGuides();
  // for debug
  bool    bConnected();
  /////////////////////////////////////////
  //    Helper functions                 //
  /////////////////////////////////////////
  bool    bNeighbor(const Point3d<Int_t>& p1, const Point3d<Int_t>& p2);
  void    neighbors(const GrAstarNode* pNode, Vector_t<GrAstarNode*>& vpNeighbors);
  Int_t   scaledMDist(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  bool    hasBend(const GrAstarNode* pU, const GrAstarNode* pV, const Int_t i);
  PathDir findDir(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  bool    bNeedUpdate(const GrAstarNode* pV, const Int_t i, const Int_t costG, const Int_t bendCnt);
  void    add2Path(const Int_t i, const Point3d<Int_t>& u, List_t<Point3d<Int_t>>& lPathPts);
  bool    bOverflow(const GrAstarNode* pU, const GrAstarNode* pV);
  void    boundSymX(Int_t& symX);
};

PROJECT_NAMESPACE_END

#endif /// _GR_ASTAR_HPP_
