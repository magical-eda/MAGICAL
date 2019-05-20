/**
 * @file   GreedyColoring.h
 * @brief  coloring a graph with saturation degree based heuristics
 *
 * Implement the algorithm in 
 * "New Methods to Color the Vertics of a Graph"
 * Daniel Brelaz, 
 * CACM, 1979
 *
 * @author Yibo Lin
 * @date   Feb 2015
 */

#ifndef LIMBO_ALGORITHMS_COLORING_GREEDYCOLORING
#define LIMBO_ALGORITHMS_COLORING_GREEDYCOLORING

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <boost/graph/graph_concepts.hpp>
using std::cout;
using std::endl;
using std::vector;
using std::set;
using std::map;

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

/// @class limbo::algorithms::coloring::DsatColoring
/// Coloring a graph with saturation degree based heuristics
/// @tparam GraphType graph type 
template <typename GraphType>
class DsatColoring
{
	public:
        /// @nowarn 
		typedef GraphType graph_type;
		typedef typename boost::graph_traits<graph_type>::vertex_descriptor graph_vertex_type;
        /// @endnowarn

		/// for sorting and comparison;  
		/// calculate saturation degree 
		class saturation_degree_type
		{
			public:
                /// constructor 
                /// @param dc bind @ref limbo::algorithms::coloring::DsatColoring object 
				saturation_degree_type(DsatColoring const& dc) : m_dc(dc) {}

                /// calculate saturation degree 
                /// @param v vertex 
                /// @return saturation degree 
				int saturation_degree(graph_vertex_type const& v) const 
				{
					set<int> sColor; // used colors 
					typename boost::graph_traits<graph_type>::out_edge_iterator ite, ite_end;
					for (boost::tie(ite, ite_end) = boost::out_edges(v, m_dc.m_graph); 
							ite != ite_end; ++ite)
					{
						BOOST_AUTO(found, m_dc.m_mColor.find(boost::target(*ite, m_dc.m_graph)));
						assert(found != m_dc.m_mColor.end());
						int color = found->second;
						if (color >= 0)
							sColor.insert(color);
					}

					return sColor.size();
				}
				/// sort by saturation_degree degree first and then by degree
                /// @param v1 vertex 
                /// @param v2 vertex 
                /// @return comparison result 
				bool operator()(graph_vertex_type const& v1, graph_vertex_type const& v2) const
				{
					return this->saturation_degree(v1) < this->saturation_degree(v2)
						|| boost::out_degree(v1, m_dc.m_graph) < boost::out_degree(v2, m_dc.m_graph);
				}
			protected:
				DsatColoring const& m_dc; ///< bind DsatColoring class
		};

        /// constructor 
        /// @param g graph 
		DsatColoring(graph_type const& g) : m_graph(g)
		{
			BGL_FORALL_VERTICES_T(v, g, graph_type)
			{
				m_mColor[v] = -1;
			}
		}

        /// access to color map 
        /// @return reference to color map 
		map<graph_vertex_type, int> const& color_map() const {return m_mColor;}
        /// get color of vertex 
        /// @param v vertex 
        /// @return color 
		int color(graph_vertex_type v) const 
		{
			BOOST_AUTO(found, m_mColor.find(v));
			if (found == m_mColor.end()) return -1;
			else return found->second;
		}

        /// API to run the algorithm 
        /// @return objective 
		int operator()()
		{
			return this->run();
		}
	protected:
        /// kernel function to run the algorithm 
        /// @return objective 
		int run()
		{
			vector<graph_vertex_type> vNode;
			vNode.reserve(boost::num_vertices(m_graph));
			BGL_FORALL_VERTICES_T(v, m_graph, graph_type)
			{
				vNode.push_back(v);
			}

			int color_cnt = 0; // total number of colors used 
			// color assignment starts from 0 to color_cnt-1
			while (!vNode.empty())
			{
				// choose a node
				typename vector<graph_vertex_type>::iterator itv = std::max_element(vNode.begin(), vNode.end(), saturation_degree_type(*this));

				// assign color to current node 
				set<int> sUsedColor; // used colors 
				typename boost::graph_traits<graph_type>::out_edge_iterator ite, ite_end;
				for (boost::tie(ite, ite_end) = boost::out_edges(*itv, m_graph);
						ite != ite_end; ++ite)
				{
					int color = m_mColor[boost::target(*ite, m_graph)];
					sUsedColor.insert(color);
				}
				for (int i = 0; i < color_cnt+1; ++i)
				{
					if (!sUsedColor.count(i))
					{
						m_mColor[*itv] = i;
						break;
					}
				}
				assert(m_mColor[*itv] >= 0);
				color_cnt = std::max(color_cnt, 1+m_mColor[*itv]);

				// erase itv 
				*itv = vNode.back();
				vNode.pop_back();
			}

			return color_cnt;
		}

		graph_type const& m_graph; ///< graph 
		map<graph_vertex_type, int> m_mColor; ///< color map 

};

} // namespace coloring
} // namespace algorithms
} // namespace limbo
	
#endif
