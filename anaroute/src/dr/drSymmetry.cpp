/**
 * @file   drSymmetry.cpp
 * @brief  Degree of symmetric detection
 * @author Hao Chen
 * @date   02/26/2020
 *
 **/

#include "drSymmetry.hpp"
#include "src/graph/matching.hpp"

using namespace std;

PROJECT_NAMESPACE_START

void DrSymmetry::solve(const bool bUseSymFile) {
  if (bUseSymFile) {
    for (UInt_t i = 0; i < _cir.numNets(); ++i) {
      Net& net = _cir.net(i);
      net.setSymAxisX(_cir.symAxisX());
    }
    return;
  }
  
  fprintf(stderr, "DrSymmetry::%s Start max symmetric detection\n", __func__);

  MaxWeightedMatching<Float_t> mx(2 * _cir.numNets());

  Vector_t<Vector_t<Int_t>> vvAxis;
  vvAxis.resize(_cir.numNets());
  for (auto& vec : vvAxis)
    vec.resize(_cir.numNets(), -1);

  for (UInt_t i = 0; i < _cir.numNets(); ++i) {
    Net& net1 = _cir.net(i);
    for (UInt_t j = i; j < _cir.numNets(); ++j) {
      Net& net2 = _cir.net(j);
      // prohibit power nets to be sym with other nets
      if (i != j) {
        if (net1.bPower() or net2.bPower())
          continue;
      }
      Int_t bestSymAxisX = 0;
      Float_t degSym = 0;
      bestMatching(net1, net2, bestSymAxisX, degSym);
      //cerr << net1.name() << " " << net2.name() << " " << degSym << endl;
      
      vvAxis[i][j] = bestSymAxisX;
      vvAxis[j][i] = bestSymAxisX;
      if (j == i) {
        mx.addEdge(i, i + _cir.numNets(), degSym);
      }
      else {
        mx.addEdge(i, j, degSym);
      }
    }
  }
  
  Vector_t<Pair_t<Int_t, Int_t>> vEdges;
  mx.solve(vEdges);

  for (const auto pair : vEdges) {
    Int_t netIdx1 = pair.first;
    Int_t netIdx2 = pair.second;
    if (netIdx1 >= (Int_t)_cir.numNets())
      netIdx1 -= _cir.numNets();
    if (netIdx2 >= (Int_t)_cir.numNets())
      netIdx2 -= _cir.numNets();

    const Int_t symAxisX = vvAxis[netIdx1][netIdx2];
    assert(symAxisX != -1);
    if (netIdx1 == netIdx2) {
      Net& net = _cir.net(netIdx1);
      net.setSymAxisX(symAxisX);
      net.setSelfSym();
      fprintf(stderr, "DrSymmetry::%s Self-sym: %s\n", __func__, net.name().c_str());
      //cerr << net.name() << endl;
      
    }
    else {
      Net& net1 = _cir.net(netIdx1);
      Net& net2 = _cir.net(netIdx2);
      net1.setSymAxisX(symAxisX);
      net2.setSymAxisX(symAxisX);
      net1.setSymNetIdx(net2.idx());
      net2.setSymNetIdx(net1.idx());

      net1.setMinWidth(std::max(net1.minWidth(), net2.minWidth()));
      net1.setMinCuts(std::max(net1.minCuts(), net2.minCuts()));
      net1.setNumCutsRow(std::max(net1.numCutsRow(), net2.numCutsRow()));
      net1.setNumCutsCol(std::max(net1.numCutsCol(), net2.numCutsCol()));
      
      net2.setMinWidth(std::max(net1.minWidth(), net2.minWidth()));
      net2.setMinCuts(std::max(net1.minCuts(), net2.minCuts()));
      net2.setNumCutsRow(std::max(net1.numCutsRow(), net2.numCutsRow()));
      net2.setNumCutsCol(std::max(net1.numCutsCol(), net2.numCutsCol()));
      
      fprintf(stderr, "DrSymmetry::%s Sym:      %s %s\n", __func__, net1.name().c_str(), net2.name().c_str());
      //cerr << net1.name() << " " << net2.name() << endl;
    }
    
  }
}

