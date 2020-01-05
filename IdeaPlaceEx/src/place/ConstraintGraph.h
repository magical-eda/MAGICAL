/**
 * @file ConstraintGraph.h
 * @brief The constraint graph implementation with boost graph
 * @author Keren Zhu
 * @date 11/25/2019
 */

#ifndef IDEAPLACE_CONSTRAINT_GRAPH_H_
#define IDEAPLACE_CONSTRAINT_GRAPH_H_

/* Boost libraries. Just copied from Biying's include */

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/bellman_ford_shortest_paths.hpp>
//#include <boost/property_map/property_map.hpp>
// 
//#include <boost/graph/bron_kerbosch_all_cliques.hpp>
//#include <boost/dynamic_bitset.hpp>
//#include <boost/timer/timer.hpp>
//
//#include <boost/geometry/geometries/geometries.hpp>
//#include <boost/geometry/index/rtree.hpp>

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
        /// @brief add edge 
        /// @param the source vertex
        /// @param the target vertex
        /// @param the weight of the edge
        void addEdge(IndexType sourceVex, IndexType targetVex, IntType weight)
        {
            boost::add_edge(sourceVex, targetVex, weight, _cg);
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
