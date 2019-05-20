/**
 * @file   GraphSimplification.h
 * @brief  Various graph simplification techniques for graph coloring. 
 * Some of them can also be used in other applications. 
 *
 * Current simplification techniques include iterative vertex removal, biconnected component, merge 4-clique structures, etc. 
 * See @ref limbo::algorithms::coloring::GraphSimplification::strategy_type for detailed strategies. 
 *
 * @author Yibo Lin
 * @date   May 2015
 */

#ifndef LIMBO_ALGORITHMS_GRAPHSIMPLIFICATION_H
#define LIMBO_ALGORITHMS_GRAPHSIMPLIFICATION_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <list>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/property_map/property_map.hpp>
#include <limbo/preprocessor/AssertMsg.h>
#include <limbo/math/Math.h>
#include <limbo/algorithms/GraphUtility.h>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

namespace la = ::limbo::algorithms;

/// @class limbo::algorithms::coloring::GraphSimplification
/// Various graph simplification techniques for graph coloring.  
/// 
/// @tparam GraphType graph type 
template <typename GraphType>
class GraphSimplification
{
	public:
        /// @nowarn
		typedef GraphType graph_type;
		typedef typename boost::graph_traits<graph_type>::vertex_descriptor graph_vertex_type;
		typedef typename boost::graph_traits<graph_type>::edge_descriptor graph_edge_type;
		typedef typename boost::graph_traits<graph_type>::vertex_iterator vertex_iterator;
		typedef typename boost::graph_traits<graph_type>::adjacency_iterator adjacency_iterator;
		typedef typename boost::graph_traits<graph_type>::edge_iterator edge_iterator;
        /// @endnowarn

        /// @brief status of vertex. 
		enum vertex_status_type {
			GOOD, ///< still in graph 
			HIDDEN, ///< vertex is hidden by simplification  
			MERGED ///< vertex is merged to other vertex 
		};
		/// @brief simplification strategies available. 
		/// These strategies are order-sensitive.  
		/// It is recommended to call simplify(), 
		/// e.g. simplify(HIDE_SMALL_DEGREE | BICONNECTED_COMPONENT) 
		enum strategy_type {
			NONE = 0, ///< no simplification 
			HIDE_SMALL_DEGREE = 1, ///< hide vertices whose degree is smaller than number of colors available 
			MERGE_SUBK4 = 2, ///< merge 4-clique structures, no longer optimal 
			BICONNECTED_COMPONENT = 4 ///< divide graph by biconnected components 
		};
		/// constructor 
        /// @param g graph 
        /// @param color_num number of colors 
		GraphSimplification(graph_type const& g, uint32_t color_num) 
			: m_graph (g)
			, m_color_num (color_num)
			, m_level (NONE)
            , m_max_merge_level(std::numeric_limits<uint32_t>::max())
			, m_vStatus(boost::num_vertices(g), GOOD)
			, m_vParent(boost::num_vertices(g))
			, m_vChildren(boost::num_vertices(g))
			, m_vHiddenVertex()
			, m_mCompVertex()
//			, m_vComponent(boost::num_vertices(g), std::numeric_limits<uint32_t>::max())
//			, m_sBridgeEdge()
			, m_mArtiPoint()
			, m_vPrecolor(boost::num_vertices(g), -1)
		{
			graph_vertex_type v = 0; 
			for (typename std::vector<graph_vertex_type>::iterator it = m_vParent.begin(), ite = m_vParent.end(); it != ite; ++it, ++v)
				(*it) = v;
			v = 0;
			for (typename std::vector<std::vector<graph_vertex_type> >::iterator it = m_vChildren.begin(), ite = m_vChildren.end(); it != ite; ++it, ++v)
				it->push_back(v);
#ifdef DEBUG_GRAPHSIMPLIFICATION
			assert(m_vParent.size() == boost::num_vertices(m_graph));
			assert(m_vChildren.size() == boost::num_vertices(m_graph));
#endif
		}
		/// copy constructor is not allowed 
        /// @param rhs a GraphSimplification object 
		GraphSimplification(GraphSimplification const& rhs);

        /// set precolored vertices 
        /// @tparam Iterator iterator type 
        /// @param first, last begin and end of the precolored vertex array 
		template <typename Iterator>
		void precolor(Iterator first, Iterator last)
		{
			m_vPrecolor.assign(first, last);
#ifdef DEBUG_GRAPHSIMPLIFICATION
			assert(m_vPrecolor.size() == boost::num_vertices(m_graph));
#endif
		}

        /// @return status array of vertices 
		std::vector<vertex_status_type> const& status() const {return m_vStatus;}
        /// @return parent array of vertices 
		std::vector<graph_vertex_type> const& parents() const {return m_vParent;}
        /// @return children map of vertices 
		std::vector<std::vector<graph_vertex_type> > const& children() const {return m_vChildren;}
        /// @return hidden vertex array of vertices 
		std::stack<graph_vertex_type> const& hidden_vertices() const {return m_vHiddenVertex;}

		/// @return simplified graph and a std::map from merged graph vertices to original graph vertices 
		std::pair<graph_type, std::map<graph_vertex_type, graph_vertex_type> > simplified_graph() const; 
        /// extract a component from simplified graph 
        /// @param comp_id component id 
        /// @param sg component of simplified graph 
        /// @param vSimpl2Orig mapping from simplified graph to original graph 
        /// @return true if succeed 
		bool simplified_graph_component(uint32_t comp_id, graph_type& sg, std::vector<graph_vertex_type>& vSimpl2Orig) const;
        /// @return number of components 
		uint32_t num_component() const {return m_mCompVertex.size();}

        /// set maximum merge level 
        /// @param l level 
        void max_merge_level(int32_t l) {m_max_merge_level = l;}

        /// API to run the simplification algorithm 
        /// @param level simplification level, can be combination of items in @ref limbo::algorithms::coloring::GraphSimplification::strategy_type
		void simplify(uint32_t level);
        /// API to recover coloring solutions from color assignment of simplified graph 
        /// @param vColorFlat flatten coloring solutions for original graph 
        /// @param mColor coloring solutions arranged by components 
        /// @param mSimpl2Orig mapping from simplified graph components to original graph 
		void recover(std::vector<int8_t>& vColorFlat, std::vector<std::vector<int8_t> >& mColor, std::vector<std::vector<graph_vertex_type> > const& mSimpl2Orig) const;

		/// For a structure of K4 with one fewer edge,  
		/// suppose we have 4 vertices 1, 2, 3, 4, 
		/// edges 1--2, 1--3, 2--3, 2--4, 3--4. 
        /// Vertex 4 is merged to 1. 
		/// This strategy only works for 3-coloring. 
		/// It cannot guarantee minimal conflicts,
		/// but it can be used in a native conflict checker.
		void merge_subK4();

		/// hide vertices whose degree is no larger than number of colors - 1
		void hide_small_degree();
		// find all bridge edges and divided graph into components  
		//void remove_bridge();
		/// find all articulation points and biconnected components 
		void biconnected_component();