void DrSymmetry::bestMatching(const Net& net1, const Net& net2, Int_t& bestSymAxisX, Float_t& maxDegSymPre) {
  auto accumX = [&] (const Int_t pinIdx) -> Pair_t<Int_t, Int_t> {
    const Pin& pin = _cir.pin(pinIdx);
    UInt_t i, layerIdx;
    const Box<Int_t>* cpBox;
    Int_t sumX = 0, boxCnt = 0;
    Pin_ForEachLayerIdx(pin, layerIdx) {
      Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
        sumX += cpBox->centerX();
        boxCnt += 1;
      }
    }
    return std::make_pair(sumX, boxCnt);
  };
  maxDegSymPre = 0;
  for (const Int_t pinIdx1 : net1.vPinIndices()) {
    const auto pair1 = accumX(pinIdx1);
    for (const Int_t pinIdx2 : net2.vPinIndices()) {
      const auto pair2 = accumX(pinIdx2);
      const Int_t symAxisX = (pair1.first + pair2.first) / (pair1.second + pair2.second);
      Float_t degSym = degSymPre(net1, net2, symAxisX);
      if (degSym > maxDegSymPre) {
        maxDegSymPre = degSym;
        bestSymAxisX = symAxisX;
      }
    }
  }
}

Float_t DrSymmetry::degSymPre(const Net& net1, const Net& net2, const Int_t symAxisX) {
  if (&net1 == &net2) {
    return degSelfSymPre(net1, symAxisX);
  }
  // construct pin locations vector
  Vector_t<Vector_t<Box<Int_t>>> vvBoxes;
  addPinShapes(net2, vvBoxes);

  // count match pins
  Int_t match = 0;
  for (const Int_t pinIdx : net1.vPinIndices()) {
    const Pin& pin = _cir.pin(pinIdx);
    if (bExistTotallySymPin(pin, symAxisX, vvBoxes))
      ++match;
  }
  const Int_t numPins = std::max(net1.numPins(), net2.numPins());
  Float_t ratio = (Float_t)match / numPins * 2; // * 2 because of sym
  return ratio;
}

Float_t DrSymmetry::degSelfSymPre(const Net& net, const Int_t symAxisX) {
  // construct pin locations vector
  Vector_t<Vector_t<Box<Int_t>>> vvBoxes;
  addPinShapes(net, vvBoxes);

  // count match pins
  Int_t match = 0;
  for (const Int_t pinIdx : net.vPinIndices()) {
    const Pin& pin = _cir.pin(pinIdx);
    if (bExistTotallySymPin(pin, symAxisX, vvBoxes))
      ++match;
  }
  Float_t ratio = (Float_t)match / net.numPins();
  return ratio;
}

void DrSymmetry::addPinShapes(const Net& net, Vector_t<Vector_t<Box<Int_t>>>& vvBoxes) {
  vvBoxes.resize(_cir.lef().numLayers());
  for (const Int_t pinIdx : net.vPinIndices()) {
    UInt_t i, layerIdx;
    const Box<Int_t>* cpBox;
    const Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
        vvBoxes[layerIdx].emplace_back(*cpBox);
      }
    }
  }
  for (auto& vBoxes : vvBoxes) {
    std::sort(vBoxes.begin(), vBoxes.end());
  }
}

bool DrSymmetry::bExistTotallySymPin(const Pin& pin, const Int_t symAxisX, const Vector_t<Vector_t<Box<Int_t>>>& vvSymBoxes) {
  UInt_t i, layerIdx;
  const Box<Int_t>* cpBox;
  Pin_ForEachLayerIdx(pin, layerIdx) {
    Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
      Box<Int_t> symBox(*cpBox);
      symBox.flipX(symAxisX);
      if (!std::binary_search(vvSymBoxes[layerIdx].begin(), vvSymBoxes[layerIdx].end(), symBox)) {
        return false;
      }
    }
  }
  return true;
}


PROJECT_NAMESPACE_END
