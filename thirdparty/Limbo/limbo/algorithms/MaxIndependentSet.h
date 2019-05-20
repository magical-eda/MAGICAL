/**
 * @file   MaxIndependentSet.h
 * @brief  solve maximum independent sets with maximum cliques 
 *
 * The maximum independent set of a graph is the maximum clique of its complement graph. 
 *
 * @author Yibo Lin
 * @date   Feb 2015
 */

#ifndef LIMBO_ALGORITHMS_MAXINDEPENDENTSET_H
#define LIMBO_ALGORITHMS_MAXINDEPENDENTSET_H

#include <deque>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <limbo/algorithms/GraphUtility.h>

namespace limbo { namespace algorithms {

/// @class limbo::algorithms::MaxIndependentSetByMaxClique
/// a tag for solving maximum independent sets with maximum cliques 
struct MaxIndependentSetByMaxClique 
{
    /// @class limbo::algorithms::MaxIndependentSetByMaxClique::clique_visitor_type
	/// @brief callback for boost::bron_kerbosch_all_cliques
    /// @tparam GraphType graph type 
    /// @tparam MisVisitorType a function object for visiting each independent set.   
    /// Refer to @ref limbo::algorithms::coloring::LawlerChromaticNumber::mis_visitor_type for example. 
	template <typename GraphType, typename MisVisitorType>
	struct clique_visitor_type
	{
        /// @nowarn 
		typedef GraphType graph_type;
		typedef MisVisitorType mis_visitor_type;
		typedef typename boost::graph_traits<graph_type>::vertex_descriptor vertex_descriptor_type; 
		typedef std::map<vertex_descriptor_type, vertex_descriptor_type> map_type;
        /// @endnowarn

		MisVisitorType& mis_visitor; ///< bind mis visitor
		map_type& mCompG2G; ///< bind vertex mapping from complement graph to original graph

        /// constructor 
        /// @param mv visitor of maximum independent set 
        /// @param mCG2G mapping from complement graph to original graph 
		clique_visitor_type(mis_visitor_type& mv, map_type& mCG2G) : mis_visitor(mv), mCompG2G(mCG2G) {}
        /// copy constructor 
        /// @param rhs a clique_visitor_type object 
		clique_visitor_type(clique_visitor_type const& rhs) : mis_visitor(rhs.mis_visitor), mCompG2G(rhs.mCompG2G) {}

        /// @tparam CliqueType container type for vertices of a clique 
		/// @param c clique vertices in complement graph 
		/// @param cg complement graph
		template <typename CliqueType>
		void clique(CliqueType const& c, graph_type const& cg)
		{
			// convert to vertices in original graph
			typedef CliqueType clique_type;
			clique_type is;

			// for debug 
			if (boost::num_vertices(cg) > 0)
				assert(!c.empty());

			for (typename clique_type::const_iterator it = c.begin(); 
					it != c.end(); ++it)
				is.push_back(mCompG2G[*it]);

			mis_visitor.mis(is);
		}
	};
};

/// generic function to calculate maximum independent sets with different algorithms 
/// @tparam GraphType graph type 
/// @tparam VisitorType type of maximum independent set visitor 
/// @tparam AlgorithmType algorithm type 
/// @param g graph 
/// @param vis once an independent set is found, callback vis.mis(MisType const&) will be called.  
/// In this way, user can choose to store all the independent sets or process one by one. 
/// Refer to @ref limbo::algorithms::coloring::LawlerChromaticNumber::mis_visitor_type for example. 
template <typename GraphType, typename VisitorType, typename AlgorithmType>
inline void max_independent_set(GraphType const& g, VisitorType vis, AlgorithmType const&);

/// A maximum independent set of a graph g is also a maximum clique of its complement graph.  
/// This function searches maximum cliques of the complement graph to get maximum independent sets
/// @tparam GraphType graph type 
/// @tparam VisitorType type of maximum independent set visitor 
/// @param g graph 
/// @param vis once an independent set is found, callback vis.mis(MisType const&) will be called.  
/// In this way, user can choose to store all the independent sets or process one by one. 
/// Refer to @ref limbo::algorithms::coloring::LawlerChromaticNumber::mis_visitor_type for example. 
template <typename GraphType, typename MisVisitorType>
inline void max_independent_set(GraphType const& g, MisVisitorType vis, MaxIndependentSetByMaxClique const&)
{
	typedef typename boost::graph_traits<GraphType>::vertex_descriptor vertex_descriptor_type; 
	GraphType cg; // complement graph
	std::map<vertex_descriptor_type, vertex_descriptor_type> mCompG2G; // mapping from vertices in complement graph to original graph 

	// calculate complement graph 
	limbo::algorithms::complement_graph(g, cg, mCompG2G);

	// search for all cliques with at least 1 vertex 
	boost::bron_kerbosch_all_cliques(cg, MaxIndependentSetByMaxClique::clique_visitor_type<GraphType, MisVisitorType>(vis, mCompG2G), 1);
}

} // namespace algorithms
} // namespace limbo

#endif
