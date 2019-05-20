//
// Created by Keren on 05/03/2018
//

#ifndef ZKUTIL_VERTEXLISTGRAPH_H_
#define ZKUTIL_VERTEXLISTGRAPH_H_

/// ================================================================================ 
/// Vertex List Graph
/// A wrapper or implementation for boost vertex list graph type
/// Implementation use adjacency_list as graph.
/// It is VertexAndEdgeListGraph, MutablePropertyGraph, CopyConstructible, Assignable, and Serializable.
/// ================================================================================ 

#include <boost/assert.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include "global/global.h"

namespace klib {
/// EdgeWeightType: the type of weight of each edge
/// DirectedS: boost::undirectedS or boost::directedS. Whether the graph is directed or undirected
/// VertexList and OutEdgeList: the OutEdgeList and the VertexList: control how the actual vetices and edges are constructed
///   can use vecS : std::vector, listS : std::list, slistS : std::slist, setS : std::set multisetS : std::multiset hash_setS : boost::unordered_set
///   VertexList : how the vertices are represented. Usually listS or vecS. listS is in general faster but larger in storage
///   OutEdgeList : what kind of container is used to store the out-edges(possibly in edge).
///   It can also determined whether parallel edge in undirected graph is allowed
///   To ensure no parallel edge: setS or hash_setS
///   slist and list are typically faster than vec, faster than set and hash set for adding edge
///   look up edge, clear_vertex is faster with set/hashset
///   See: https://www.boost.org/doc/libs/1_60_0/libs/graph/doc/using_adjacency_list.html#sec:choosing-graph-type
template <typename EdgeWeightType, class DirectedS = boost::undirectedS, class OutEdgeList = boost::hash_mapS, class VertexList = boost::listS>
class VertexListGraph {
    static constexpr EdgeWeightType DEFAULT_EDGE_WEIGHT = 1; 
    public:
        /// It is possible to add vector/edge properties/attributes, include custom property. The one below is a built-in edge property
        typedef typename boost::property<boost::edge_weight_t, EdgeWeightType>                                                      EdgeWeightProperty;
        /// use the index of the vertex in _vectexArray as the name of the vertex
        typedef typename boost::property<boost::vertex_name_t, IndexType>                                                           VertexIndexNameProperty;
        typedef typename boost::adjacency_list<OutEdgeList, VertexList , DirectedS, VertexIndexNameProperty, EdgeWeightProperty>    Graph;
        typedef typename boost::graph_traits<Graph>::vertex_descriptor                                                              Vertex;
        typedef typename boost::graph_traits<Graph>::edge_descriptor                                                                Edge; 
        typedef typename boost::graph_traits<Graph>::out_edge_iterator                                                              out_edge_iterator;
        typedef typename boost::graph_traits<Graph>::in_edge_iterator                                                               in_edge_iterator;


        VertexListGraph() = default;
        VertexListGraph(IndexType vSize) :_graph(vSize) { _vertexArray.resize(vSize);}

        /// Getters
        const Graph &                       getGraph() const                                        { return _graph; }
        Graph &                             getGraph()                                              { return _graph; }
        const std::vector<Vertex> &         getVertexArray() const                                  { return _vertexArray; }
        std::vector<Vertex> &               getVertexArray()                                        { return _vertexArray; }
        const std::vector<Edge> &           getEdgeArray() const                                    { return _edgeArray; }
        std::vector<Edge> &                 getEdgeArray()                                          { return _edgeArray; }

        /// Graph building/remove
        IndexType addVertex(); 
        IndexType addEdge(IndexType fromIdx, IndexType toIdx)   { return addEdge(fromIdx, toIdx, DEFAULT_EDGE_WEIGHT); }
        IndexType addEdge(IndexType fromIdx, IndexType toIdx, EdgeWeightType edgeWeight);
        void      removeEdge(IndexType fromIdx, IndexType toIdx);
        void      removeEdge(IndexType edgeIdx);
        void      removeAllOutEdges(IndexType fromIdx);
        void      removeAllInEdges(IndexType toIdx);
        void      clearVertex(IndexType idx);
        void      removeVertex(IndexType idx);

        /// Acess
        Edge                    getEdge(IndexType fromIdx, IndexType toIdx) {
            std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
            if (!edgePair.second) { PROJECT_NAMESPACE::WRN("%s: (%u, %u) not existing.", __PRETTY_FUNCTION__, fromIdx, toIdx); }
            return edgePair.first;
        }
        Edge                    getEdge(IndexType fromIdx, IndexType toIdx) const {
            std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
            if (!edgePair.second) { PROJECT_NAMESPACE::WRN("%s: (%u, %u) not existing.", __PRETTY_FUNCTION__, fromIdx, toIdx); }
            return edgePair.first;
        }
        EdgeWeightType          edgeWeight(IndexType edgeIdx) const                  { return boost::get(boost::edge_weight_t(), _graph, _edgeArray.at(edgeIdx)); }
        EdgeWeightType          edgeWeight(IndexType fromIdx, IndexType toIdx) const { return boost::get(boost::edge_weight_t(), _graph, getEdge(fromIdx, toIdx)); }
        IndexType               sourceVertex(IndexType edgeIdx) const                { return boost::get(boost::vertex_name_t(), _graph, boost::source(_edgeArray.at(edgeIdx), _graph)); }
        IndexType               sourceVertex(Edge edgeIn) const                      { return boost::get(boost::vertex_name_t(), _graph, boost::source(edgeIn, _graph)); }
        IndexType               targetVertex(IndexType edgeIdx) const                { return boost::get(boost::vertex_name_t(), _graph, boost::target(_edgeArray.at(edgeIdx), _graph)); }
        IndexType               targetVertex(Edge edgeIn) const                      { return boost::get(boost::vertex_name_t(), _graph, boost::target(edgeIn, _graph)); }
        std::vector<IndexType>  outEdges(IndexType vertexIdx) const;
        std::vector<IndexType>  inEdges(IndexType vertexIdx) const;