		/// recover merged vertices 
		/// @param vColor must be partially assigned colors except simplified vertices  
		void recover_merge_subK4(std::vector<int8_t>& vColor) const;
		// recover color for bridges 
		// @param vColor must be partially assigned colors except simplified vertices  
		//void recover_bridge(std::vector<int8_t>& vColor) const;

		/// recover color for biconnected components  
        /// @param mColor coloring solutions arranged by components 
        /// @param mSimpl2Orig mapping from simplified graph components to original graph 
		void recover_biconnected_component(std::vector<std::vector<int8_t> >& mColor, std::vector<std::vector<graph_vertex_type> > const& mSimpl2Orig) const;
        /// recover color for hidden vertices 
        /// need to be called manually, no density balance considered 
        /// this function is mutable because it pops out elements in m_vHiddenVertex 
		/// @param vColor coloring solutions, it must be partially assigned colors except simplified vertices  
        void recover_hide_small_degree(std::vector<int8_t>& vColor);

        /// write graph in graphviz format 
        /// @param filename output file name 
		void write_graph_dot(std::string const& filename) const; 
        /// write simplified graph in graphviz format 
        /// @param filename output file name 
		void write_simplified_graph_dot(std::string const& filename) const;
		
	protected:
        /// recursive implementation of computing biconnected components 
        /// @param v current vertex 
        /// @param vVisited an array records whether a vertex has been visited 
        /// @param vDisc discovery time of vertices 
        /// @param vLow low values of vertices 
        /// @param vParent parents of vertices 
        /// @param visit_time records current visiting time 
        /// @param vEdge a stack of edges for DFS 
        /// @param mCompVertex vertices arranged by independent components 
		void biconnected_component_recursion(graph_vertex_type v, std::deque<bool>& vVisited, std::vector<uint32_t>& vDisc, 
				std::vector<uint32_t>& vLow, std::vector<graph_vertex_type>& vParent, uint32_t& visit_time, 
				std::stack<std::pair<graph_vertex_type, graph_vertex_type> >& vEdge, 
				std::list<std::pair<graph_vertex_type, std::set<graph_vertex_type> > >& mCompVertex) const;
		/// compute connected components 
		void connected_component();
        /// @param v vertex 
		/// @return the root parent 
		/// i.e. the vertex that is not merged 
		graph_vertex_type parent(graph_vertex_type v) const 
		{
#ifdef DEBUG_GRAPHSIMPLIFICATION
			assert(!this->hidden(v));
#endif
			while (v != m_vParent.at(v))
				v = m_vParent.at(v);
			return v;
		}
        /// @param v vertex 
        /// @param vParent parent array of vertices 
		/// @return the root parent 
		/// generalized version 
		graph_vertex_type parent(uint32_t v, std::vector<uint32_t> const& vParent) const 
		{
			while (v != vParent.at(v))
				v = vParent.at(v);
			return v;
		}
        /// @param v1 vertex 
        /// @param v2 vertex 
		/// @return true if two vertices (parents) are connected by conflict edges 
		bool connected_conflict(graph_vertex_type v1, graph_vertex_type v2) const 
		{
			graph_vertex_type vp1 = this->parent(v1);
			graph_vertex_type vp2 = this->parent(v2);
			std::vector<graph_vertex_type> const& vChildren1 = m_vChildren.at(vp1);
			std::vector<graph_vertex_type> const& vChildren2 = m_vChildren.at(vp2);
			for (typename std::vector<graph_vertex_type>::const_iterator vic1 = vChildren1.begin(); vic1 != vChildren1.end(); ++vic1)
				for (typename std::vector<graph_vertex_type>::const_iterator vic2 = vChildren2.begin(); vic2 != vChildren2.end(); ++vic2)
				{
					std::pair<graph_edge_type, bool> e12 = boost::edge(*vic1, *vic2, m_graph);
					// only count conflict edge 
					if (e12.second && boost::get(boost::edge_weight, m_graph, e12.first) >= 0) return true;
				}
			return false;
		}
        /// @param v1 vertex 
        /// @param v2 vertex 
		/// @return true if two vertices (parents) are connected by stitch edges 
		bool connected_stitch(graph_vertex_type v1, graph_vertex_type v2) const 
		{
			graph_vertex_type vp1 = this->parent(v1);
			graph_vertex_type vp2 = this->parent(v2);
			std::vector<graph_vertex_type> const& vChildren1 = m_vChildren.at(vp1);
			std::vector<graph_vertex_type> const& vChildren2 = m_vChildren.at(vp2);
			for (typename std::vector<graph_vertex_type>::const_iterator vic1 = vChildren1.begin(); vic1 != vChildren1.end(); ++vic1)
				for (typename std::vector<graph_vertex_type>::const_iterator vic2 = vChildren2.begin(); vic2 != vChildren2.end(); ++vic2)
				{
					std::pair<graph_edge_type, bool> e12 = boost::edge(*vic1, *vic2, m_graph);
					// only count conflict edge 
					if (e12.second && boost::get(boost::edge_weight, m_graph, e12.first) < 0) return true;
				}
			return false;
		}
        /// @param v1 vertex 
        /// @param v2 vertex 
		/// @return the edge of two vertices (parents)
		std::pair<graph_edge_type, bool> connected_edge(graph_vertex_type v1, graph_vertex_type v2) const 
		{
			graph_vertex_type vp1 = this->parent(v1);
			graph_vertex_type vp2 = this->parent(v2);
			std::vector<graph_vertex_type> const& vChildren1 = m_vChildren.at(vp1);
			std::vector<graph_vertex_type> const& vChildren2 = m_vChildren.at(vp2);
			for (typename std::vector<graph_vertex_type>::const_iterator vic1 = vChildren1.begin(); vic1 != vChildren1.end(); ++vic1)
				for (typename std::vector<graph_vertex_type>::const_iterator vic2 = vChildren2.begin(); vic2 != vChildren2.end(); ++vic2)
				{
					std::pair<graph_edge_type, bool> e12 = boost::edge(*vic1, *vic2, m_graph);
					if (e12.second) return e12;
				}
			return std::make_pair(graph_edge_type(), false);
		}
        /// @param v1 vertex 
		/// @return true if current vertex is merged 
		bool merged(graph_vertex_type v1) const 
		{
			bool flag = (m_vStatus.at(v1) == MERGED);
#ifdef DEBUG_GRAPHSIMPLIFICATION
			assert(flag == m_vChildren.at(v1).empty());
			if (!flag) assert(v1 == m_vParent.at(v1));
			else assert(v1 != m_vParent.at(v1));
#endif
			return flag;
		}
        /// @param v1 vertex 
		/// @return true if current vertex is still in graph 
		bool good(graph_vertex_type v1) const 
		{
			return (m_vStatus.at(v1) == GOOD);
		}
        /// @param v1 vertex 
		/// @return true if current vertex is hidden  
		bool hidden(graph_vertex_type v1) const 
		{
			return (m_vStatus.at(v1) == HIDDEN);
		}
        /// @param v1 vertex 
		/// @return true if current vertex is precolored
		bool precolored(graph_vertex_type v1) const 
		{
			return (m_vPrecolor.at(v1) >= 0);
		}
        /// @param v vertex 
		/// @return true if the point is a articulation point 
		bool articulation_point(graph_vertex_type v) const 
		{
			return m_mArtiPoint.count(v);
		}
		/// @return true if there exist precolored vertices 
		bool has_precolored() const 
		{
			for (std::vector<int8_t>::const_iterator it = m_vPrecolor.begin(); it != m_vPrecolor.end(); ++it)
			{
				if (*it >= 0) 
					return true;
			}
			return false;
		}
        /// @param l merge level 
        /// @return true if the vertex satisfies maximum merge level constraint 
        bool check_max_merge_level(uint32_t l) const 
        {
            return l <= m_max_merge_level;
        }
		graph_type const& m_graph; ///< original graph 
		uint32_t m_color_num; ///< number of colors 
		uint32_t m_level; ///< simplification level 
        uint32_t m_max_merge_level; ///< in MERGE_SUBK4, any merge that results in the children number of a vertex larger than m_max_merge_level is disallowed 
		std::vector<vertex_status_type> m_vStatus; ///< status of each vertex 

