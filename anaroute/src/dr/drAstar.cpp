/**
 * @file   drAstar.cpp
 * @brief  Detailed Routing - A* search kernel
 * @author Hao Chen
 * @date   10/25/2019
 *
 **/
using namespace std;
#include "drAstar.hpp"
#include "drViaSelector.hpp"
#include "src/ds/pqueue.hpp"
#include "src/writer/wrGds.hpp"

#include <lemon/maps.h>
#include <lemon/kruskal.h>
#include <lemon/list_graph.h>

PROJECT_NAMESPACE_START

bool DrAstar::runKernel() {
  fprintf(stdout, "DrAstar::%s Route net \"%s\"\n", __func__, _net.name().c_str());
  
  if (_net.bRouted())
    return true;
  
  _vAllAstarNodesMap.resize(_cir.lef().numLayers());
  for (auto& m : _vAllAstarNodesMap)
    m.set_empty_key(Point<Int_t>(-1, -1));
  
  constructAllLayerBoxes();
  init();
  splitSubNetMST();
  
  if (_net.hasSymNet() and
      _net.drFailCnt() < (Int_t)_param.maxSymTry and
      _cir.net(_net.symNetIdx()).drFailCnt() < (Int_t)_param.maxSymTry) {
    computeSymAxisX();
    if (!bSatisfySymCondition()) {
      fprintf(stderr, "DrAstar::%s WARNING: Net %s %s does not satisfy symmetric net requirements!\n", __func__, _net.name().c_str(), _cir.net(_net.symNetIdx()).name().c_str());
    }
    else {
      _bSatisfySym = true;
    }
  }
  else if (_net.bSelfSym()) {
    computeSelfSymAxisX();
    if (!bSatisfySelfSymCondition()) {
      fprintf(stderr, "DrAstar::%s WARNING: Net %s does not satisfy self symmetric requirements!\n", __func__, _net.name().c_str());
    }
    else {
      _bSatisfySelfSym = true;
    }
  }


  for (const Pair_t<UInt_t, UInt_t>& pair : _vCompPairs) {
    if (!routeSubNet(pair.first, pair.second)) {
      fprintf(stderr, "DrAstar::%s ERROR: Route net %s failed!\n", __func__, _net.name().c_str());
      ripup();
      return false;
    }
  }
  saveResult2Net();
  _net.setRouted();
  if (_bSatisfySym) {
    saveResult2SymNet();
    _cir.net(_net.symNetIdx()).setRouted();
  }
  //visualize();
  return true;
}

void DrAstar::init() {
  _compDS.init(_net.numPins());
  _vCompBoxes.resize(_net.numPins());
  _vCompAcsPts.resize(_net.numPins());
  _vCompSpatialBoxes.resize(_net.numPins());
  //for (auto& v : _vCompSpatialBoxes)
    //v.resize(_cir.lef().numLayers());

  UInt_t i, j, pinIdx, layerIdx;
  const Box<Int_t>* cpBox;
  Net_ForEachPinIdx(_net, pinIdx, i) {
    _vCompAcsPts[i].set_empty_key(Point3d<Int_t>(MIN_INT, MIN_INT, MIN_INT));
    _vCompAcsPts[i].set_deleted_key(Point3d<Int_t>(MAX_INT, MAX_INT, MAX_INT));
    const Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      //const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(layerIdx);
      //assert(layerPair.first == LefLayerType::ROUTING);
      //const LefRoutingLayer& layer = _cir.lef().routingLayer(layerPair.second);
      //const Int_t minWidth = layer.minWidth();
      //const Int_t halfWidth = minWidth / 2;
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
        _vCompBoxes[i].emplace_back((*cpBox), layerIdx);
        _vCompSpatialBoxes[i][layerIdx].insert(*cpBox);
        addAcsPts(i, layerIdx, *cpBox);
        //const Int_t minX = cpBox->min_corner().x();  
        //const Int_t minY = cpBox->min_corner().y();
        //const Int_t maxX = cpBox->max_corner().x();
        //const Int_t maxY = cpBox->max_corner().y();
        // make peripheral acs points
        //for (Int_t x = minX + halfWidth, y = minY; x + halfWidth <= maxX; x += minWidth) {
          //Point3d<Int_t> acsPt(x, y, layerIdx);
          //_vCompAcsPts[i].insert(acsPt);
        //}
        //for (Int_t x = maxX, y = minY + halfWidth; y + halfWidth <= maxY; y += minWidth) {
          //Point3d<Int_t> acsPt(x, y, layerIdx);
          //_vCompAcsPts[i].insert(acsPt);
        //}
        //for (Int_t x = maxX - halfWidth, y = maxY; x - halfWidth >= minX; x -= minWidth) {
          //Point3d<Int_t> acsPt(x, y, layerIdx);
          //_vCompAcsPts[i].insert(acsPt);
        //}
        //for (Int_t x = minX, y = maxY - halfWidth; y - halfWidth >= minY; y -= minWidth) {
          //Point3d<Int_t> acsPt(x, y, layerIdx);
          //_vCompAcsPts[i].insert(acsPt);
        //}
      }
    }
  }

}

void DrAstar::addAcsPts(const UInt_t idx, const Int_t z, const Box<Int_t>& box) {
  const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(z);
  assert(layerPair.first == LefLayerType::ROUTING);
  const LefRoutingLayer& layer = _cir.lef().routingLayer(layerPair.second);
  const Int_t minWidth = layer.minWidth();
  const Int_t halfWidth = minWidth / 2;
  const Int_t minX = box.min_corner().x();  
  const Int_t minY = box.min_corner().y();
  const Int_t maxX = box.max_corner().x();
  const Int_t maxY = box.max_corner().y();
  for (Int_t x = minX + halfWidth, y = minY; x + halfWidth <= maxX; x += minWidth) {
    Point3d<Int_t> acsPt(x, y, z);
    _vCompAcsPts[idx].insert(acsPt);
  }
  for (Int_t x = maxX, y = minY + halfWidth; y + halfWidth <= maxY; y += minWidth) {
    Point3d<Int_t> acsPt(x, y, z);
    _vCompAcsPts[idx].insert(acsPt);
  }
  for (Int_t x = maxX - halfWidth, y = maxY; x - halfWidth >= minX; x -= minWidth) {
    Point3d<Int_t> acsPt(x, y, z);
    _vCompAcsPts[idx].insert(acsPt);
  }
  for (Int_t x = minX, y = maxY - halfWidth; y - halfWidth >= minY; y -= minWidth) {
    Point3d<Int_t> acsPt(x, y, z);
    _vCompAcsPts[idx].insert(acsPt);
  }
}