        const Vertex &          getVertex(IndexType vertexIdx) const                 { return _vertexArray.at(vertexIdx); }
        Vertex &                getVertex(IndexType vertexIdx)                       { return _vertexArray.at(vertexIdx); }

        /// Modify
        bool                    modifyEdgeWeight(IndexType fromIdx, IndexType toIdx, EdgeWeightType newWeight);
        bool                    addEdgeWeight(IndexType fromIdx, IndexType toIdx, EdgeWeightType weightToAdd);
        bool                    reduceEdgeWeight(IndexType fromIdx, IndexType toIdx, EdgeWeightType weightToReduce);

        /// Size
        IndexType               vertexSize() const                                   { return _vertexArray.size(); }

        
    private:
        Graph               _graph;
        std::vector<Vertex> _vertexArray;
        std::vector<Edge>   _edgeArray;
};

/// Graph building/ajustment
template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList> 
inline IndexType VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::addVertex() {
    IndexType idx = _vertexArray.size();
    _vertexArray.emplace_back(boost::add_vertex(idx, _graph));
    return idx;
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline IndexType VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::addEdge(IndexType fromIdx, IndexType toIdx, EdgeWeightType edgeWeight) {
    std::pair<Edge, bool> returnedEdge = boost::add_edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), edgeWeight, _graph);
    if (!returnedEdge.second) {
        PROJECT_NAMESPACE::WRN("%s: Failed to add edge (%u, %u), the edge has already existed and the graph is not allowing parallel edge \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
    }
    IndexType idx = _edgeArray.size();
    _edgeArray.emplace_back(returnedEdge.first);
    return idx;
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline void VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::removeEdge(IndexType fromIdx, IndexType toIdx) {
    std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
    if (!edgePair.second) {
        PROJECT_NAMESPACE::WRN("%s: Failed to remove the return, (%u, %u) does not exist \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
    }
    boost::remove_edge(edgePair.first, _graph);
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline void VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::removeEdge(IndexType edgeIdx) {
    boost::remove_edge(_edgeArray.at(edgeIdx));
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline void VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::removeAllOutEdges(IndexType fromIdx) {
    boost::clear_out_edges(_vertexArray.at(fromIdx),  _graph);
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline void VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::removeAllInEdges(IndexType toIdx) {
    boost::clear_in_edges(_vertexArray.at(toIdx), _graph);
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline void VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::clearVertex(IndexType idx) {
    boost::clear_vertex(_vertexArray.at(idx), _graph);
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline void VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::removeVertex(IndexType idx) {
    boost::remove_vertex(_vertexArray.at(idx), _graph);
}

/// Accesss

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline std::vector<IndexType> VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::outEdges(IndexType vertexIdx) const {
    std::vector<IndexType> nodeIdxArray;
    std::pair<out_edge_iterator, out_edge_iterator> iters = out_edges(_vertexArray.at(vertexIdx), _graph);
    for (auto it = iters.first; it!=iters.second; ++it) {
        nodeIdxArray.emplace_back(targetVertex(*it));
    }
    return nodeIdxArray;
}

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline std::vector<IndexType> VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::inEdges(IndexType vertexIdx) const {
    std::vector<IndexType> nodeIdxArray;
    std::pair<in_edge_iterator, in_edge_iterator> iters = in_edges(_vertexArray.at(vertexIdx), _graph);
    for (auto it = iters.first; it!=iters.second; ++it) {
        nodeIdxArray.emplace_back(targetVertex(*it));
    }
    return nodeIdxArray;
}

/// Modify

template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline bool VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::modifyEdgeWeight(IndexType fromIdx, IndexType toIdx, EdgeWeightType newWeight) {
    std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
    if (!edgePair.second) {
        PROJECT_NAMESPACE::WRN("%s: (%u, %u) does not exist \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
        return false;
    }
    boost::put(boost::edge_weight_t(), _graph, edgePair.first, newWeight);
    return true;
}


template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline bool VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::addEdgeWeight(IndexType fromIdx, IndexType toIdx, EdgeWeightType weightToAdd) {
    std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
    if (!edgePair.second) {
        PROJECT_NAMESPACE::WRN("%s: (%u, %u) does not exist \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
        return false;
    }
    EdgeWeightType weight = boost::get(boost::edge_weight_t(), _graph, edgePair.first);
    boost::put(boost::edge_weight_t(), _graph, edgePair.first, weight + weightToAdd);
    return true;
}


template<typename EdgeWeightType, class DirectedS, class OutEdgeList, class VertexList>
inline bool VertexListGraph<EdgeWeightType, DirectedS, OutEdgeList, VertexList>::reduceEdgeWeight(IndexType fromIdx, IndexType toIdx, EdgeWeightType weightToReduce) {
    std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
    if (!edgePair.second) {
        PROJECT_NAMESPACE::WRN("%s: (%u, %u) does not exist \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
        return false;
    }
    EdgeWeightType weight = boost::get(boost::edge_weight_t(), _graph, edgePair.first);
    boost::put(boost::edge_weight_t(), _graph, edgePair.first, weight - weightToReduce);
    AssertMsg(boost::get(boost::edge_weight_t(), _graph, edgePair.first) <= weight, "%s: reduced weight overflow \n", __PRETTY_FUNCTION__);
    return true;
}


} /// namespace klib
#endif ///ZKUTIL_VERTEXLISTGRAPH_H_
