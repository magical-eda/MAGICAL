/**
 * @file   grAstar.cpp
 * @brief  Global Routing - Astar Kernel
 * @author Hao Chen
 * @date   10/09/2019
 *
 **/

#include "grAstar.hpp"
#include "src/geo/kdtree.hpp"
#include "src/ds/pqueue.hpp"

#include <lemon/maps.h>
#include <lemon/kruskal.h>
#include <lemon/list_graph.h>

PROJECT_NAMESPACE_START

bool GrAstar::runKernel() {
  fprintf(stdout, "GrAstar::%s Route net \"%s\"\n", __func__, _net.name().c_str());
  if (_net.bSelfSym()) {
    if (!bSatisfySelfSymCondition()) {
      fprintf(stderr, "GrAstar::%s WARNING: Net %s does not satisfy self symmetric condition!\n", __func__, _net.name().c_str());
      //fprintf(stderr, "GrAstar::%s Make Self Sym %s\n", __func__, _net.name().c_str());
      //makeSelfSym();
    }
  }
  else if (_net.hasSymNet()) {
    if (!bSatisfySymCondition()) {
      fprintf(stderr, "GrAstar::%s WARNING: Net %s does not satisfy symmetric net condition!\n", __func__, _net.name().c_str());
      //fprintf(stderr, "GrAstar::%s Make Sym %s %s\n", __func__, _net.name().c_str(), _cir.net(_net.symNetIdx()).name().c_str());
      //makeSym();
    }
  }

  initAstarNodes();
  initComps(_vPinLocs);
  splitSubNetMST(_vCompPairs);
  
  //std::cout<<_net.name();
  //std::cout<< (_net.bSelfSym()? " SelfSym" :"");
  //std::cout<< (_net.hasSymNet()? " Sym " + _cir.net(_net.symNetIdx()).name() : "") << std::endl;
  //std::cout<<"Comps:"<<std::endl;
  //for (auto& v : _vCompDatas) {
    //for (auto& p : v)
      //std::cout << p;
    //std::cout<<std::endl;
  //}
  //std::cout<<std::endl;
  
  
  if (_net.bSelfSym()) {
    for (const Pair_t<UInt_t, UInt_t>& pair : _vCompPairs) {
      if (!routeSubNet(pair.first, pair.second)) {
        fprintf(stderr, "GrAstar::%s ERROR: Route SelfSym Net failed!\n", __func__);
        return false;
      }
    }
  }
  else if (_net.hasSymNet()) {
    for (const Pair_t<UInt_t, UInt_t>& pair : _vCompPairs) {
      if (!routeSubNet(pair.first, pair.second)) {
        fprintf(stderr, "GrAstar::%s ERROR: Route Sym Net failed!\n", __func__);
        return false;
      }
    }
    genSymNetGuidesFromGuides();
  }
  else {
    // normal net
    for (const Pair_t<UInt_t, UInt_t>& pair : _vCompPairs) {
      if (!routeSubNet(pair.first, pair.second)) {
        fprintf(stderr, "GrAstar::%s ERROR: Route subnet failed!\n", __func__);
        return false;
      }
    }
  }
  
  //std::cout<<"sol:"<<std::endl;
  //for (auto& v : _vvGuidePaths) {
    //for (auto& p : v)
      //std::cout << p;
    //std::cout << std::endl;
  //}
  //std::cout<<std::endl;
  
  updateGridEdges();
  saveGuide2Net();

  //assert(bConnected());
  return true;
}

/////////////////////////////////////////
//    Private functions                //
/////////////////////////////////////////
void GrAstar::initAstarNodes() {
  GrGridMap3d& gridMap = _grGridRoute._gridMap;
  UInt_t i, j, k;
  _vvvAstarNodes.resize(gridMap.numGridCellsZ());
  for (i = 0; i < _vvvAstarNodes.size(); ++i) {
    _vvvAstarNodes[i].resize(gridMap.numGridCellsX());
    for (j = 0; j < _vvvAstarNodes[i].size(); ++j) {
      _vvvAstarNodes[i][j].resize(gridMap.numGridCellsY());
      for (k = 0; k < _vvvAstarNodes[i][j].size(); ++k) {
        GrAstarNode& node = _vvvAstarNodes[i][j][k];
        node.setCoord(Point3d<Int_t>(j, k, i));
      }
    }
  }
}

