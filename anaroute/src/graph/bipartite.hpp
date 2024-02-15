/**
 * @file   bipartite.hpp
 * @brief  Graph algorithms - bipartite
 * @author Hao Chen
 * @date   02/29/2020
 *
 **/

#ifndef _GRAPH_BIPARTITE_HPP_
#define _GRAPH_BIPARTITE_HPP_

#include <lemon/smart_graph.h>
#include <lemon/matching.h>

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

class BipartiteMatching {

  using Graph       = lemon::SmartBpGraph;
  using RedNode     = typename Graph::RedNode;
  using BlueNode    = typename Graph::BlueNode;
  using Node        = typename Graph::Node;
  using Edge        = typename Graph::Edge;
  using MatchingMap = typename lemon::MaxMatching<Graph>::MatchingMap;
  using Solver      = lemon::template MaxMatching<Graph>;

 public:
  explicit BipartiteMatching(const Int_t numRed, const Int_t numBlue)
    : _solver(_graph)
  {
    _graph.reserveNode(numRed + numBlue);
    _vRedNodes.reserve(numRed);
    _vBlueNodes.reserve(numBlue);
    for (Int_t i = 0; i < numRed; ++i) {
      _vRedNodes.emplace_back(_graph.addRedNode());
    }
    for (Int_t i = 0; i < numBlue; ++i) {
      _vBlueNodes.emplace_back(_graph.addBlueNode());
    }
  }
  ~BipartiteMatching() {}

  void addEdge(const Int_t redIdx, const Int_t blueIdx) {
    _graph.addEdge(_vRedNodes[redIdx], _vBlueNodes[blueIdx]);
  }

  void solve(Vector_t<Pair_t<Int_t, Int_t>>& vEdges) {
    _solver.run();
    const MatchingMap& map = _solver.matchingMap();
    for (const RedNode& node : _vRedNodes) {
      const Edge& edge = map[node];
      if (edge != lemon::INVALID)
        vEdges.emplace_back(_graph.id(_graph.redNode(edge)),
                            _graph.id(_graph.blueNode(edge)));
    }
    std::sort(vEdges.begin(), vEdges.end());
    vEdges.resize(std::unique(vEdges.begin(), vEdges.end()) - vEdges.begin());
  }

 private:
  Graph               _graph;
  Vector_t<RedNode>   _vRedNodes;
  Vector_t<BlueNode>  _vBlueNodes;
  Solver              _solver;
};

template<typename CostType>
class BipartiteWeightedMatching {

  using Graph       = lemon::SmartBpGraph;
  using RedNode     = typename Graph::RedNode;
  using BlueNode    = typename Graph::BlueNode;
  using Node        = typename Graph::Node;
  using Edge        = typename Graph::Edge;
  using EdgeMap     = typename Graph::EdgeMap<CostType>;
  using MatchingMap = typename lemon::MaxWeightedMatching<Graph>::MatchingMap;
  using Solver      = lemon::template MaxWeightedMatching<Graph, EdgeMap>;

 public:
  explicit BipartiteWeightedMatching(const Int_t numRed, const Int_t numBlue)
    : _edgeMap(_graph), _solver(_graph, _edgeMap)
  {
    _graph.reserveNode(numRed + numBlue);
    _vRedNodes.reserve(numRed);
    _vBlueNodes.reserve(numBlue);
    for (Int_t i = 0; i < numRed; ++i) {
      _vRedNodes.emplace_back(_graph.addRedNode());
    }
    for (Int_t i = 0; i < numBlue; ++i) {
      _vBlueNodes.emplace_back(_graph.addBlueNode());
    }
  }
  ~BipartiteWeightedMatching() {}

  void addEdge(const Int_t redIdx, const Int_t blueIdx, const CostType c) {
    Edge edge = _graph.addEdge(_vRedNodes[redIdx], _vBlueNodes[blueIdx]);
    _edgeMap[edge] = c;
  }

  void solve(Vector_t<Pair_t<Int_t, Int_t>>& vEdges) {
    _solver.run();
    const MatchingMap& map = _solver.matchingMap();
    for (const RedNode& node : _vRedNodes) {
      const Edge& edge = map[node];
      if (edge != lemon::INVALID)
        vEdges.emplace_back(_graph.id(_graph.redNode(edge)),
                            _graph.id(_graph.blueNode(edge)));
    }
    std::sort(vEdges.begin(), vEdges.end());
    vEdges.resize(std::unique(vEdges.begin(), vEdges.end()) - vEdges.begin());
  }

 private:
  Graph               _graph;
  Vector_t<RedNode>   _vRedNodes;
  Vector_t<BlueNode>  _vBlueNodes;
  EdgeMap             _edgeMap;
  Solver              _solver;
};

PROJECT_NAMESPACE_END

#endif /// _GRAPH_BIPARTITE_HPP_

