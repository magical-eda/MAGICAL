/**
 * @file   drGridAstar.cpp
 * @brief  Detailed Routing - Grid-Based A* search kernel
 * @author Hao Chen
 * @date   02/05/2020
 *
 **/

#include "drGridAstar.hpp"
#include "src/ds/pqueue.hpp"
#include "src/geo/kdtree.hpp"
#include "src/acs/acsMgr.hpp"
#include "src/geo/box2polygon.hpp"
#include "src/graph/mst.hpp"

#include <lemon/maps.h>
#include <lemon/kruskal.h>
#include <lemon/list_graph.h>

PROJECT_NAMESPACE_START

bool DrGridAstar::run() {
  
  fprintf(stderr, "DrGridAstar::%s\tRoute net %s, Sym: %d, SelfSym: %d, StrictDRC: %d \n",
          __func__,
          _net.name().c_str(),
          _bSym,
          _bSelfSym,
          _bStrictDRC);
  
  if (_net.bRouted()) {
    return true;
  }
  if (_ro.bRouted()) {
    return true;
  }

  init();
  splitSubNetMST();

  // route net with sym and self-sym constraint
  bool bSuccess = route();

  // ripup or not
  if (!bSuccess) {
    fprintf(stderr, "DrGridAstar::%s\tERROR: Route net %s failed!\n", __func__, _net.name().c_str());
    ripup();
    return false;
  }
  saveResult2Net();
  return true;
}

void DrGridAstar::initSelfSym()
{
  assert(_ro.numRoutables() == 0);
  UInt_t i, pinIdx, layerIdx;
  for (i = 0; i < (UInt_t)_ro.numPins(); ++i) {
    pinIdx = _ro.pinIdx(i);
    const auto &pin = _cir.pin(pinIdx);
    bool inLeft = false;
    bool inRight = false;
    Pin_ForEachLayerIdx(pin, layerIdx)
    {
      for (UInt_t boxIdx = 0; boxIdx < pin.numBoxes(layerIdx); ++boxIdx)
      {
        const auto &box = pin.box(layerIdx, boxIdx);
        if (box.xl() < _net.symAxisX())
        {
          inLeft = true;
        }
        if (box.xh() > _net.symAxisX())
        {
          inRight = true;
        }
      }
    }
    if (inLeft && inRight)
    {
      _bSelfSymHasPinInBothSide = true;
    }
    if (inLeft)
    {
      _vPinIdx.emplace_back(pinIdx);
    }
  }
  Int_t numHalfPins = _vPinIdx.size();
  if (!_bSelfSymHasPinInBothSide)
  {
    numHalfPins += 1; // Dummy pin at the symmetric axis
  }
  _compDS.init(numHalfPins);
  _vCompBoxes.resize(numHalfPins);
  _vCompAcsPts.resize(numHalfPins);
  _vCompSpatialBoxes.resize(numHalfPins);
}

