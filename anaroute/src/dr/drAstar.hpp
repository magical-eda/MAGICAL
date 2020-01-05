/**
 * @file   drAstar.hpp
 * @brief  Detailed Routing - A* search kernel
 * @author Hao Chen
 * @date   10/25/2019
 *
 **/

#ifndef _DR_ASTAR_HPP_
#define _DR_ASTAR_HPP_

#include "drGridlessRoute.hpp"
#include "drAstarNode.hpp"
#include "src/ds/disjointSet.hpp"
#include "src/ds/hash.hpp"
#include "src/geo/point3d.hpp"
#include "src/geo/spatial.hpp"

PROJECT_NAMESPACE_START

class DrAstar {
 public:
  DrAstar(CirDB& c, Net& n, DrGridlessRoute& d)
    : _cir(c), _net(n), _drGridlessRoute(d), _drcMgr(d._drcMgr),
      _vSpatialNetGuides(c.vSpatialNetGuides(n.idx()))
      //_vAllAstarNodesMap(d._vAllAstarNodesMap)
      //_vSpatialRoutedWires(c.v)
  {
    
    //const LefRoutingLayer& M1 = _cir.lef().routingLayer(1);
    //const Int_t minSpacing = M1.spacingTable().table.size() ?
                             //M1.spacingTable().table[0].second[0] :
                             //M1.spacing(0);
    //const Int_t xc = _cir.width() / minSpacing;
    //const Int_t yc = _cir.height() / minSpacing;
    //_param.maxExplore = xc * yc;
    _param.maxExplore = 200000;
  }
  ~DrAstar() {
    for (auto& v : _vAllAstarNodesMap) {
      for (auto& m : v) {
        delete m.second;
      }
    }
  }

  bool runKernel();

 private:
  CirDB&            _cir;
  Net&              _net;
  DrGridlessRoute&  _drGridlessRoute;
  DrcMgr&           _drcMgr;
  
  const Vector_t<Spatial<Int_t>>& _vSpatialNetGuides;

  Vector_t<Vector_t<Box<Int_t>>> _vAllLayerBoxes;

  DisjointSet                                                     _compDS;
  Vector_t<Vector_t<Pair_t<Box<Int_t>, Int_t>>>                   _vCompBoxes;
  Vector_t<DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher>>  _vCompAcsPts;
  Vector_t<UMap_t<Int_t, Spatial<Int_t>>>                         _vCompSpatialBoxes;
  Vector_t<Pair_t<UInt_t, UInt_t>>                                _vCompPairs;
  
  //Vector_t<DenseHashMap<Point<Int_t>, DrAstarNode*, Point<Int_t>::hasher>>&  _vAllAstarNodesMap;
  Vector_t<DenseHashMap<Point<Int_t>, DrAstarNode*, Point<Int_t>::hasher>>  _vAllAstarNodesMap;

  Vector_t<Vector_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>>>     _vvRoutePaths;
  Vector_t<Vector_t<UInt_t>>                                     _vvRouteViaIndices;
  Vector_t<Vector_t<Pair_t<Box<Int_t>, Int_t>>>                  _vvRoutedWires;

  struct Param {
    Int_t horCost = 1;
    Int_t verCost = 1;
    Int_t viaCost = 800;
    Int_t factorG = 1;
    Int_t factorH = 1;
    Int_t guideCost = -5000;
    UInt_t maxExplore;
    UInt_t maxSymTry = 3;
  } _param;
  
  enum class PathDir : Byte_t {
    LEFT      = 0,
    RIGHT     = 1,
    UP        = 2,
    DOWN      = 3,
    VIA_UP    = 4,
    VIA_DOWN  = 5
  };

  // for self sym
  Int_t _selfSymAxisX = 0;
  bool  _bSatisfySelfSym = false;