void DrAstar::splitSubNetMST() {
  lemon::ListGraph graph;
  Vector_t<lemon::ListGraph::Node> vGraphNodes;
  Vector_t<lemon::ListGraph::Edge> vGraphEdges;
  lemon::ListGraph::EdgeMap<Int_t> edgeCostMap(graph);

  Vector_t<Int_t> vEdgeCostValues;
  UInt_t i, j;
  for (i = 0; i < _vCompBoxes.size(); ++i) {
    vGraphNodes.emplace_back(graph.addNode());
  }
  for (i = 0; i < _vCompBoxes.size(); ++i) {
    for (j = i + 1; j < _vCompBoxes.size(); ++j) {
      vGraphEdges.emplace_back(graph.addEdge(vGraphNodes[i], vGraphNodes[j]));
      Int_t minDist = MAX_INT;
      for (const Pair_t<Box<Int_t>, Int_t>& u : _vCompBoxes[i]) {
        for (const Pair_t<Box<Int_t>, Int_t>& v : _vCompBoxes[j]) {
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
  assert(vResEdges.size() == _vCompBoxes.size() - 1);
  for (i = 0; i < vResEdges.size(); ++i) {
    lemon::ListGraph::Edge& edge = vResEdges[i];
    _vCompPairs.emplace_back(graph.id(graph.u(edge)), graph.id(graph.v(edge)));
  }
}

void DrAstar::computeSelfSymAxisX() {
  UInt_t i, j, pinIdx, layerIdx;
  const Box<Int_t>* cpBox;
  Int_t numBoxes = 0;
  Net_ForEachPinIdx(_net, pinIdx, i) {
    const Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      numBoxes += pin.numBoxes(layerIdx);
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
        _selfSymAxisX += cpBox->center().x();
      }
    }
  }
  _selfSymAxisX /= numBoxes;
}

void DrAstar::computeSymAxisX() {
  Net& symNet = _cir.net(_net.symNetIdx());
  UInt_t i, j, pinIdx, layerIdx;
  const Box<Int_t>* cpBox;
  Int_t numBoxes = 0;
  Net_ForEachPinIdx(_net, pinIdx, i) {
    const Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      numBoxes += pin.numBoxes(layerIdx);
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
        _symAxisX += cpBox->center().x();
      }
    }
  }
  Net_ForEachPinIdx(symNet, pinIdx, i) {
    const Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      numBoxes += pin.numBoxes(layerIdx);
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
        _symAxisX += cpBox->center().x();
      }
    }
  }
  _symAxisX /= numBoxes;
}

void DrAstar::constructAllLayerBoxes() {
  _vAllLayerBoxes.resize(_cir.lef().numLayers());
  UInt_t i, j, pinIdx, layerIdx;
  const Box<Int_t>* cpBox;
  Net_ForEachPinIdx(_net, pinIdx, i) {
    const Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
        _vAllLayerBoxes[layerIdx].emplace_back(*cpBox);
      }
    }
  }
  if (_net.hasSymNet()) {
    const Net& symNet = _cir.net(_net.symNetIdx());
    Net_ForEachPinIdx(symNet, pinIdx, i) {
      const Pin& pin = _cir.pin(pinIdx);
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
          _vAllLayerBoxes[layerIdx].emplace_back(*cpBox);
        }
      }
    }
  }
  for (Vector_t<Box<Int_t>>& v : _vAllLayerBoxes) {
    std::sort(v.begin(), v.end());
  }
}

bool DrAstar::bSatisfySelfSymCondition() const {
  for (const Vector_t<Box<Int_t>>& v : _vAllLayerBoxes) {
    for (const Box<Int_t>& box : v) {
      Box<Int_t> symBox;
      const Int_t symXL = 2 * _selfSymAxisX - box.xh();
      const Int_t symXH = 2 * _selfSymAxisX - box.xl();
      symBox.setXL(symXL);
      symBox.setYL(box.yl());
      symBox.setXH(symXH);
      symBox.setYH(box.yh());
      if (!std::binary_search(v.begin(), v.end(), symBox)) {
        return false;
      }
    }
  }
  return true;
}

bool DrAstar::bSatisfySymCondition() const {
  const bool bLessThanSymAxis = _vCompBoxes[0][0].first.centerX() < _symAxisX;
  for (const auto& vBoxes : _vCompBoxes) {
    for (const auto& pair : vBoxes) {
      const bool bLess = pair.first.centerX() < _symAxisX;
      if (bLess != bLessThanSymAxis)
        return false;
    }
  }
  for (const Vector_t<Box<Int_t>>& v : _vAllLayerBoxes) {
    for (const Box<Int_t>& box : v) {
      Box<Int_t> symBox;
      const Int_t symXL = 2 * _symAxisX - box.xh();
      const Int_t symXH = 2 * _symAxisX - box.xl();
      symBox.setXL(symXL);
      symBox.setYL(box.yl());
      symBox.setXH(symXH);
      symBox.setYH(box.yh());
      if (!std::binary_search(v.begin(), v.end(), symBox)) {
        return false;
      }
    }
  }
  //if (_net.name() == "INTERN") {
    //std::cerr << std::endl;;
    //Net& net = _cir.net("INTERP");
    //UInt_t i, pinIdx, layerIdx, j;
    //Box<Int_t>* pBox;
    //Net_ForEachPinIdx(net, pinIdx, i) {
      //Pin& pin = _cir.pin(pinIdx);
      //Pin_ForEachLayerIdx(pin, layerIdx) {
        //Pin_ForEachLayerBox(pin, layerIdx, pBox, j) {
          //std::cerr << *pBox << std::endl;
        //}
      //}
    //}
  //}
  return true;
}