void GrAstar::resetAstarNodes() {
  for (auto& vv : _vvvAstarNodes) {
    for (auto& v : vv) {
      for (GrAstarNode& node : v) {
        node.reset();
      }
    }
  }
}

void GrAstar::initComps(const Vector_t<Point3d<Int_t>>& vPinLocs) {
  DisjointSet pinLocDS;
  pinLocDS.init(vPinLocs.size());
  UInt_t i, j;
  for (i = 0; i < vPinLocs.size(); ++i) {
    const Point3d<Int_t>& p1 = vPinLocs[i];
    for (j = i + 1; j < vPinLocs.size(); ++j) {
      const Point3d<Int_t>& p2 = vPinLocs[j];
      if (bNeighbor(p1, p2) and pinLocDS.find(i) != pinLocDS.find(j)) {
        pinLocDS.merge(i, j);
      }
    }
  }
  //printf("%s %d\n", _net.name().c_str(), pinLocDS.nSets());
  _vCompDatas.resize(pinLocDS.nSets());
  for (i = 0; i < _vCompDatas.size(); ++i) {
    _vCompDatas[i].set_empty_key(Point3d<Int_t>(MIN_INT, MIN_INT, MIN_INT));
    _vCompDatas[i].set_deleted_key(Point3d<Int_t>(MAX_INT, MAX_INT, MAX_INT));
  }
  j = 0;
  for (i = 0; i < vPinLocs.size(); ++i) {
    if (_mPinLocIdx2CompIdx.find(pinLocDS.find(i)) == _mPinLocIdx2CompIdx.end()) {
      _mPinLocIdx2CompIdx[pinLocDS.find(i)] = j++;
    }
    _vCompDatas[_mPinLocIdx2CompIdx[pinLocDS.find(i)]].insert(vPinLocs[i]);
  }
  _compDS.init(_vCompDatas.size());
}

void GrAstar::splitSubNetMST(Vector_t<Pair_t<UInt_t, UInt_t>>& vCompPairs) {
  lemon::ListGraph graph;
  Vector_t<lemon::ListGraph::Node> vGraphNodes;
  Vector_t<lemon::ListGraph::Edge> vGraphEdges;
  lemon::ListGraph::EdgeMap<Int_t> edgeCostMap(graph);

  Vector_t<Int_t> vEdgeCostValues;
  UInt_t i, j;
  for (i = 0; i < _vCompDatas.size(); ++i) {
    vGraphNodes.emplace_back(graph.addNode());
  }
  for (i = 0; i < _vCompDatas.size(); ++i) {
    for (j = i + 1; j < _vCompDatas.size(); ++j) {
      vGraphEdges.emplace_back(graph.addEdge(vGraphNodes[i], vGraphNodes[j]));
      Int_t minDist = MAX_INT;
      for (const Point3d<Int_t>& u : _vCompDatas[i]) {
        for (const Point3d<Int_t>& v : _vCompDatas[j]) {
          Int_t dist = scaledMDist(u, v);
          if (dist < minDist) {
            minDist = dist; 
          }
        }
      }
      assert(minDist != MAX_INT);
      vEdgeCostValues.emplace_back(minDist);
    }
  }
  assert(vEdgeCostValues.size() == vGraphEdges.size());
  for (i = 0; i < vGraphEdges.size(); ++i) {
    edgeCostMap.set(vGraphEdges[i], vEdgeCostValues[i]);
  }

  Vector_t<lemon::ListGraph::Edge> vResEdges;
  lemon::kruskal(graph, edgeCostMap, std::back_inserter(vResEdges));
  assert(vResEdges.size() == _vCompDatas.size() - 1);
  for (i = 0; i < vResEdges.size(); ++i) {
    lemon::ListGraph::Edge& edge = vResEdges[i];
    vCompPairs.emplace_back(graph.id(graph.u(edge)), graph.id(graph.v(edge)));
    //vCompPairs.emplace_back(i, i + 1);
  }
}