		std::vector<graph_vertex_type> m_vParent; ///< parent vertex of current vertex 
		std::vector<std::vector<graph_vertex_type> > m_vChildren; ///< children verices of current vertex, a vertex is the child of itself if it is not merged  

		std::stack<graph_vertex_type> m_vHiddenVertex; ///< a std::stack that keeps a reverse order of vertices hidden, useful for color recovery 

		std::vector<std::vector<graph_vertex_type> > m_mCompVertex; ///< group vertices by components 
//		std::vector<uint32_t> m_vComponent; ///< component id for each vertex 

//		std::set<graph_edge_type> m_sBridgeEdge; ///< bridge edges that are removed during graph division 
		std::map<graph_vertex_type, std::set<uint32_t> > m_mArtiPoint; ///< map of (vertex of articulation point, set of components split by the vertex)

		std::vector<int8_t> m_vPrecolor; ///< precolor information, if uncolored, std::set to -1
};

/// @return simplified graph and a std::map from merged graph vertices to original graph vertices 
template <typename GraphType>
std::pair<typename GraphSimplification<GraphType>::graph_type, std::map<typename GraphSimplification<GraphType>::graph_vertex_type, typename GraphSimplification<GraphType>::graph_vertex_type> > 
GraphSimplification<GraphType>::simplified_graph() const 
{
	size_t vertex_cnt = 0;
	std::map<graph_vertex_type, graph_vertex_type> mG2MG;
	std::map<graph_vertex_type, graph_vertex_type> mMG2G;
	vertex_iterator vi1, vie1;
	for (boost::tie(vi1, vie1) = boost::vertices(m_graph); vi1 != vie1; ++vi1)
	{
		graph_vertex_type v1 = *vi1;
		if (this->good(v1))
		{
			mG2MG[*vi1] = vertex_cnt;
			mMG2G[vertex_cnt] = v1;
			vertex_cnt += 1;
		}
	}
	graph_type mg (vertex_cnt);

	edge_iterator ei, eie;
	for (boost::tie(ei, eie) = boost::edges(m_graph); ei != eie; ++ei)
	{
		graph_edge_type e = *ei;
		graph_vertex_type s = boost::source(e, m_graph);
		graph_vertex_type t = boost::target(e, m_graph);
		// skip edge with hidden vertices  
		if (this->hidden(s) || this->hidden(t)) continue;

		graph_vertex_type sp = this->parent(s);
		graph_vertex_type tp = this->parent(t);

#ifdef DEBUG_GRAPHSIMPLIFICATION
		assert(mG2MG.count(sp));
		assert(mG2MG.count(tp));
#endif
		graph_vertex_type msp = mG2MG[sp];
		graph_vertex_type mtp = mG2MG[tp];
		std::pair<graph_edge_type, bool> emg = boost::edge(msp, mtp, mg);
		if (!emg.second)
		{
			emg = boost::add_edge(msp, mtp, mg);
			assert(emg.second);
			boost::put(boost::edge_weight, mg, emg.first, boost::get(boost::edge_weight, m_graph, e));
		}
		else 
		{
			// use cumulative weight 
			// this is to make sure we can still achieve small conflict number in the simplified graph 
			// no longer optimal if merge_subK4() is called 
			boost::put(
					boost::edge_weight, mg, emg.first, 
					boost::get(boost::edge_weight, mg, emg.first) + boost::get(boost::edge_weight, m_graph, e)
					);
		}
	}

	return std::make_pair(mg, mMG2G);
}

template <typename GraphType>
bool GraphSimplification<GraphType>::simplified_graph_component(uint32_t comp_id, typename GraphSimplification<GraphType>::graph_type& simplG, 
		std::vector<typename GraphSimplification<GraphType>::graph_vertex_type>& vSimpl2Orig) const
{
	if (comp_id >= m_mCompVertex.size()) return false;

	std::vector<graph_vertex_type> const& vCompVertex = m_mCompVertex[comp_id];

	graph_type sg (vCompVertex.size());
	std::map<graph_vertex_type, graph_vertex_type> mOrig2Simpl;
	vSimpl2Orig.assign(vCompVertex.begin(), vCompVertex.end());
	for (uint32_t i = 0; i != vCompVertex.size(); ++i)
		mOrig2Simpl[vCompVertex[i]] = i;
#ifdef DEBUG_GRAPHSIMPLIFICATION
	std::cout << "Comp " << comp_id << ": ";
	for (uint32_t i = 0; i != vCompVertex.size(); ++i)
		std::cout << vCompVertex[i] << " ";
	std::cout << std::endl;
#endif

	for (typename std::vector<graph_vertex_type>::const_iterator vi = vCompVertex.begin(); vi != vCompVertex.end(); ++vi)
	{
		graph_vertex_type v = *vi;
		graph_vertex_type vsg = mOrig2Simpl[v];
		assert(this->good(v));
		bool ap = this->articulation_point(v);
		std::vector<graph_vertex_type> const& vChildren = m_vChildren.at(v);
		for (typename std::vector<graph_vertex_type>::const_iterator vic = vChildren.begin(); vic != vChildren.end(); ++vic)
		{
			graph_vertex_type vc = *vic;
			typename boost::graph_traits<graph_type>::adjacency_iterator ui, uie;
			for (boost::tie(ui, uie) = boost::adjacent_vertices(vc, m_graph); ui != uie; ++ui)
			{
				graph_vertex_type uc = *ui;
				// skip hidden 
				if (this->hidden(uc)) continue;
				graph_vertex_type u = this->parent(uc);
				// skip non-good 
				if (!this->good(u)) continue;
				else if (v >= u) continue; // avoid duplicate 
				else if (ap && !mOrig2Simpl.count(u)) continue; // skip vertex that is not in component 
				//else if (u == v) continue;
				assert_msg(u != v, u << " == " << v);

				std::pair<graph_edge_type, bool> e = boost::edge(vc, uc, m_graph);
				assert(e.second);
				// skip bridge 
				//if (m_sBridgeEdge.count(e.first)) continue;
				graph_vertex_type usg = mOrig2Simpl[u];
				assert_msg(usg != vsg, u << "==" << v << ": " << usg << " == " << vsg);
				
				std::pair<graph_edge_type, bool> esg = boost::edge(vsg, usg, sg);
				if (!esg.second)
				{
					esg = boost::add_edge(vsg, usg, sg);
					assert(esg.second);
					boost::put(boost::edge_weight, sg, esg.first, boost::get(boost::edge_weight, m_graph, e.first));
				}
				else 
				{
					// use cumulative weight 
					// this is to make sure we can still achieve small conflict number in the simplified graph 
					// no longer optimal if merge_subK4() is called 
					boost::put(
							boost::edge_weight, sg, esg.first, 
							boost::get(boost::edge_weight, sg, esg.first) + boost::get(boost::edge_weight, m_graph, e.first)
							);
				}
			}
		}
	}
	simplG.swap(sg);
	return true;
}