void DrGridAstar::init() {
  _vPinIdx.clear();
  _bSelfSymHasPinInBothSide = false;
  if (_bSelfSym) {
    initSelfSym();
  }
  else {
    assert(!_ro.bSelfSym());
    for (Int_t ii = 0; ii < _ro.numPins(); ++ii) {
      _vPinIdx.emplace_back(_ro.pinIdx(ii));
    }
    for (Int_t ii = 0; ii < _ro.numRoutables(); ++ii) {
      _vRoutableIdx.emplace_back(_ro.routableIdx(ii));
    }
    _compDS.init(_ro.numPins() + _ro.numRoutables());
    _vCompBoxes.resize(_ro.numPins() + _ro.numRoutables());
    _vCompAcsPts.resize(_ro.numPins() + _ro.numRoutables());
    _vCompSpatialBoxes.resize(_ro.numPins() + _ro.numRoutables());
  }

  UInt_t i, j, k, layerIdx;
  Int_t yRangeLo = MAX_INT;
  Int_t yRangeHi = MIN_INT;
  const Box<Int_t>* cpBox;
  const AcsPt* cpPt;
  for (i = 0; i < _vPinIdx.size(); ++ i) {
    _vCompAcsPts[i].set_empty_key(Point3d<Int_t>(MIN_INT, MIN_INT, MIN_INT));
    _vCompAcsPts[i].set_deleted_key(Point3d<Int_t>(MAX_INT, MAX_INT, MAX_INT));
    const Int_t pinIdx = _vPinIdx[i];
    const auto& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      Pin_ForEachLayerBox(pin, layerIdx, cpBox, j) {
        yRangeLo = std::min(yRangeLo, cpBox->yl());
        yRangeHi = std::max(yRangeHi, cpBox->yh());
        _vCompBoxes[i].emplace_back((*cpBox), layerIdx);
        _vCompSpatialBoxes[i][layerIdx].insert(*cpBox);
      }
    }

    Pin_ForEachAcsPt(pin, cpPt, j) {
      Point3d<Int_t> pt = cpPt->gridPt();
      _vCompAcsPts[i].insert(pt);
      _pinAcsMap[pt] = *cpPt;
    }
  }
  for (i = 0; i < _vRoutableIdx.size(); ++i) {
    const Int_t compIdx = i + _vPinIdx.size();
    _vCompAcsPts[compIdx].set_empty_key(Point3d<Int_t>(MIN_INT, MIN_INT, MIN_INT));
    _vCompAcsPts[compIdx].set_deleted_key(Point3d<Int_t>(MAX_INT, MAX_INT, MAX_INT));
    
    const Int_t routableIdx = _vRoutableIdx[i];
    const auto& routable = _net.routable(routableIdx);
    
    for (j = 0; j < (UInt_t)routable.numPins(); ++j) {
      const Int_t pinIdx = routable.pinIdx(j);
      const auto& pin = _cir.pin(pinIdx);
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBox(pin, layerIdx, cpBox, k) {
          yRangeLo = std::min(yRangeLo, cpBox->yl());
          yRangeHi = std::max(yRangeHi, cpBox->yh());
          _vCompBoxes[compIdx].emplace_back((*cpBox), layerIdx);
          _vCompSpatialBoxes[compIdx][layerIdx].insert(*cpBox);
        }
      }
      Pin_ForEachAcsPt(pin, cpPt, k) {
        Point3d<Int_t> pt = cpPt->gridPt();
        _vCompAcsPts[compIdx].insert(pt);
        _pinAcsMap[pt] = *cpPt;
      }

    }
    for (j = 0; j < routable.vWireIndices().size(); ++j) {
      const Int_t wireIdx = routable.vWireIndices()[j];
      const auto& wire = _net.vWires()[wireIdx];
      const auto& box = wire.first;
      const Int_t layerIdx = wire.second;
      if (!_cir.lef().bRoutingLayer(layerIdx))
        continue;
      yRangeLo = std::min(yRangeLo, box.yl());
      yRangeHi = std::max(yRangeHi, box.yh());
      _vCompBoxes[compIdx].emplace_back(box, layerIdx);
      _vCompSpatialBoxes[compIdx][layerIdx].insert(box);
      addAcsPts(compIdx, layerIdx, box);
    }
    
  }
  yRangeLo -= 10 * _cir.gridStep();
  yRangeHi += 10 * _cir.gridStep();
  // Add dummy pin at the sym axis for self-symmetric nets
  // Init access points pointing to the left
  if (!_bSelfSymHasPinInBothSide && _bSelfSym) {
    assert(_ro.bSelfSym());
    UInt_t dummyIdx = _vPinIdx.size();
    _vPinIdx.emplace_back(MAX_INT);
    Int_t xWidth = _cir.gridStep();
    Box<Int_t> dummyPinRect(_net.symAxisX() - xWidth, yRangeLo,
                            _net.symAxisX() + xWidth, yRangeHi);
    _vCompAcsPts[dummyIdx].set_empty_key(Point3d<Int_t>(MIN_INT, MIN_INT, MIN_INT));
    _vCompAcsPts[dummyIdx].set_deleted_key(Point3d<Int_t>(MAX_INT, MAX_INT, MAX_INT));
    
    Cir_ForEachLayerIdx(_cir, layerIdx) {
      if (_cir.lef().bRoutingLayer(layerIdx)) {
        _vCompBoxes[dummyIdx].emplace_back(dummyPinRect, layerIdx);
        _vCompSpatialBoxes[dummyIdx][layerIdx].insert(dummyPinRect);
      }
    }
    Int_t gridX = (_net.symAxisX() - _cir.gridOffsetX()) / _cir.gridStep();
    if ((_net.symAxisX() - _cir.gridOffsetX()) % _cir.gridStep() != 0) {
      ++gridX;
    }
    Int_t gridYLo = std::ceil(static_cast<Float_t>(yRangeLo - _cir.gridOffsetY()) / _cir.gridStep());
    Int_t gridYHi = std::floor(static_cast<Float_t>(yRangeHi - _cir.gridOffsetY()) / _cir.gridStep());
    for (Int_t yIdx = gridYLo; yIdx <= gridYHi; ++yIdx)
    {
      Cir_ForEachLayerIdx(_cir, layerIdx) {
        if (_cir.lef().bRoutingLayer(layerIdx)) {
          Point3d<Int_t> pt = Point3d<Int_t>(_cir.gridCenterX(gridX), _cir.gridCenterY(yIdx), layerIdx);
          _vCompAcsPts[dummyIdx].insert(pt);
          _pinAcsMap[pt] = AcsPt(pt, AcsPt::DirType::WEST);
        }
      }
    }
  }
}

void DrGridAstar::splitSubNetMST() {
  // init pins
  UInt_t i, j;
  UInt_t numRealPins = _vCompBoxes.size();
  if (!_bSelfSymHasPinInBothSide and _bSelfSym)
  {
    --numRealPins; // Remove the dummy
  }

  // compute cost value of each edge
  MinimumSpanningTree<Int_t> mst(numRealPins);
  for (i = 0; i < numRealPins; ++i) {
    for (j = i + 1; j < numRealPins; ++j) {
      Int_t minDist = MAX_INT;
      for (const auto& u : _vCompBoxes[i]) {
        for (const auto& v : _vCompBoxes[j]) {
          Int_t dist = scaledMDist(u, v);
          if (dist < minDist) {
            minDist = dist;
          }
        }
      }
      AssertMsg(minDist != MAX_INT, "net %s check i %d j %d \n", _net.name().c_str(), i, j);
      mst.addEdge(i, j, minDist);
    }
  }
  
  mst.solve(_vSubNets);

  // Handle the dummy case
  if (!_bSelfSymHasPinInBothSide and _bSelfSym)
  {
    UInt_t luckyIdx = 0; // it's connected to the dummy pin!
    UInt_t dummyIdx = numRealPins; 
    Int_t minDist = MAX_INT;
    for (i = 0; i < numRealPins; ++i)
    {
      for (const auto &u : _vCompBoxes[i])
      {
        for (const auto &v : _vCompBoxes[dummyIdx])
        {
          Int_t dist = scaledMDist(u, v);
          if (dist < minDist)
          {
            minDist = dist;
            luckyIdx = i;
          }
        }
      }
    }
    _vSubNets.emplace_back(luckyIdx, dummyIdx);
  }
}

bool DrGridAstar::route() {
  for (const auto& pair : _vSubNets) {
    const Int_t srcIdx = pair.first;
    const Int_t tarIdx = pair.second;
    if (!routeSubNet(srcIdx, tarIdx)) {
      return false;
    }
  }
  return true;
}