bool GrAstar::bSatisfySelfSymCondition() {
  std::sort(_vPinLocs.begin(), _vPinLocs.end());
  for (const Point3d<Int_t>& p : _vPinLocs) {
    _selfSymAxisX += p.x();
  }
  _selfSymAxisX /= _vPinLocs.size();
  for (const Point3d<Int_t>& p : _vPinLocs) {
    const Int_t symX = (Int_t)(2 * _selfSymAxisX - p.x());
    const Point3d<Int_t> symPt(symX, p.y(), p.z());
    if (!std::binary_search(_vPinLocs.begin(), _vPinLocs.end(), symPt)) {
      return false;
    }
  }
  return true;
}

void GrAstar::makeSelfSym() {
  Vector_t<Point3d<Int_t>> vNewPinLocs;
  for (const Point3d<Int_t>& p : _vPinLocs) {
    Int_t symX = (Int_t)(2 * _selfSymAxisX - p.x());
    //if (symX >= (Int_t)_grGridRoute._gridMap.numGridCellsX())
      //symX = _grGridRoute._gridMap.numGridCellsX() - 1;
    //else if (symX < 0)
      //symX = 0;
    boundSymX(symX);
    const Point3d<Int_t> symPt(symX, p.y(), p.z());
    if (!std::binary_search(_vPinLocs.begin(), _vPinLocs.end(), symPt)) {
      vNewPinLocs.emplace_back(symPt);
    }
  }
  for (const Point3d<Int_t>& p : vNewPinLocs) {
    _vPinLocs.emplace_back(p);
  }
}

bool GrAstar::bSatisfySymCondition() {
  const Net& symNet = _cir.net(_net.symNetIdx());
  Vector_t<Point3d<Int_t>>& vSymPinLocs = _grGridRoute._vvNetPinLocs[symNet.idx()];
  std::sort(vSymPinLocs.begin(), vSymPinLocs.end());
  for (const Point3d<Int_t>& p : _vPinLocs) {
    _symAxisX += p.x();
  }
  for (const Point3d<Int_t>& p : vSymPinLocs) {
    _symAxisX += p.x();
  }
  _symAxisX /= (_vPinLocs.size() + vSymPinLocs.size());
  if (vSymPinLocs.size() != _vPinLocs.size()) {
    return false;
  }
  for (const Point3d<Int_t>& p : _vPinLocs) {
    const Int_t symX = (Int_t)(2 * _symAxisX - p.x());
    const Point3d<Int_t> symPt(symX, p.y(), p.z());
    if (!std::binary_search(vSymPinLocs.begin(), vSymPinLocs.end(), symPt)) {
      return false;
    }
  }
  return true;
}

void GrAstar::makeSym() {
  assert(_net.hasSymNet() and _symAxisX > 0);
  Net& symNet = _cir.net(_net.symNetIdx());
  Vector_t<Point3d<Int_t>>& vSymPinLocs = _grGridRoute._vvNetPinLocs[symNet.idx()];
  for (const auto& p : _vPinLocs) {
    Int_t symX = (Int_t)(2 * _symAxisX - p.x());
    //if (symX >= (Int_t)_grGridRoute._gridMap.numGridCellsX())
      //symX = _grGridRoute._gridMap.numGridCellsX() - 1;
    //else if (symX < 0)
      //symX = 0;
    boundSymX(symX);
    const Point3d<Int_t> symPt(symX, p.y(), p.z());
    if (!std::binary_search(vSymPinLocs.begin(), vSymPinLocs.end(), symPt)) {
      vSymPinLocs.emplace_back(symPt);
    }
  }
  for (const auto& p : vSymPinLocs) {
    Int_t symX = (Int_t)(2 * _symAxisX - p.x());
    //if (symX >= (Int_t)_grGridRoute._gridMap.numGridCellsX())
      //symX = _grGridRoute._gridMap.numGridCellsX() - 1;
    //else if (symX < 0)
      //symX = 0;
    boundSymX(symX);
    const Point3d<Int_t> symPt(symX, p.y(), p.z());
    if (!std::binary_search(_vPinLocs.begin(), _vPinLocs.end(), symPt)) {
      _vPinLocs.emplace_back(symPt);
    }
  }
}