bool DrAstar::routeSubNet(UInt_t srcIdx, UInt_t tarIdx) {
  assert(_compDS.find(srcIdx) != _compDS.find(tarIdx));

  srcIdx = _compDS.find(srcIdx);
  tarIdx = _compDS.find(tarIdx);
  // init source and target
  DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher>& src = _vCompAcsPts[srcIdx];
  DenseHashSet<Point3d<Int_t>, Point3d<Int_t>::hasher>& tar = _vCompAcsPts[tarIdx];


  // init priority queue
  typedef PairingHeap<DrAstarNode*, DrAstarNodeCmp0> PQueue_t;
  PQueue_t pq;
  typedef DenseHashMap<DrAstarNode*, PQueue_t::point_iterator> Itermap_t;
  Itermap_t itMap;
  itMap.set_empty_key(nullptr);

  // reset cost
  resetAstarNodes();

  // add new nodes
  for (const Point3d<Int_t>& p : src) {
    Point<Int_t> p2d(p.x(), p.y());
    if (_vAllAstarNodesMap[p.z()].find(p2d) == _vAllAstarNodesMap[p.z()].end())
      _vAllAstarNodesMap[p.z()][p2d] = new DrAstarNode(p);
  }
  for (const Point3d<Int_t>& p : tar) {
    Point<Int_t> p2d(p.x(), p.y());
    if (_vAllAstarNodesMap[p.z()].find(p2d) == _vAllAstarNodesMap[p.z()].end())
      _vAllAstarNodesMap[p.z()][p2d] = new DrAstarNode(p);
  }

  // add src to pq
  for (const Point3d<Int_t>& p : src) {
    Point<Int_t> p2d(p.x(), p.y());
    Int_t layerIdx = p.z();
    Int_t dist_nearest_scaled, dist_nearest_unscaled;
    nearestTarBoxDist(p, tarIdx, dist_nearest_scaled, dist_nearest_unscaled);
    assert(_vAllAstarNodesMap[layerIdx].find(p2d) != _vAllAstarNodesMap[layerIdx].end());
    DrAstarNode* pNode = _vAllAstarNodesMap[layerIdx][p2d];
    Int_t costF = _param.factorH * dist_nearest_scaled;
    //if (_net.bSelfSym()) {
      //costF += abs(p.x() - _selfSymAxisX) * _param.horCost;
    //}
    if (bInsideGuide(pNode))
      costF += _param.guideCost;
    pNode->setCostF(0, costF);
    pNode->setCostG(0, 0);
    pNode->setBendCnt(0, 0);
    pNode->setDist2Tar(0, dist_nearest_unscaled);
    itMap[pNode] = pq.push(pNode);
  }
  // path search
  while (!pq.empty()) {
    auto __pathSearch = [&] (const UInt_t i) -> bool {
      DrAstarNode* pU = pq.top();
      const Point3d<Int_t>& u = pU->coord();
      const Point<Int_t> u2d(u.x(), u.y());
      if (tar.find(u) != tar.end()
          or bConnected2TarBox(pU, tarIdx)
          //or bConnected2TarRoutedWire(pU, tarIdx)
         ) {
        const UInt_t bigCompIdx = mergeComp(srcIdx, tarIdx);
        backTrack(pU, bigCompIdx, srcIdx, tarIdx);
        return true;
      }
      pq.pop();
      pU->setExplored(i, true);
      if (pU->vpNeighbors().empty()) {
        Vector_t<DrAstarNode*> vpNeighbors;
        neighbors(pU, vpNeighbors);
        pU->setNeighbors(vpNeighbors);
      }
      for (DrAstarNode* pV : pU->vpNeighbors()) {
        if (pV->bExplored(i))
          continue;
        const Point3d<Int_t>& v = pV->coord();
        if (u.z() != v.z()) {
          //UInt_t viaIdx = selectVia(pU, pV);
          UInt_t viaIdx = _bSatisfySym ? selectViaSym(pU, pV) : selectVia(pU, pV);
          if (viaIdx == MAX_INT)
            continue;
        }
        if (bViolateDRC(pU, pV))
          continue;
        if (_bSatisfySym) {
          if (bViolateSymDRC(pU, pV))
            continue;
        }
        Int_t costG = pU->costG(i) + scaledMDist(u, v);
        Int_t bendCnt = pU->bendCnt(i) + hasBend(pU, pV, i);
        if (bNeedUpdate(pV, i, costG, bendCnt)) {
          pV->setParent(i, pU);
          Int_t dist_nearest_scaled, dist_nearest_unscaled;
          nearestTarBoxDist(v, tarIdx, dist_nearest_scaled, dist_nearest_unscaled);
          Int_t costF = (costG * _param.factorG + dist_nearest_scaled * _param.factorH);
          if (bInsideGuide(pV))
            costF += _param.guideCost;
          //***************************
          //  Sym and Self Sym
          //  TODO
          //***************************a
          pV->setDist2Tar(i, dist_nearest_unscaled);
          pV->setCostG(i, costG);
          pV->setCostF(i, costF);
          pV->setBendCnt(i, bendCnt);
          auto it = itMap.find(pV);
          if (it != itMap.end())
            pq.modify(it->second, pV);
          else
            itMap[pV] = pq.push(pV);
        }
      }
      return false;
    };
    if (pq.size() > _param.maxExplore) {
      return false;
    }
    else if (__pathSearch(0))
      return true;
  }
  return false;
}