bool DrGridAstar::routeSubNet(Int_t srcIdx, Int_t tarIdx) {
  assert(_compDS.find(srcIdx) != _compDS.find(tarIdx));
  
  // init source and target access point sets
  srcIdx = _compDS.find(srcIdx);
  tarIdx = _compDS.find(tarIdx);
  auto& src = _vCompAcsPts[srcIdx]; // DenseHashSet
  auto& tar = _vCompAcsPts[tarIdx]; // DenseHashSet

  // reset nodes cost
  resetAllNodes();

  // init access points to node maps
  // memory free in destructor
  for (const auto& p : src) { // p: Point3d<Int>
    Point<Int_t> p2d(p.x(), p.y());
    if (_vAllNodesMap[p.z()].find(p2d) == _vAllNodesMap[p.z()].end()) {
      _vAllNodesMap[p.z()][p2d] = new DrGridAstarNode(p);
    }
  }
  for (const auto& p : tar) { // p: Point3d<Int>
    Point<Int_t> p2d(p.x(), p.y());
    if (_vAllNodesMap[p.z()].find(p2d) == _vAllNodesMap[p.z()].end()) {
      _vAllNodesMap[p.z()][p2d] = new DrGridAstarNode(p);
    }
  }

  return pathSearch(srcIdx, tarIdx);
}

 bool DrGridAstar::pathSearch(const Int_t srcIdx, const Int_t tarIdx) {
  auto& src = _vCompAcsPts[srcIdx]; // DenseHashSet
  auto& tar = _vCompAcsPts[tarIdx]; // DenseHashSet
  
  // init KD-Tree of tar
  K3dTree<Int_t> tarKD;
  for (const auto& p : tar) {
    tarKD.insert(p.x() * _param.horCost,
                 p.y() * _param.verCost,
                 p.z() * _param.viaCost);
  }
  tarKD.buildIndex();
  
  // init Astar priority queue
  using PQueue_t = PairingHeap<DrGridAstarNode*, DrGridAstarNodeCmp>;
  using Itermap_t = DenseHashMap<DrGridAstarNode*, PQueue_t::point_iterator>;
  PQueue_t pq;
  Itermap_t itMap;
  itMap.set_empty_key(nullptr);

  // add src points to pq
  for (const auto& p : src) {
    Point<Int_t> p2d(p.x(), p.y());
    Int_t layerIdx = p.z();
    Point3d<Int_t> scaledP(p.x() * _param.horCost,
                           p.y() * _param.verCost,
                           p.z() * _param.viaCost);
    Point3d<Int_t> scaledNearestP;
    Int_t scaledNearestDist = MAX_INT;
    tarKD.nearestSearch(scaledP, scaledNearestP, scaledNearestDist);
    assert(_vAllNodesMap[layerIdx].find(p2d) != _vAllNodesMap[layerIdx].end());
    DrGridAstarNode* pNode = _vAllNodesMap[layerIdx][p2d];
    Int_t costF = _param.factorH * scaledNearestDist;
    if (bInsideGuide(pNode)) {
      costF += _param.guideCost;
    }
    pNode->setCostF(costF);
    pNode->setCostG(0);
    pNode->setBendCnt(0);
    //pNode->setDist2Tar()
    itMap[pNode] = pq.push(pNode);
  }
  
  // start exploring nodes
  while (!pq.empty() and (Int_t)pq.size() < _param.maxExplore) {
    DrGridAstarNode* pU = pq.top();
    if (bFindAcsPt(pU, tarIdx)) { // u \in tar
      const UInt_t bigCompIdx = mergeComp(srcIdx, tarIdx);
      //connect2AcsPt(pU);
      backTrack(pU, bigCompIdx, srcIdx, tarIdx);
      return true;
    }
    pq.pop();
    pU->setExplored(true);
    if (pU->vpNeighbors().empty()) {
      findNeighbors(pU);
    }
    for (auto pV : pU->vpNeighbors()) {
      if (pV->bExplored())
        continue;
      const Int_t costG = pU->costG() + scaledMDist(pU->coord(), pV->coord());
      const Int_t bendCnt = pU->bendCnt() + hasBend(pU, pV);
      if (bNeedUpdate(pV, costG, bendCnt)) {
        Point3d<Int_t> scaledP(pV->coord().x() * _param.horCost,
                               pV->coord().y() * _param.verCost,
                               pV->coord().z() * _param.viaCost);
        Point3d<Int_t> scaledNearestP;
        Int_t scaledNearestDist;
        tarKD.nearestSearch(scaledP, scaledNearestP, scaledNearestDist);
        Int_t costF = (costG * _param.factorG + scaledNearestDist * _param.factorH);
        if (bViolateDRC(pU, pV, srcIdx, tarIdx)) {
          if (_bStrictDRC) // do not allow any DRC violation
            continue;
          else // add high cost to DRC violations
            costF += _param.drcCost;
        }
        if (bStackedVia(pU, pV)) {
          //costF += _param.stackedViaCost;
          continue;
        }
        if (bInsideGuide(pV)) {
          costF += _param.guideCost;
        }
        costF += history(pV);
        pV->setParent(pU);
        pV->setCostF(costF);
        pV->setCostG(costG);
        pV->setBendCnt(bendCnt);
        auto it = itMap.find(pV);
        if (it != itMap.end())
          pq.modify(it->second, pV);
        else
          itMap[pV] = pq.push(pV);
      }
    }
  }
  return false;
}

bool DrGridAstar::bFindAcsPt(const DrGridAstarNode* pU, const Int_t tarIdx) {
  const auto& tar = _vCompAcsPts[tarIdx];
  if (tar.find(pU->coord()) != tar.end()) {
    //const AcsPt& acsPt = _pinAcsMap.at(pU->coord());
    //if (findAcsDir(pU) == acsPt.dir())
      return true;
  }
  return false;
}

