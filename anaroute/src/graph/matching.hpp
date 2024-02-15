/**
 * @file   matching.hpp
 * @brief  Graph algorithms - Matching
 * @author Hao Chen
 * @date   02/28/2020
 *
 **/

#ifndef _GRAPH_MATCHING_HPP_
#define _GRAPH_MATCHING_HPP_

#include <lemon/smart_graph.h>
#include <lemon/matching.h>

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

template<typename GraphType = lemon::SmartGraph>
class MaxMatching {

  using Graph       = GraphType;
  using Node        = typename Graph::Node;
  using Edge        = typename Graph::Edge;
  //using Status      = typename lemon::template MaxMatching<Graph>::Status;
  //using StatusMap   = typename lemon::template MaxMatching<Graph>::StatusMap; 
  using MatchingMap = typename lemon::template MaxMatching<Graph>::MatchingMap;
  using Solver      = lemon::template MaxMatching<Graph>;

 public:
  explicit MaxMatching(const Int_t numNodes)
    : _solver(_graph)
  {
    _graph.reserveNode(numNodes);
    _vNodes.reserve(numNodes);
    for (Int_t i = 0; i < numNodes; ++i) {
      _vNodes.emplace_back(_graph.addNode());
    }
  }
  
  ~MaxMatching() {}

  void addEdge(const Int_t u, const Int_t v) {
    _graph.addEdge(_vNodes[u], _vNodes[v]);
  }

  void solve(Vector_t<Pair_t<Int_t, Int_t>>& vEdges) {
    _solver.run();
    const MatchingMap& map = _solver.matchingMap();
    for (const Node& node : _vNodes) {
      const Edge& edge = map[node];
      if (edge != lemon::INVALID)
        vEdges.emplace_back(_graph.id(_graph.u(edge)), _graph.id(_graph.v(edge))); 
    }
    std::sort(vEdges.begin(), vEdges.end());
    vEdges.resize(std::unique(vEdges.begin(), vEdges.end()) - vEdges.begin());
  }

  Int_t mate(const Int_t u) {
    auto val = _solver.mate(_vNodes[u]);
    return (val == lemon::INVALID) ? -1 : _graph.id(val);
  }
  
  Pair_t<Int_t, Int_t> matching(const Int_t u) {
    Edge edge = _solver.matching(u);
    return (edge == lemon::INVALID) ?
            std::make_pair(-1, -1) :
            std::make_pair(_graph.id(_graph.u(edge)), _graph.id(_graph.v(edge)));
  }
  

 private:
  Graph           _graph;
  Vector_t<Node>  _vNodes;
  Solver          _solver;
};

template<typename CostType, typename GraphType = lemon::SmartGraph>
class MaxWeightedMatching {
  
  using Graph       = GraphType;
  using Node        = typename Graph::Node;
  using Edge        = typename Graph::Edge;
  using EdgeMap     = typename Graph::template EdgeMap<CostType>;
  using MatchingMap = typename lemon::template MaxWeightedMatching<Graph>::MatchingMap;
  using Solver      = lemon::template MaxWeightedMatching<Graph, EdgeMap>;
 
 public:
  explicit MaxWeightedMatching(const Int_t numNodes)
    : _edgeMap(_graph), _solver(_graph, _edgeMap)
  {
    _graph.reserveNode(numNodes);
    _vNodes.reserve(numNodes);
    for (Int_t i = 0; i < numNodes; ++i) {
      _vNodes.emplace_back(_graph.addNode());
    }
  }
  ~MaxWeightedMatching() {}

  void addEdge(const Int_t u, const Int_t v, const CostType c) {
    Edge edge = _graph.addEdge(_vNodes[u], _vNodes[v]);
    _edgeMap[edge] = c;
  }

  void solve(Vector_t<Pair_t<Int_t, Int_t>>& vEdges) {
    _solver.run();
    const MatchingMap& map = _solver.matchingMap();
    for (const Node& node : _vNodes) {
      const Edge& edge = map[node];
      if (edge != lemon::INVALID)
        vEdges.emplace_back(_graph.id(_graph.u(edge)), _graph.id(_graph.v(edge)));
    }
    std::sort(vEdges.begin(), vEdges.end());
    vEdges.resize(std::unique(vEdges.begin(), vEdges.end()) - vEdges.begin());
  }

  CostType matchingWeight() {
    return _solver.matchingWeight();
  }

  Int_t mate(const Int_t u) {
    auto val = _solver.mate(_vNodes[u]);
    return (val == lemon::INVALID) ? -1 : _graph.id(val);
  }
  
  Pair_t<Int_t, Int_t> matching(const Int_t u) {
    Edge edge = _solver.matching(u);
    return (edge == lemon::INVALID) ?
            std::make_pair(-1, -1) :
            std::make_pair(_graph.id(_graph.u(edge)), _graph.id(_graph.v(edge)));
  }

 private:
  Graph           _graph;
  Vector_t<Node>  _vNodes;
  EdgeMap         _edgeMap;
  Solver          _solver;
};

template<typename CostType, typename GraphType = lemon::SmartGraph>
class MaxWeightedPerfectMatching {
  
  using Graph       = GraphType;
  using Node        = typename Graph::Node;
  using Edge        = typename Graph::Edge;
  using EdgeMap     = typename Graph::template EdgeMap<CostType>;
  using MatchingMap = typename lemon::template MaxWeightedMatching<Graph>::MatchingMap;
  using Solver      = lemon::template MaxWeightedMatching<Graph, EdgeMap>;
 
 public:
  explicit MaxWeightedPerfectMatching(const Int_t numNodes)
    : _edgeMap(_graph), _solver(_graph, _edgeMap)
  {
    _graph.reserveNode(numNodes);
    _vNodes.reserve(numNodes);
    for (Int_t i = 0; i < numNodes; ++i) {
      _vNodes.emplace_back(_graph.addNode());
    }
  }
  ~MaxWeightedPerfectMatching() {}
  
  void addEdge(const Int_t u, const Int_t v, const CostType c) {
    Edge edge = _graph.addEdge(_vNodes[u], _vNodes[v]);
    _edgeMap[edge] = c;
  }

  void solve(Vector_t<Pair_t<Int_t, Int_t>>& vEdges) {
    _solver.run();
    const MatchingMap& map = _solver.matchingMap();
    for (const Node& node : _vNodes) {
      const Edge& edge = map[node];
      if (edge != lemon::INVALID)
        vEdges.emplace_back(_graph.id(_graph.u(edge)), _graph.id(_graph.v(edge)));
    }
    std::sort(vEdges.begin(), vEdges.end());
    vEdges.resize(std::unique(vEdges.begin(), vEdges.end()) - vEdges.begin());
  }

  CostType matchingWeight() {
    return _solver.matchingWeight();
  }

  Int_t mate(const Int_t u) {
    auto val = _solver.mate(_vNodes[u]);
    return (val == lemon::INVALID) ? -1 : _graph.id(val);
  }
  
  Pair_t<Int_t, Int_t> matching(const Int_t u) {
    Edge edge = _solver.matching(u);
    return (edge == lemon::INVALID) ?
            std::make_pair(-1, -1) :
            std::make_pair(_graph.id(_graph.u(edge)), _graph.id(_graph.v(edge)));
  }


 private:
  Graph           _graph;
  Vector_t<Node>  _vNodes;
  EdgeMap         _edgeMap;
  Solver          _solver;
};

PROJECT_NAMESPACE_END

#endif /// _GRAPH_MATCHING_HPP_