void DrAstar::resetAstarNodes() {
  for (auto& m : _vAllAstarNodesMap) {
    for (auto& p : m) {
      DrAstarNode* pNode = p.second;
      pNode->reset();
    }
  }
}

UInt_t DrAstar::mergeComp(const UInt_t srcIdx, const UInt_t tarIdx) {
  _compDS.merge(srcIdx, tarIdx);
  UInt_t bigCompIdx = srcIdx;
  UInt_t smallCompIdx = tarIdx;
  if (_vCompAcsPts[bigCompIdx].size() < _vCompAcsPts[smallCompIdx].size()) {
    std::swap(bigCompIdx, smallCompIdx);
  }
  auto& bigComp = _vCompAcsPts[bigCompIdx];
  auto& smallComp = _vCompAcsPts[smallCompIdx];
  if (_compDS.nSets() > 1) {
    bigComp.insert(smallComp.begin(), smallComp.end());
  }
  return bigCompIdx;
}

void DrAstar::backTrack(const DrAstarNode* pNode, const UInt_t bigCompIdx, const UInt_t srcIdx, const UInt_t tarIdx) {
  List_t<Point3d<Int_t>> lPathPts;
  add2Path(0, pNode->coord(), lPathPts);
  auto __backTrack = [&] (const Int_t i) {
    const DrAstarNode* pPar = pNode->pParent(i);
    if (pPar == nullptr)
      return;
    while (pPar != nullptr) {
      add2Path(i, pPar->coord(), lPathPts);
      pPar = pPar->pParent(i);
    }
  };
  __backTrack(0);
  assert(_compDS.find(srcIdx) == _compDS.find(tarIdx));
  const UInt_t rootIdx = _compDS.find(srcIdx);
  const UInt_t childIdx = (rootIdx == srcIdx) ? tarIdx : srcIdx;
  _vCompAcsPts[rootIdx] = _vCompAcsPts[bigCompIdx];
  List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>> lPathVec;
  mergePath(lPathPts, lPathVec);
  // save path
  savePath(lPathVec);
  // add to comp box
  for (const auto& pair : _vCompBoxes[childIdx]) {
    _vCompBoxes[rootIdx].emplace_back(pair);
    _vCompSpatialBoxes[rootIdx][pair.second].insert(pair.first);
  }
  const auto& vRoutePath = _vvRoutePaths.back();
  for (const auto& vec : vRoutePath) {
    const Point3d<Int_t>& u = vec.first;
    const Point3d<Int_t>& v = vec.second;
    if (u.z() == v.z()) {
      Box<Int_t> wire;
      toWire(u, v, wire);
      addAcsPts(rootIdx, u.z(), wire);
      _vCompBoxes[rootIdx].emplace_back(wire, u.z());
      _vCompSpatialBoxes[rootIdx][u.z()].insert(wire);
    }
  }

}

void DrAstar::savePath(const List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>>& lPathVec) {
  _vvRoutePaths.resize(_vvRoutePaths.size() + 1);
  _vvRoutePaths.back().reserve(lPathVec.size());
  _vvRouteViaIndices.resize(_vvRouteViaIndices.size() + 1);
  _vvRouteViaIndices.back().reserve(lPathVec.size());
  _vvRoutedWires.resize(_vvRoutedWires.size() + 1);
  _vvRoutedWires.back().reserve(lPathVec.size());
  for (const auto& pair : lPathVec) {
    _vvRoutePaths.back().emplace_back(pair);
  }
  // add exact shapes to spatial
  const auto& vPathVec = _vvRoutePaths.back();
  auto& vPathViaIndices = _vvRouteViaIndices.back();
  auto& vRoutedWires = _vvRoutedWires.back();
  for (const auto& pair : vPathVec) {
    const Point3d<Int_t>& u = pair.first;
    const Point3d<Int_t>& v = pair.second;
    if (u.z() == v.z()) {
      vPathViaIndices.emplace_back(MAX_INT);
      addWire2RoutedSpatial(_net.idx(), u, v);
      Box<Int_t> wire;
      toWire(u, v, wire);
      vRoutedWires.emplace_back(wire, u.z());
      /////////////////////////////////
      // Sym                         //
      /////////////////////////////////
      if (_bSatisfySym) {
        const Point3d<Int_t> symU(2 * _symAxisX - u.x(), u.y(), u.z());
        const Point3d<Int_t> symV(2 * _symAxisX - v.x(), v.y(), v.z());
        Box<Int_t> symWire(wire);
        symWire.flipX(_symAxisX);
        addWire2RoutedSpatial(_net.symNetIdx(), symU, symV);
      }
    }
    else {
      //UInt_t viaIdx = selectVia(u, v);
      UInt_t viaIdx = _bSatisfySym ? selectViaSym(u, v) : selectVia(u, v);
      assert(viaIdx != MAX_INT);
      vPathViaIndices.emplace_back(viaIdx);
      addVia2RoutedSpatial(_net.idx(), u, v, viaIdx);
      Vector_t<Pair_t<Box<Int_t>, Int_t>> vLayerBoxes;
      toVia(u, v, viaIdx,  vLayerBoxes);
      for (const auto& pair : vLayerBoxes) {
        vRoutedWires.emplace_back(pair);
      }
      /////////////////////////////////
      // Sym                         //
      /////////////////////////////////
      if (_bSatisfySym) {
        const Point3d<Int_t> symU(2 * _symAxisX - u.x(), u.y(), u.z());
        const Point3d<Int_t> symV(2 * _symAxisX - v.x(), v.y(), v.z());
        addVia2RoutedSpatial(_net.symNetIdx(), symU, symV, viaIdx);
      }
    }
  }
}

void DrAstar::saveResult2Net() {
  for (const auto& vRoutedWires : _vvRoutedWires) {
    for (const auto& pair : vRoutedWires) {
      _net.vWires().emplace_back(pair);
    }
  }
}