template <typename GraphType>
void GraphSimplification<GraphType>::simplify(uint32_t level)
{
	m_level = level; // record level for recover()
	if (this->has_precolored()) // this step does not support precolored graph yet 
		m_level = m_level & (~MERGE_SUBK4) & (~BICONNECTED_COMPONENT);

    bool reconstruct = true; // whether needs to reconstruct m_mCompVertex

	if (m_level & HIDE_SMALL_DEGREE)
    {
		this->hide_small_degree(); // connected components are computed inside 
        reconstruct = false;
    }
	if (m_level & MERGE_SUBK4)
    {
		this->merge_subK4();
    }
	if (m_level & BICONNECTED_COMPONENT)
    {
		this->biconnected_component();
        reconstruct = false;
    }

    if (reconstruct) // if BICONNECTED_COMPONENT or HIDE_SMALL_DEGREE is not on, we need to construct m_mCompVertex with size 1 
    {
        m_mCompVertex.assign(1, std::vector<graph_vertex_type>());
        for (graph_vertex_type v = 0, ve = boost::num_vertices(m_graph); v != ve; ++v)
            if (this->good(v))
                m_mCompVertex[0].push_back(v);
    }
}

template <typename GraphType>
void GraphSimplification<GraphType>::recover(std::vector<int8_t>& vColorFlat, std::vector<std::vector<int8_t> >& mColor, std::vector<std::vector<graph_vertex_type> > const& mSimpl2Orig) const
{
	// reverse order w.r.t simplify()
	if (m_level & BICONNECTED_COMPONENT)
		this->recover_biconnected_component(mColor, mSimpl2Orig);

	// std::map mColor to vColorFlat
	for (uint32_t sub_comp_id = 0; sub_comp_id < mColor.size(); ++sub_comp_id)
	{
		std::vector<int8_t> const& vColor = mColor[sub_comp_id];
		std::vector<graph_vertex_type> const& vSimpl2Orig = mSimpl2Orig[sub_comp_id];
		for (uint32_t subv = 0; subv < vColor.size(); ++subv)
		{
			graph_vertex_type v = vSimpl2Orig[subv];
			if (vColorFlat[v] >= 0)
				assert(vColorFlat[v] == vColor[subv]);
			else 
				vColorFlat[v] = vColor[subv];
		}
	}

	if (m_level & MERGE_SUBK4)
	{
		this->recover_merge_subK4(vColorFlat);
	}
	if (m_level & HIDE_SMALL_DEGREE)
	{
		// TO DO: this part has custom requirement, such as density balancing 
		// so now it is recovered manually 
	}
}

/// for a structure of K4 with one fewer edge 
/// suppose we have 4 vertices 1, 2, 3, 4
/// 1--2, 1--3, 2--3, 2--4, 3--4, vertex 4 is merged to 1 
/// this strategy only works for 3-coloring 
/// it cannot guarantee minimal conflicts 
/// but it can be used in a native conflict checker 
template <typename GraphType>
void GraphSimplification<GraphType>::merge_subK4()
{
	// when applying this function, be aware that other merging strategies may have already been applied 
	// so m_vParent is valid 
	//
	// merge iteratively 
	bool merge_flag; // true if merge occurs
	do 
	{
		merge_flag = false;
		// traverse the neighbors of vertex 1 to find connected vertex 2 and 3 
		vertex_iterator vi1, vie1;
		for (boost::tie(vi1, vie1) = boost::vertices(m_graph); vi1 != vie1; ++vi1)
		{
			graph_vertex_type v1 = *vi1;
			// only track good vertices 
			if (!this->good(v1)) continue;
			// find vertex 2 by searching the neighbors of vertex 1
			std::vector<graph_vertex_type> const& vChildren1 = m_vChildren.at(v1);
			for (typename std::vector<graph_vertex_type>::const_iterator vic1 = vChildren1.begin(); vic1 != vChildren1.end(); ++vic1)
			{
#ifdef DEBUG_GRAPHSIMPLIFICATION
				assert(vic1-vChildren1.begin() >= 0);
#endif
				graph_vertex_type vc1 = *vic1;
				adjacency_iterator vi2, vie2;
				for (boost::tie(vi2, vie2) = boost::adjacent_vertices(vc1, m_graph); vi2 != vie2; ++vi2)
				{
					// skip hidden vertex 
					if (this->hidden(*vi2)) continue;
					// skip stitch edges 
					std::pair<graph_edge_type, bool> e12 = boost::edge(vc1, *vi2, m_graph);
					assert(e12.second);
					if (boost::get(boost::edge_weight, m_graph, e12.first) < 0) continue;

					graph_vertex_type v2 = this->parent(*vi2);
					// find vertex 3 by searching the neighbors of vertex 2 
					std::vector<graph_vertex_type> const& vChildren2 = m_vChildren.at(v2);
					for (typename std::vector<graph_vertex_type>::const_iterator vic2 = vChildren2.begin(); vic2 != vChildren2.end(); ++vic2)
					{
						graph_vertex_type vc2 = *vic2;
						adjacency_iterator vi3, vie3;
						for (boost::tie(vi3, vie3) = boost::adjacent_vertices(vc2, m_graph); vi3 != vie3; ++vi3)
						{
							// skip hidden vertex 
							if (this->hidden(*vi3)) continue;
							// skip stitch edges 
							std::pair<graph_edge_type, bool> e23 = boost::edge(vc2, *vi3, m_graph);
							assert(e23.second);
							if (boost::get(boost::edge_weight, m_graph, e23.first) < 0) continue;

							graph_vertex_type v3 = this->parent(*vi3);
							// skip v1, v1 must be a parent  
							if (v3 == v1) continue;
							// only connected 1 and 3 are considered 
							if (!this->connected_conflict(v1, v3)) continue;

							// find vertex 4 by searching the neighbors of vertex 3  
							std::vector<graph_vertex_type> const& vChildren3 = m_vChildren.at(v3);
							for (typename std::vector<graph_vertex_type>::const_iterator vic3 = vChildren3.begin(); vic3 != vChildren3.end(); ++vic3)
							{
								graph_vertex_type vc3 = *vic3;
								adjacency_iterator vi4, vie4;
								for (boost::tie(vi4, vie4) = boost::adjacent_vertices(vc3, m_graph); vi4 != vie4; ++vi4)
								{
									// skip hidden vertex and skip precolored vertex  
									if (this->hidden(*vi4) || this->precolored(*vi4)) continue;
									// skip stitch edges 
									std::pair<graph_edge_type, bool> e34 = boost::edge(vc3, *vi4, m_graph);
									assert(e34.second);
									if (boost::get(boost::edge_weight, m_graph, e34.first) < 0) continue;

									graph_vertex_type v4 = this->parent(*vi4);
									// skip v1 or v2, v1 and v2 must be parent, and v4 must not be precolored
									if (v4 == v1 || v4 == v2 || this->precolored(v4)) continue;
									// vertex 2 and vertex 4 must be connected 
									// vertex 1 and vertex 4 must not be connected (K4)
									if (!this->connected_conflict(v2, v4) || this->connected_conflict(v1, v4)) continue;
                                    // check max merge level 
                                    if (!this->check_max_merge_level(m_vChildren[v1].size()+m_vChildren[v4].size())) continue;
									// merge vertex 4 to vertex 1 
									m_vStatus[v4] = MERGED;
									m_vChildren[v1].insert(m_vChildren[v1].end(), m_vChildren[v4].begin(), m_vChildren[v4].end());
									m_vChildren[v4].resize(0); // clear and shrink to fit 
									m_vParent[v4] = v1;
									merge_flag = true;
#ifdef DEBUG_GRAPHSIMPLIFICATION
									assert(m_vStatus[v1] == GOOD);
									//this->write_graph_dot("graph_simpl");
#endif
									break;
								}
								if (merge_flag) break;
							}
							if (merge_flag) break;
						}
						if (merge_flag) break;
					}
					if (merge_flag) break;
				}
				if (merge_flag) break;
			}
			if (merge_flag) break;
		}
	} while (merge_flag);
}