bool GrAstar::routeSubNet(UInt_t srcIdx, UInt_t tarIdx) {
  assert(_compDS.find(srcIdx) != _compDS.find(tarIdx));
  UInt_t i = 0;
  
  // init source and target
  Int_t id[2] = {_compDS.find(srcIdx), _compDS.find(tarIdx)};
  DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher>* pTar[2] = {&_vCompDatas[id[0]], &_vCompDatas[id[1]]};
  
  // check if already connected
  for (const Point3d<Int_t>& p : _vCompDatas[id[0]]) {
    if (_vCompDatas[id[1]].find(p) != _vCompDatas[id[1]].end()) {
      mergeComp(id[0], id[1]);
      return true;
    }
  }

  // init kdtrees
  K3dTree<Int_t> kd[2];
  for (i = 0; i < 2; ++i) {
    for (const Point3d<Int_t>& p : *pTar[i]) {
      kd[i].insert(p.x() * _param.horCost,
                   p.y() * _param.verCost,
                   p.z() * _param.viaCost);
    }
    kd[i].buildIndex();
  }
  
  // init priority queue
  typedef PairingHeap<GrAstarNode*, GrAstarNodeCmp0> PQueue0_t;
  typedef PairingHeap<GrAstarNode*, GrAstarNodeCmp1> PQueue1_t;
  PQueue0_t pq0;
  PQueue1_t pq1;
  typedef DenseHashMap<GrAstarNode*, PQueue0_t::point_iterator> Itermap0_t;
  typedef DenseHashMap<GrAstarNode*, PQueue1_t::point_iterator> Itermap1_t;
  Itermap0_t itMap0;
  Itermap1_t itMap1;
  itMap0.set_empty_key(nullptr);
  itMap1.set_empty_key(nullptr);
  
  // reset cost
  resetAstarNodes();

  // path search
  for (i = 0; i < 2; ++i) {
    for (const Point3d<Int_t>& p : *pTar[i]) {
      Point3d<Int_t> p_scaled(p.x() * _param.horCost,
                              p.y() * _param.verCost,
                              p.z() * _param.viaCost);
      Point3d<Int_t> p_nearest;
      Int_t dist_nearest;
      kd[i ^ 1].nearestSearch(p_scaled, p_nearest, dist_nearest);
      //p_nearest.setX(p_nearest.x() / _param.horCost);
      //p_nearest.setY(p_nearest.y() / _param.verCost);
      //p_nearest.setZ(p_nearest.z() / _param.viaCost);
      // init cost
      GrAstarNode* pNode = &_vvvAstarNodes[p.z()][p.x()][p.y()];
      Int_t costF = _param.factorH * dist_nearest;
      if (_net.bSelfSym()) {
        costF *= abs(p.x() - _selfSymAxisX);
      }
      pNode->setCostF(i, costF);
      pNode->setCostG(i, 0);
      pNode->setBendCnt(i, 0);
      if (i == 0) {
        itMap0[pNode] = pq0.push(pNode);
      }
      else {
        itMap1[pNode] = pq1.push(pNode);
      }
    }
  }
  while (!pq0.empty() and !pq1.empty()) {
    auto __pathSearch = [&] (const UInt_t i) -> bool {
      GrAstarNode* pU = (i == 0) ? pq0.top() : pq1.top();
      if (pU->bExplored(i ^ 1) or
          pTar[i ^ 1]->find(pU->coord()) != pTar[i ^ 1]->end()) {
        const UInt_t bigCompIdx = mergeComp(id[0], id[1]);
        backTrack(pU, bigCompIdx, id[0], id[1]);
        return true;
      }
      if (i == 0)
        pq0.pop();
      else
        pq1.pop();
      pU->setExplored(i, true);
      if (pU->vpNeighbors().empty()) {
        Vector_t<GrAstarNode*> vpNeighbors;
        neighbors(pU, vpNeighbors);
        pU->setNeighbors(vpNeighbors);
      }
      for (GrAstarNode* pV : pU->vpNeighbors()) {
        if (pV->bExplored(i))
          continue;
        Int_t costG = pU->costG(i) + scaledMDist(pU->coord(), pV->coord());
        Int_t bendCnt = pU->bendCnt(i) + hasBend(pU, pV, i);
        if (bNeedUpdate(pV, i, costG, bendCnt)) {
          pV->setParent(i, pU);
          Point3d<Int_t> p_scaled(pV->coord().x() * _param.horCost,
                                  pV->coord().y() * _param.verCost,
                                  pV->coord().z() * _param.viaCost);
          Point3d<Int_t> p_nearest;
          Int_t dist_nearest;
          kd[i ^ 1].nearestSearch(p_scaled, p_nearest, dist_nearest);
          Int_t costF = (costG * _param.factorG + dist_nearest * _param.factorH) *
                        (bOverflow(pU, pV) ? _param.overflowCost : 1);
          if (_net.bSelfSym()) {
            Int_t symX_U = (Int_t)(2 * _selfSymAxisX - pU->coord().x());
            Int_t symX_V = (Int_t)(2 * _selfSymAxisX - pV->coord().x());
            boundSymX(symX_U);
            boundSymX(symX_V);
            const GrAstarNode* pU_sym = &_vvvAstarNodes[pU->coord().z()][symX_U][pU->coord().y()];
            const GrAstarNode* pV_sym = &_vvvAstarNodes[pV->coord().z()][symX_V][pV->coord().y()];
            if (bOverflow(pU_sym, pV_sym))
              costF *= _param.overflowCost;
            costF *= std::abs(pV->coord().x() - _selfSymAxisX);
          }
          if (_net.hasSymNet()) {
            Int_t symX_U = (Int_t)(2 * _symAxisX - pU->coord().x());
            Int_t symX_V = (Int_t)(2 * _symAxisX - pV->coord().x());
            boundSymX(symX_U);
            boundSymX(symX_V);
            const GrAstarNode* pU_sym = &_vvvAstarNodes[pU->coord().z()][symX_U][pU->coord().y()];
            const GrAstarNode* pV_sym = &_vvvAstarNodes[pV->coord().z()][symX_V][pV->coord().y()];
            if (bOverflow(pU_sym, pV_sym))
              costF *= _param.overflowCost;
          }
          pV->setCostG(i, costG);
          pV->setCostF(i, costF);
          pV->setBendCnt(i, bendCnt);
          if (i == 0) {
            auto it = itMap0.find(pV);
            if (it != itMap0.end())
              pq0.modify(it->second, pV);
            else
              itMap0[pV] = pq0.push(pV);
          }
          else {
            auto it = itMap1.find(pV);
            if (it != itMap1.end())
              pq1.modify(it->second, pV);
            else
              itMap1[pV] = pq1.push(pV);
          }
        }
      }
      return false;
    };
    if (__pathSearch(0))
      return true;
    if (__pathSearch(1))
      return true;
  }
  return false;
}

