/**
 * @file   MaxClique.h
 * @brief  use boost::bron_kerbosch_all_cliques to find all cliques and the maximum ones 
 * @author Yibo Lin
 * @date   May 2015
 */

#ifndef LIMBO_ALGORITHMS_MAXCLIQUE_H
#define LIMBO_ALGORITHMS_MAXCLIQUE_H

#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using std::vector;

#include <deque>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{

/// @brief callback for boost::bron_kerbosch_all_cliques
/// @tparam GraphType graph type 
template <typename GraphType>
struct max_clique_visitor_type
{
    /// @nowarn
	typedef GraphType graph_type;
	typedef typename boost::graph_traits<graph_type>::vertex_descriptor vertex_descriptor_type; 
	typedef vector<vertex_descriptor_type> clique_type;
	typedef vector<clique_type> clique_container_type;
    /// @endnowarn

	clique_container_type& vClique; ///< container to store cliques 

    /// constructor 
    /// @param vc clique container 
	max_clique_visitor_type(clique_container_type& vc) : vClique(vc) {}
    /// copy constructor 
    /// @param rhs a clique_container_type object 
	max_clique_visitor_type(clique_container_type const& rhs) : vClique(rhs.vClique) {}

    /// @tparam CliqueType container type for vertices of a clique 
	/// @param c clique vertices in graph 
    /// @param cg graph 
	template <typename CliqueType>
	void clique(CliqueType const& c, graph_type const& cg)
	{
		// convert to vertices in original graph

		// for debug 
		if (boost::num_vertices(cg) > 0)
			assert(!c.empty());

		vClique.push_back(clique_type(c.begin(), c.end()));
	}
};

/// @brief use boost::bron_kerbosch_all_cliques to find all cliques and the maximum ones 
/// @tparam GraphType graph type 
/// @param g graph 
/// @param clique_num the minimum number of vertices the cliques contain 
/// @return a set of cliques with at least \a clique_num vertices 
template <typename GraphType>
inline vector<vector<typename boost::graph_traits<GraphType>::vertex_descriptor> >
max_clique(GraphType const& g, size_t clique_num)
{
	vector<vector<typename boost::graph_traits<GraphType>::vertex_descriptor> > vClique;
	// search for all cliques with at least clique_num vertices
	boost::bron_kerbosch_all_cliques(g, max_clique_visitor_type<GraphType>(vClique), clique_num);

	return vClique;
}

} // namespace algorithms
} // namespace limbo

#endif