/// hide vertices whose degree is no larger than color_num-1
template <typename GraphType>
void GraphSimplification<GraphType>::hide_small_degree()
{
	// when applying this function, be aware that other strategies may have already been applied 
	// make sure it is compatible 

	bool hide_flag;
	do 
	{
		hide_flag = false;
		// traverse the neighbors of vertex 1 to find connected vertex 2 and 3 
		vertex_iterator vi1, vie1;
		for (boost::tie(vi1, vie1) = boost::vertices(m_graph); vi1 != vie1; ++vi1)
		{
			graph_vertex_type v1 = *vi1;
			// only track good and uncolored vertices  
			if (!this->good(v1) || this->precolored(v1)) continue;
			size_t conflict_degree = 0;
			// find vertex 2 by searching the neighbors of vertex 1
			std::vector<graph_vertex_type> const& vChildren1 = m_vChildren.at(v1);
			for (typename std::vector<graph_vertex_type>::const_iterator vic1 = vChildren1.begin(); vic1 != vChildren1.end(); ++vic1)
			{
#ifdef DEBUG_GRAPHSIMPLIFICATION
				assert(vic1-vChildren1.begin() >= 0);
#endif
				graph_vertex_type vc1 = *vic1;
				adjacency_iterator vi2, vie2;
				for (boost::tie(vi2, vie2) = boost::adjacent_vertices(vc1, m_graph); vi2 != vie2; ++vi2)
				{
					// skip hidden vertex 
					if (this->hidden(*vi2)) continue;
					// skip stitch edges 
					std::pair<graph_edge_type, bool> e12 = boost::edge(vc1, *vi2, m_graph);
					assert(e12.second);
					if (boost::get(boost::edge_weight, m_graph, e12.first) < 0) continue;

					conflict_degree += 1;
				}
			}
			if (conflict_degree < m_color_num) // hide v1 
			{
				//m_vStatus[v1] = HIDDEN;
				m_vHiddenVertex.push(v1);
				hide_flag = true;
				// hide all the children of v1 
				// but no need to push them to the std::stack m_vHiddenVertex
				// v1 is also in its children 
				std::vector<graph_vertex_type> const& vChildren1 = m_vChildren.at(v1);
				for (typename std::vector<graph_vertex_type>::const_iterator vic1 = vChildren1.begin(); vic1 != vChildren1.end(); ++vic1)
					m_vStatus[*vic1] = HIDDEN;
#ifdef DEBUG_GRAPHSIMPLIFICATION
				std::cout << "std::stack +" << v1 << std::endl;
				assert(m_vStatus[v1] == HIDDEN);
#endif
				break;
			}
		}
	} while (hide_flag);

	this->connected_component();
}

// refer to http://www.geeksforgeeks.org/articulation-points-or-cut-vertices-in-a-graph/
// for the recursive implementation 
template <typename GraphType>
void GraphSimplification<GraphType>::biconnected_component()
{
	uint32_t vertex_num = boost::num_vertices(m_graph);
	std::stack<graph_vertex_type> vStack; // std::stack for dfs 
	// for speed instead of memory (no bool)
	std::deque<bool> vVisited (vertex_num, false); 
	std::vector<graph_vertex_type> vParent (vertex_num); 
	std::vector<uint32_t> vChildrenCnt (vertex_num, 0);
	// vLow[u] = min(vDisc[u], vDisc[w]), where w is an ancestor of u
	// there is a back edge from some descendant of u to w
	std::vector<uint32_t> vLow (vertex_num, std::numeric_limits<uint32_t>::max()); // lowest vertex reachable from subtree under v  
	std::vector<uint32_t> vDisc(vertex_num, std::numeric_limits<uint32_t>::max()); // discovery time 
	std::deque<bool> vArtiPoint (vertex_num, false); // true if it is articulation point 
	std::stack<std::pair<graph_vertex_type, graph_vertex_type> > vEdge; // virtual edge, it can be connection between parents 
	std::list<std::pair<graph_vertex_type, std::set<graph_vertex_type> > > mCompVertex; // save bi-connected components 
	uint32_t visit_time = 0;

	// set initial parent of current vertex to itself 
	vertex_iterator vi, vie;
	for (boost::tie(vi, vie) = boost::vertices(m_graph); vi != vie; ++vi)
		vParent[*vi] = *vi;

	for (boost::tie(vi, vie) = boost::vertices(m_graph); vi != vie; ++vi)
	{
		graph_vertex_type source = *vi;
		if (!vVisited[source])
		{
			biconnected_component_recursion(source, vVisited, vDisc, vLow, vParent, visit_time, vEdge, mCompVertex);
		}
		// if stack is not empty, pop all edges from stack
		if (!vEdge.empty())
		{
			mCompVertex.push_back(std::make_pair(std::numeric_limits<graph_vertex_type>::max(), std::set<graph_vertex_type>()));
			do
			{
				mCompVertex.back().second.insert(vEdge.top().first);
				mCompVertex.back().second.insert(vEdge.top().second);
				vEdge.pop();
			} while (!vEdge.empty());
		}
	}
	// collect articulation points 
	uint32_t comp_id = 0;
	// reset members 
	m_mArtiPoint.clear();
	m_mCompVertex.assign(mCompVertex.size(), std::vector<graph_vertex_type>());
	for (typename std::list<std::pair<graph_vertex_type, std::set<graph_vertex_type> > >::const_iterator it = mCompVertex.begin(); it != mCompVertex.end(); ++it, ++comp_id)
	{
		graph_vertex_type vap = it->first;
		std::set<graph_vertex_type> const& sComp = it->second;
		if (vap < std::numeric_limits<graph_vertex_type>::max()) // valid 
			m_mArtiPoint.insert(std::make_pair(vap, std::set<uint32_t>()));
		m_mCompVertex[comp_id].assign(sComp.begin(), sComp.end());
	}
	comp_id = 0;
	for (typename std::list<std::pair<graph_vertex_type, std::set<graph_vertex_type> > >::const_iterator it = mCompVertex.begin(); it != mCompVertex.end(); ++it, ++comp_id)
	{
		//graph_vertex_type vap = it->first;
		std::set<graph_vertex_type> const& sComp = it->second;
		for (typename std::set<graph_vertex_type>::const_iterator itc = sComp.begin(); itc != sComp.end(); ++itc)
		{
			graph_vertex_type v = *itc;
			if (this->articulation_point(v))
				m_mArtiPoint[v].insert(comp_id);
		}
	}
#ifdef DEBUG_GRAPHSIMPLIFICATION
	comp_id = 0;
	for (typename std::list<std::pair<graph_vertex_type, std::set<graph_vertex_type> > >::const_iterator it = mCompVertex.begin(); it != mCompVertex.end(); ++it, ++comp_id)
	{
		graph_vertex_type vap = it->first;
		std::set<graph_vertex_type> const& sComp = it->second;
		std::cout << "+ articulation point " << vap << " --> comp " << comp_id << ": ";
		for (typename std::set<graph_vertex_type>::const_iterator itc = sComp.begin(); itc != sComp.end(); ++itc)
			std::cout << *itc << " ";
		std::cout << std::endl;
	}
	for (typename std::map<graph_vertex_type, std::set<uint32_t> >::const_iterator it = m_mArtiPoint.begin(); it != m_mArtiPoint.end(); ++it)
	{
		graph_vertex_type vap = it->first;
		std::set<uint32_t> const& sComp = it->second;
		std::cout << "ap " << vap << ": ";
		for (typename std::set<uint32_t>::const_iterator itc = sComp.begin(); itc != sComp.end(); ++itc)
			std::cout << *itc << " ";
		std::cout << std::endl;
	}
#endif
}