UInt_t GrAstar::mergeComp(const UInt_t srcIdx, const UInt_t tarIdx) {
  _compDS.merge(srcIdx, tarIdx);
  UInt_t bigCompIdx = srcIdx;
  UInt_t smallCompIdx = tarIdx;
  if (_vCompDatas[bigCompIdx].size() < _vCompDatas[smallCompIdx].size()) {
    std::swap(bigCompIdx, smallCompIdx);
  }
  DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher>& bigComp = _vCompDatas[bigCompIdx];
  DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher>& smallComp = _vCompDatas[smallCompIdx];
  if (_compDS.nSets() > 1) {
    bigComp.insert(smallComp.begin(), smallComp.end());
  }
  return bigCompIdx;
}

void GrAstar::backTrack(const GrAstarNode* pNode, const UInt_t bigCompIdx, const UInt_t srcIdx, const UInt_t tarIdx) {
  List_t<Point3d<Int_t>> lPathPts;
  add2Path(0, pNode->coord(), lPathPts);
  if (_compDS.nSets() > 1) {
    _vCompDatas[bigCompIdx].insert(pNode->coord());
  }
  auto __backTrack = [&] (const Int_t i) {
    const GrAstarNode* pPar = pNode->pParent(i);
    if (pPar == nullptr)
      return;
    if (_compDS.nSets() > 1) {
      while (pPar != nullptr) {
        add2Path(i, pPar->coord(), lPathPts);
        _vCompDatas[bigCompIdx].insert(pPar->coord());
        pPar = pPar->pParent(i);
      }
    }
    else {
      while (pPar != nullptr) {
        add2Path(i, pPar->coord(), lPathPts);
        pPar = pPar->pParent(i);
      }
    }
  };
  __backTrack(0);
  __backTrack(1);
  assert(_compDS.find(srcIdx) == _compDS.find(tarIdx));
  _vCompDatas[_compDS.find(srcIdx)] = _vCompDatas[bigCompIdx];
  _vvGuidePaths.resize(_vvGuidePaths.size() + 1);
  _vvGuidePaths.back().reserve(lPathPts.size());
  for (const Point3d<Int_t>& p : lPathPts) {
    _vvGuidePaths.back().emplace_back(p);
  }
}

