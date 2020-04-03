/**
 * @file CSFlow.cpp
 * @brief Compute current & signal flow of a CktGraph
 * @author Hao Chen
 * @date 03/30/2020
 */

#include <queue>
#include <set>

#include "CSFlow.h"

PROJECT_NAMESPACE_BEGIN

void CSFlow::computeCurrentFlow(CktGraph& ckt) {
  for (IndexType i = 0; i < ckt.numPins(); ++i) {
    Pin& pin1 = ckt.pin(i);
    Net& net1 = ckt.net(pin1.netIdx());
    CktNode& node1 = ckt.node(pin1.nodeIdx());

    ImplType implType1 = getCktNodeImplType(node1);
    
    if (implType1 == ImplType::PCELL_Pch
        and i == node1.pinIdx(2)
        and net1.isVdd()) {
      for (IndexType j = 0; j < ckt.numPins(); ++j) {
        Pin& pin2 = ckt.pin(j);
        Net& net2 = ckt.net(pin2.netIdx());
        CktNode& node2 = ckt.node(pin2.nodeIdx());
        ImplType implType2 = getCktNodeImplType(node2);
        if (implType2 == ImplType::PCELL_Nch
            and j == node2.pinIdx(2)
            and net2.isVss()) {
          std::vector<std::string> pinNames;
          std::vector<std::string> cellNames;
          std::vector<unsigned char> visited(ckt.numPins(), 0);
          currentDFS(ckt, i, j, visited, pinNames, cellNames);
        }
      }
    }
  }
  //for (auto& vec : _currentCellPaths) {
    //for (auto pinIdx : vec) {
      //std::cerr << pinIdx << " ";
    //}
    //std::cerr << std::endl;
  //}
}

void CSFlow::currentDFS(CktGraph& ckt, const IndexType sPinIdx, const IndexType tPinIdx, std::vector<unsigned char>& visited, std::vector<std::string>& pinNames, std::vector<std::string>& cellNames) {
  Pin& pin = ckt.pin(sPinIdx);
  Net& net = ckt.net(pin.netIdx());
  CktNode& node = ckt.node(pin.nodeIdx());

  const auto &pinName = _db.subCkt(node.subgraphIdx()).net(pin.intNetIdx()).name();
  
  visited[sPinIdx] = 1;
  pinNames.emplace_back(pinName);
  cellNames.emplace_back(node.name());

  if (sPinIdx == tPinIdx) {
    _currentPinPaths.emplace_back(pinNames);
    _currentCellPaths.emplace_back(cellNames);
    visited[sPinIdx] = 0;
    pinNames.pop_back();
    cellNames.pop_back();
    return;
  }


  std::vector<IndexType> adjPins;
 
  ImplType implType = getCktNodeImplType(node);

  switch (implType) {
    case ImplType::PCELL_Pch:
    {
        
      // source -> drain
      if (sPinIdx == node.pinIdx(2)) {
        if (!visited[node.pinIdx(0)])
          adjPins.emplace_back(node.pinIdx(0));
      }
      // drain -> other
      else if (sPinIdx == node.pinIdx(0)) {
        for (IndexType pinIdx : net.pinIdxArray()) {
          if (!visited[pinIdx]) {
            CktNode& adjNode = ckt.node(ckt.pin(pinIdx).nodeIdx());
            ImplType adjNodeImpl = getCktNodeImplType(adjNode);
            if ((adjNodeImpl == ImplType::PCELL_Pch and pinIdx == adjNode.pinIdx(2))
                or (adjNodeImpl == ImplType::PCELL_Nch and pinIdx == adjNode.pinIdx(0)))
              adjPins.emplace_back(pinIdx);
          }
        }  
      }
      // gate or body
      else {
      }
      break;
    }
    case ImplType::PCELL_Nch:
    {
      // drain -> source
      if (sPinIdx == node.pinIdx(0)) {
        if (!visited[node.pinIdx(2)])
          adjPins.emplace_back(node.pinIdx(2));
      }
      // source -> other
      else if (sPinIdx == node.pinIdx(2)) {
        for (IndexType pinIdx : net.pinIdxArray()) {
          if (!net.isVss() and !visited[pinIdx]) {
            CktNode& adjNode = ckt.node(ckt.pin(pinIdx).nodeIdx());
            ImplType adjNodeImpl = getCktNodeImplType(adjNode);
            if (adjNodeImpl == ImplType::PCELL_Nch and pinIdx == adjNode.pinIdx(0))
              adjPins.emplace_back(pinIdx);
          }
        }
      }
      // gate or body
      else {
      }
      break;
    }
    default:
      break;
  }


  for (IndexType pinIdx : adjPins) {
    assert(!visited[pinIdx]);
    currentDFS(ckt, pinIdx, tPinIdx, visited, pinNames, cellNames);
  }

  visited[sPinIdx] = 0;
  pinNames.pop_back();
  cellNames.pop_back();
}

void CSFlow::computeSignalFlow(CktGraph& ckt) {
  
}

ImplType CSFlow::getCktNodeImplType(const CktNode& node) {
  const CktGraph& nodeSubGraph = _db.subCkt(node.subgraphIdx());
  return nodeSubGraph.implType();
}

PROJECT_NAMESPACE_END