template <typename GraphType>
void GraphSimplification<GraphType>::biconnected_component_recursion(graph_vertex_type v, std::deque<bool>& vVisited, std::vector<uint32_t>& vDisc, 
		std::vector<uint32_t>& vLow, std::vector<graph_vertex_type>& vParent, uint32_t& visit_time, 
		std::stack<std::pair<graph_vertex_type, graph_vertex_type> >& vEdge, 
		std::list<std::pair<graph_vertex_type, std::set<graph_vertex_type> > >& mCompVertex) const
{ 
    // Count of children in DFS Tree
    uint32_t children = 0;
 
    // Mark the current node as visited
    vVisited[v] = true;
 
    // Initialize discovery time and low value
    vDisc[v] = vLow[v] = visit_time++;
 
    // Go through all vertices adjacent to this
	if (!this->good(v)) return;

	bool isolate = true;
	// find neighbors of all merged vertices
	std::vector<graph_vertex_type> const& vChildren = m_vChildren.at(v);
	for (typename std::vector<graph_vertex_type>::const_iterator vic = vChildren.begin(); vic != vChildren.end(); ++vic)
	{
		graph_vertex_type vc = *vic;
		// skip hidden vertex 
		if (this->hidden(vc)) continue;

		adjacency_iterator ui, uie;
		for (boost::tie(ui, uie) = boost::adjacent_vertices(vc, m_graph); ui != uie; ++ui)
		{
			graph_vertex_type uc = *ui;
			// skip hidden vertex 
			if (this->hidden(uc)) continue;

			isolate = false; 
			graph_vertex_type u = this->parent(uc);
			assert(this->good(u));

			// If v is not visited yet, then make it a child of u
			// in DFS tree and recur for it
			if (!vVisited[u])
			{
				++children;
				vParent[u] = v;
				vEdge.push(std::make_pair(std::min(v, u), std::max(v, u)));
				biconnected_component_recursion(u, vVisited, vDisc, vLow, vParent, visit_time, vEdge, mCompVertex);

				// Check if the subtree rooted with v has a connection to
				// one of the ancestors of u
				vLow[v] = std::min(vLow[v], vLow[u]);

				// u is an articulation point in following cases

				// (1) u is root of DFS tree and has two or more chilren.
				// (2) If u is not root and low value of one of its child is more
				// than discovery value of u.
				if ((vParent[v] == v && children > 1)
						|| (vParent[v] != v && vLow[u] >= vDisc[v]))
				{
					mCompVertex.push_back(std::make_pair(v, std::set<graph_vertex_type>()));
					while (vEdge.top().first != std::min(v, u) || vEdge.top().second != std::max(v, u))
					{
						mCompVertex.back().second.insert(vEdge.top().first);
						mCompVertex.back().second.insert(vEdge.top().second);
						vEdge.pop();
					}
					mCompVertex.back().second.insert(vEdge.top().first);
					mCompVertex.back().second.insert(vEdge.top().second);
					vEdge.pop();
				}

			}
			else if (u != vParent[v] && vDisc[u] < vLow[v])
			{
				vLow[v] = std::min(vLow[v], vDisc[u]);
				vEdge.push(std::make_pair(std::min(v, u), std::max(v, u)));
			}
		}
	}
	// for isolated vertex, create a component 
	if (isolate)
	{
		mCompVertex.push_back(std::make_pair(std::numeric_limits<graph_vertex_type>::max(), std::set<graph_vertex_type>()));
		mCompVertex.back().second.insert(v);
	}
}