void GrAstar::updateGridEdges() {
  UInt_t i, j;
  auto __updateGridEdges = [&] (const Vector_t<Vector_t<Point3d<Int_t>>>& vvGuides, const UInt_t netIdx) {
    for (i = 0; i < vvGuides.size(); ++i) {
      for (j = 0; j + 1 < vvGuides[i].size(); ++j) {
        const Point3d<Int_t>& u = vvGuides[i][j];
        const Point3d<Int_t>& v = vvGuides[i][j + 1];
        if (u == v) {
          continue;
        }
        switch (findDir(u, v)) {
          case PathDir::LEFT:
          case PathDir::RIGHT:
            {
              assert(u.z() == v.z() and u.y() == v.y());
              _grGridRoute._gridMap.gridEdge(0, u.z(), std::min(u.x(), v.x()), u.y()).addNetIdx(netIdx, _netWeight);
              break;
            }
          case PathDir::UP:
          case PathDir::DOWN:
            {
              assert(u.z() == v.z() and u.x() == v.x());
              _grGridRoute._gridMap.gridEdge(1, u.z(), u.x(), std::min(u.y(), v.y())).addNetIdx(netIdx, _netWeight);
              break;
            }
          case PathDir::VIA_UP:
          case PathDir::VIA_DOWN:
            {
              assert(u.x() == v.x() and u.y() == v.y());
              _grGridRoute._gridMap.gridEdge(2, std::min(u.z(), v.z()), u.x(), u.y()).addNetIdx(netIdx, _netWeight);
              break;
            }
          default:
            assert(false);
        }
      }
    }
  };
  __updateGridEdges(_vvGuidePaths, _net.idx());
  if (_net.hasSymNet())
    __updateGridEdges(_vvSymGuidePaths, _net.symNetIdx());
}

void GrAstar::saveGuide2Net() {
  Vector_t<Pair_t<Box<Int_t>, Int_t>> vGuides;
  auto __saveGuides = [&] (const Vector_t<Vector_t<Point3d<Int_t>>>& vvGuidePaths, Vector_t<Pair_t<Box<Int_t>, Int_t>>& vGuides) {
    UInt_t numPts = 0;
    for (const Vector_t<Point3d<Int_t>>& v : vvGuidePaths)
      numPts += v.size();
    vGuides.reserve(numPts);
    for (const Vector_t<Point3d<Int_t>>& v : vvGuidePaths) {
      for (const Point3d<Int_t>& p : v) {
        const GrGridCell& gridCell = _grGridRoute._gridMap.gridCell(p.z(), p.x(), p.y());
        vGuides.emplace_back(gridCell.box(), _cir.lef().routingLayerIdx2LayerIdx(p.z()));
      }
    }
  };
  __saveGuides(_vvGuidePaths, vGuides);
  _net.setGuides(vGuides);
  if (_net.hasSymNet()) {
    Vector_t<Pair_t<Box<Int_t>, Int_t>> vSymGuides;
    __saveGuides(_vvSymGuidePaths, vSymGuides);
    _cir.net(_net.symNetIdx()).setGuides(vSymGuides);
  }
}

