/**
 * @file   GreedySearch.h
 * @brief  A greedy search algorithm for placement. 
 *
 * This class provides generic functions to bruce-force search 
 * all legal positions (including swap with smaller cells) within 
 * displacment range. 
 * The flow can be customized, but the basic functions can be shared. 
 * This approach is very powerful to legalize a placement or 
 * find positions without stitch or conflict in DFM aware placement. 
 *
 * @author Yibo Lin
 * @date   May 2015
 */

#ifndef _LIMBO_ALGORITHMS_PLACEMENT_GREEDYSEARCH_H
#define _LIMBO_ALGORITHMS_PLACEMENT_GREEDYSEARCH_H

#include <iostream>
#include <iterator>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Placement 
namespace placement 
{

using std::cout;
using std::endl;
using std::pair;

/// do not delete it 
/// it is an example of callbacks 
#if 0
/// sample callback type 
struct GSCallback
{
	/// required types 
	typedef Component node_type;
	typedef Segment row_type;
	typedef int site_coordinate_type;
	typedef list<node_type*> node_vector_type;
	typedef list<node_type*> node_fail_vector_type;
	typedef vector<row_type*> row_vector_type;
	struct cost_type
	{
		long cost;
		int seg_id;
		int site_id;
		int color_id;
		vector<list<node_type*>::iterator> vItNode; ///< the last node indicates the position for insertion 
													///< other nodes should be removed and collected to vFailNode
		cost_type()
		{
			cost = numeric_limits<long>::max();
			seg_id = -1;
			site_id = -1;
			color_id = -1;
		}
		cost_type(cost_type const& rhs)
		{
			cost = rhs.cost;
			seg_id = rhs.seg_id;
			site_id = rhs.site_id;
			color_id = rhs.color_id;
			vItNode = rhs.vItNode;
		}
		/// \return true if c1 has smaller cost than c2 
		/// equivalent to c1.cost < c2.cost 
		friend bool operator<(cost_type const& c1, cost_type const& c2) {return c1.cost < c2.cost;}
	};

	SegLegalizerCF* pslmr;

	GSCallback(SegLegalizerCF* psl) : pslmr(psl) {}
	GSCallback(GSCallback const& rhs) : pslmr(rhs.pslmr) {}

	/// required functions 
	/// user-defined node_traits provides cell informations 
	/// the following class defines the basic required functions 
	site_coordinate_type site_xl(const node_type* pn) const {}
	site_coordinate_type site_xh(const node_type* pn) const {}

	/// \return pair of starting row and ending row 
	pair<size_t, size_t> row_range(const node_type* pn) const {}

	/// \return a set of nodes in the row 
	node_vector_type& nodes_in_row(size_t row_idx) const {}

	/// \return true if the target position is within the displacment range 
	bool check_displace(const node_type* pn, site_coordinate_type x, site_coordinate_type y) const {}

	/// user-defined cost callbacks
	/// and return the best cost with the best solution 
	/// @param CostType should be comparable according to cost values 
	/// 
	/// it should also contain the method to obtain the corresponding solution
	/// @param swap_cnt indicates how many smaller cells have to be replaced to place node @param n 
	/// if swap_cnt is zero, then n is directly placed into a whitespace 
	cost_type calc_cost(const node_type* pcomp2, site_coordinate_type seg_id2, 
			site_coordinate_type site_id2, vector<list<node_type*>::iterator> const& vItNode, unsigned int swap_cnt) const{}

	/// check if a cost is valid 
	bool check_valid(cost_type const& c) const {}
	/// place the cell to somewhere and remove swapped cells 
	void apply(node_type* pcomp, cost_type const& c, node_fail_vector_type& vFailNode, int swap_cnt) const {}

	/// user-defined row callbacks 
	row_type* row(size_t row_idx) const {}
	site_coordinate_type site_xl(const row_type* pr) const {}
	site_coordinate_type site_xh(const row_type* pr) const {}
};
#endif

/// choose T& or T*
/// @tparam T 
template <typename T>
struct gs_choose_type
{
	typedef T& value_type; ///< value type 
	typedef T const& const_value_type; ///< constant value type 
};
/// choose T& or T*
/// @tparam T 
template <typename T>
struct gs_choose_type<T*>
{
	typedef T* value_type; ///< value type 
	typedef const T* const_value_type; ///< constant value type 
};

/// core class to perform greedy search functions
/// @tparam CallbackType provides all the information needed 
template <typename CallbackType>
class GreedySearch
{
	public:
        /// @nowarn
		typedef CallbackType callback_type;
		typedef typename callback_type::node_type node_type;
		typedef typename callback_type::cost_type cost_type;
		typedef typename callback_type::row_type row_type;
		typedef typename callback_type::site_coordinate_type site_coordinate_type;
		typedef typename callback_type::node_vector_type node_vector_type; ///< node container for a single row 
		typedef typename callback_type::node_fail_vector_type node_fail_vector_type;
		typedef typename callback_type::row_vector_type row_vector_type;

		// it can be row_type& or row_type*
		typedef typename gs_choose_type<typename row_vector_type::value_type>::value_type row_value_type;
		typedef typename gs_choose_type<typename row_vector_type::value_type>::const_value_type row_const_value_type;
		// it can be node_type& or node_type*
		typedef typename gs_choose_type<typename node_vector_type::value_type>::value_type node_value_type;
		typedef typename gs_choose_type<typename node_vector_type::value_type>::const_value_type node_const_value_type;
        /// @endnowarn

        /// constructor 
        /// @param cbk callback object 
		GreedySearch(callback_type cbk = callback_type()) : m_cbk(cbk) {}

