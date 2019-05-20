/**
 * @file   ChromaticNumber.h
 * @brief  return chromatic number of a graph
 * @author Yibo Lin
 * @date   Feb 2015
 */

#ifndef LIMBO_ALGORITHMS_COLORING_CHROMATICNUMBER
#define LIMBO_ALGORITHMS_COLORING_CHROMATICNUMBER

#include <iostream>
#include <vector>
#include <set>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/subgraph.hpp>
#include <limbo/algorithms/MaxIndependentSet.h>

using std::vector;
using std::set;
using std::cout;
using std::endl;

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{
	
/// @class limbo::algorithms::coloring::LawlerChromaticNumber
/// 
/// Implement the algorithm in 
/// "A note on the complexity of the chromatic number problem", 
/// E.L. Lawler
/// Inf. Process. Lett. 
///
/// Candidates for possible improvments
///
/// - "Chromatic Number in Time O(2.4023^n) Using Maximal Independent Sets", 
/// Jesper Makholm Byskov, 
/// BRICS, 2002
///
/// - "Small Maximal Independent Sets and Faster Exact Graphing Coloring", 
/// David Eppstein, 
/// Worksh. Algorithms and Data Structures, 2001
/// 
/// @tparam GraphType graph type 
template <typename GraphType>
class LawlerChromaticNumber
{
	public:
        /// @nowarn 
		typedef GraphType graph_type;
		typedef boost::subgraph<graph_type> subgraph_type;
		typedef typename boost::graph_traits<graph_type>::vertex_descriptor graph_vertex_type;
        /// @endnowarn

        /// @class limbo::algorithms::coloring::LawlerChromaticNumber::mis_visitor_type
        /// A maximum independent set visitor for @ref limbo::algorithms::MaxIndependentSetByMaxClique that records the largest independent sets so far 
		struct mis_visitor_type
		{
			vector<set<graph_vertex_type> >& mMisNode; ///< bind mis nodes 

            /// constructor 
            /// @param mMisNode_ container to store all the largest independent sets so far 
			mis_visitor_type(vector<set<graph_vertex_type> >& mMisNode_) : mMisNode(mMisNode_) {}
            /// copy constructor 
            /// @param rhs a mis_visitor_type object 
			mis_visitor_type(mis_visitor_type const& rhs) : mMisNode(rhs.mMisNode) {}

			/// required callback for max_independent_set function
            /// @tparam MisType a container type, default is std::deque 
			/// @param is independent set 
			template <typename MisType>
			void mis(MisType const& is)
			{
				// only record maximal independent sets 
				if (!mMisNode.empty())
				{
					if (mMisNode.front().size() < is.size())
						mMisNode.clear();
					else if (mMisNode.front().size() > is.size())
						return;
				}

				mMisNode.push_back(set<graph_vertex_type>());
				for (typename MisType::const_iterator it = is.begin(); 
						it != is.end(); ++it)
					mMisNode.back().insert(*it);
			}
		};
        /// API for computing chromatic number 
        /// @param g graph 
        /// @return chromatic number 
		int operator()(subgraph_type g) const 
		{
			return chromatic_number(g);
		}

	protected:
		/// The chromatic number of G is related to the complement graph of G \ I, 
		/// where I is maximum independent set.  
		/// Traversing over all maximum independent sets are necessary to calculate the chromatic number. 
		/// This function is implemented in a recursive way
        /// @param g graph 
        /// @return chromatic number of the graph 
		int chromatic_number(subgraph_type& g) const
		{
			int cn = boost::num_vertices(g); // initial chromatic number 

			// stop recursion
			if (cn <= 1) return cn;
			else if (cn == 2) 
			{
				BGL_FORALL_EDGES_T(e, g, subgraph_type)
				{
					// the only two vertices are connected
					if (boost::source(e, g) != boost::target(e, g))
						return 2;
				}
				// they are not connected
				return 1;
			}

			vector<set<graph_vertex_type> > mMisNode;
			limbo::algorithms::max_independent_set(g, mis_visitor_type(mMisNode), limbo::algorithms::MaxIndependentSetByMaxClique());

#ifdef DEBUG_CHROMATICNUMBER
#if 0
			typename boost::property_map<subgraph_type, boost::vertex_index_t>::type vertex_index_map = boost::get(boost::vertex_index, g);

			for (typename vector<set<graph_vertex_type> >::const_iterator it1 = mMisNode.begin(); 
					it1 != mMisNode.end(); ++it1)
			{
				set<graph_vertex_type> const& sMisNode = *it1;

				for (typename set<graph_vertex_type>::const_iterator it2 = sMisNode.begin();
						it2 != sMisNode.end(); ++it2)
					cout << vertex_index_map[*it2] << " ";
				cout << endl;
			}
#endif
#endif

			for (typename vector<set<graph_vertex_type> >::const_iterator it1 = mMisNode.begin(); 
					it1 != mMisNode.end(); ++it1)
			{
				set<graph_vertex_type> const& sMisNode = *it1;
				subgraph_type& g_s = g.create_subgraph(); // subgraph, G \ I

				BGL_FORALL_VERTICES_T(v, g, subgraph_type)
				{
					if (!sMisNode.count(v))
						boost::add_vertex(v, g_s);
				}

#ifdef DEBUG_CHROMATICNUMBER
				//boost::print_graph(g_s, vertex_index_map);
#endif
				// get chromatic number of complementary MIS graph
				int comp_cn = chromatic_number(g_s);

				if (cn > comp_cn) cn = comp_cn;

				// the assumption is that all mis have the same size 
				//
				// if current graph has more than 1 vertices 
				// and we only need 1 color, that is already smallest number of colors
				// so we can exit early
				if (boost::num_vertices(g_s) == 0 
						|| (boost::num_vertices(g_s) > 0 && cn == 1)) 
					break;
			}

			return cn+1;
		}
};

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