// for symnet
void GrAstar::genSymNetGuidesFromGuides() {
  assert(_net.hasSymNet());
  for (const Vector_t<Point3d<Int_t>>& v : _vvGuidePaths) {
    _vvSymGuidePaths.emplace_back();
    for (const Point3d<Int_t>& p : v) {
      Int_t symX = (Int_t)(2 * _symAxisX - p.x());
      boundSymX(symX);
      _vvSymGuidePaths.back().emplace_back(symX, p.y(), p.z());
    }
  }

}

// for debug
bool GrAstar::bConnected() {
  auto __bConnected = [&] (const Vector_t<Point3d<Int_t>>& vPinLocs, const Vector_t<Vector_t<Point3d<Int_t>>>& vvGuidePaths) -> bool {
    DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher> pts;
    pts.set_empty_key(Point3d<Int_t>(MIN_INT, MIN_INT, MIN_INT));
    pts.set_deleted_key(Point3d<Int_t>(MAX_INT, MAX_INT, MAX_INT));
    for (const auto& p : vPinLocs)
      pts.insert(p);
    for (const auto& v : vvGuidePaths)
      for (const auto& p : v)
        pts.insert(p);
    DisjointSet ds;
    ds.init(pts.size());
    Vector_t<Point3d<Int_t>> vPts;
    vPts.reserve(pts.size());
    for (const auto& p : pts)
      vPts.emplace_back(p);
    UInt_t i, j;
    for (i = 0; i < vPts.size(); ++i) {
      for (j = i + 1; j < vPts.size(); ++j) {
        if (bNeighbor(vPts[i], vPts[j]))
          ds.merge(i, j);
      }
    }
    return ds.nSets() == 1;
  };
  //if (_net.hasSymNet()) {
    //if (!__bConnected(_grGridRoute._vvNetPinLocs[_net.symNetIdx()], _vvSymGuidePaths)) {
      //return false;
    //}
  //}
  return __bConnected(_vPinLocs, _vvGuidePaths);
}

/////////////////////////////////////////
//    Helper functions                 //
/////////////////////////////////////////
bool GrAstar::bNeighbor(const Point3d<Int_t>& p1, const Point3d<Int_t>& p2) {
  return Point3d<Int_t>::Mdistance(p1, p2) == 1;
}

void GrAstar::neighbors(const GrAstarNode* pNode, Vector_t<GrAstarNode*>& vpNeighbors) {
  const Point3d<Int_t>& p = pNode->coord();
  // lower layer
  if (p.z() > 0)
    vpNeighbors.emplace_back(&_vvvAstarNodes[p.z() - 1][p.x()][p.y()]);
  // upper layer
  if (p.z() < (Int_t)_grGridRoute._gridMap.numGridCellsZ() - 1)
    vpNeighbors.emplace_back(&_vvvAstarNodes[p.z() + 1][p.x()][p.y()]);
  
  if (p.x() > 0)
    vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x() - 1][p.y()]);
  if (p.x() < (Int_t)_grGridRoute._gridMap.numGridCellsX() - 1)
    vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x() + 1][p.y()]);
  if (p.y() > 0)
    vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x()][p.y() - 1]);
  if (p.y() < (Int_t)_grGridRoute._gridMap.numGridCellsY() - 1)
    vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x()][p.y() + 1]);

  //switch (_cir.lef().routingLayer(p.z()).routeDir()) {
    //case LefRoutingLayer::RouteDir::HORIZONTAL:
      //if (p.x() > 0)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x() - 1][p.y()]);
      //if (p.x() < (Int_t)_grGridRoute._gridMap.numGridCellsX() - 1)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x() + 1][p.y()]);
      //break;
    //case LefRoutingLayer::RouteDir::VERTICAL:
      //if (p.y() > 0)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x()][p.y() - 1]);
      //if (p.y() < (Int_t)_grGridRoute._gridMap.numGridCellsY() - 1)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x()][p.y() + 1]);
      //break;
    //case LefRoutingLayer::RouteDir::BOTH:
      //if (p.x() > 0)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x() - 1][p.y()]);
      //if (p.x() < (Int_t)_grGridRoute._gridMap.numGridCellsX() - 1)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x() + 1][p.y()]);
      //if (p.y() > 0)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x()][p.y() - 1]);
      //if (p.y() < (Int_t)_grGridRoute._gridMap.numGridCellsY() - 1)
        //vpNeighbors.emplace_back(&_vvvAstarNodes[p.z()][p.x()][p.y() + 1]);
      //break;
    //default:
      //assert(false);
  //}
}