        /// API to run the algorithm 
        /// @param vFailNode container to store failed nodes 
        /// @param max_swap_cnt one cell can swap with how many other cells 
		void operator()(node_fail_vector_type& vFailNode, int max_swap_cnt) {this->run(vFailNode, max_swap_cnt);}
        /// kernel function to run the algorithm 
        /// @param vFailNode container to store failed nodes 
        /// @param max_swap_cnt one cell can swap with how many other cells 
		void run(node_fail_vector_type& vFailNode, int max_swap_cnt)
		{
			for (typename node_fail_vector_type::iterator it = vFailNode.begin();
					it != vFailNode.end(); )
			{
				node_value_type n = *it;
				bool success_flag = false;
				for (int i = 0; i <= max_swap_cnt; ++i)
				{
					if (search_swap(n, vFailNode, i)) 
					{
						success_flag = true;
						break;
					}
				}
				if (success_flag)
				{
					typename node_fail_vector_type::iterator itPrev = it;
					++it;
					vFailNode.erase(itPrev);
				}
				else ++it;
			}
		}
		/// node_map_type can be vector<list<node_type> >
		/// row_vector_type can be vector<row_type>
        /// @param n node 
        /// @param vFailNode container to store failed nodes 
        /// @param swap_cnt number of swaps occurs 
		bool search_swap(node_value_type n, node_fail_vector_type& vFailNode, int swap_cnt) 
		{
			typedef typename node_vector_type::iterator node_iterator_type;
			pair<size_t, size_t> row_range = m_cbk.row_range(n);

			size_t row_idx1 = row_range.first;
			size_t row_idx2 = row_range.second;
			assert(row_idx1 < row_idx2);

			// it should be initialized to invalid 
			cost_type best_cost;
			assert(!m_cbk.check_valid(best_cost));
			for (size_t row_idx = row_idx1; row_idx != row_idx2; ++row_idx)
			{
				node_vector_type& vNode = m_cbk.nodes_in_row(row_idx);
				// row can be T& or T*
				row_const_value_type row = m_cbk.row(row_idx);
				// prepare a set of consecutive iterator2 
				vector<node_iterator_type> vIt2 (swap_cnt+1, vNode.begin());
				bool next_flag1 = false; // if true, perform continue 
				for (int i = 0; i < swap_cnt; ++i)
				{
					if (vIt2.back() == vNode.end())
					{
						next_flag1 = true;
						break;
					}
					++(vIt2.back());
				}
				if (next_flag1) continue;
				for (; vIt2.back() != vNode.end(); ++vIt2.back())
				{
					// prepare the set of consecutive iterator2 
					for (int i = vIt2.size()-1; i > 0; --i)
					{
						vIt2[i-1] = vIt2[i];
						--vIt2[i-1];
					}

					site_coordinate_type site_size_x = this->node_site_size_x(n);
					bool next_flag2 = false; // if true, perform continue 
					// check node size 
					for (int i = 0; i < vIt2.size()-1; ++i)
					{
						// n should not be included in the node map 
						assert(n != *vIt2[i]);
						if (this->node_site_size_x(*vIt2[i]) >= site_size_x) 
						{
							next_flag2 = true;
							break;
						}
					}
					if (next_flag2) continue;

					// check whitespace size 
					site_coordinate_type ws_site_xl;
					site_coordinate_type ws_site_xh = m_cbk.site_xl(*vIt2.back());
					if (vIt2.front() != vNode.begin())
					{
						node_iterator_type it2_0 = vIt2.front();
						--it2_0;
						ws_site_xl = this->site_xh(*it2_0);
					}
					else ws_site_xl = m_cbk.site_xl(row);
					// if whitespace is too small 
					if (ws_site_xh-ws_site_xl < site_size_x) continue;

					// enumerate all possible positions in the whitespace 
					for (site_coordinate_type tgt_site = ws_site_xl; 
							tgt_site <= ws_site_xh-site_size_x; ++tgt_site)
					{
						// check displacment constraint 
						if (!m_cbk.check_displace(n, tgt_site, row_idx)) continue;
						cost_type cur_cost = m_cbk.calc_cost(n, row_idx, tgt_site, vIt2, swap_cnt);
						if (!m_cbk.check_valid(best_cost) || cur_cost < best_cost)
						{
							best_cost = cur_cost;
						}
					}
				}
			}
			if (m_cbk.check_valid(best_cost))
			{
				// I assume apply() will remove replaced nodes and insert current node 
				m_cbk.apply(n, best_cost, vFailNode, swap_cnt);
				return true;
			}
			return false;
		}
	protected:
        /// @param n node 
        /// @return width of node 
		site_coordinate_type node_site_size_x(node_const_value_type n) const 
		{
			return m_cbk.site_xh(n)-m_cbk.site_xl(n);
		}
        /// @param n1 node 
        /// @param n2 node 
        /// @return gap in number of sites between nodes 
		site_coordinate_type node_site_gap_x(node_const_value_type n1, node_const_value_type n2) const 
		{
			return m_cbk.site_xl(n2)-m_cbk.site_xh(n1);
		}
        /// @param n node 
        /// @return left coordiate in sites 
		site_coordinate_type site_xl(node_const_value_type n) const 
		{
			return m_cbk.site_xl(n);
		}
        /// @param n node 
        /// @return right coordiate in sites 
		site_coordinate_type site_xh(node_const_value_type n) const 
		{
			return m_cbk.site_xh(n);
		}

		callback_type m_cbk; ///< a copiable callback_type is required
};

} // namespace placement
} // namespace algorithms
} // namespace limbo 

#endif