  // for sym
  Int_t _symAxisX = 0;
  bool  _bSatisfySym = false;

  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  void    splitSubNetMST();
  void    init();
  void    computeSelfSymAxisX();
  void    computeSymAxisX();
  void    constructAllLayerBoxes();
  bool    bSatisfySelfSymCondition() const;
  bool    bSatisfySymCondition() const;
  bool    routeSubNet(UInt_t srcIdx, UInt_t tarIdx);
  void    resetAstarNodes();
  UInt_t  mergeComp(const UInt_t srcIdx, const UInt_t tarIdx);
  void    backTrack(const DrAstarNode* pNode, const UInt_t bigCompIdx, const UInt_t srcIdx, const UInt_t tarIdx);
  void    savePath(const List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>>& lPathVec);
  void    saveResult2Net();
  void    saveResult2SymNet();
  void    ripup();

  /////////////////////////////////////////
  //    Helper functions                 //
  /////////////////////////////////////////
  void    addAcsPts(const UInt_t idx, const Int_t z, const Box<Int_t>& box);
  void    neighbors(const DrAstarNode* pU, Vector_t<DrAstarNode*>& vpNeighbors);
  bool    bViolateDRC(const DrAstarNode* pU, const DrAstarNode* pV);
  bool    bViolateDRC(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  bool    bViolateSymDRC(const DrAstarNode* pU, const DrAstarNode* pV);
  bool    bViolateSymDRC(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  UInt_t  selectVia(const DrAstarNode* pU, const DrAstarNode* pV);
  UInt_t  selectVia(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  UInt_t  selectViaSym(const DrAstarNode* pU, const DrAstarNode* pV);
  UInt_t  selectViaSym(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  void    addWire2RoutedSpatial(const UInt_t netIdx, const DrAstarNode* pU, const DrAstarNode* pV);
  void    addWire2RoutedSpatial(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  void    addVia2RoutedSpatial(const UInt_t netIdx, const DrAstarNode* pU, const DrAstarNode* pV, const UInt_t viaIdx);
  void    addVia2RoutedSpatial(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v, const UInt_t viaIdx);
  Int_t   scaledMDist(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  Int_t   scaledMDist(const Box<Int_t>& u, const Box<Int_t>& v);
  Int_t   scaledMDist(const Pair_t<Box<Int_t>, Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& v);
  Int_t   scaledMDist(const Point3d<Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& box);
  Int_t   MDist(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  Int_t   MDist(const Box<Int_t>& u, const Box<Int_t>& v);
  Int_t   MDist(const Pair_t<Box<Int_t>, Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& v);
  Int_t   MDist(const Point3d<Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& box);
  void    nearestTarBoxDist(const Point3d<Int_t>& u, const UInt_t tarIdx, Int_t& scaledDist, Int_t& unscaledDist);
  bool    hasBend(const DrAstarNode* pU, const DrAstarNode* pV, const Int_t i);
  PathDir findDir(const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  bool    bNeedUpdate(const DrAstarNode* pV, const Int_t i, const Int_t costG, const Int_t bendCnt);
  bool    bInsideGuide(const DrAstarNode* pV);
  void    add2Path(const Int_t i, const Point3d<Int_t>& u, List_t<Point3d<Int_t>>& lPathPts);
  bool    bConnected2TarBox(const DrAstarNode* pU, const UInt_t tarIdx);
  bool    bNeedMergePath(const Point3d<Int_t>& u1, const Point3d<Int_t>& v1, const Point3d<Int_t>& u2, const Point3d<Int_t>& v2);
  void    mergePath(const List_t<Point3d<Int_t>>& lPathPts, List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>>& lPathVec);
  void    toWire(const Point3d<Int_t>& u, const Point3d<Int_t>& v, Box<Int_t>& wire);
  void    toVia(const Point3d<Int_t>& u, const Point3d<Int_t>& v, const UInt_t viaIdx, Vector_t<Pair_t<Box<Int_t>, Int_t>>& vRet);
  // for debug
  void    visualize();
};

PROJECT_NAMESPACE_END

#endif /// _DR_ASTAR_HPP_