template <typename GraphType>
void GraphSimplification<GraphType>::connected_component() 
{
	// we only need to filter out hidden vertices and bridges 
	// merged vertices are not a problem because they are initially connected with their parents 

	uint32_t vertex_num = boost::num_vertices(m_graph);
	std::stack<graph_vertex_type> vStack; // std::stack for dfs 
	// for speed instead of memory (no bool)
	std::deque<bool> vVisited (vertex_num, false); 
	uint32_t comp_id = 0;
	std::vector<uint32_t> vComponent (vertex_num, std::numeric_limits<uint32_t>::max());

	// std::set initial parent of current vertex to itself 
	vertex_iterator vi, vie;
	for (boost::tie(vi, vie) = boost::vertices(m_graph); vi != vie; ++vi)
	{
		graph_vertex_type source = *vi;
		if (this->hidden(source)) continue;
		if (this->articulation_point(source)) continue;
		// skip visited vertex 
		if (vVisited[source]) continue; 
		vStack.push(source);
		vVisited[source] = true;
		while (!vStack.empty())
		{
			graph_vertex_type v = vStack.top();
			vStack.pop();
#ifdef DEBUG_GRAPHSIMPLIFICATION
			std::cout << v << " ";
#endif

			vComponent[v] = comp_id; // std::set component id 

			// for a articulation point, do not explore the neighbors 
			if (this->articulation_point(v))
			{
				m_mArtiPoint[v].insert(comp_id);
				vVisited[v] = false;
				continue;
			}

			adjacency_iterator ui, uie;
			for (boost::tie(ui, uie) = boost::adjacent_vertices(v, m_graph); ui != uie; ++ui)
			{
				graph_vertex_type u = *ui;
				if (this->hidden(u)) continue;

#ifdef DEBUG_GRAPHSIMPLIFICATION
				std::pair<graph_edge_type, bool> e = boost::edge(u, v, m_graph);
				assert(e.second);
#endif

				if (!vVisited[u]) // non-visited vertex 
				{
					vStack.push(u);
					vVisited[u] = true;
				}
			}
		}
		comp_id += 1;
	}
#ifdef DEBUG_GRAPHSIMPLIFICATION
	std::cout << std::endl;
#endif
	// explore the connection between articulation points
	// create additional components for connected articulation pairs 
	for (typename std::map<graph_vertex_type, std::set<uint32_t> >::const_iterator vapi = m_mArtiPoint.begin(); vapi != m_mArtiPoint.end(); ++vapi)
	{
		graph_vertex_type v = vapi->first;
		if (!vVisited[v])
		{
			adjacency_iterator ui, uie;
			for (boost::tie(ui, uie) = boost::adjacent_vertices(v, m_graph); ui != uie; ++ui)
			{
				graph_vertex_type u = *ui;
				if (this->hidden(u)) continue;
				if (!vVisited[u] && this->articulation_point(u))
				{
					m_mArtiPoint[v].insert(comp_id);
					m_mArtiPoint[u].insert(comp_id);

#ifdef DEBUG_GRAPHSIMPLIFICATION
					std::cout << "detect " << v << ", " << u << " ==> comp " << comp_id << std::endl;
#endif

					comp_id += 1;
				}
			}
			vVisited[v] = true;
		}
	}

	// std::set m_mCompVertex
	m_mCompVertex.assign(comp_id, std::vector<graph_vertex_type>());
	for (boost::tie(vi, vie) = boost::vertices(m_graph); vi != vie; ++vi)
	{
		graph_vertex_type v = *vi;
		// consider good vertices only 
		// skip articulation_point
		if (this->good(v) && !this->articulation_point(v))
			m_mCompVertex[vComponent[v]].push_back(v);
	}
	for (typename std::map<graph_vertex_type, std::set<uint32_t> >::const_iterator vapi = m_mArtiPoint.begin(); vapi != m_mArtiPoint.end(); ++vapi)
	{
		graph_vertex_type vap = vapi->first;
		std::set<uint32_t> const& sComp = vapi->second;
		for (typename std::set<uint32_t>::const_iterator itc = sComp.begin(); itc != sComp.end(); ++itc)
			m_mCompVertex[*itc].push_back(vap);
	}
}

/// recover merged vertices 
/// @param vColor must be partially assigned colors except simplified vertices  
template <typename GraphType>
void GraphSimplification<GraphType>::recover_merge_subK4(std::vector<int8_t>& vColor) const
{
	vertex_iterator vi, vie;
	for (boost::tie(vi, vie) = boost::vertices(m_graph); vi != vie; ++vi)
	{
		graph_vertex_type v = *vi;
		if (this->good(v))
		{
			assert(vColor[v] >= 0 && vColor[v] < (int8_t)this->m_color_num);
			for (uint32_t j = 0; j != m_vChildren[v].size(); ++j)
			{
				graph_vertex_type u = m_vChildren[v][j];
				if (v != u) 
					vColor[u] = vColor[v];
			}
		}
	}
}

/// recover color for biconnected components  
template <typename GraphType>
void GraphSimplification<GraphType>::recover_biconnected_component(std::vector<std::vector<int8_t> >& mColor, std::vector<std::vector<graph_vertex_type> > const& mSimpl2Orig) const
{
	// a single articulation point must correspond to two components 
	// std::map<graph_vertex_type, std::pair<uint32_t, uint32_t> > m_mArtiPoint
	// std::vector<std::vector<graph_vertex_type> > m_mCompVertex
	
	// only contain mapping for articulation points 
	std::vector<std::map<graph_vertex_type, graph_vertex_type> > mApOrig2Simpl (mSimpl2Orig.size());
	for (uint32_t i = 0; i < mSimpl2Orig.size(); ++i)
	{
		std::vector<graph_vertex_type> const& vSimpl2Orig = mSimpl2Orig[i];
		std::map<graph_vertex_type, graph_vertex_type>& vApOrig2Simpl = mApOrig2Simpl[i];

		for (uint32_t j = 0; j < vSimpl2Orig.size(); ++j)
		{
			if (m_mArtiPoint.count(vSimpl2Orig[j]))
				vApOrig2Simpl[vSimpl2Orig[j]] = j;
		}
	}

	std::vector<int32_t> vRotation (m_mCompVertex.size(), 0); // rotation amount for each component
	std::deque<bool> vVisited (m_mCompVertex.size(), false); // visited flag 
	std::vector<std::set<graph_vertex_type> > vCompAp (m_mCompVertex.size()); // articulation points for each component 

	for (typename std::map<graph_vertex_type, std::set<uint32_t> >::const_iterator vapi = m_mArtiPoint.begin(); vapi != m_mArtiPoint.end(); ++vapi)
	{
		graph_vertex_type vap = vapi->first;
		std::set<uint32_t> const& sComp = vapi->second;
		for (typename std::set<uint32_t>::const_iterator itc = sComp.begin(); itc != sComp.end(); ++itc)
		{
			uint32_t comp = *itc;
			vCompAp[comp].insert(vap);
			//assert(vCompAp[comp].size() < 3);
		}
	}

	// dfs based approach to propagate rotation 
	for (uint32_t comp_id = 0; comp_id < vCompAp.size(); ++comp_id)
	{
		if (!vVisited[comp_id])
		{
			std::stack<uint32_t> vStack;
			vStack.push(comp_id);
			vVisited[comp_id] = true;
			while (!vStack.empty())
			{
				uint32_t c = vStack.top(); // current component 
				vStack.pop();

				for (typename std::set<graph_vertex_type>::const_iterator vapi = vCompAp[c].begin(); vapi != vCompAp[c].end(); ++vapi)
				{
					graph_vertex_type vap = *vapi;
					std::set<uint32_t> const& sComp = m_mArtiPoint.at(vap);

					for (typename std::set<uint32_t>::const_iterator itcc = sComp.begin(); itcc != sComp.end(); ++itcc)
					{
						uint32_t cc = *itcc; // child component
						if (!vVisited[cc])
						{
							vStack.push(cc);
							vVisited[cc] = true;
							int8_t color_c = mColor[c][mApOrig2Simpl[c][vap]];
							int8_t color_cc = mColor[cc][mApOrig2Simpl[cc][vap]];
							vRotation[cc] += vRotation[c] + color_c - color_cc;
						}
					}
				}
			}
		}
	}

	// apply color rotation 
	for (uint32_t comp_id = 0; comp_id < mColor.size(); ++comp_id)
	{
		std::vector<int8_t>& vColor = mColor[comp_id];
		int32_t rotation = vRotation[comp_id];
		if (rotation < 0) // add a large enough K*m to achieve positive value 
			rotation += (limbo::abs(rotation)/m_color_num+1)*m_color_num;
		assert(rotation >= 0);
		rotation %= (int32_t)m_color_num;
		for (uint32_t v = 0; v < vColor.size(); ++v)
		{
			assert(vColor[v] >= 0);
			vColor[v] = (vColor[v] + rotation) % (int32_t)m_color_num;
		}
	}

#ifdef DEBUG_GRAPHSIMPLIFICATION
	for (typename std::map<graph_vertex_type, std::set<uint32_t> >::const_iterator vapi = m_mArtiPoint.begin(); vapi != m_mArtiPoint.end(); ++vapi)
	{
		graph_vertex_type vap = vapi->first;
		std::set<uint32_t> const& sComp = vapi->second;

		int8_t prev_color = -1;
		for (typename std::set<uint32_t>::const_iterator itc = sComp.begin(); itc != sComp.end(); ++itc)
		{
			uint32_t comp = *itc;
			int8_t color = mColor[comp][mApOrig2Simpl[comp][vap]];
			if (itc == sComp.begin())
				prev_color = color;
			else assert_msg(prev_color == color, 
					vap << ": " << "comp " << comp << ", c[" << mApOrig2Simpl[comp][vap] << "] = " << color << "; " 
					<< "prev_color = " << prev_color);
		}
	}
#endif
}