Int_t DrGridAstar::mergeComp(const Int_t srcIdx, const Int_t tarIdx) {
  _compDS.merge(srcIdx, tarIdx);
  Int_t bigCompIdx = srcIdx, smallCompIdx = tarIdx;
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

void DrGridAstar::backTrack(const DrGridAstarNode* pU, const Int_t bigCompIdx, const Int_t srcIdx, const Int_t tarIdx) {
  List_t<Point3d<Int_t>> lPathPts;
  add2Path(pU, lPathPts);
  const DrGridAstarNode* pParent = pU->pParent();
  if (pParent == nullptr)
    return;
  do {
    add2Path(pParent, lPathPts);
    pParent = pParent->pParent();
  } while (pParent != nullptr);
  assert(_compDS.find(srcIdx) == _compDS.find(tarIdx));
  const Int_t rootIdx = _compDS.find(srcIdx);
  const Int_t childIdx = (rootIdx == srcIdx) ? tarIdx : srcIdx;
  _vCompAcsPts[rootIdx] = _vCompAcsPts[bigCompIdx];
  List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>> lPathVec;
  mergePath(lPathPts, lPathVec);
  
  savePath(lPathVec);
  

  // add to comp box
  for (const auto& pair : _vCompBoxes[childIdx]) {
    _vCompBoxes[rootIdx].emplace_back(pair);
    _vCompSpatialBoxes[rootIdx][pair.second].insert(pair.first);
  }
  const auto& vRoutePath = _vvRoutePaths.back();
  for (Int_t i = 0; i < (Int_t)vRoutePath.size(); ++i) {
  //for (const auto& vec : vRoutePath) {
    const auto& vec = vRoutePath[i];
    const auto& u = vec.first;
    const auto& v = vec.second;
    if (u.z() == v.z()) {
      // get the layer information
      //assert(_cir.lef().bRoutingLayer(u.z()));
      //const auto& layerPair = _cir.lef().layerPair(u.z());
      //const auto& layer = _cir.lef().routingLayer(layerPair.second);
      //const Int_t width = layer.minWidth();
      const Int_t width = _net.minWidth();
      const Int_t extension = width / 2;
      //const Int_t extension = (i == 0 or i == (Int_t)vRoutePath.size() - 1) ?
                              //0 : width / 2;
      // generate the exact wire shape
      Box<Int_t> wire;
      toWire(u, v, width, extension, wire);
      addAcsPts(rootIdx, u.z(), wire);
      _vCompBoxes[rootIdx].emplace_back(wire, u.z());
      _vCompSpatialBoxes[rootIdx][u.z()].insert(wire);
    }
  }
}

void DrGridAstar::add2Path(const DrGridAstarNode* pU, List_t<Point3d<Int_t>>& lPathPts) {
  lPathPts.emplace_front(pU->coord()); 
}

void DrGridAstar::mergePath(const List_t<Point3d<Int_t>>& lPathPts, List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>>& lPathVec) {
  if (lPathPts.size() == 1) {
    lPathVec.emplace_back(lPathPts.front(), lPathPts.front());
    return;
  }
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

bool DrGridAstar::bNeedMergePath(const Point3d<Int_t>& u1, const Point3d<Int_t>& v1, const Point3d<Int_t>& u2, const Point3d<Int_t>& v2) {
  // path1: u1 -> v1, path2: u2 -> v2
  if (u1.z() != v1.z()) {
    assert(u1.x() == v1.x() and u1.y() == v1.y());
    return false;
  }
  return findDir(u1, v1) == findDir(u2, v2);
}

void DrGridAstar::savePath(const List_t<Pair_t<Point3d<Int_t>, Point3d<Int_t>>>& lPathVec) {
  _vvRoutePaths.resize(_vvRoutePaths.size() + 1);
  _vvRoutePaths.back().reserve(lPathVec.size());
  _vvRoutedWires.resize(_vvRoutedWires.size() + 1);
  _vvRoutedWires.back().reserve(lPathVec.size());
  for (const auto& pair : lPathVec) {
    _vvRoutePaths.back().emplace_back(pair);
  }
  // add exact shapes to spatial
  const auto& vPathVec = _vvRoutePaths.back();
  auto& vRoutedWires = _vvRoutedWires.back();
  for (Int_t i = 0; i < (Int_t)vPathVec.size(); ++i) {
    const auto& pair = vPathVec[i];
    const auto& u = pair.first;
    const auto& v = pair.second;
    if (u.z() == v.z()) {
      // get the layer information
      //assert(_cir.lef().bRoutingLayer(u.z()));
      //const auto& layerPair = _cir.lef().layerPair(u.z());
      //const auto& layer = _cir.lef().routingLayer(layerPair.second);
      //const Int_t width = layer.minWidth();
      const Int_t width = _net.minWidth();
      const Int_t extension = width / 2;
      //const Int_t extension = (i == 0 or i == (Int_t)vPathVec.size() - 1) ?
                              //0 : width / 2;
      // generate the exact wire
      Box<Int_t> wire;
      toWire(u, v, width, extension, wire);
      vRoutedWires.emplace_back(wire, u.z());
      _cir.addSpatialRoutedWire(_net.idx(), u.z(), wire);
      // add history cost
      _dr.addWireHistoryCost(_param.historyCost, u.z(), wire);
      
      // add symmetric wire to spatial routed wire, for DRC
      if (_bSym) {
        Box<Int_t> symWire(wire);
        symWire.flipX(_net.symAxisX());
        _cir.addSpatialRoutedWire(_net.symNetIdx(), u.z(), symWire);
        _dr.addWireHistoryCost(_param.historyCost, u.z(), symWire);
      }
      if (_bSelfSym) {
        Box<Int_t> symWire(wire);
        symWire.flipX(_net.symAxisX());
        _cir.addSpatialRoutedWire(_net.idx(), u.z(), symWire);
        _dr.addWireHistoryCost(_param.historyCost, u.z(), symWire);
      }
    }
    else {
      // choose via
      assert(u.x() == v.x() and u.y() == v.y());
      const Int_t x = u.x();
      const Int_t y = u.y();
      const Int_t botLayerIdx = std::min(u.z(), v.z());

      const Pair_t<Point3d<Int_t>, Point3d<Int_t>>* pPrev = (i > 0) ? &vPathVec[i - 1] : nullptr;
      const Pair_t<Point3d<Int_t>, Point3d<Int_t>>* pNext = (i < (Int_t)vPathVec.size() - 1) ? &vPathVec[i + 1] : nullptr;
      //Orient2D_t topType = Orient2D_t::DEFAULT, botType = Orient2D_t::DEFAULT;
      Int_t topViaWidth = -1, topViaHeight = -1;
      Int_t botViaWidth = -1, botViaHeight = -1;
      if (pPrev) {
        //Orient2D_t* p1 = (pPrev->first.z() == botLayerIdx
                                //or pPrev->second.z() == botLayerIdx) ? &botType : &topType;
        Int_t* w1 = (pPrev->first.z() == botLayerIdx
                     or pPrev->second.z() == botLayerIdx) ? &botViaWidth : &topViaWidth;
        Int_t* h1 = (pPrev->first.z() == botLayerIdx
                     or pPrev->second.z() == botLayerIdx) ? &botViaHeight : &topViaHeight;
        PathDir dir = findDir(pPrev->first, pPrev->second);
        switch(dir) {
          case PathDir::UP:
          case PathDir::DOWN:
            //*p1 = Orient2D_t::VERTICAL;
            *w1 = _net.minWidth();
            break;
          case PathDir::LEFT:
          case PathDir::RIGHT:
            //*p1 = Orient2D_t::HORIZONTAL;
            *h1 = _net.minWidth();
            break;
          case PathDir::VIA_UP:
          case PathDir::VIA_DOWN:
            //*p1 = Orient2D_t::DEFAULT;
            break;
          default: assert(false);
        }
      }
      if (pNext) {
        //Orient2D_t* p2 = (pNext->first.z() == botLayerIdx
                                //or pNext->second.z() == botLayerIdx) ? &botType : &topType;
        Int_t* w2 = (pNext->first.z() == botLayerIdx
                     or pNext->second.z() == botLayerIdx) ? &botViaWidth : &topViaWidth;
        Int_t* h2 = (pNext->first.z() == botLayerIdx
                     or pNext->second.z() == botLayerIdx) ? &botViaHeight : &topViaHeight;
        PathDir dir = findDir(pNext->first, pNext->second);
        switch(dir) {
          case PathDir::UP:
          case PathDir::DOWN:
            //*p2 = Orient2D_t::VERTICAL;
            *w2 = _net.minWidth();
            break;
          case PathDir::LEFT:
          case PathDir::RIGHT:
            //*p2 = Orient2D_t::HORIZONTAL;
            *h2 = _net.minWidth();
            break;
          case PathDir::VIA_UP:
          case PathDir::VIA_DOWN:
            //*p2= Orient2D_t::DEFAULT;
            break;
          default: assert(false);
        }
      }

      //const LefVia& via = _cir.lef().via(botLayerIdx, 1, 1, botType, topType);
      const LefVia& via = _cir.lef().via(botLayerIdx, _param.numCutsRow, _param.numCutsCol, botViaWidth, botViaHeight, topViaWidth, topViaHeight);
      via2LayerBoxes(x, y, via, vRoutedWires);     
      _cir.addSpatialRoutedVia(_net.idx(), x, y, via);
      // add history cost
      _dr.addViaHistoryCost(_param.historyCost, x, y, via);
      // add symmetric via to spatial routed wire, for DRC
      if (_bSym) {
        const Int_t symX = 2 * _net.symAxisX() - x;
        _cir.addSpatialRoutedVia(_net.symNetIdx(), symX, y, via);
        _dr.addViaHistoryCost(_param.historyCost, symX, y, via);
      }
      if (_bSelfSym) {
        const Int_t symX = 2 * _net.symAxisX() - x;
        _cir.addSpatialRoutedVia(_net.idx(), symX, y, via);
        _dr.addViaHistoryCost(_param.historyCost, symX, y, via);
      }
    }
  }
}

void DrGridAstar::via2LayerBoxes(const Int_t x, const Int_t y, const LefVia& via, Vector_t<Pair_t<Box<Int_t>, Int_t>>& vLayerBoxes) {
  for (auto box : via.vBotBoxes()) {
    box.shift(x, y);
    vLayerBoxes.emplace_back(box, via.botLayerIdx());
  }
  for (auto box : via.vCutBoxes()) {
    box.shift(x, y);
    vLayerBoxes.emplace_back(box, via.cutLayerIdx());
  }
  for (auto box : via.vTopBoxes()) {
    box.shift(x, y);
    vLayerBoxes.emplace_back(box, via.topLayerIdx());
  }
}

void DrGridAstar::addAcsPts(const Int_t idx, const Int_t z, const Box<Int_t>& box) {
  Vector_t<Point3d<Int_t>> vAcs;
  AcsMgr acs(_cir);
  acs.computeBoxAcs(box, z, vAcs);
  _vCompAcsPts[idx].insert(vAcs.begin(), vAcs.end());
}

void DrGridAstar::findNeighbors(DrGridAstarNode* pU) {
  Vector_t<DrGridAstarNode*> vpNeighbors;
  const auto& p = pU->coord();
  assert(p != Point3d<Int_t>(0, 0, 0));
  assert(_cir.lef().bRoutingLayer(p.z()));
  
  // set step size
  const Int_t step = _cir.gridStep();

  // find lower layer neighbor
  const Int_t minLowerLayerIdx = _cir.lef().routingLayerIdx2LayerIdx(0);
  //const Int_t minLowerLayerIdx = _net.bPower() ? 
                                 //_cir.lef().routingLayerIdx2LayerIdx(5) :
                                 //_cir.lef().routingLayerIdx2LayerIdx(0);
  if (p.z() > minLowerLayerIdx) {
    const Point<Int_t> p2d(p.x(), p.y());
    const Int_t layerIdx = p.z() - 2;
    if (layerIdx >= 0 and _cir.lef().bRoutingLayer(layerIdx)) {
      // add new node if not visited before
      if (_vAllNodesMap[layerIdx].find(p2d) == _vAllNodesMap[layerIdx].end()) {
        _vAllNodesMap[layerIdx][p2d] = new DrGridAstarNode(Point3d<Int_t>(p.x(), p.y(), layerIdx));
      }
      pU->addNeighbor(_vAllNodesMap[layerIdx][p2d]);
    }
  }
  // find upper layer neighbor
  //const Int_t maxUpperLayerIdx = _cir.lef().routingLayerIdx2LayerIdx(5);
  const Int_t maxUpperLayerIdx = _net.bPower() ?
                                 _cir.lef().routingLayerIdx2LayerIdx(5) : // M6
                                 _cir.lef().routingLayerIdx2LayerIdx(4); // M5
  if (p.z() < maxUpperLayerIdx) { 
    const Point<Int_t> p2d(p.x(), p.y());
    const Int_t layerIdx = p.z() + 2;
    if (_cir.lef().bRoutingLayer(layerIdx)) {
      // add new node if not visited before
      if (_vAllNodesMap[layerIdx].find(p2d) == _vAllNodesMap[layerIdx].end()) {
        _vAllNodesMap[layerIdx][p2d] = new DrGridAstarNode(Point3d<Int_t>(p.x(), p.y(), layerIdx));
      }
      pU->addNeighbor(_vAllNodesMap[layerIdx][p2d]);
    }
  }
  // find left neighbor
  if (p.x() > _cir.xl()) {
    const Int_t layerIdx = p.z();
    const Int_t newX = p.x() - step;
    const Point<Int_t> neighborP(newX, p.y());
    if (_vAllNodesMap[layerIdx].find(neighborP) == _vAllNodesMap[layerIdx].end()) {
      _vAllNodesMap[layerIdx][neighborP] = new DrGridAstarNode(Point3d<Int_t>(neighborP.x(), neighborP.y(), layerIdx));
    }
    pU->addNeighbor(_vAllNodesMap[layerIdx][neighborP]);
  }
  // find right neighbor
  if (p.x() < _cir.xh()) {
    const Int_t layerIdx = p.z();
    const Int_t newX = p.x() + step;
    const Point<Int_t> neighborP(newX, p.y());
    if (_vAllNodesMap[layerIdx].find(neighborP) == _vAllNodesMap[layerIdx].end()) {
      _vAllNodesMap[layerIdx][neighborP] = new DrGridAstarNode(Point3d<Int_t>(neighborP.x(), neighborP.y(), layerIdx));
    }
    pU->addNeighbor(_vAllNodesMap[layerIdx][neighborP]);
  }
  // find down neighbor
  if (p.y() > _cir.yl()) {
    const Int_t layerIdx = p.z();
    const Int_t newY = p.y() - step;
    const Point<Int_t> neighborP(p.x(), newY);
    if (_vAllNodesMap[layerIdx].find(neighborP) == _vAllNodesMap[layerIdx].end()) {
      _vAllNodesMap[layerIdx][neighborP] = new DrGridAstarNode(Point3d<Int_t>(neighborP.x(), neighborP.y(), layerIdx));
    }
    pU->addNeighbor(_vAllNodesMap[layerIdx][neighborP]);
  }
  // find up neighbor
  if (p.y() < _cir.yh()) {
    const Int_t layerIdx = p.z();
    const Int_t newY = p.y() + step;
    const Point<Int_t> neighborP(p.x(), newY);
    if (_vAllNodesMap[layerIdx].find(neighborP) == _vAllNodesMap[layerIdx].end()) {
      _vAllNodesMap[layerIdx][neighborP] = new DrGridAstarNode(Point3d<Int_t>(neighborP.x(), neighborP.y(), layerIdx));
    }
    pU->addNeighbor(_vAllNodesMap[layerIdx][neighborP]);
  }
}

bool DrGridAstar::bViolateDRC(const DrGridAstarNode* pU, const DrGridAstarNode* pV, const Int_t srcIdx, const Int_t tarIdx) {
  const auto& u = pU->coord();
  const auto& v = pV->coord();
  if (u.z() == v.z()) {
    assert(u.x() == v.x() or u.y() == v.y());
    const Int_t z = u.z();
    // get the layer information
    //assert(_cir.lef().bRoutingLayer(u.z()));
    //const auto& layerPair = _cir.lef().layerPair(u.z());
    //const auto& layer = _cir.lef().routingLayer(layerPair.second);
    //const Int_t width = layer.minWidth();
    const Int_t width = _net.minWidth();
    const Int_t extension = width / 2;
    //const Int_t extension = (bFindAcsPt(pU, srcIdx) or bFindAcsPt(pV, tarIdx)) ?
                            //0 : width / 2;
    // generate exact wire shape
    Box<Int_t> wire;

    toWire(u, v, width, extension, wire);
    // check DRC
    if (!_drc.checkWireRoutingLayerSpacing(_net.idx(), z, wire))
      return true;
    if (!_drc.checkWireEolSpacing(_net.idx(), z, wire))
      return true;
   
    // check symmetric DRC
    if (_bSym) {
      Box<Int_t> symWire(wire);
      symWire.flipX(_net.symAxisX());
      if (!_drc.checkWireRoutingLayerSpacing(_net.symNetIdx(), z, symWire))
        return true;
      if (!_drc.checkWireEolSpacing(_net.symNetIdx(), z, symWire))
        return true;
    }
    
    // Self-symmetric
    if (_bSelfSym)
    {
      Box<Int_t> symWire(wire);
      symWire.flipX(_net.symAxisX());
      if (!_drc.checkWireRoutingLayerSpacing(_net.idx(), z, symWire))
        return true;
      if (!_drc.checkWireEolSpacing(_net.idx(), z, symWire))
        return true;
    }
  }
  else {
    // generate via and check min area and adj edges
    assert(u.x() == v.x() and u.y() == v.y());
    const Int_t x = u.x();
    const Int_t y = u.y();
    const Int_t botLayerIdx = std::min(u.z(), v.z());
    const LefVia& via = _cir.lef().via(botLayerIdx, _param.numCutsRow, _param.numCutsCol);
    if (!_drc.checkViaSpacing(_net.idx(), x, y, via))
      return true;
    // TODO: minarea, minstep
    // check min area
    if (!checkMinArea(pU, pV))
      return true;
    
    if (_bSym) {
      const Int_t symX = 2 * _net.symAxisX() - x;
      if (!_drc.checkViaSpacing(_net.symNetIdx(), symX, y, via))
        return true;
    }
    if (_bSelfSym) {
      const Int_t symX = 2 * _net.symAxisX() - x;
      if (!_drc.checkViaSpacing(_net.idx(), symX, y, via))
        return true;
    }
  }
  return false;
}

bool DrGridAstar::checkMinArea(const DrGridAstarNode* pU, const DrGridAstarNode* pV) {
  const Int_t layerIdx = pU->coord().z();
  const DrGridAstarNode* pN = pU;
  Vector_t<Box<Int_t>> vPrePath;
  // get the layer information
  //assert(_cir.lef().bRoutingLayer(layerIdx));
  //const auto& layerPair = _cir.lef().layerPair(layerIdx);
  //const auto& layer = _cir.lef().routingLayer(layerPair.second);
  //const Int_t width = layer.minWidth();
  const Int_t width = _net.minWidth();
  const Int_t extension = width / 2;

  while (pN->pParent() and pN->pParent()->coord().z() == layerIdx) {
    // generate the exact wire shape
    Box<Int_t> wire;
    toWire(pN->coord(), pN->pParent()->coord(), width, extension, wire);
    vPrePath.emplace_back(wire);
    pN = pN->pParent();
  }
  assert(layerIdx == pN->coord().z());
  return _drc.checkWireMinArea(layerIdx, vPrePath);
}

bool DrGridAstar::bNeedUpdate(const DrGridAstarNode* pV, const Int_t costG, const Int_t bendCnt) {
  if (pV->costG() > costG)
    return true;
  else if (pV->bendCnt() > bendCnt)
    return true;
  return false;
}

void DrGridAstar::resetAllNodes() {
  for (auto& map : _vAllNodesMap) {
    for (auto& pair : map) {
      DrGridAstarNode* pNode = pair.second;
      pNode->reset();
    }
  }
}

void DrGridAstar::saveResult2Net() {
  for (const auto& vRoutePath : _vvRoutePaths) {
    for (const auto& pair : vRoutePath) {
      _ro.vPathIndices().emplace_back(_net.vRoutePaths().size());
      _net.vRoutePaths().emplace_back(pair);
    }
  } 
  for (const auto& vRoutedWires : _vvRoutedWires) {
    for (const auto& pair : vRoutedWires) {
      _ro.vWireIndices().emplace_back(_net.vWires().size());
      _net.vWires().emplace_back(pair);
    }
  }
  _ro.setRouted(true);
  bool bNetRouted = true;
  for (Int_t i = 0; i < _net.numRoutables(); ++i) {
    bNetRouted &= _net.routable(i).bRouted();
  }
  _net.setRouted(bNetRouted);

  if (_bSym) {
    Net& symNet = _cir.net(_net.symNetIdx());
    Routable& symRo = symNet.routable(_ro.symNetRoutableIdx());
    for (const auto& vRoutePath : _vvRoutePaths) {
      for (const auto& pair : vRoutePath) {
        const Point3d<Int_t>& u = pair.first;
        const Point3d<Int_t>& v = pair.second;
        Point3d<Int_t> symU(u); symU.flipX(_net.symAxisX());
        Point3d<Int_t> symV(v); symV.flipX(_net.symAxisX());
        symRo.vPathIndices().emplace_back(symNet.vRoutePaths().size());
        symNet.vRoutePaths().emplace_back(symU, symV);
      }
    }
    for (const auto& vRoutedWires : _vvRoutedWires) {
      for (const auto& pair : vRoutedWires) {
        const Box<Int_t>& wire = pair.first;
        const Int_t layerIdx = pair.second;
        Box<Int_t> symWire(wire);
        symWire.flipX(_net.symAxisX());
        symRo.vWireIndices().emplace_back(symNet.vWires().size());
        symNet.vWires().emplace_back(symWire, layerIdx);
      }
    }
    symRo.setRouted(true);
    bool bNetRouted = true;
    for (Int_t i = 0; i < symNet.numRoutables(); ++i) {
      bNetRouted &= symNet.routable(i).bRouted();
    }
    symNet.setRouted(bNetRouted);
  }
  if (_bSelfSym)
  {
    for (const auto& vRoutePath : _vvRoutePaths) {
      for (const auto& pair : vRoutePath) {
        const Point3d<Int_t>& u = pair.first;
        const Point3d<Int_t>& v = pair.second;
        Point3d<Int_t> symU(u); symU.flipX(_net.symAxisX());
        Point3d<Int_t> symV(v); symV.flipX(_net.symAxisX());
        Pair_t<Point3d<Int_t>, Point3d<Int_t>> symPath(symU, symV);
        if (symPath != pair) {
          _ro.vPathIndices().emplace_back(_net.vRoutePaths().size());
          _net.vRoutePaths().emplace_back(symPath);
        }
      }
    }
    for (const auto& vRoutedWires : _vvRoutedWires) {
      for (const auto& pair : vRoutedWires) {
        const Box<Int_t>& wire = pair.first;
        const Int_t layerIdx = pair.second;
        Box<Int_t> symWire(wire);
        symWire.flipX(_net.symAxisX());
        if (symWire != wire) {
          _ro.vWireIndices().emplace_back(_net.vWires().size());
          _net.vWires().emplace_back(symWire, layerIdx);
        }
      }
    }
  }
}

void DrGridAstar::ripup() {
  for (const auto& vRoutedWires : _vvRoutedWires) {
    for (const auto& pair : vRoutedWires) {
      const Box<Int_t>& wire = pair.first;
      const Int_t layerIdx = pair.second;
      bool bExist = _cir.removeSpatialRoutedWire(_net.idx(), layerIdx, wire);
      assert(bExist);
      if (_bSym) {
        Box<Int_t> symWire(wire);
        symWire.flipX(_net.symAxisX());
        bool bExist = _cir.removeSpatialRoutedWire(_net.symNetIdx(), layerIdx, symWire);
        assert(bExist);
      }
      if (_bSelfSym)
      {
        Box<Int_t> symWire(wire);
        symWire.flipX(_net.symAxisX());
        bool bExist = _cir.removeSpatialRoutedWire(_net.idx(), layerIdx, symWire);
        assert(bExist);
      }
    }
  }
}

Int_t DrGridAstar::scaledMDist(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
  Int_t dist = 0;
  dist += abs(u.x() - v.x()) * _param.horCost;
  dist += abs(u.y() - v.y()) * _param.verCost;
  dist += abs(u.z() - v.z()) * _param.viaCost;
  return dist;
}

Int_t DrGridAstar::scaledMDist(const Box<Int_t>& u, const Box<Int_t>& v) {
  Int_t dist = 0;
  dist += std::max({u.bl().x() - v.tr().x(), v.bl().x() - u.tr().x(), (Int_t)0}) * _param.horCost;
  dist += std::max({u.bl().y() - v.tr().y(), v.bl().y() - u.tr().y(), (Int_t)0}) * _param.verCost;
  return dist;
}

Int_t DrGridAstar::scaledMDist(const Point3d<Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& pair) {
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

Int_t DrGridAstar::scaledMDist(const Pair_t<Box<Int_t>, Int_t>& u, const Pair_t<Box<Int_t>, Int_t>& v) {
  Int_t dist = scaledMDist(u.first, v.first);
  dist += abs(u.second - v.second) * _param.viaCost;
  return dist;
}

bool DrGridAstar::hasBend(const DrGridAstarNode* pU, const DrGridAstarNode* pV) {
  if (pU->pParent() != nullptr) {
    return findDir(pU->pParent()->coord(), pU->coord()) != findDir(pU->coord(), pV->coord());
  }
  return false;
}

DrGridAstar::PathDir DrGridAstar::findDir(const Point3d<Int_t>& u, const Point3d<Int_t>& v) {
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

bool DrGridAstar::bInsideGuide(const DrGridAstarNode* pU) {
  const auto& u = pU->coord();
  assert(u.z() < (Int_t)_cir.vSpatialNetGuides(_net.idx()).size() and u.z() > 0);
  const Point<Int_t> u2d(u.x(), u.y());
  return _cir.vSpatialNetGuides(_net.idx())[u.z()].exist(u2d, u2d);
}

Int_t DrGridAstar::history(const DrGridAstarNode* pU) {
  const auto& u = pU->coord();
  const Point<Int_t> u2d(u.x(), u.y());
  Vector_t<Int_t> vHis;
  _dr._vSpatialHistoryMaps[u.z()].query(u2d, u2d, vHis);
  return std::accumulate(vHis.begin(), vHis.end(), 0);
}

void DrGridAstar::toWire(const Point3d<Int_t>& u, const Point3d<Int_t>& v, const Int_t width, const Int_t extension, Box<Int_t>& wire) {
  // change wire width here
  assert(u.z() == v.z());
  const Int_t halfWidth = width / 2;
  Int_t xl = 0, yl = 0, xh = 0, yh = 0;
  if (u.x() == v.x()) { // vertical edge
    xl = u.x() - halfWidth;
    yl = std::min(u.y(), v.y()) - extension;
    xh = u.x() + halfWidth;
    yh = std::max(u.y(), v.y()) + extension;
  }
  else if (u.y() == v.y()) { // horizontal edge
    xl = std::min(u.x(), v.x()) - extension;
    yl = u.y() - halfWidth;
    xh = std::max(u.x(), v.x()) + extension;
    yh = u.y() + halfWidth;
  }
  wire.setXL(xl);
  wire.setXH(xh);
  wire.setYL(yl);
  wire.setYH(yh);
}

AcsPt::DirType DrGridAstar::findAcsDir(const DrGridAstarNode* pU) {
  assert(pU->pParent() != nullptr);
  switch (findDir(pU->coord(), pU->pParent()->coord())) {
    case PathDir::LEFT: return AcsPt::DirType::WEST;
    case PathDir::RIGHT: return AcsPt::DirType::EAST;
    case PathDir::UP: return AcsPt::DirType::NORTH;
    case PathDir::DOWN: return AcsPt::DirType::SOUTH;
    case PathDir::VIA_UP: return AcsPt::DirType::UP;
    case PathDir::VIA_DOWN: return AcsPt::DirType::DOWN;
    default: assert(false);
  }
}

void DrGridAstar::connect2AcsPt(const DrGridAstarNode* pU) {
  _vvRoutedWires.resize(_vvRoutedWires.size() + 1);
  const auto& pt = pU->coord();
  const auto& acsPt = _pinAcsMap.at(pt).gridPt();

  //assert(_cir.lef().bRoutingLayer(pt.z()));
  //const auto& layerPair = _cir.lef().layerPair(pt.z());
  //const auto& layer = _cir.lef().routingLayer(layerPair.second);
  //const Int_t width = layer.minWidth();
  const Int_t width = _net.minWidth();
  const Int_t extension = width / 2;

  Box<Int_t> wire;
  toWire(pt, acsPt, width, extension, wire);
  _vvRoutedWires.back().emplace_back(wire, pt.z());
  _cir.addSpatialRoutedWire(_net.idx(), pt.z(), wire);
  if (_bSym) {
    Box<Int_t> symWire(wire);
    symWire.flipX(_net.symAxisX());
    _cir.addSpatialRoutedWire(_net.symNetIdx(), pt.z(), symWire);
  }
  if (_bSelfSym) {
    Box<Int_t> symWire(wire);
    symWire.flipX(_net.symAxisX());
    _cir.addSpatialRoutedWire(_net.idx(), pt.z(), symWire);
  }
}

bool DrGridAstar::bStackedVia(const DrGridAstarNode* pU, const DrGridAstarNode* pV) {
  if (pU->coord().z() != pV->coord().z()) {
    assert(pU->coord().x() == pV->coord().x()
           and pU->coord().y() == pV->coord().y());
    if (pU->pParent()
        and pU->pParent()->coord().z() != pU->coord().z()
        and pU->pParent()->coord().z() != pV->coord().z()) {
      return true;
    }
  }
  return false;
}

PROJECT_NAMESPACE_END