void DrAstar::saveResult2SymNet() {
  Net& symNet = _cir.net(_net.symNetIdx());
  for (const auto& vRoutedWires : _vvRoutedWires) {
    for (const auto& pair : vRoutedWires) {
      const Box<Int_t>& wire = pair.first;
      const Int_t layerIdx = pair.second;
      Box<Int_t> symWire(wire);
      symWire.flipX(_symAxisX);
      symNet.vWires().emplace_back(symWire, layerIdx);
    }
  }
}

void DrAstar::ripup() {
  for (const auto& vRoutedWires : _vvRoutedWires) {
    for (const auto& pair : vRoutedWires) {
      const Box<Int_t>& wire = pair.first;
      const Int_t layerIdx = pair.second;
      _cir.removeSpatialRoutedWire(_net.idx(), layerIdx, wire);
    }
  }
}

/////////////////////////////////////////
//    Helper functions                 //
/////////////////////////////////////////
void DrAstar::neighbors(const DrAstarNode* pU, Vector_t<DrAstarNode*>& vpNeighbors) {
  const Point3d<Int_t>& p = pU->coord();
  assert(p != Point3d<Int_t>(0,0,0));
  const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(p.z());
  assert(_cir.lef().bRoutingLayer(p.z()));
  const LefRoutingLayer& layer = _cir.lef().routingLayer(layerPair.second);
  const Int_t minSpacing = layer.spacingTable().table.size() ?
                           layer.spacingTable().table[0].second[0] :
                           layer.spacing(0);
  const Int_t minWidth = layer.minWidth();
  const Int_t minBorderDist = minSpacing + minWidth;
  Int_t step = minSpacing;
 
  if (_net.drFailCnt() == 0 and pU->dist2Tar(0) > 12 * minSpacing)
    step *= 6;
  else if (_net.drFailCnt() <= 1 and pU->dist2Tar(0) > 8 * minSpacing)
    step *= 4;
  else if (_net.drFailCnt() <= 2 and pU->dist2Tar(0) > 4 * minSpacing)
    step *= 2;
  if (pU->dist2Tar(0) < minSpacing)
    step = 10;
  
  if (p.z() > 0) {
    Point<Int_t> p2d(p.x(), p.y());
    Int_t layerIdx = p.z() - 2;
    if (layerIdx >= 0 and _cir.lef().bRoutingLayer(layerIdx)) {
      if (_vAllAstarNodesMap[layerIdx].find(p2d) == _vAllAstarNodesMap[layerIdx].end()) {
        _vAllAstarNodesMap[layerIdx][p2d] = new DrAstarNode(Point3d<Int_t>(p.x(), p.y(), layerIdx));
      }
      vpNeighbors.emplace_back(_vAllAstarNodesMap[layerIdx][p2d]);
    }
  }
  if (p.z() < (Int_t)_vAllAstarNodesMap.size() - 2) {
    Point<Int_t> p2d(p.x(), p.y());
    Int_t layerIdx = p.z() + 2;
    if (_cir.lef().bRoutingLayer(layerIdx)) {
      if (_vAllAstarNodesMap[layerIdx].find(p2d) == _vAllAstarNodesMap[layerIdx].end()) {
        _vAllAstarNodesMap[layerIdx][p2d] = new DrAstarNode(Point3d<Int_t>(p.x(), p.y(), layerIdx));
      }
      vpNeighbors.emplace_back(_vAllAstarNodesMap[layerIdx][p2d]);
    }
  }

  //if (layer.routeDir() == LefRoutingLayer::RouteDir::HORIZONTAL) {
    if (p.x() - minBorderDist > _cir.xl()) {
      Int_t newX = p.x() - step;
      Point<Int_t> p_neighbor(newX, p.y());
      Int_t layerIdx = p.z();
      if (_vAllAstarNodesMap[layerIdx].find(p_neighbor) == _vAllAstarNodesMap[layerIdx].end()) {
        _vAllAstarNodesMap[layerIdx][p_neighbor] = new DrAstarNode(Point3d<Int_t>(newX, p.y(), p.z()));
      }
      vpNeighbors.emplace_back(_vAllAstarNodesMap[layerIdx][p_neighbor]);
    }
    if (p.x() + minBorderDist < _cir.xh()) {
      Int_t newX = p.x() + step;
      Point<Int_t> p_neighbor(newX, p.y());
      Int_t layerIdx = p.z();
      if (_vAllAstarNodesMap[layerIdx].find(p_neighbor) == _vAllAstarNodesMap[layerIdx].end()) {
        _vAllAstarNodesMap[layerIdx][p_neighbor] = new DrAstarNode(Point3d<Int_t>(newX, p.y(), p.z()));
      }
      vpNeighbors.emplace_back(_vAllAstarNodesMap[layerIdx][p_neighbor]);
    }
  //}
  //else if (layer.routeDir() == LefRoutingLayer::RouteDir::VERTICAL) {
    if (p.y() - minBorderDist > _cir.yl()) {
      Int_t newY = p.y() - step;
      Point<Int_t> p_neighbor(p.x(), newY);
      Int_t layerIdx = p.z();
      if (_vAllAstarNodesMap[layerIdx].find(p_neighbor) == _vAllAstarNodesMap[layerIdx].end()) {
        _vAllAstarNodesMap[layerIdx][p_neighbor] = new DrAstarNode(Point3d<Int_t>(p.x(), newY, p.z()));
      }
      vpNeighbors.emplace_back(_vAllAstarNodesMap[layerIdx][p_neighbor]);
    }
    if (p.y() + minBorderDist < _cir.yh()) {
      Int_t newY = p.y() + step;
      Point<Int_t> p_neighbor(p.x(), newY);
      Int_t layerIdx = p.z();
      if (_vAllAstarNodesMap[layerIdx].find(p_neighbor) == _vAllAstarNodesMap[layerIdx].end()) {
        _vAllAstarNodesMap[layerIdx][p_neighbor] = new DrAstarNode(Point3d<Int_t>(p.x(), newY, p.z()));
      }
      vpNeighbors.emplace_back(_vAllAstarNodesMap[layerIdx][p_neighbor]);
    }
  //}

}