Int_t GrAstar::scaledMDist(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  Int_t cost = 0;
  cost += abs(u.x() - v.x()) * _param.horCost;
  cost += abs(u.y() - v.y()) * _param.verCost;
  cost += abs(u.z() - v.z()) * _param.viaCost;
  return cost;
}

bool GrAstar::hasBend(const GrAstarNode* pU, const GrAstarNode* pV, const Int_t i) {
  if (pU->pParent(i) != nullptr) {
    if (pU->coord() == pV->coord()) {
      return false;
    }
    else {
      return findDir(pU->pParent(i)->coord(), pU->coord()) != findDir(pU->coord(), pV->coord());
    }
  }
  return false;
}

GrAstar::PathDir GrAstar::findDir(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  if (u.z() == v.z()) {
    if (u.x() == v.x()) {
      assert(u.y() != v.y());
      return u.y() < v.y() ? PathDir::UP : PathDir::DOWN;
    }
    else {
      assert(u.x() != v.x());
      return u.x() < v.x() ? PathDir::RIGHT : PathDir::LEFT;
    }
  }
  else {
    assert(u.x() == v.x() and u.y() == v.y());
    return u.z() < v.z() ? PathDir::VIA_UP : PathDir::VIA_DOWN;
  }
}

bool GrAstar::bNeedUpdate(const GrAstarNode* pV, const Int_t i, const Int_t costG, const Int_t bendCnt) {
  if (pV->costG(i) > costG)
    return true;
  else if (pV->bendCnt(i) > bendCnt)
    return true;
  return false;
}

void GrAstar::add2Path(const Int_t i, const Point3d<Int_t>& u, List_t<Point3d<Int_t>>& lPathPts) {
  if (i == 0)
    lPathPts.emplace_front(u);
  else
    lPathPts.emplace_back(u);
}

bool GrAstar::bOverflow(const GrAstarNode* pU, const GrAstarNode* pV) {
  if (pU->coord() == pV->coord())
    return false;
  assert(bNeighbor(pU->coord(), pV->coord()));
  const Point3d<Int_t>& u = pU->coord();
  const Point3d<Int_t>& v = pV->coord();
  switch (findDir(u, v)) {
    case PathDir::LEFT:
    case PathDir::RIGHT:
      {
        assert(u.z() == v.z() and u.y() == v.y());
        return _grGridRoute._gridMap.gridEdge(0, u.z(), std::min(u.x(), v.x()), u.y()).bOverflow();
      }
    case PathDir::UP:
    case PathDir::DOWN:
      {
        assert(u.z() == v.z() and u.x() == v.x());
        return _grGridRoute._gridMap.gridEdge(1, u.z(), u.x(), std::min(u.y(), v.y())).bOverflow();
      }
    case PathDir::VIA_UP:
    case PathDir::VIA_DOWN:
      {
        assert(u.x() == v.x() and u.y() == v.y());
        return _grGridRoute._gridMap.gridEdge(2, std::min(u.z(), v.z()), u.x(), u.y()).bOverflow();
      }
    default:
      assert(false);
      return false;
  }
}

void GrAstar::boundSymX(Int_t& symX) {
  if (symX >= (Int_t)_grGridRoute._gridMap.numGridCellsX())
    symX = _grGridRoute._gridMap.numGridCellsX() - 1;
  else if (symX < 0)
    symX = 0;
}


PROJECT_NAMESPACE_END
