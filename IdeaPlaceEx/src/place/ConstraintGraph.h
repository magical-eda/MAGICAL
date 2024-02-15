/**
 * @file ConstraintGraph.h
 * @brief The constraint graph implementation with boost graph
 * @author Keren Zhu
 * @date 11/25/2019
 */

#ifndef IDEAPLACE_CONSTRAINT_GRAPH_H_
#define IDEAPLACE_CONSTRAINT_GRAPH_H_

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

class ConstraintGraph
{
    public:
        /// @brief default constructor
        explicit ConstraintGraph() 
        {
            _cg.clear();
        }
        typedef boost::adjacency_list < boost::setS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property < boost::edge_weight_t, IntType > > graph_t;
        typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
        typedef boost::graph_traits < graph_t >::edge_descriptor edge_descriptor;
        typedef boost::graph_traits < graph_t >::edge_iterator edge_iterator;
        typedef boost::property_map<graph_t, boost::vertex_index_t>::type IndexMap;
        typedef boost::graph_traits < graph_t >::adjacency_iterator adjacency_iterator;
        /// @brief return the boost graph
        graph_t & boostGraph() { return _cg; }
        /// @brief construct the graph with number of vertices
        /// @param the number of vertices
        void allocateVertices(IndexType numVex) { _cg.clear(); _cg = graph_t(numVex);}
        /// @brief get the number of nodes
        /// @return the number of nodes
        IndexType numNodes() const { return boost::num_vertices(_cg); }
        /// @brief get the source node index
        /// @return the index of the source node
        IndexType sourceNodeIdx() const { return numNodes() - 2;}
        /// @brief get the target node index
        /// @return the index of the target node
        IndexType targetNodeIdx() const { return numNodes() - 1; }
        /// @brief get the number of cells 
        /// @return the number of cell nodes in the graph
        IndexType numCellNodes() const { return numNodes() - 2; }
        /// @brief add edge to the graph
        /// @param the source node index
        /// @param the target node index
        void addEdge(IndexType sourceIdx, IndexType targetIdx, IntType weight=1)
        {
            boost::add_edge(boost::vertex(sourceIdx, _cg),
                    boost::vertex(targetIdx, _cg),
                    weight, _cg);
        }
        /// @brief remove a edge from the graph
        /// @param the source index
        /// @param the target index
        void removeEdge(IndexType sourceIdx, IndexType targetIdx)
        {
            boost::remove_edge(boost::vertex(sourceIdx, _cg), 
                    boost::vertex(targetIdx, _cg), 
                    _cg);
        }
        /// @brief determine whether the graph has one specific edge
        /// @param the source index of the edge
        /// @param the target index of the edge
        /// @return true if has edge. false if not
        bool hasEdge(IndexType sourceIdx, IndexType targetIdx)
        {
            auto edge = boost::edge(boost::vertex(sourceIdx, _cg),
                    boost::vertex(targetIdx, _cg), _cg);
            return edge.second;
        }
        
        void clear()
        {
            _cg.clear();
        }

    private:
        graph_t _cg; ///< The boost graph
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_CONSTRAINT_GRAPH_H_
