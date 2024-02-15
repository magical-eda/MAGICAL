/**
 * @file   mst.hpp
 * @brief  Graph algorithms - MST
 * @author Hao Chen
 * @date   02/28/2020
 *
 **/

#ifndef _GRAPH_MST_HPP_
#define _GRAPH_MST_HPP_

#include <lemon/smart_graph.h>
#include <lemon/kruskal.h>

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

template<typename CostType, typename GraphType = lemon::SmartGraph>
class MinimumSpanningTree {
 
  using Graph   = GraphType;
  using Node    = typename GraphType::Node;
  using Edge    = typename GraphType::Edge;
  using EdgeMap = typename GraphType::template EdgeMap<CostType>;

 public:
  explicit MinimumSpanningTree(const Int_t numNodes)
    : _edgeMap(_graph)
  {
    _vNodes.reserve(numNodes);
    for (Int_t i = 0; i < numNodes; ++i) {
      _vNodes.emplace_back(_graph.addNode());
    }
  }

  ~MinimumSpanningTree() {}

  void addEdge(const Int_t u, const Int_t v, const CostType c) {
    Edge edge = _graph.addEdge(_vNodes[u], _vNodes[v]);
    _edgeMap[edge] = c;
  }

  // return the cost of the mst
  CostType solve(Vector_t<Pair_t<Int_t, Int_t>>& vEdges) {
    Vector_t<Edge> vRes;
    const Int_t cost = lemon::kruskal(_graph, _edgeMap, std::back_inserter(vRes));
    for (Int_t i = 0; i < (Int_t)vRes.size(); ++i) {
      const Edge& edge = vRes[i];
      vEdges.emplace_back(_graph.id(_graph.u(edge)), _graph.id(_graph.v(edge)));
    }
    return cost;
  }



 private:
  Graph           _graph;
  EdgeMap         _edgeMap;
  Vector_t<Node>  _vNodes;
};

PROJECT_NAMESPACE_END

#endif /// _GRAPH_MST_HPP_