bool DrAstar::bViolateDRC(const DrAstarNode* pU, const DrAstarNode* pV) {
  const Point3d<Int_t>& u = pU->coord();
  const Point3d<Int_t>& v = pV->coord();
  return bViolateDRC(u, v);
}

bool DrAstar::bViolateDRC(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  if (u.z() == v.z()) {
    assert(u.x() == v.x() or u.y() == v.y());
    const Int_t z = u.z();
    Box<Int_t> wire;
    toWire(u, v, wire);
    //if (!_drcMgr.checkWireRoutingLayerShort(_net.idx(), z, wire))
      //return true;
    if (!_drcMgr.checkWireRoutingLayerSpacing(_net.idx(), z, wire))
      return true;
    if (!_drcMgr.checkWireEolSpacing(_net.idx(), z, wire))
      return true;
  }
  else {
  }
  return false;
}

bool DrAstar::bViolateSymDRC(const DrAstarNode* pU, const DrAstarNode* pV) {
  const Point3d<Int_t>& u = pU->coord();
  const Point3d<Int_t>& v = pV->coord();
  return bViolateSymDRC(u, v);
}

bool DrAstar::bViolateSymDRC(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  if (u.z() == v.z()) {
    assert(u.x() == v.x() or u.y() == v.y());
    const Int_t z = u.z();
    const Point3d<Int_t> symU(2 * _symAxisX - u.x(), u.y(), z);
    const Point3d<Int_t> symV(2 * _symAxisX - v.x(), v.y(), z);
    Box<Int_t> wire;
    toWire(symU, symV, wire);
    //if (!_drcMgr.checkWireRoutingLayerShort(_net.idx(), z, wire))
      //return true;
    if (!_drcMgr.checkWireRoutingLayerSpacing(_net.idx(), z, wire))
      return true;
    if (!_drcMgr.checkWireEolSpacing(_net.idx(), z, wire))
      return true;
  }
  else {
  }
  return false;
}

UInt_t DrAstar::selectVia(const DrAstarNode* pU, const DrAstarNode* pV) {
  const Point3d<Int_t>& u = pU->coord();
  const Point3d<Int_t>& v = pV->coord();
  return selectVia(u, v);
}

UInt_t DrAstar::selectVia(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  DrViaSelector vs(_cir, _drcMgr);
  return vs.selectViaIdx(_net.idx(), u, v);
}

UInt_t DrAstar::selectViaSym(const DrAstarNode* pU, const DrAstarNode* pV) {
  const Point3d<Int_t>& u = pU->coord();
  const Point3d<Int_t>& v = pV->coord();
  return selectViaSym(u, v);
}

UInt_t DrAstar::selectViaSym(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  DrViaSelector vs(_cir, _drcMgr);
  return vs.selectViaIdxSym(_net.idx(), u, v, _symAxisX);
}

void DrAstar::addWire2RoutedSpatial(const UInt_t netIdx, const DrAstarNode* pU, const DrAstarNode* pV) {
  const Point3d<Int_t>& u = pU->coord();
  const Point3d<Int_t>& v = pV->coord();
  addWire2RoutedSpatial(netIdx, u, v);
}

void DrAstar::addWire2RoutedSpatial(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  //assert(u.z() == v.z());
  //const Int_t z = u.z();
  //const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(z);
  //assert(layerPair.first == LefLayerType::ROUTING);
  //const LefRoutingLayer& layer = _cir.lef().routingLayer(layerPair.second);
  //const Int_t halfWidth = layer.minWidth() / 2;
  //const Int_t xl = std::min(u.x(), v.x()) - halfWidth;
  //const Int_t xh = std::max(u.x(), v.x()) + halfWidth;
  //const Int_t yl = std::min(u.y(), v.y()) - halfWidth;
  //const Int_t yh = std::max(u.y(), v.y()) + halfWidth;
  //Box<Int_t> wire(xl, yl, xh, yh);
  //_cir.addSpatialRoutedWire(_net.idx(), )
  //_vSpatialRoutedPath[z].insert(wire);
  _cir.addSpatialRoutedWire(netIdx, u, v);
}

void DrAstar::addVia2RoutedSpatial(const UInt_t netIdx, const DrAstarNode* pU, const DrAstarNode* pV, const UInt_t viaIdx) {
  const Point3d<Int_t>& u = pU->coord();
  const Point3d<Int_t>& v = pV->coord();
  addVia2RoutedSpatial(netIdx, u, v, viaIdx);
}

void DrAstar::addVia2RoutedSpatial(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v, const UInt_t viaIdx) {
  //const Int_t zl = std::min(u.z(), v.z());
  //const Int_t zc = zl + 1;
  //const Int_t zh = std::max(u.z(), v.z());
  //assert(zh - zl == 2);
  //assert(u.x() == v.x() and u.y() == v.y());
  //const Int_t x = u.x();
  //const Int_t y = u.y();
  //UInt_t i;
  //for (i = 0; i < lefVia.numBotBoxes(); ++i) {
    //Box<Int_t> box(lefVia.botBox(i));
    //box.shift(x, y);
    //_vSpatialRoutedPath[zl].insert(box);
  //}
  //for (i = 0; i < lefVia.numCutBoxes(); ++i) {
    //Box<Int_t> box(lefVia.cutBox(i));
    //box.shift(x, y);
    //_vSpatialRoutedPath[zc].insert(box);
  //}
  //for (i = 0; i < lefVia.numTopBoxes(); ++i) {
    //Box<Int_t> box(lefVia.topBox(i));
    //box.shift(x, y);
    //_vSpatialRoutedPath[zh].insert(box);
  //}
  _cir.addSpatialRoutedVia(netIdx, viaIdx, u, v);
}

Int_t DrAstar::scaledMDist(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  Int_t dist = 0;
  dist += abs(u.x() - v.x()) * _param.horCost;
  dist += abs(u.y() - v.y()) * _param.verCost;
  dist += abs(u.z() - v.z()) * _param.viaCost;
  return dist;
}

Int_t DrAstar::scaledMDist(const Box<Int_t>& u, const Box<Int_t>& v) {
  Int_t dist = 0;
  dist += std::max({u.bl().x() - v.tr().x(), v.bl().x() - u.tr().x(), (Int_t)0}) * _param.horCost;
  dist += std::max({u.bl().y() - v.tr().y(), v.bl().y() - u.tr().y(), (Int_t)0}) * _param.verCost;
  return dist;
}

Int_t DrAstar::scaledMDist(const Point3d<Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& pair) {
  Int_t dx = 0, dy = 0, dz = 0;
  const Box<Int_t>& box = pair.first;
  if (u.x() < box.xl())
    dx = box.xl() - u.x();
  else if (u.x() > box.xh())
    dx = u.x() - box.xh();
  if (u.y() < box.yl())
    dy = box.yl() - u.y();
  else if (u.y() > box.yh())
    dy = u.y() - box.yh();
  dz = abs(u.z() - pair.second);
  return dx * _param.horCost + 
         dy * _param.verCost +
         dz * _param.viaCost;
}

Int_t DrAstar::scaledMDist(const Pair_t<Box<Int_t>, Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& v) {
  Int_t dist = scaledMDist(u.first, v.first);
  dist += abs(u.second - v.second) * _param.viaCost;
  return dist;
}

Int_t DrAstar::MDist(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  Int_t dist = 0;
  dist += abs(u.x() - v.x());
  dist += abs(u.y() - v.y());
  dist += abs(u.z() - v.z());
  return dist;
}

Int_t DrAstar::MDist(const Box<Int_t>& u, const Box<Int_t>& v) {
  Int_t dist = 0;
  dist += std::max({u.bl().x() - v.tr().x(), v.bl().x() - u.tr().x(), (Int_t)0});
  dist += std::max({u.bl().y() - v.tr().y(), v.bl().y() - u.tr().y(), (Int_t)0});
  return dist;
}

Int_t DrAstar::MDist(const Point3d<Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& pair) {
  Int_t dx = 0, dy = 0, dz = 0;
  const Box<Int_t>& box = pair.first;
  if (u.x() < box.xl())
    dx = box.xl() - u.x();
  else if (u.x() > box.xh())
    dx = u.x() - box.xh();
  if (u.y() < box.yl())
    dy = box.yl() - u.y();
  else if (u.y() > box.yh())
    dy = u.y() - box.yh();
  dz = abs(u.z() - pair.second);
  return dx + 
         dy +
         dz;
}

Int_t DrAstar::MDist(const Pair_t<Box<Int_t>, Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& v) {
  Int_t dist = MDist(u.first, v.first);
  dist += abs(u.second - v.second);
  return dist;
}

void DrAstar::nearestTarBoxDist(const Point3d<Int_t>& u, const UInt_t tarIdx, Int_t& scaledDist, Int_t& unscaledDist) {
  scaledDist = MAX_INT;
  unscaledDist = MAX_INT;
  if (_vCompBoxes[tarIdx].size() > 10) {
    const Point<Int_t> u2d(u.x(), u.y());
    Vector_t<Box<Int_t>> vNearestBoxes;
    Vector_t<Pair_t<Box<Int_t>, Int_t>> vLayerBoxes;
    for (const auto& pair : _vCompSpatialBoxes[tarIdx]) {
      const UInt_t layerIdx = pair.first;
      auto& spatial = _vCompSpatialBoxes[tarIdx].at(layerIdx);
      if (!spatial.empty())
        spatial.nearestSearch(u2d, 1, vNearestBoxes);
      for (const auto& b : vNearestBoxes) {
        vLayerBoxes.emplace_back(b, layerIdx);
      }
    }
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vLayerBoxes) {
      Int_t dist = scaledMDist(u, pair);
      Int_t dist2 = MDist(u, pair);
      if (scaledDist > dist)
        scaledDist = dist;
      if (unscaledDist > dist2)
        unscaledDist = dist2;
    }
  }
  else {
    for (const Pair_t<Box<Int_t>, Int_t>& pair : _vCompBoxes[tarIdx]) {
      Int_t dist = scaledMDist(u, pair);
      Int_t dist2 = MDist(u, pair);
      if (scaledDist > dist)
        scaledDist = dist;
      if (unscaledDist > dist2)
        unscaledDist = dist2;
    }
  }
}

bool DrAstar::hasBend(const DrAstarNode* pU, const DrAstarNode* pV, const Int_t i) {
  if (pU->pParent(i) != nullptr) {
    return findDir(pU->pParent(i)->coord(), pU->coord()) != findDir(pU->coord(), pV->coord());
  }
  return false;
}

DrAstar::PathDir DrAstar::findDir(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
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

bool DrAstar::bNeedUpdate(const DrAstarNode* pV, const Int_t i, const Int_t costG, const Int_t bendCnt) {
  if (pV->costG(i) > costG)
    return true;
  else if (pV->bendCnt(i) > bendCnt)
    return true;
  return false;
}

bool DrAstar::bInsideGuide(const DrAstarNode* pV) {
  const Point3d<Int_t>& v = pV->coord();
  assert(v.z() < (Int_t)_vSpatialNetGuides.size() and v.z() > 0);
  const Point<Int_t> v2d(v.x(), v.y());
  return _vSpatialNetGuides[v.z()].exist(v2d, v2d);
}

void DrAstar::add2Path(const Int_t i, const Point3d<Int_t>& u, List_t<Point3d<Int_t>>& lPathPts) {
  if (i == 0)
    lPathPts.emplace_front(u);
  else
    lPathPts.emplace_back(u);
}

bool DrAstar::bConnected2TarBox(const DrAstarNode* pU, const UInt_t tarIdx) {
  const Point3d<Int_t>& u = pU->coord();
  //const Point3d<Int_t>& v = (pU->pParent(0) == nullptr) ? u : pU->pParent(0)->coord();
  //if (u.z() != v.z()) {
    //Box<Int_t> wire;
    //toWire(u, u, wire);
    //return _vCompSpatialBoxes[tarIdx][u.z()].exist(wire);
  //}
  //else {
    //Box<Int_t> wire;
    //toWire(u, v, wire);
    //return _vCompSpatialBoxes[tarIdx][u.z()].exist(wire);
  //}
  const Point<Int_t> u2d(u.x(), u.y());
  return _vCompSpatialBoxes[tarIdx][u.z()].exist(u2d, u2d);
  //const Vector_t<Pair_t<Box<Int_t>, Int_t>>& vTarBoxes = _vCompBoxes[tarIdx];
  //for (const Pair_t<Box<Int_t>, Int_t>& pair : vTarBoxes) {
    //if (pair.second == u.z() and Box<Int_t>::bConnect(pair.first, wire)) {
      //return true;
    //} 
  //}
  //return false;
}

bool DrAstar::bNeedMergePath(const Point3d<Int_t>& u1, const Point3d<Int_t>& v1, const Point3d<Int_t>& u2, const Point3d<Int_t>& v2) {
  // path1: u1 -> v1, path2: u2 -> v2
  if (u1.z() != v1.z()) {
    assert(u1.x() == v1.x() and u1.y() == v1.y());
    return false;
  }
  return findDir(u1, v1) == findDir(u2, v2);
}

void DrAstar::mergePath(const List_t<Point3d<Int_t>>& lPathPts, List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>>& lPathVec) {
  if (lPathPts.size() == 1) {
    lPathVec.emplace_back(lPathPts.front(), lPathPts.front());
    return;
  }
  else {
    auto u1 = lPathPts.begin();
    auto v1 = std::next(u1, 1);
    auto v2 = std::next(v1, 1);
    for (; v2 != lPathPts.end(); ++v2) {
      if (bNeedMergePath(*u1, *v1, *v1, *v2)) {
        v1 = v2;
      }
      else {
        lPathVec.emplace_back(*u1, *v1);
        u1 = v1;
        v1 = v2;
      }
    }
    lPathVec.emplace_back(*u1, *v1);
  }
  //cerr << "================" << endl;
  //for (auto& p : lPathPts) {
    //cerr << p << endl;
  //}
  //cerr << endl;
  //for (auto& p : newList) {
    //cerr << p.first << p.second << endl;
  //}
  //cerr << "================" << endl;
}

void DrAstar::toWire(const Point3d<Int_t>& u, const Point3d<Int_t>& v, Box<Int_t>& wire) {
  assert(u.z() == v.z());
  const Int_t z = u.z();
  const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(z);
  assert(layerPair.first == LefLayerType::ROUTING);
  const LefRoutingLayer& layer = _cir.lef().routingLayer(layerPair.second);
  const Int_t halfWidth = layer.minWidth() / 2;
  const Int_t xl = std::min(u.x(), v.x()) - halfWidth;
  const Int_t xh = std::max(u.x(), v.x()) + halfWidth;
  const Int_t yl = std::min(u.y(), v.y()) - halfWidth;
  const Int_t yh = std::max(u.y(), v.y()) + halfWidth;
  wire.setXL(xl);
  wire.setXH(xh);
  wire.setYL(yl);
  wire.setYH(yh);
}

void DrAstar::toVia(const Point3d<Int_t>& u, const Point3d<Int_t>& v, const UInt_t viaIdx, Vector_t<Pair_t<Box<Int_t>, Int_t>>& vRet) {
  const Int_t zl = std::min(u.z(), v.z());
  const Int_t zc = zl + 1;
  const Int_t zh = std::max(u.z(), v.z());
  assert(zh - zl == 2);
  assert(u.x() == v.x() and u.y() == v.y());
  const Int_t x = u.x();
  const Int_t y = u.y();
  const LefVia& lefVia = _cir.lef().via(viaIdx);
  UInt_t i;
  for (i = 0; i < lefVia.numBotBoxes(); ++i) {
    Box<Int_t> box(lefVia.botBox(i));
    box.shift(x, y);
    vRet.emplace_back(box, zl);
  }
  for (i = 0; i < lefVia.numCutBoxes(); ++i) {
    Box<Int_t> box(lefVia.cutBox(i));
    box.shift(x, y);
    vRet.emplace_back(box, zc);
  }
  for (i = 0; i < lefVia.numTopBoxes(); ++i) {
    Box<Int_t> box(lefVia.topBox(i));
    box.shift(x, y);
    vRet.emplace_back(box, zh);
  }
  
}

// for debug
void DrAstar::visualize() {
  GdsWriter gw(_cir.name() + "_" + _net.name() + ".gds");
  gw.initWriter();
  gw.createLib("TOP", 2000, 1e-6/2000);
  gw.writeCellBgn("INTERCONNECTION");

  UInt_t i, j, pinIdx, layerIdx;
  const Box<Int_t>* cpBox;
  Net_ForEachPinIdx(_net, pinIdx, i) {
    Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
        gw.writeRectangle(*cpBox, layerIdx, 0);
      }
    }
  }
  
  const auto& vGuides = _net.vGuides();
  for (const auto& p : vGuides) {
    gw.writeRectangle(p.first, p.second + 2000, 0);
  }

  for (const auto& v : _vvRoutePaths) {
    for (const auto& p : v) {
      const Point3d<Int_t> u = p.first;
      const Point3d<Int_t> v = p.second;
      if (u.z() != v.z())
        continue;
      Box<Int_t> box;
      toWire(u, v, box);
      gw.writeRectangle(box, u.z() + 1000, 0);
    }
  }

  gw.writeCellEnd();
  gw.endLib();
}

PROJECT_NAMESPACE_END