template <typename GraphType>
void GraphSimplification<GraphType>::recover_hide_small_degree(std::vector<int8_t>& vColor) 
{
	while (!m_vHiddenVertex.empty())
	{
		graph_vertex_type v = m_vHiddenVertex.top();
		m_vHiddenVertex.pop();

		// find available colors 
        std::vector<char> vUnusedColor (m_color_num, true);
        typename boost::graph_traits<graph_type>::adjacency_iterator vi, vie;
		for (boost::tie(vi, vie) = boost::adjacent_vertices(v, this->m_graph); vi != vie; ++vi)
		{
			graph_vertex_type u = *vi;
			if (vColor[u] >= 0)
				vUnusedColor[vColor[u]] = false;
		}

		// choose the first available color 
		for (int8_t i = 0; i != (int8_t)m_color_num; ++i)
		{
			if (vUnusedColor[i])
			{
                vColor[v] = i;
                break;
			}
		}
		assert(vColor[v] >= 0 && vColor[v] < (int8_t)m_color_num);
	}
}

template <typename GraphType>
void GraphSimplification<GraphType>::write_graph_dot(std::string const& filename) const 
{
	std::ofstream out((filename+".gv").c_str());
	out << "graph D { \n"
		<< "  randir = LR\n"
		<< "  size=\"4, 3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\",fontsize=200]\n" 
		<< "  node[shape=\"circle\",fontsize=200]\n";

	//output nodes 
	vertex_iterator vi1, vie1;
	for (boost::tie(vi1, vie1) = boost::vertices(m_graph); vi1 != vie1; ++vi1)
	{
		graph_vertex_type v1 = *vi1;
		if (!this->good(v1)) continue;

		out << "  " << v1 << "[shape=\"circle\"";
		//output coloring label
		out << ",label=\"" << v1 << ":(";
		for (typename std::vector<graph_vertex_type>::const_iterator it1 = m_vChildren[v1].begin(); it1 != m_vChildren[v1].end(); ++it1)
		{
			if (it1 != m_vChildren[v1].begin())
				out << ",";
			out << *it1;
		}
		out << ")\"]\n";
	}

	//output edges
	for (boost::tie(vi1, vie1) = boost::vertices(m_graph); vi1 != vie1; ++vi1)
	{
		graph_vertex_type v1 = *vi1;
		if (!this->good(v1)) continue;

		vertex_iterator vi2, vie2;
		for (boost::tie(vi2, vie2) = boost::vertices(m_graph); vi2 != vie2; ++vi2)
		{
			graph_vertex_type v2 = *vi2;
			if (!this->good(v2)) continue;
			if (v1 >= v2) continue;

			std::pair<graph_edge_type, bool> e = this->connected_edge(v1, v2);
			if (e.second)
			{
				// if two hyper vertices are connected by conflict edges, 
				// there is no need to consider stitch edges 
				if (this->connected_conflict(v1, v2)) 
					out << "  " << v1 << "--" << v2 << "[color=\"black\",style=\"solid\",penwidth=3]\n";
				else if (this->connected_stitch(v1, v2))
					out << "  " << v1 << "--" << v2 << "[color=\"black\",style=\"dashed\",penwidth=3]\n";
				// virtual connection showing that a bridge exists
				out << " " << v1 << "--" << v2 << "[color=\"black\",style=\"dotted\",penwidth=1]\n";
			}
		}
	}
	out << "}";
	out.close();

    la::graphviz2pdf(filename);
}
template <typename GraphType>
void GraphSimplification<GraphType>::write_simplified_graph_dot(std::string const& filename) const
{
	std::ofstream out((filename+".gv").c_str());
	out << "graph D { \n"
		<< "  randir = LR\n"
		<< "  size=\"4, 3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\",fontsize=200]\n" 
		<< "  node[shape=\"circle\",fontsize=200]\n";

	uint32_t offset = 0;
	// component by component 
	for (uint32_t comp_id = 0; comp_id != m_mCompVertex.size(); ++comp_id)
	{
		graph_type sg;
		std::vector<graph_vertex_type> vSimpl2Orig;
		bool flag = this->simplified_graph_component(comp_id, sg, vSimpl2Orig);
		if (flag)
		{
			//output nodes 
			vertex_iterator vi1, vie1;
			for (boost::tie(vi1, vie1) = boost::vertices(sg); vi1 != vie1; ++vi1)
			{
				graph_vertex_type mv1 = *vi1;
				graph_vertex_type v1 = vSimpl2Orig[mv1];
				if (m_vChildren[v1].empty()) continue;

				out << "  " << offset+mv1 << "[shape=\"circle\"";
				//output coloring label
				out << ",label=\"" << mv1 << ":(";
				for (typename std::vector<graph_vertex_type>::const_iterator it1 = m_vChildren[v1].begin(); it1 != m_vChildren[v1].end(); ++it1)
				{
					if (it1 != m_vChildren[v1].begin())
						out << ",";
					out << *it1;
				}
				out << "):" << comp_id << "\"]\n";
			}
			//output edges
			edge_iterator ei, eie;
			for (boost::tie(ei, eie) = boost::edges(sg); ei != eie; ++ei)
			{
				graph_edge_type e = *ei;
				graph_vertex_type mv1 = boost::source(e, sg);
				graph_vertex_type mv2 = boost::target(e, sg);
				int32_t w = boost::get(boost::edge_weight, sg, e);
				assert_msg(mv1 != mv2, mv1 << " == " << mv2);

				char buf[256];
				if (w >= 0)
					sprintf(buf, "  %lu--%lu[label=%d,color=\"black\",style=\"solid\",penwidth=3]", offset+mv1, offset+mv2, w);
				else 
					sprintf(buf, "  %lu--%lu[label=%d,color=\"black\",style=\"dashed\",penwidth=3]", offset+mv1, offset+mv2, w);
				out << buf << std::endl;
			}
			offset += boost::num_vertices(sg);
		}
	}

	out << "}";
	out.close();

    la::graphviz2pdf(filename);
}

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif

