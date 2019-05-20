/**
 * @file   LPColoringOld.h
 * @brief  coloring algorithm based on iterative linear programming (LP) and rounding 
 *
 * Implementation algorithm in 
 * "Triple/quadruple patterning layout decomposition via novel linear programming and iterative rounding", 
 * Yibo Lin, Xiaoqing Xu, Bei Yu, Ross Baldick, and David Z. Pan, 
 * SPIE Advanced Lithography, volume 9781, 2016.
 *
 * @author Yibo Lin, Xiaoqing Xu 
 * @date   Sep 2015
 */
/**
 * Old implementation, deprecated 
 */

/// @cond 

#ifndef LIMBO_ALGORITHMS_COLORING_LP
#define LIMBO_ALGORITHMS_COLORING_LP

#include <iostream>
#include <vector>
#include <set>
#include <cassert>
#include <cmath>
#include <stdlib.h>
#include <cstdio>
#include <sstream>
#include <boost/cstdint.hpp>
#include <boost/unordered_map.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/bipartite.hpp>
#include "gurobi_c++.h"

using std::vector;
using std::set;
using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::pair;
using std::ostringstream;
using std::string;
using boost::unordered_map;
using boost::uint32_t;
using boost::int32_t;

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

template <typename GraphType>
class LPColoring 
{
	public:
		typedef GraphType graph_type;
		//typedef boost::subgraph_type<graph_type> subgraph_type;
		typedef typename boost::graph_traits<graph_type>::vertex_descriptor graph_vertex_type;
		typedef typename boost::graph_traits<graph_type>::edge_descriptor graph_edge_type;
		/// edge weight is used to differentiate conflict edge and stitch edge 
		/// non-negative weight implies conflict edge 
		/// negative weight implies stitch edge 
		typedef typename boost::property_map<graph_type, boost::edge_weight_t>::const_type edge_weight_map_type;
		/// use vertex color to save vertex stitch candidate number 
		typedef typename boost::property_map<graph_type, boost::vertex_color_t>::const_type vertex_color_map_type;

		enum RoundingScheme 
		{
			DIRECT_ILP, // directly solve ILP to get optimal solution 
			FIXED_ILP, // this rounding scheme is not in use now, because feasible solution is not guaranteed  
			ITERATIVE_ILP, 
			GREEDY, 
            POST_ILP
		};
		enum ColorNum 
		{
			THREE, 
			FOUR
		};
		// hasher class for graph_edge_type
		struct edge_hash_type : std::unary_function<graph_edge_type, std::size_t>
		{
			std::size_t operator()(graph_edge_type const& e) const 
			{
				std::size_t seed = 0;
				boost::hash_combine(seed, e.m_source);
				boost::hash_combine(seed, e.m_target);
				return seed;
			}
		};

		/// member functions
		/// constructor
		LPColoring(graph_type const& g);
		/// destructor
		~LPColoring() {};

		/// top api 
		void operator()() {this->graphColoring();}

		bool conflictCost() const {return m_conflict_cost;}
		void conflictCost(bool flag) {m_conflict_cost = flag;}
		double stitchWeight() const {return m_stitch_weight;}
		void stitchWeight(double w) {m_stitch_weight = w;}
		RoundingScheme roundingScheme() const {return m_rounding_scheme;}
		void roundingScheme(RoundingScheme rs) {m_rounding_scheme = rs;}
		ColorNum colorNum() const {return m_color_num;}
		void colorNum(ColorNum cn) {m_color_num = cn;}

		/// DFS to search for the odd cycles, stored in m_odd_cycles
		void oddCycles(graph_vertex_type& v);

		/// relaxed linear programming based coloring for the conflict graph (m_graph)
		void graphColoring(); 
		/// ILP based coloring, not in use now
		/// fix all integer solutions from LP 
		/// set non-integer solutions to integer in ILP 
		/// call ILP only once 
		void ILPColoring(vector<GRBVar>& coloringBits, vector<GRBVar>& vEdgeBit);

		/// Optimal rounding based on binding constraints
		void rounding_bindingAnalysis(GRBModel& opt_model, vector<GRBVar>& coloringBits, vector<GRBVar>& vEdgeBit);

		/// iterative ILP based rounding
		/// set non-integer solutions to integer 
		/// set integer solutions from LP to continuous 
		/// iteratively call ILP until no non-integer solutions 
		/// ILP based rounding 
		void rounding_ILP(GRBModel& opt_model, vector<GRBVar>& coloringBits, vector<GRBVar>& vEdgeBit);
		/// Greedy rounding scheme
		void rounding_Greedy_v0(vector<GRBVar>& coloringBits);
		void rounding_Greedy(vector<GRBVar>& coloringBits);

		/// coloring info
		uint32_t vertexColor(graph_vertex_type& node) const;
		uint32_t conflictNum() const;
		uint32_t stitchNum() const;
		/// coloring info for non-rounded LP results
		uint32_t lpConflictNum() const;
		uint32_t lpStitchNum() const;
		///store the lp coloring results 
		void store_lp_coloring(vector<GRBVar>& coloringBits);

		pair<uint32_t, uint32_t> nonIntegerNum(const vector<GRBVar>& coloringBits, const vector<GRBVar>& vEdgeBit) const;
		/// for debug use
		void printBoolVec(vector<bool>& vec) const;
		void printBoolArray(bool* vec, uint32_t vec_size) const;
		/// print graphviz
		void write_graph_dot(graph_vertex_type& v) const;
		void write_graph_dot() const;
		void write_graph_color() const;

		/// members
		/// coloring bits
		const uint32_t COLORING_BITS;
	protected:
		/// set up the vertex map
		void setMap();
		/// check if a variable is integer or not 
		bool isInteger(double value) const {return value == floor(value);}
		/// check same color for two vertices from m_coloring
		bool sameColor(graph_vertex_type v1, graph_vertex_type v2) const;
		/// check same color for two vertices from m_lp_coloring
		bool lpSameColor(graph_vertex_type v1, graph_vertex_type v2) const;
		/// check conflict of an edge from m_coloring
		bool hasConflict(graph_edge_type e) const;
		/// check conflict of an edge from m_lp_coloring
		bool hasLPConflict(graph_edge_type e) const;
		/// check stitch of an edge from m_coloring
		bool hasStitch(graph_edge_type e) const;
		/// check stitch of an edge from m_lp_coloring
		bool hasLPStitch(graph_edge_type e) const;

		/// members
		/// the graph 
		graph_type const& m_graph;
		/// the vertex map for the graph 
		unordered_map<graph_vertex_type, uint32_t> m_vertex_map;
		unordered_map<uint32_t, graph_vertex_type> m_inverse_vertex_map;
		/// edge map for the graph 
		unordered_map<graph_edge_type, uint32_t, edge_hash_type> m_edge_map;
		/// edge weight map 
		edge_weight_map_type m_edge_weight_map;
		/// vertex color map for vertex stitch candidate number
		vertex_color_map_type m_vertex_color_map;
		/// LP coloring results
		unordered_map<graph_vertex_type, uint32_t> m_coloring;
		/// the lp coloring before rounding
		vector<double> m_lp_coloring;
		/// store the odd cycles
		vector< vector<graph_vertex_type> > m_odd_cycles;
		/// conflict control flag 
		/// whether put conflicts in ILP cost 
		bool m_conflict_cost;
		/// weight for stitch in the cost 
		double m_stitch_weight;
		/// rounding scheme option 
		RoundingScheme m_rounding_scheme;
		/// number of colors available, three or four 
		ColorNum m_color_num;
		/// record number of LP iterations 
		uint32_t m_lp_iter_cnt;
		/// record number of ILP iterations 
		uint32_t m_ilp_iter_cnt;

		//linear programming constraints property 
		unordered_map<string, bool> m_stitch_constrs;
		uint32_t m_constrs_num;
};

/// constructor
template <typename GraphType>
LPColoring<GraphType>::LPColoring(graph_type const& g) : COLORING_BITS(2), m_graph(g)
{
	m_conflict_cost = true;
	m_stitch_weight = 0.1;
	m_rounding_scheme = FIXED_ILP;
	m_color_num = THREE;
	m_lp_iter_cnt = 0;
	m_ilp_iter_cnt = 0;
  m_constrs_num = 0;
	this->setMap();
}

//set the vertex map
template<typename GraphType>
void LPColoring<GraphType>::setMap() 
{
	// build the vertex-index map
	pair<typename graph_type::vertex_iterator, typename graph_type::vertex_iterator> vertex_range = vertices(m_graph);
	m_vertex_map.clear();
	m_inverse_vertex_map.clear();
	uint32_t vertex_num = 0;
	for(BOOST_AUTO(itr, vertex_range.first); itr != vertex_range.second; ++itr) 
	{
		m_vertex_map[*itr] = vertex_num;
		m_inverse_vertex_map[vertex_num] = *itr;
		++vertex_num;
	}
	// build edge-index map 
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	m_edge_map.clear();
	uint32_t edge_num = 0;
	for (BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr)
	{
		m_edge_map[*itr] = edge_num;
		++edge_num;
	}
	// build edge weight map and vertex color map 
	m_edge_weight_map = get(boost::edge_weight, m_graph);
  m_vertex_color_map = get(boost::vertex_color, m_graph);
}

//DFS to search for the odd cycles, stored in m_odd_cycles
template<typename GraphType>
void LPColoring<GraphType>::oddCycles(graph_vertex_type& v) 
{
	//if(m_vertex_map.empty() || m_inverse_vertex_map.empty()) this->setMap();
#ifdef DEBUG_LPCOLORING
	assert(m_vertex_map.find(v) != m_vertex_map.end());
#endif
	//odd_cycle results
	this->m_odd_cycles.clear();

	//the array denoting the distancevisiting of the graph 
	uint32_t vertex_num = num_edges(m_graph);
	int32_t nodeDist[vertex_num];
	bool nodeVisited[vertex_num];
	for(uint32_t k = 0; k < vertex_num; k++) 
	{
		nodeDist[k] = -1;
		nodeVisited[k] = false;
	}

	//inCycle flag
	bool inCycle[vertex_num];
	for(uint32_t k = 0; k < vertex_num; k++) inCycle[k] = false;

	//dfs_stack for DFS
	vector<graph_vertex_type> dfs_stack;
	dfs_stack.reserve(vertex_num);
	uint32_t v_index = m_vertex_map[v];
	nodeVisited[v_index] = true;
	nodeDist[v_index] = 0;
	dfs_stack.push_back(v);
	while(false == dfs_stack.empty()) 
	{
		//determine whether the top element needs to be popped
		bool popFlag = true;
		//access the top element on the dfs_stack
		graph_vertex_type curr_v = dfs_stack.back();
		uint32_t curr_index = m_vertex_map[curr_v];
		//access the neighbors 
		typename boost::graph_traits<graph_type>::adjacency_iterator vi_begin, vi_end;
		boost::tie(vi_begin, vi_end) = adjacent_vertices(curr_v, m_graph);
		for(BOOST_AUTO(vi, vi_begin); vi != vi_end; ++vi) 
		{
			BOOST_AUTO(found_edge, boost::edge(curr_v, *vi, m_graph));
#ifdef DEBUG_LPCOLORING
			assert(found_edge.second);
#endif
			// skip stitch edges 
			if (m_edge_weight_map[found_edge.first] < 0) continue;

			uint32_t next_index = m_vertex_map[*vi];
			if(nodeDist[next_index] < 0) 
			{
				nodeDist[next_index] = 1 - nodeDist[curr_index];
				nodeVisited[next_index] = true;
				//push to the dfs_stack
				dfs_stack.push_back(*vi);
				popFlag = false;
				break;
			}
		} //end for 

		if(true == popFlag) 
		{
			//detect the odd cycle
			for(BOOST_AUTO(vi, vi_begin); vi != vi_end; ++vi) 
			{
				uint32_t next_index = m_vertex_map[*vi];
				if(true == nodeVisited[next_index] && (nodeDist[next_index] == nodeDist[curr_index])) 
				{
					//suppose v/v_index is not in the current cycle 
					inCycle[v_index] = true;
					//detect the cycle between curr_v and *vi
					vector<graph_vertex_type> cycle;
					int cnt = dfs_stack.size();
					for(int k = cnt - 1; k >= 0; k--) 
					{
						cycle.push_back(dfs_stack[k]);
						//flag the nodes in cycle
						inCycle[m_vertex_map[dfs_stack[k]]] = true;
						if(dfs_stack[k] == (*vi)) break;
					}
					//store the cycle, when v/v_index is in cycle
					if(cycle.size() > 0 && inCycle[v_index]) 
					{
						this->m_odd_cycles.push_back(cycle);
					} 
					else if(cycle.size() == 0) 
					{
						cout << "ERROR: the cycle detected contains no nodes" << endl;
					}
				}//end if
			}//end for vi

			//pop the top element
			dfs_stack.pop_back();
			nodeVisited[curr_index] = false;
		}//end if popFlag

	}//end while
#ifdef DEBUG_LPCOLORING
	if(m_odd_cycles.size() > 0) cout << m_odd_cycles.size() << " cycles detected." << endl;
	else cout << "No cycles detected." << endl;
#endif
}

//relaxed linear programming based coloring for the conflict graph (m_graph)
template<typename GraphType> 
void LPColoring<GraphType>::graphColoring() 
{
	//build the vertex-index map
	//pair<typename graph_type::vertex_iterator, typename graph_type::vertex_iterator> vertex_range = vertices(m_graph);
#if 0
	//unordered_map<graph_vertex_type, uint32_t> m_vertex_map;
	//unordered_map<uint32_t, graph_vertex_type> m_inverse_vertex_map;
	m_vertex_map.clear(), m_inverse_vertex_map.clear();
	uint32_t vertex_num = 0;
	for(BOOST_AUTO(itr, vertex_range.first); itr != vertex_range.second; ++itr) 
	{
		m_vertex_map[*itr] = vertex_num;
		m_inverse_vertex_map[vertex_num] = *itr;
		vertex_num++;
	}
#endif
  cout << "---------------------------------------Iterative LP based coloring scheme------------------------------------\n";
	// reset iteration counters 
	m_lp_iter_cnt = m_ilp_iter_cnt = 0;

	uint32_t vertex_num = boost::num_vertices(m_graph);
	uint32_t edge_num = boost::num_edges(m_graph);
	uint32_t coloring_bits_num = COLORING_BITS*vertex_num;
	//uint32_t variable_num = coloring_bits_num+edge_num;

	//set up the LP environment
	GRBEnv env = GRBEnv();
	//mute the log from the LP solver
	//env.set(GRB_IntParam_OutputFlag, 0);
	GRBModel opt_model = GRBModel(env);
	//set up the LP variables
	vector<GRBVar> coloringBits;
	vector<GRBVar> vEdgeBit;
	// vertex and edge variables 
	coloringBits.reserve(coloring_bits_num);
	for (uint32_t i = 0; i != coloring_bits_num; ++i)
	{
		ostringstream oss;
		oss << "v" << i;
		if (roundingScheme() == DIRECT_ILP)
			coloringBits.push_back(opt_model.addVar(0.0, 1.0, 0.0, GRB_INTEGER, oss.str()));
		else
			coloringBits.push_back(opt_model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, oss.str()));
	}
	vEdgeBit.reserve(edge_num);
	for (uint32_t i = 0; i != edge_num; ++i)
	{
		// some variables here may not be used 
		ostringstream oss;
		oss << "e" << i;
		vEdgeBit.push_back(opt_model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, oss.str()));
	}
	// conflict edge variables 
	// they are used in objective function to minimize conflict cost 
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	//Integrate new variables
	opt_model.update();

	//set up the objective
	GRBLinExpr obj_init (0);
	GRBLinExpr obj (0); // maybe useful 
	if (this->conflictCost())
	{
		for (BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr)
		{
			BOOST_AUTO(w, m_edge_weight_map[*itr]);
			uint32_t edge_idx = m_edge_map[*itr];
			if (w >= 0) // conflict 
				obj_init += vEdgeBit[edge_idx];
		}
	}
	for (BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr)
	{
		BOOST_AUTO(w, m_edge_weight_map[*itr]);
		uint32_t edge_idx = m_edge_map[*itr];
		if (w < 0) // stitch 
			obj_init += m_stitch_weight*vEdgeBit[edge_idx];
	}
	obj = obj_init;
	opt_model.setObjective(obj, GRB_MINIMIZE);

	//set up the LP constraints
	//typename graph_type::edges_size_type edge_num = num_edges(m_graph);
	//typename graph_type::vertices_size_type vertex_num = num_vertices(m_graph); 
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		BOOST_AUTO(from, source(*itr, m_graph));
		uint32_t f_ind = m_vertex_map[from];
		BOOST_AUTO(to, target(*itr, m_graph));
		uint32_t t_ind = m_vertex_map[to];
		//coloring conflict constraints
		uint32_t vertex_idx1 = f_ind<<1;
		uint32_t vertex_idx2 = t_ind<<1;

		BOOST_AUTO(w, m_edge_weight_map[*itr]);
    GRBConstr tmpConstr;
    char buf[100];
    string tmpConstr_name;
		if (w >= 0) // constraints for conflict edges 
		{
			if (!conflictCost())
			{
        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						coloringBits[vertex_idx1] + coloringBits[vertex_idx1+1] 
						+ coloringBits[vertex_idx2] + coloringBits[vertex_idx2+1] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						1 - coloringBits[vertex_idx1] + coloringBits[vertex_idx1+1] 
						+ 1 - coloringBits[vertex_idx2] + coloringBits[vertex_idx2+1] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						coloringBits[vertex_idx1] + 1 - coloringBits[vertex_idx1+1] 
						+ coloringBits[vertex_idx2] + 1 - coloringBits[vertex_idx2+1] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						1 - coloringBits[vertex_idx1] + 1 - coloringBits[vertex_idx1+1] 
						+ 1 - coloringBits[vertex_idx2] + 1 - coloringBits[vertex_idx2+1] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

			}
			else 
			{
				uint32_t edge_idx = m_edge_map[*itr];

        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						coloringBits[vertex_idx1] + coloringBits[vertex_idx1+1] 
						+ coloringBits[vertex_idx2] + coloringBits[vertex_idx2+1] 
						+ vEdgeBit[edge_idx] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						1 - coloringBits[vertex_idx1] + coloringBits[vertex_idx1+1] 
						+ 1 - coloringBits[vertex_idx2] + coloringBits[vertex_idx2+1] 
						+ vEdgeBit[edge_idx] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						coloringBits[vertex_idx1] + 1 - coloringBits[vertex_idx1+1] 
						+ coloringBits[vertex_idx2] + 1 - coloringBits[vertex_idx2+1] 
						+ vEdgeBit[edge_idx] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

        sprintf(buf, "%u", m_constrs_num++);  
        tmpConstr_name = "R" + string(buf);
				tmpConstr = opt_model.addConstr(
						1 - coloringBits[vertex_idx1] + 1 - coloringBits[vertex_idx1+1] 
						+ 1 - coloringBits[vertex_idx2] + 1 - coloringBits[vertex_idx2+1] 
						+ vEdgeBit[edge_idx] >= 1
						, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
        assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
        m_stitch_constrs[tmpConstr_name] = false;

			}
		}
		else // constraints for stitch edges 
		{
			uint32_t edge_idx = m_edge_map[*itr];

      sprintf(buf, "%u", m_constrs_num++);  
      tmpConstr_name = "R" + string(buf);
			tmpConstr = opt_model.addConstr(
					coloringBits[vertex_idx1] - coloringBits[vertex_idx2] - vEdgeBit[edge_idx] <= 0
					, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
      assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
      m_stitch_constrs[tmpConstr_name] = true;

      sprintf(buf, "%u", m_constrs_num++);  
      tmpConstr_name = "R" + string(buf);
			tmpConstr = opt_model.addConstr(
					coloringBits[vertex_idx2] - coloringBits[vertex_idx1] - vEdgeBit[edge_idx] <= 0
					, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
      assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
      m_stitch_constrs[tmpConstr_name] = true;

      sprintf(buf, "%u", m_constrs_num++);  
      tmpConstr_name = "R" + string(buf);
			tmpConstr = opt_model.addConstr(
					coloringBits[vertex_idx1+1] - coloringBits[vertex_idx2+1] - vEdgeBit[edge_idx] <= 0
					, tmpConstr_name);      
#ifdef DEBUG_LPCOLORING
      assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
      m_stitch_constrs[tmpConstr_name] = true;

      sprintf(buf, "%u", m_constrs_num++);  
      tmpConstr_name = "R" + string(buf);
			tmpConstr = opt_model.addConstr(
					coloringBits[vertex_idx2+1] - coloringBits[vertex_idx1+1] - vEdgeBit[edge_idx] <= 0
					, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
      assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
      m_stitch_constrs[tmpConstr_name] = true;

		}
	}

	if (colorNum() == THREE)
	{
    GRBConstr tmpConstr;
    string tmpConstr_name;
    char buf[100];
		for(uint32_t k = 0; k < coloring_bits_num; k += COLORING_BITS) 
		{
      sprintf(buf, "%u", m_constrs_num++);  
      tmpConstr_name = "R" + string(buf);
			tmpConstr = opt_model.addConstr(coloringBits[k] + coloringBits[k+1] <= 1, tmpConstr_name);
#ifdef DEBUG_LPCOLORING
      assert(m_stitch_constrs.find(tmpConstr_name) == m_stitch_constrs.end());
#endif
      m_stitch_constrs[tmpConstr_name] = false;
		}
	}
	//optimize model 
	opt_model.update();
	cout << "================== LP iteration #" << m_lp_iter_cnt++ << " ==================\n";
	opt_model.optimize();
#ifdef DEBUG_LPCOLORING
	opt_model.write("graph.lp");
	opt_model.write("graph.sol");
#endif 
	int optim_status = opt_model.get(GRB_IntAttr_Status);
	if(optim_status == GRB_INFEASIBLE) 
	{
		cout << "ERROR: The model is infeasible... EXIT" << endl;
		exit(1);
	}
	cout << endl;

	//iteratively scheme
	while(true) 
	{
		uint32_t non_integer_num, half_integer_num;
		BOOST_AUTO(pair, this->nonIntegerNum(coloringBits, vEdgeBit));
		non_integer_num = pair.first;
		half_integer_num = pair.second;
		if(non_integer_num == 0) break;
		//store the lp coloring results 
		this->store_lp_coloring(coloringBits);
		/*
		   m_lp_coloring.clear();
		   m_lp_coloring.reserve(coloring_bits_num);
		   for(uint32_t k = 0; k < coloring_bits_num; k++) 
		   {
		   double value = coloringBits[k].get(GRB_DoubleAttr_X);
		   m_lp_coloring.push_back(value);
		   }
		   */
#ifdef DEBUG_LPCOLORING
		// compare unrounded conflict number is more fair 
		this->lpConflictNum();
		this->write_graph_dot();
#endif

		vector<bool> non_integer_flag(coloring_bits_num, false);
		//set the new objective
		//push the non-half_integer to 0/1
		// only check for vertices 
    
		for(uint32_t k = 0; k < coloring_bits_num; k += COLORING_BITS) 
		{
			double value1 = coloringBits[k].get(GRB_DoubleAttr_X);
			double value2 = coloringBits[k+1].get(GRB_DoubleAttr_X);
			if (!isInteger(value1) || !isInteger(value2))
			{
				if (value1 > value2)
				{
					if (!conflictCost())
						obj += coloringBits[k+1]-coloringBits[k];
				}
				else if (value1 < value2)
				{
					if (!conflictCost())
						obj += coloringBits[k]-coloringBits[k+1];
				}
				if (value1 == 0.5) non_integer_flag[k] = true;
				if (value2 == 0.5) non_integer_flag[k+1] = true;
			}
#if 0
			if(value < 0.5) 
			{
				if(!conflictCost()) 
					obj = obj + 2*coloringBits[k];
				non_integer_flag[k] = false;
			} 
			else if (value > 0.5) 
			{
				if(!conflictCost()) 
					obj = obj - 2*coloringBits[k];
				non_integer_flag[k] = false;
			} 
			else 
			{
				non_integer_flag[k] = true;
			}
#endif
		}//end for 
    
		//minimize the conflict number 
		if (!conflictCost())
		{
			for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
			{
				BOOST_AUTO(from, source(*itr, m_graph));
				uint32_t f_ind = m_vertex_map[from];
				BOOST_AUTO(to, target(*itr, m_graph));
				uint32_t t_ind = m_vertex_map[to];
				uint32_t vertex_idx1 = f_ind<<1;
				uint32_t vertex_idx2 = t_ind<<1;
				if (m_lp_coloring[vertex_idx1] > m_lp_coloring[vertex_idx2]) 
				{
					obj += coloringBits[vertex_idx2] - coloringBits[vertex_idx1];
				} 
				else if (m_lp_coloring[vertex_idx1] < m_lp_coloring[vertex_idx2]) 
				{
					obj += coloringBits[vertex_idx1] - coloringBits[vertex_idx2];
				}

				vertex_idx1 += 1;
				vertex_idx2 += 1;
				if (m_lp_coloring[vertex_idx1] > m_lp_coloring[vertex_idx2]) 
				{
					obj += coloringBits[vertex_idx2] - coloringBits[vertex_idx1];
				} 
				else if (m_lp_coloring[vertex_idx1] < m_lp_coloring[vertex_idx2]) 
				{
					obj += coloringBits[vertex_idx1] - coloringBits[vertex_idx2];
				}
			}//end for 
		}

		opt_model.setObjective(obj);

		//add the new constraints
		//odd cycle trick from Prof. Baldick
		for(uint32_t k = 0; k < coloring_bits_num; k++) 
		{
			//if the current bit is already handled
			if(false == non_integer_flag[k]) continue;

			uint32_t vertex_index = k/COLORING_BITS;
#ifdef DEBUG_LPCOLORING
			assert(m_inverse_vertex_map.find(vertex_index) != m_inverse_vertex_map.end());
#endif
			graph_vertex_type curr_v = m_inverse_vertex_map[vertex_index];
			//detect the odd cycles related, stored in the m_odd_cycles; 
			this->oddCycles(curr_v);
#ifdef DEBUG_LPCOLORING
			this->write_graph_dot(curr_v);
#endif

			uint32_t odd_cycle_cnt = m_odd_cycles.size();
			for(uint32_t m = 0; m < odd_cycle_cnt; m++) 
			{
				uint32_t cycle_len = m_odd_cycles[m].size();
				GRBLinExpr constraint1 = 0;
				GRBLinExpr constraint2 = 0;
#ifdef DEBUG_LPCOLORING 
				assert(cycle_len%2 == 1);
#endif
				for(uint32_t n = 0; n < cycle_len; ++n) 
				{
#ifdef DEBUG_LPCOLORING
					assert(m_vertex_map.find(m_odd_cycles[m][n]) != m_vertex_map.end());
#endif
					uint32_t vi_index = m_vertex_map[m_odd_cycles[m][n]];
					constraint1 += coloringBits[vi_index<<1];
					constraint2 += coloringBits[(vi_index<<1)+1];

					////non_integer nodes already handled
					for(uint32_t x = 0; x < COLORING_BITS; x++) 
					{
						non_integer_flag[vi_index*COLORING_BITS + x] = false;
					}//end for x
				}//end for
				opt_model.addConstr(constraint1 >= 1);
				opt_model.addConstr(constraint2 >= 1);
			}//end for m
			//for(uint32_t m = 0; m < COLORING_BITS; m++) 
			//{
			//  non_integer_flag[vertex_index*COLORING_BITS + m] = false;
			//}
		}//end for k

		//optimize the new model
		opt_model.update();
		cout << "================== LP iteration #" << m_lp_iter_cnt++ << " ==================\n";
		opt_model.optimize();
#ifdef DEBUG_LPCOLORING
		opt_model.write("graph.lp");
    opt_model.write("graph.sol");
#endif
		optim_status = opt_model.get(GRB_IntAttr_Status);
		if(optim_status == GRB_INFEASIBLE) 
		{
			cout << "ERROR: the model is infeasible... EXIT" << endl;
			exit(1);
		}
		cout << endl;

		//no more non-integers are removed 
		uint32_t non_integer_num_updated, half_integer_num_updated;
		pair = this->nonIntegerNum(coloringBits, vEdgeBit);
		non_integer_num_updated = pair.first;
		half_integer_num_updated = pair.second;

		if (/*non_integer_num_updated == 0*/ half_integer_num_updated == 0 || 
				(non_integer_num_updated >= non_integer_num && half_integer_num_updated >= half_integer_num)) break;

    cout << "\n\n-----------------------------------Next iteration------------------------------" << endl;
	}//end while

	//store the lp coloring results 
	this->store_lp_coloring(coloringBits);
	/*
	   m_lp_coloring.clear();
	   m_lp_coloring.reserve(coloring_bits_num);
	   for(uint32_t k = 0; k < coloring_bits_num; k++) 
	   {
	   double value = coloringBits[k].get(GRB_DoubleAttr_X);
	   m_lp_coloring.push_back(value);
	   }
	   */

#ifdef DEBUG_LPCOLORING
	// compare unrounded conflict number is more fair 
	this->lpConflictNum();
	this->write_graph_dot();
#endif

  //rounding the coloring results  
  this->rounding_bindingAnalysis(opt_model, coloringBits, vEdgeBit);
	if (roundingScheme() == DIRECT_ILP)
		this->rounding_Greedy(coloringBits);
	else if (roundingScheme() == FIXED_ILP)
		this->ILPColoring(coloringBits, vEdgeBit);
	else if (roundingScheme() == ITERATIVE_ILP)
		this->rounding_ILP(opt_model, coloringBits, vEdgeBit);
	else if (roundingScheme() == GREEDY)
		this->rounding_Greedy(coloringBits);

	this->conflictNum();
	this->stitchNum();
	this->write_graph_color();
#ifdef DEBUG_LPCOLORING
	//this->write_graph_dot();
#endif
	switch (roundingScheme())
	{
		case DIRECT_ILP:
			cout << "DIRECT_ILP mode "; break;
		case FIXED_ILP:
			cout << "FIXED_ILP mode "; break;
		case ITERATIVE_ILP:
			cout << "ITERATIVE_ILP mode "; break;
		case GREEDY:
			cout << "GREEDY mode "; break;
		default:
			cout << "Unknown mode "; assert(0);
	}
	cout << "problem solved in " << m_lp_iter_cnt << " LP iterations, " << m_ilp_iter_cnt << " ILP iterations" << endl;

}//end coloring

// ILP based coloring
// all integer bits are fixed 
// non-integer bits are set to integer in the ILP problem 
template<typename GraphType>
void LPColoring<GraphType>::ILPColoring(vector<GRBVar>& coloringBits, vector<GRBVar>& vEdgeBit) 
{
	uint32_t coloring_bits_num = coloringBits.size();

	//set up the LP environment
	GRBEnv env = GRBEnv();
	//mute the log from the LP solver
	//env.set(GRB_IntParam_OutputFlag, 0);
	GRBModel opt_model_updated = GRBModel(env);
	//set up the LP variables
	vector<GRBVar> coloringBits_updated;
	vector<GRBVar> vEdgeBit_updated;
	// vertex and edge variables 
	coloringBits_updated.reserve(coloring_bits_num);
	//set the non-integer solution of the 
	for(uint32_t k = 0; k < coloring_bits_num; k++) 
	{
		ostringstream oss;
		oss << "v" << k;
		double value = coloringBits[k].get(GRB_DoubleAttr_X);
		if(value == 0.0) 
		{
			//integer solution 0.0
			coloringBits_updated.push_back(opt_model_updated.addVar(0.0, 0.0, 0.0, GRB_CONTINUOUS, oss.str()));
		} 
		else if (value == 1.0) 
		{
			//integer solution 1.0
			coloringBits_updated.push_back(opt_model_updated.addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, oss.str()));
		} 
		else 
		{
			//non-integer solution 
			coloringBits_updated.push_back(opt_model_updated.addVar(0.0, 1.0, 0.0, GRB_INTEGER, oss.str()));
		}
	}//end for
	uint32_t edge_num = vEdgeBit.size();
	vEdgeBit_updated.reserve(edge_num);
	for (uint32_t i = 0; i != edge_num; ++i)
	{
		// some variables here may not be used 
		ostringstream oss;
		oss << "e" << i;
		//vEdgeBit_updated.push_back(opt_model_updated.addVar(0.0, 1.0, 0.0, GRB_INTEGER));
		vEdgeBit_updated.push_back(opt_model_updated.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, oss.str()));
	}

	opt_model_updated.update();
	//iterate through the edges
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	//set new objective
	GRBLinExpr obj_init (0);
	for (BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr)
	{
		BOOST_AUTO(w, m_edge_weight_map[*itr]);
		uint32_t edge_idx = m_edge_map[*itr];
		if (w >= 0) // conflict 
			obj_init += vEdgeBit_updated[edge_idx];
		else // stitch 
			obj_init += m_stitch_weight*vEdgeBit_updated[edge_idx];
	}
	opt_model_updated.setObjective(obj_init, GRB_MINIMIZE);

	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		BOOST_AUTO(from, source(*itr, m_graph));
		uint32_t f_ind = m_vertex_map[from];
		BOOST_AUTO(to, target(*itr, m_graph));
		uint32_t t_ind = m_vertex_map[to];
		//coloring conflict constraints
		uint32_t vertex_idx1 = f_ind<<1;
		uint32_t vertex_idx2 = t_ind<<1;

		BOOST_AUTO(w, m_edge_weight_map[*itr]);
		if (w >= 0) // constraints for conflict edges 
		{
			uint32_t edge_idx = m_edge_map[*itr];
			opt_model_updated.addConstr(
					coloringBits_updated[vertex_idx1] + coloringBits_updated[vertex_idx1+1] 
					+ coloringBits_updated[vertex_idx2] + coloringBits_updated[vertex_idx2+1] 
					+ vEdgeBit_updated[edge_idx] >= 1
					);
			opt_model_updated.addConstr(
					1 - coloringBits_updated[vertex_idx1] + coloringBits_updated[vertex_idx1+1] 
					+ 1 - coloringBits_updated[vertex_idx2] + coloringBits_updated[vertex_idx2+1] 
					+ vEdgeBit_updated[edge_idx] >= 1
					);
			opt_model_updated.addConstr(
					coloringBits_updated[vertex_idx1] + 1 - coloringBits_updated[vertex_idx1+1] 
					+ coloringBits_updated[vertex_idx2] + 1 - coloringBits_updated[vertex_idx2+1] 
					+ vEdgeBit_updated[edge_idx] >= 1
					);
			opt_model_updated.addConstr(
					1 - coloringBits_updated[vertex_idx1] + 1 - coloringBits_updated[vertex_idx1+1] 
					+ 1 - coloringBits_updated[vertex_idx2] + 1 - coloringBits_updated[vertex_idx2+1] 
					+ vEdgeBit_updated[edge_idx] >= 1
					);
		}
		else // constraints for stitch edges 
		{
			uint32_t edge_idx = m_edge_map[*itr];
			opt_model_updated.addConstr(
					coloringBits_updated[vertex_idx1] - coloringBits_updated[vertex_idx2] - vEdgeBit_updated[edge_idx] <= 0
					);
			opt_model_updated.addConstr(
					coloringBits_updated[vertex_idx2] - coloringBits_updated[vertex_idx1] - vEdgeBit_updated[edge_idx] <= 0
					);
			opt_model_updated.addConstr(
					coloringBits_updated[vertex_idx1+1] - coloringBits_updated[vertex_idx2+1] - vEdgeBit_updated[edge_idx] <= 0
					);
			opt_model_updated.addConstr(
					coloringBits_updated[vertex_idx2+1] - coloringBits_updated[vertex_idx1+1] - vEdgeBit_updated[edge_idx] <= 0
					);
		}
	}//end for itr
	if (colorNum() == THREE)
	{
		for(uint32_t k = 0; k < coloring_bits_num; k += COLORING_BITS) 
		{
			opt_model_updated.addConstr(coloringBits_updated[k] + coloringBits_updated[k+1] <= 1);
		}
	}

	opt_model_updated.update();

#ifdef DEBUG_LPCOLORING
	opt_model_updated.write("graph_ilp.lp");
#endif

	//optimize model 
	cout << "================== ILP iteration #" << m_ilp_iter_cnt++ << " ==================\n";
	opt_model_updated.optimize();
	int optim_status = opt_model_updated.get(GRB_IntAttr_Status);
	if(optim_status == GRB_INFEASIBLE) 
	{
		cout << "ERROR: The model is infeasible... EXIT" << endl;
		exit(1);
	}
	cout << endl;
	cout << "ILP solved to seek a coloring solution." << endl;

	//store the lp coloring results 
	this->store_lp_coloring(coloringBits_updated);
	//store the coloring results
	for(uint32_t k = 0; k < coloring_bits_num; k = k + COLORING_BITS) {
		uint32_t vertex_index = k/COLORING_BITS;
		graph_vertex_type vertex_key = this->m_inverse_vertex_map[vertex_index];
		uint32_t color = 0;
		uint32_t base = 1;
		for(uint32_t m = 0; m < COLORING_BITS; m++) {
			double value = coloringBits_updated[k+m].get(GRB_DoubleAttr_X);
#if DEBUG_LPCOLORING
			assert(value == 0.0 || value == 1.0);
#endif
			if(value == 1.0) color = color + base;
			base = base<<1;
		}
		//set the coloring
		this->m_coloring[vertex_key] = color;
	}
}

/// optimal rounding based on the binding analysis
template<typename GraphType>
void LPColoring<GraphType>::rounding_bindingAnalysis(GRBModel& opt_model, vector<GRBVar>& coloringBits, vector<GRBVar>& vEdgeBit) {
  cout << "\n\n---------------------------------------Optimal rounding from binding analysis------------------------------------\n";
  /*
//{{{
  GRBConstr* constrs_ptr = opt_model.getConstrs();
  int constrs_num = opt_model.get(GRB_IntAttr_NumConstrs);
  for(int k = 0; k < constrs_num; k++) {
    double slack = constrs_ptr[k].get(GRB_DoubleAttr_Slack);
    cout << "The slack for the " << k << "th constraint - " << constrs_ptr[k].get(GRB_StringAttr_ConstrName) << ": " << slack << endl;
  }//end for k
  */
  uint32_t non_integer_num, half_integer_num;
  uint32_t non_integer_num_updated, half_integer_num_updated;
/*
  //single digit rounding scheme
  while(true) {
  	BOOST_AUTO(pair, this->nonIntegerNum(coloringBits, vEdgeBit));
  	non_integer_num = pair.first;
  	half_integer_num = pair.second;
  	if(non_integer_num == 0) return;
  	//store the lp coloring results 
    this->store_lp_coloring(coloringBits);
  
    //rounding scheme
    bool rounding_flag = true;
    uint32_t variable_num = coloringBits.size();
    for(uint32_t k = 0; k < variable_num; k++) {
      double value = coloringBits[k].get(GRB_DoubleAttr_X);
      if(isInteger(value)) continue;
#ifdef DEBUG_LPCOLORING
      cout << endl << endl << k << "th non-integer variable" << endl;
      //string check_point;
      //cin >> check_point;
#endif
      rounding_flag = true;
      char pre_sense = '=';
      bool pre_sense_flag = false;
      GRBColumn column = opt_model.getCol(coloringBits[k]);
      int column_size = column.size();
      for(int m = 0; m < column_size; m++) {
        GRBConstr constr = column.getConstr(m);
        double slack = constr.get(GRB_DoubleAttr_Slack);
        string name = constr.get(GRB_StringAttr_ConstrName);
        //ignore the non-binding constr
        if(slack != 0.0) continue;
        //ignore the stitch constraint
        if(true == m_stitch_constrs[name]) continue;
        double coeff = opt_model.getCoeff(constr, coloringBits[k]);
        char sense = constr.get(GRB_CharAttr_Sense);
#ifdef DEBUG_LPCOLORING
        cout << "The slack for " << constr.get(GRB_StringAttr_ConstrName) << ": " << slack << endl;
        cout << "The constraint sense for " << constr.get(GRB_StringAttr_ConstrName) << ": " << sense << endl;
        cout << "The coefficient for " << constr.get(GRB_StringAttr_ConstrName) << ": " << coeff << endl << endl;
#endif
#ifdef DEBUG_LPCOLORING
        assert(coeff == 1.0 || coeff == -1.0);
        assert(sense == '>' || sense == '<' || sense == '=');
#endif
        //flip the sense based on coeff
        if(coeff == -1.0) {
          if(sense == '>') sense = '<';
          else if(sense == '<') sense = '>';
        }//end if 

        //set the rounding_flag
        if(false == pre_sense_flag) {
          pre_sense = sense;
          pre_sense_flag = true;
        } else {
          if(sense != pre_sense) {
            rounding_flag = false;
            //break;
          }
        }//end if
      }//end for m
      
      //rounding the first feasible variable
      if(true == rounding_flag) {
#ifdef DEBUG_LPCOLORING
        assert(pre_sense == '<' || pre_sense == '>');
#endif
        if(pre_sense == '<') coloringBits[k].set(GRB_DoubleAttr_UB, 0.0);
        else coloringBits[k].set(GRB_DoubleAttr_LB, 1.0);
        break;
      }
    }//end for k

    if(true == rounding_flag) {
      //update the model and optimize
      opt_model.update();
  	  opt_model.optimize();
#ifdef DEBUG_LPCOLORING
      opt_model.write("graph.lp");
      opt_model.write("graph.sol");
#endif
  	  int optim_status = opt_model.get(GRB_IntAttr_Status);
  	  if(optim_status == GRB_INFEASIBLE) 
  	  {
  		  cout << "ERROR: the ILP model is infeasible... Quit ILP based rounding" << endl;
        exit(1);
  	  }
    }//end if
  
  	//no more non-integers are removed 
  	pair = this->nonIntegerNum(coloringBits, vEdgeBit);
  	non_integer_num_updated = pair.first;
  	half_integer_num_updated = pair.second;
  	if (half_integer_num_updated == 0 || 
  			(non_integer_num_updated >= non_integer_num && half_integer_num_updated >= half_integer_num)) break;

  }//end while
//}}}
*/

  //a pair of digits rounding scheme, essentially this is a hard coding implementation
  while(true) {
  	BOOST_AUTO(pair, this->nonIntegerNum(coloringBits, vEdgeBit));
  	non_integer_num = pair.first;
  	half_integer_num = pair.second;
  	if(non_integer_num == 0) return;
  	//store the lp coloring results 
    this->store_lp_coloring(coloringBits);
  
    //rounding scheme
    bool rounding_flag = true;
    double bit1 = 0.0, bit2 = 0.0;
    uint32_t variable_num = coloringBits.size();
    for(uint32_t k = 0; k < variable_num; k = k + COLORING_BITS) {
      double value1 = coloringBits[k].get(GRB_DoubleAttr_X);
      double value2 = coloringBits[k+1].get(GRB_DoubleAttr_X);
      if(isInteger(value1) && isInteger(value2)) continue;
#ifdef DEBUG_LPCOLORING
      //cout << endl << endl << k << "th and " << (k + 1) << "th non-integer variable" << endl;
      //string check_point;
      //cin >> check_point;
#endif
      GRBColumn column1 = opt_model.getCol(coloringBits[k]);
      GRBColumn column2 = opt_model.getCol(coloringBits[k+1]);
      int column1_size = column1.size();
      int column2_size = column2.size();
      //constraints flag, shared constraints betweeen column1 & 2 are flagged true, otherwise false
      //coefficient, -1 or 1
      //sense, < or >
      unordered_map<string, bool> constrs_flag1;
      unordered_map<string, double> constrs_coeff1;
      unordered_map<string, char> constrs_sense1;
      unordered_map<string, bool> constrs_flag2;
      unordered_map<string, double> constrs_coeff2;
      unordered_map<string, char> constrs_sense2;
      //process the first set of binding constraints
#ifdef DEBUG_LPCOLORING
      cout << endl << endl << k << "th non-integer variable" << endl;
#endif
      for(int m = 0; m < column1_size; m++) {
        GRBConstr constr = column1.getConstr(m);
        double slack = constr.get(GRB_DoubleAttr_Slack);
        //ignore the non-binding constr
        if(slack != 0.0) continue;
        double coeff = opt_model.getCoeff(constr, coloringBits[k]);
        char sense = constr.get(GRB_CharAttr_Sense);
#ifdef DEBUG_LPCOLORING
        cout << "The slack for " << constr.get(GRB_StringAttr_ConstrName) << ": " << slack << endl;
        cout << "The constraint sense for " << constr.get(GRB_StringAttr_ConstrName) << ": " << sense << endl;
        cout << "The coefficient for " << constr.get(GRB_StringAttr_ConstrName) << ": " << coeff << endl << endl;
#endif
#ifdef DEBUG_LPCOLORING
        assert(coeff == 1.0 || coeff == -1.0);
        assert(sense == '>' || sense == '<');
#endif
        string name = constr.get(GRB_StringAttr_ConstrName);
        constrs_flag1[name] = false;
        constrs_coeff1[name] = coeff;
        constrs_sense1[name] = sense;
      }//end for m

#ifdef DEBUG_LPCOLORING
      cout << endl << endl << (k+1) << "th non-integer variable" << endl;
#endif
      //process the second set of binding constraints
      for(int m = 0; m < column2_size; m++) {
        GRBConstr constr = column2.getConstr(m);
        double slack = constr.get(GRB_DoubleAttr_Slack);
        //ignore the non-binding constr
        if(slack != 0.0) continue;
        double coeff = opt_model.getCoeff(constr, coloringBits[k+1]);
        char sense = constr.get(GRB_CharAttr_Sense);
#ifdef DEBUG_LPCOLORING
        cout << "The slack for " << constr.get(GRB_StringAttr_ConstrName) << ": " << slack << endl;
        cout << "The constraint sense for " << constr.get(GRB_StringAttr_ConstrName) << ": " << sense << endl;
        cout << "The coefficient for " << constr.get(GRB_StringAttr_ConstrName) << ": " << coeff << endl << endl;
#endif
#ifdef DEBUG_LPCOLORING
        assert(coeff == 1.0 || coeff == -1.0);
        assert(sense == '>' || sense == '<');
#endif
        string name = constr.get(GRB_StringAttr_ConstrName);
        if(constrs_flag1.find(name) == constrs_flag1.end()) 
        {
          constrs_flag2[name] = false;
          constrs_coeff2[name] = coeff;
          constrs_sense2[name] = sense;
        } 
        else 
        {
          constrs_flag1[name] = true;
          constrs_flag2[name] = true;
          constrs_coeff2[name] = coeff;
          constrs_sense2[name] = sense;
        }//end if
      }//end for m
      
      //flip the sense of the un-shared constraints if necessary
      unordered_map<string, double> shared_constrs_coeff1;
      unordered_map<string, char> shared_constrs_sense1;
      BOOST_AUTO(end, constrs_flag1.end());
      for(BOOST_AUTO(itr, constrs_flag1.begin()); itr != end; ++itr) {
        //ignore shared constraint
        if(true == itr->second) 
        {
          shared_constrs_coeff1[itr->first] = constrs_coeff1[itr->first];
          shared_constrs_sense1[itr->first] = constrs_sense1[itr->first];
          continue;
        }
        if(-1.0 == constrs_coeff1[itr->first]) 
        {
          //flip sense
          constrs_coeff1[itr->first] = 1.0;
          if(constrs_sense1[itr->first] == '<') constrs_sense1[itr->first] = '>';
          else constrs_sense1[itr->first] = '<';
        }//end if
      }

      unordered_map<string, double> shared_constrs_coeff2;
      unordered_map<string, double> shared_constrs_sense2;
      end = constrs_flag2.end();
      for(BOOST_AUTO(itr, constrs_flag2.begin()); itr != end; ++itr) {
        //ignore shared constraint
        if(true == itr->second) 
        {
          shared_constrs_coeff2[itr->first] = constrs_coeff2[itr->first];
          shared_constrs_sense2[itr->first] = constrs_sense2[itr->first];
          continue;
        }
        if(-1.0 == constrs_coeff2[itr->first])
        {
          //flip sense
          constrs_coeff2[itr->first] = 1.0;
          if(constrs_sense2[itr->first] == '<') constrs_sense2[itr->first] = '>';
          else constrs_sense2[itr->first] = '<';
        }//end if
      }
#ifdef DEBUG_LPCOLORING
      assert(shared_constrs_coeff1.size() == shared_constrs_coeff2.size());
#endif
      
      //check whether value1 can be rounded
      bool rounding_flag1 = true;
      char pre_sense1 = '=';
      bool pre_sense_flag = false;
      end = constrs_flag1.end();
      for(BOOST_AUTO(itr, constrs_flag1.begin()); itr != end; ++itr) {
        //ignore shared constraint
        if(true == itr->second) continue;
#ifdef DEBUG_LPCOLORING
        assert(1.0 == constrs_coeff1[itr->first]);
#endif
        if(false == pre_sense_flag) 
        {
          pre_sense1 = constrs_sense1[itr->first];
          pre_sense_flag = true;
        }
        else 
        {
          if(pre_sense1 != constrs_sense1[itr->first]) {
            rounding_flag1 = false;
            break;
          }
        }//end if
      }//end for itr
      
      //check whether value2 can be rounded
      bool rounding_flag2 = true;
      char pre_sense2 = '=';
      pre_sense_flag = false;
      end = constrs_flag2.end();
      for(BOOST_AUTO(itr, constrs_flag2.begin()); itr != end; ++itr) {
        //ignore the shared constraint
        if(true == itr->second) continue;
#ifdef DEBUG_LPCOLORING
        assert(1.0 == constrs_coeff2[itr->first]);
#endif
        if(false == pre_sense_flag) 
        {
          pre_sense2 = constrs_sense2[itr->first];
          pre_sense_flag = true;
        }
        else 
        {
          if(pre_sense2 != constrs_sense2[itr->first]) {
            rounding_flag2 = false;
            break;
          }
        }//end if
      }//end for itr
      
      //check value1 and value2 can be rounded on un-shared constraints
      rounding_flag = rounding_flag1 & rounding_flag2;
      if(false == rounding_flag) break;

      //check whether value1&2 can be rounded simultaneously for shared constraints
      //flag whether a feasible flag is found or not
      bool bit_pair_flag = false;
      bit1 = 0.0, bit2 = 0.0;
      for(bit1 = 0.0; bit1 <= 1.0; bit1 = bit1 + 1.0) {
        if(pre_sense1 == '<' && bit1 == 1.0) continue;
        if(pre_sense1 == '>' && bit1 == 0.0) continue;
        for(bit2 = 0.0; bit2 <= 1.0; bit2 = bit2 + 1.0) {
          if(pre_sense2 == '<' && bit2 == 1.0) continue;
          if(pre_sense2 == '>' && bit2 == 0.0) continue;
          cout << "bit1: " << bit1 << "; bit2: " << bit2 << endl;
	        if (colorNum() == THREE && (bit1 == 1.0) && (bit2 == 1.0)) continue;
          //check each shared constraint
          BOOST_AUTO(itr_end, shared_constrs_coeff1.end());
          for(BOOST_AUTO(itr, shared_constrs_coeff1.begin()); itr != itr_end; ++itr) {
            double delta_value = itr->second * (bit1 - value1) + shared_constrs_coeff2[itr->first] * (bit2 - value2);
            char sense = shared_constrs_sense1[itr->first];
            if(delta_value < 0 && sense == '>') {
              rounding_flag = false; 
              break;
            }
            if(delta_value > 0 && sense == '<') {
              rounding_flag = false;
              break;
            }
          }//end for itr
          bit_pair_flag = true;
          if(true == rounding_flag) break;
        }//end for bit2
        if(true == rounding_flag) break;
      }//end for bit1
      //rounding the first feasible variable
      if(true == rounding_flag && true == bit_pair_flag) {
#ifdef DEBUG_LPCOLORING
#endif
        coloringBits[k].set(GRB_DoubleAttr_UB, bit1);
        coloringBits[k].set(GRB_DoubleAttr_LB, bit1);
        coloringBits[k+1].set(GRB_DoubleAttr_UB, bit2);
        coloringBits[k+1].set(GRB_DoubleAttr_LB, bit2);
        break;
      }
    }//end for k

    if(true == rounding_flag) {
      //update the model and optimize
      opt_model.update();
  	  opt_model.optimize();
#ifdef DEBUG_LPCOLORING
      opt_model.write("graph.lp");
      opt_model.write("graph.sol");
      this->store_lp_coloring(coloringBits);
	    this->write_graph_dot();
#endif
  	  int optim_status = opt_model.get(GRB_IntAttr_Status);
  	  if(optim_status == GRB_INFEASIBLE) 
  	  {
  		  cout << "ERROR: the model after binding analysis is infeasible... Quit ILP based rounding" << endl;
        exit(1);
  	  }
    }//end if
  
  	//no more non-integers are removed 
  	pair = this->nonIntegerNum(coloringBits, vEdgeBit);
  	non_integer_num_updated = pair.first;
  	half_integer_num_updated = pair.second;
  	if (/*non_integer_num_updated == 0*/ half_integer_num_updated == 0 || 
  			(non_integer_num_updated >= non_integer_num && half_integer_num_updated >= half_integer_num)) break;

  }//end while


  this->store_lp_coloring(coloringBits);
  return;
}

/// ILP based rounding
template<typename GraphType>
void LPColoring<GraphType>::rounding_ILP(GRBModel& opt_model, vector<GRBVar>& coloringBits, vector<GRBVar>& vEdgeBit) 
{
  cout << "\n\n---------------------------------------ILP rounding scheme------------------------------------\n";
  uint32_t non_integer_num, half_integer_num;
  uint32_t non_integer_num_updated, half_integer_num_updated;
  while(true) 
  {
		BOOST_AUTO(pair, this->nonIntegerNum(coloringBits, vEdgeBit));
		non_integer_num = pair.first;
		half_integer_num = pair.second;
		if(non_integer_num == 0) break;
		//store the lp coloring results 
		this->store_lp_coloring(coloringBits);
#ifdef DEBUG_LPCOLORING
		// compare unrounded conflict number is more fair 
		this->lpConflictNum();
#endif
		//update the ILP model
		uint32_t coloring_bits_num = coloringBits.size();
		for(uint32_t k = 0; k < coloring_bits_num; ++k) 
		{
			double value = coloringBits[k].get(GRB_DoubleAttr_X);
			if(value == 1.0 || value == 0.0) 
			{
				coloringBits[k].set(GRB_CharAttr_VType, 'C');
			}
			else 
			{
				coloringBits[k].set(GRB_CharAttr_VType, 'I');
			}
		}//end for k
		opt_model.update();
#ifdef DEBUG_LPCOLORING
		opt_model.write("graph_ilp.lp");
#endif
		cout << "================== ILP iteration #" << m_ilp_iter_cnt++ << " ==================\n";
		opt_model.optimize();
		int optim_status = opt_model.get(GRB_IntAttr_Status);
		if(optim_status == GRB_INFEASIBLE) 
		{
			cout << "ERROR: the ILP model is infeasible... Quit ILP based rounding" << endl;
#ifdef DEBUG_LPCOLORING
			opt_model.computeIIS();
			opt_model.write("graph_ilp.ilp");
#endif
			exit(1);
		}

		//no more non-integers are removed 
		pair = this->nonIntegerNum(coloringBits, vEdgeBit);
		non_integer_num_updated = pair.first;
		half_integer_num_updated = pair.second;
		if (/*non_integer_num_updated == 0*/ half_integer_num_updated == 0 || 
				(non_integer_num_updated >= non_integer_num && half_integer_num_updated >= half_integer_num)) break;
	}//end while 

	//final greedy rounding
	this->rounding_Greedy(coloringBits);
}

//greedy rounding scheme, need better scheme later on 
template<typename GraphType>
void LPColoring<GraphType>::rounding_Greedy_v0(vector<GRBVar>& coloringBits) 
{
  cout << "\n\n---------------------------------------Greedy rounding scheme------------------------------------\n";

	//greedy rounding scheme
	uint32_t vec_size = coloringBits.size();
	//the rounding results
	vector<bool> coloringBinary(vec_size, false);
	//the flag denoting whether current bit is rounded or not
	vector<bool> roundingFlag(vec_size, false); 
	//rounding by range
	for(uint32_t k = 0; k < vec_size; k++) 
	{
		double value = coloringBits[k].get(GRB_DoubleAttr_X);
		if (0.0 == value) 
		{
			coloringBinary[k] = false;
			roundingFlag[k] = true;
		} 
		else if (1.0 == value)
		{
			coloringBinary[k] = true;
			roundingFlag[k] = true;
		} 
		else 
		{
			coloringBinary[k] = false;
			roundingFlag[k] = false;
		}//end if 
	}//end for 

	//rounding of the half integer
	//greedy rounding schme should minimize the conflict and stitch number
	for(uint32_t k = 0; k < vec_size; k++) 
	{
		if(true == roundingFlag[k]) continue;
#ifdef DEBUG_LPCOLORING
		//this->printBoolArray(roundingFlag, vec_size);
		//this->printBoolArray(coloringBinary, vec_size);
		//cout << endl << endl;
#endif
		//greedy rounding scheme 
		uint32_t vertex_index = k/COLORING_BITS;
		vector<bool> color_bits;
		color_bits.reserve(COLORING_BITS);
		vector<bool> best_bits;
		best_bits.reserve(COLORING_BITS);
		//initialize the color_bits
		for(uint32_t m = 0; m < COLORING_BITS; ++m) 
		{
			color_bits.push_back(coloringBinary[vertex_index*COLORING_BITS + m]);
		}//end for

		//get the neighbors
		typename boost::graph_traits<graph_type>::adjacency_iterator vi_begin, vi_end;
		BOOST_AUTO(vertex_key, m_inverse_vertex_map[vertex_index]);
		boost::tie(vi_begin, vi_end) = adjacent_vertices(vertex_key, m_graph);
		//calculate the current 
		uint32_t same_color_bound = std::numeric_limits<uint32_t>::max(); 
		uint32_t same_color_count = 0;
		uint32_t color = 0;
		uint32_t base = 1;
		while(true) 
		{
			same_color_bound = std::numeric_limits<uint32_t>::max();
			same_color_count = 0;
			color = 0;
			base = 1;
			for(uint32_t m = 0; m < COLORING_BITS; ++m) 
			{
				if(color_bits[m]) color = color + base;
				base = base<<1;
			}//end for
			//check the same color neighbors
			for(BOOST_AUTO(vi, vi_begin); vi != vi_end; ++vi) 
			{
				if(this->m_coloring.find(*vi) == m_vertex_map.end()) continue;
				if(this->m_coloring[*vi] == color) same_color_count++;
			}//end for
			//assign better color
			if(same_color_count < same_color_bound) 
			{
				same_color_bound = same_color_count;
				best_bits = color_bits;
			}

			//explore the next color_bits
			bool nextFlag = false;
			for(uint32_t m = 0; m < COLORING_BITS; ++m) 
			{
				if(color_bits[m] == false && roundingFlag[vertex_index*COLORING_BITS + m] == false) 
				{
					//flip the color bit that has not be rounded 
					color_bits[m] = true;
					nextFlag = true;
          //for Triple patterning mode
          if((colorNum() == THREE) && (m == COLORING_BITS - 1)) nextFlag = false;
          break;
				}
			}//end for m
			//all the color_bits are explored
			if(nextFlag == false) break;
		}//end while

		//the vertex is colored
		color = 0;
		base = 1;
		for(uint32_t m = 0; m < COLORING_BITS; m++) 
		{
			coloringBinary[vertex_index*COLORING_BITS + m] = best_bits[m];
			roundingFlag[vertex_index*COLORING_BITS + m] = true;
			if(best_bits[m]) color = color + base;
			base = base<<1;
		}
#ifdef DEBUG_LPCOLORING
		assert(color < 4);
#endif
		this->m_coloring[vertex_key] = color;
	}//end for k

	//assign the coloring results 
	for(uint32_t k = 0; k < vec_size; k = k + COLORING_BITS) 
	{
		BOOST_AUTO(vertex_key, this->m_inverse_vertex_map[(k/COLORING_BITS)]);
		uint32_t color = 0;
		uint32_t base = 1;
		for(uint32_t m = 0; m < COLORING_BITS; ++m) 
		{
			if(coloringBinary[k + m]) {
        this->m_lp_coloring[k + m] = 1.0;
        color = color + base;
      } else {
        this->m_lp_coloring[k + m] = 0.0;
      }
			base = base<<1;
		}//end for k
		if(this->m_coloring.find(vertex_key) == this->m_coloring.end())
			this->m_coloring[vertex_key] = color;
#ifdef DEBUG_LPCOLORING
		else 
		{ 
			//cout << "stored color-" << this->m_coloring[vertex_key] << " vs assigned color-" << color << endl;
			assert(this->m_coloring[vertex_key] == color);
		}
#endif
	}//end for 
}

template<typename GraphType>
void LPColoring<GraphType>::rounding_Greedy(vector<GRBVar>& coloringBits) 
{
  cout << "\n\n---------------------------------------Greedy rounding scheme------------------------------------\n";

	//greedy rounding scheme
	uint32_t vec_size = coloringBits.size();
  this->store_lp_coloring(coloringBits);

	//greedy rounding schme should minimize the conflict and stitch number
	for(uint32_t k = 0; k < vec_size; k = k + COLORING_BITS) 
	{
    double value1 = coloringBits[k].get(GRB_DoubleAttr_X);
    double value2 = coloringBits[k+1].get(GRB_DoubleAttr_X);
		if(isInteger(value1) && isInteger(value2)) continue;
		//greedy rounding scheme 
		uint32_t vertex_index = k/COLORING_BITS;

		//get the neighbors
		typename boost::graph_traits<graph_type>::adjacency_iterator vi_begin, vi_end;
		BOOST_AUTO(vertex_key, m_inverse_vertex_map[vertex_index]);
		boost::tie(vi_begin, vi_end) = adjacent_vertices(vertex_key, m_graph);
		//calculate the current 
		uint32_t same_color_bound = std::numeric_limits<uint32_t>::max(); 
		uint32_t same_color_count = 0;
		double best_bit1 = 0.0, best_bit2 = 0.0;
    //greedy selection 
    for(double bit1 = 0.0; bit1 <= 1.0; bit1 = bit1 + 1.0)
		{
      if(isInteger(value1) && (bit1 != value1)) continue;
      for(double bit2 = 0.0; bit2 <= 1.0; bit2 = bit2 + 1.0) 
      {
        if(isInteger(value2) && (bit2 != value2)) continue;
	      if (colorNum() == THREE && (bit1 == 1.0) && (bit2 == 1.0)) continue;
        same_color_count = 0;
        //cout << endl << "current_index: " << vertex_index << ", bits: " << bit1 << bit2 << endl;
  			//check the same color neighbors
  			for(BOOST_AUTO(vi, vi_begin); vi != vi_end; ++vi) 
  			{
#ifdef DEBUG_LPCOLORING
  				assert(this->m_vertex_map.find(*vi) != m_vertex_map.end());
#endif
          uint32_t neighbor_index = this->m_vertex_map[*vi];
          //same color neighbor
  				if((m_lp_coloring[2*neighbor_index] == bit1) && (m_lp_coloring[2*neighbor_index+1] == bit2)) 
          { 
            same_color_count++;
            //cout << "conflict neighbor_index: " << neighbor_index << endl;
          }
  			}//end for
  			//assign better color
  			if(same_color_count < same_color_bound) 
  			{
  				same_color_bound = same_color_count;
  				best_bit1 = bit1;
          best_bit2 = bit2;
  			}
      }//end for bit2
		}//end for bit1

		//the vertex is colored
    this->m_lp_coloring[k] = best_bit1;
    this->m_lp_coloring[k+1] = best_bit2;
	}//end for k

	//assign the coloring results 
	for(uint32_t k = 0; k < vec_size; k = k + COLORING_BITS) 
	{
		BOOST_AUTO(vertex_key, this->m_inverse_vertex_map[(k/COLORING_BITS)]);
		uint32_t color = 0;
		uint32_t base = 1;
		for(uint32_t m = 0; m < COLORING_BITS; ++m) 
		{
#ifdef DEBUG_LPCOLORING
      assert(isInteger(this->m_lp_coloring[k + m]));
#endif
			if(this->m_lp_coloring[k + m] == 1.0) color = color + base;
      base = base<<1;
		}//end for k
		if(this->m_coloring.find(vertex_key) == this->m_coloring.end())
			this->m_coloring[vertex_key] = color;
#ifdef DEBUG_LPCOLORING
		else 
		{ 
			//cout << "stored color-" << this->m_coloring[vertex_key] << " vs assigned color-" << color << endl;
			assert(this->m_coloring[vertex_key] == color);
		}
#endif
	}//end for 
}



//get the vertex color
template<typename GraphType>
uint32_t LPColoring<GraphType>::vertexColor(graph_vertex_type& node) const
{
	//the graph is not colored
	if(this->m_coloring.empty()) return 0; //this->graphColoring(m_graph);
	return this->m_coloring.at(node);
}

template <typename GraphType>
bool LPColoring<GraphType>::sameColor(typename LPColoring<GraphType>::graph_vertex_type v1, 
		typename LPColoring<GraphType>::graph_vertex_type v2) const 
{
	assert(!m_coloring.empty());
#ifdef DEBUG_LPCOLORING
	assert(this->m_coloring.find(v1) != this->m_coloring.end());
	assert(this->m_coloring.find(v2) != this->m_coloring.end());
#endif
	return m_coloring.at(v1) == m_coloring.at(v2);
}

template <typename GraphType>
bool LPColoring<GraphType>::hasConflict(typename LPColoring<GraphType>::graph_edge_type e) const 
{
	BOOST_AUTO(w, m_edge_weight_map[e]);
	// skip stitch edges 
	if (w < 0) return false;

	BOOST_AUTO(from, source(e, m_graph));
	BOOST_AUTO(to, target(e, m_graph));
	return sameColor(from, to);
}

template <typename GraphType>
bool LPColoring<GraphType>::hasStitch(typename LPColoring<GraphType>::graph_edge_type e) const 
{
	BOOST_AUTO(w, m_edge_weight_map[e]);
	// skip conflict edges 
	if (w >= 0) return false;

	BOOST_AUTO(from, source(e, m_graph));
	BOOST_AUTO(to, target(e, m_graph));
	return !sameColor(from, to);
}

//report the conflict number
template<typename GraphType>
uint32_t LPColoring<GraphType>::conflictNum() const
{
	//the graph is not colored
	if (this->m_coloring.empty()) return 0; //this->graphColoring();
	//check the coloring results
	uint32_t conflict_edge_num = 0;
	uint32_t conflict_num = 0;
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		if (hasConflict(*itr))
			++conflict_num;
		++conflict_edge_num;
	}//end for itr
	cout << "Conflict number: " << conflict_num << " out of " << conflict_edge_num << " conflict edges" << endl;
	return conflict_num;
}

//report the stitch number
template<typename GraphType>
uint32_t LPColoring<GraphType>::stitchNum() const
{
	//the graph is not colored
	if(this->m_coloring.empty()) return 0; //this->graphColoring(m_graph);

	//check the coloring results
	uint32_t stitch_edge_num = 0;
	uint32_t stitch_num = 0;
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		if (hasStitch(*itr))
			++stitch_num;
		++stitch_edge_num;
	}//end for itr
	cout << "Stitch number: " << stitch_num << " out of " << stitch_edge_num << " stitch edges" << endl;
	return stitch_num;
}

template <typename GraphType>
bool LPColoring<GraphType>::lpSameColor(typename LPColoring<GraphType>::graph_vertex_type v1, 
		typename LPColoring<GraphType>::graph_vertex_type v2) const 
{
	assert(!m_lp_coloring.empty());

	uint32_t f_ind = m_vertex_map.at(v1);
	uint32_t t_ind = m_vertex_map.at(v2);
	uint32_t vertex_idx1 = f_ind<<1;
	uint32_t vertex_idx2 = t_ind<<1;

	return m_lp_coloring.at(vertex_idx1) == m_lp_coloring.at(vertex_idx2)
		&& m_lp_coloring.at(vertex_idx1+1) == m_lp_coloring.at(vertex_idx2+1);
}

template <typename GraphType>
bool LPColoring<GraphType>::hasLPConflict(typename LPColoring<GraphType>::graph_edge_type e) const 
{
	BOOST_AUTO(w, m_edge_weight_map[e]);
	// skip stitch edges 
	if (w < 0) return false;

	BOOST_AUTO(from, source(e, m_graph));
	BOOST_AUTO(to, target(e, m_graph));
	return lpSameColor(from, to);
}

template <typename GraphType>
bool LPColoring<GraphType>::hasLPStitch(typename LPColoring<GraphType>::graph_edge_type e) const 
{
	BOOST_AUTO(w, m_edge_weight_map[e]);
	// skip conflict edges 
	if (w >= 0) return false;

	BOOST_AUTO(from, source(e, m_graph));
	BOOST_AUTO(to, target(e, m_graph));
	return !lpSameColor(from, to);
}

//report the unrounded LP conflict number
template<typename GraphType>
uint32_t LPColoring<GraphType>::lpConflictNum() const
{
	if (m_lp_coloring.empty()) return 0;
	//check the coloring results
	uint32_t conflict_edge_num = 0;
	uint32_t conflict_num = 0;
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		if (hasLPConflict(*itr))
			++conflict_num;
		++conflict_edge_num;
	}//end for itr
	cout << "LP Conflict number: " << conflict_num << " out of " << conflict_edge_num << " conflict edges" << endl;
	return conflict_num;
}

//report the unrounded LP stitch number
template<typename GraphType>
uint32_t LPColoring<GraphType>::lpStitchNum() const
{
	if (m_lp_coloring.empty()) return 0;
	//check the coloring results
	uint32_t stitch_edge_num = 0;
	uint32_t stitch_num = 0;
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		if (hasLPStitch(*itr))
		{
			++stitch_num;
		}
		++stitch_edge_num;
	}//end for itr
	cout << "LP Stitch number: " << stitch_num << " out of " << stitch_edge_num << " stitch edges" << endl;
	return stitch_num;
}

//store the lp coloring results 
template<typename GraphType>
void LPColoring<GraphType>::store_lp_coloring(vector<GRBVar>& coloringBits) {
  uint32_t coloring_bits_num = coloringBits.size();
	m_lp_coloring.clear();
	m_lp_coloring.reserve(coloring_bits_num);
	for(uint32_t k = 0; k < coloring_bits_num; k++) 
	{
		double value = coloringBits[k].get(GRB_DoubleAttr_X);
		m_lp_coloring.push_back(value);
	}
  return;
}

//for debug use
template<typename GraphType>
pair<uint32_t, uint32_t> LPColoring<GraphType>::nonIntegerNum(const vector<GRBVar>& coloringBits, const vector<GRBVar>& vEdgeBit) const
{
	uint32_t non_integer_num = 0;
	uint32_t half_num = 0;
	uint32_t vec_size = coloringBits.size();
	for(uint32_t k = 0; k < vec_size; k = k + COLORING_BITS) 
	{
		for(uint32_t m = 0; m < COLORING_BITS; m++) 
		{
			double value = coloringBits[k + m].get(GRB_DoubleAttr_X);
			if(value != 0.0 && value != 1.0) 
			{
				non_integer_num++;
				//break;
			}
			if(value == 0.5) half_num++;
			cout << k+m << "-" << value << " ";
		}
	}//end for k
	cout << endl;
	cout << "NonInteger count: " << non_integer_num << ", half integer count: " << half_num << ", out of " << vec_size << " vertex variable" << endl;

	uint32_t non_integer_num_edge = 0;
	uint32_t half_num_edge = 0;
	uint32_t conflict_num = 0;
	vec_size = vEdgeBit.size();
	for(uint32_t k = 0; k < vec_size; ++k) 
	{
		double value = vEdgeBit[k].get(GRB_DoubleAttr_X);
		if(value != 0.0 && value != 1.0) 
		{
			non_integer_num_edge++;
			//break;
		}
		if(value == 0.5) half_num_edge++;
		if(value == 1) conflict_num++;
		cout << k << "-" << value << " ";
	}//end for k
	cout << endl;
	cout << "NonInteger count: " << non_integer_num_edge << ", half integer count: " << half_num_edge << ", out of " << vec_size << " edge variable" << endl;
	//cout << "conflict variable number: " << conflict_num << endl;
	return pair<uint32_t, uint32_t>(non_integer_num+non_integer_num_edge, half_num+half_num_edge);
}

template<typename GraphType>
void LPColoring<GraphType>::printBoolVec(vector<bool>& vec) const
{
	uint32_t vec_size = vec.size();
	for(uint32_t k = 0; k < vec_size; k++) 
	{
		//cout << k << "-" << vec[k] << "; ";
		cout << vec[k];
	}//end for 
	cout << endl;
}

template<typename GraphType>
void LPColoring<GraphType>::printBoolArray(bool* vec, uint32_t vec_size) const
{
	for(uint32_t k = 0; k < vec_size; k++) 
	{
		//cout << k << "-" << vec[k] << "; ";
		cout << vec[k];
	}//end for 
	cout << endl;
}

//print graphviz
template<typename GraphType>
void LPColoring<GraphType>::write_graph_dot(graph_vertex_type& v) const
{
#ifdef DEBUG_LPCOLORING
	//  assert(m_graph_ptr);
#endif
	//if(m_vertex_map.empty() || m_inverse_vertex_map.empty()) this->setMap();

	ofstream dot_file("graph.dot");
	dot_file << "graph D { \n"
		<< "  randir = LR\n"
		<< "  size=\"4, 3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\",fontsize=120]\n" 
		<< "  node[shape=\"circle\",fontsize=120]\n";

	//output nodes 
	uint32_t vertex_num = num_vertices(m_graph);
	//check cycles
	bool inCycle[vertex_num];
	for(uint32_t k = 0; k < vertex_num; k++) inCycle[k] = false;
	uint32_t cycle_cnt = m_odd_cycles.size();
	for(uint32_t k = 0; k < cycle_cnt; k++) 
	{
		uint32_t cycle_len = m_odd_cycles[k].size();
		for(uint32_t m = 0; m < cycle_len; m++) 
		{
			uint32_t index = m_vertex_map.at(m_odd_cycles[k][m]);
			inCycle[index] = true;
		}
	}//end for k

#ifdef DEBUG_LPCOLORING
	assert(m_vertex_map.find(v) != m_vertex_map.end());
#endif
	uint32_t start_index = m_vertex_map.at(v);
	for(uint32_t k = 0; k < vertex_num; k++) 
	{
		if(k == start_index) dot_file << "  " << k << "[shape=\"square\"";
		else dot_file << "  " << k << "[shape=\"circle\"";
		//output coloring label
		dot_file << ",label=\"" << k << ":(" << m_lp_coloring[2*k] << "," << m_lp_coloring[2*k+1] << ")\"";
		if(inCycle[k]) dot_file << ",color=\"red\"]\n";
		else dot_file << "]\n";
	}//end for

	//output edges
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		BOOST_AUTO(from, source(*itr, m_graph));
		uint32_t f_ind = m_vertex_map.at(from);
		BOOST_AUTO(to, target(*itr, m_graph));
		uint32_t t_ind = m_vertex_map.at(to);
    bool conflict_flag = false;
    if(m_lp_coloring[2*f_ind] == m_lp_coloring[2*t_ind] && m_lp_coloring[2*f_ind+1] == m_lp_coloring[2*t_ind+1]) {
      conflict_flag = true;
    }//end if
		if (m_edge_weight_map[*itr] >= 0) // conflict edge 
    {
      if(conflict_flag)
			  dot_file << "  " << f_ind << "--" << t_ind << "[color=\"blue\",style=\"solid\"]\n";
      else 
			  dot_file << "  " << f_ind << "--" << t_ind << "[color=\"black\",style=\"solid\"]\n";
    }
		else // stitch edge 
			dot_file << "  " << f_ind << "--" << t_ind << "[color=\"black\",style=\"dashed\"]\n";
	}
	dot_file << "}";
	dot_file.close();
	system("dot -Tpdf graph.dot -o graph.pdf");
}

//print graphviz
template<typename GraphType>
void LPColoring<GraphType>::write_graph_dot() const
{
#ifdef DEBUG_LPCOLORING
	//  assert(m_graph_ptr);
#endif
	//if(m_vertex_map.empty() || m_inverse_vertex_map.empty()) this->setMap();

	ofstream dot_file("graph.dot");
	dot_file << "graph D { \n"
		<< "  randir = LR\n"
		<< "  size=\"4, 3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\",fontsize=200]\n" 
		<< "  node[shape=\"circle\",fontsize=200]\n";

	//output nodes 
	uint32_t vertex_num = num_vertices(m_graph);
	for(uint32_t k = 0; k < vertex_num; k++) 
	{
		dot_file << "  " << k << "[shape=\"circle\"";
		//output coloring label
		//dot_file << ",label=\"" << k << ":(" << m_lp_coloring[2*k] << "," << m_lp_coloring[2*k+1] << ")\"";
		dot_file << ",label=\"(" << m_lp_coloring[2*k] << "," << m_lp_coloring[2*k+1] << ")\"";
		dot_file << "]\n";
	}//end for

	//output edges
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		BOOST_AUTO(from, source(*itr, m_graph));
		uint32_t f_ind = m_vertex_map.at(from);
		BOOST_AUTO(to, target(*itr, m_graph));
		uint32_t t_ind = m_vertex_map.at(to);
		bool conflict_flag = false;
		if(m_lp_coloring[2*f_ind] == m_lp_coloring[2*t_ind] && m_lp_coloring[2*f_ind+1] == m_lp_coloring[2*t_ind+1]) 
		{
			conflict_flag = true;
		}//end if
		if (m_edge_weight_map[*itr] >= 0) // conflict edge 
		{
			if(conflict_flag)
			{
				dot_file << "  " << f_ind << "--" << t_ind << "[color=\"blue\",style=\"solid\",penwidth=3]\n";
				cout << "conflict (" << f_ind << ", " << t_ind << ")" << endl;
			}
			else 
				dot_file << "  " << f_ind << "--" << t_ind << "[color=\"black\",style=\"solid\",penwidth=3]\n";
		}
		else // stitch edge 
			dot_file << "  " << f_ind << "--" << t_ind << "[color=\"black\",style=\"dashed\",penwidth=3]\n";
	}
	dot_file << "}";
	dot_file.close();
	system("dot -Tpdf graph.dot -o graph.pdf");
}



template<typename GraphType>
void LPColoring<GraphType>::write_graph_color() const
{
#ifdef DEBUG_LPCOLORING
	//  assert(m_graph_ptr);
#endif
	//if(m_vertex_map.empty() || m_inverse_vertex_map.empty()) this->setMap();
	ofstream dot_file("color_graph.dot");
	dot_file << "graph D { \n"
		<< "  randir = LR\n"
		<< "  size=\"4, 3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\",fontsize=120]\n" 
		<< "  node[shape=\"circle\",fontsize=120]\n";

	//output nodes 
	uint32_t vertex_num = num_vertices(m_graph);
	//check cycles
	bool inCycle[vertex_num];
	for(uint32_t k = 0; k < vertex_num; k++) inCycle[k] = false;
	uint32_t cycle_cnt = m_odd_cycles.size();
	for(uint32_t k = 0; k < cycle_cnt; k++) 
	{
		uint32_t cycle_len = m_odd_cycles[k].size();
		for(uint32_t m = 0; m < cycle_len; m++) 
		{
			uint32_t index = m_vertex_map.at(m_odd_cycles[k][m]);
			inCycle[index] = true;
		}
	}//end for k

	for(uint32_t k = 0; k < vertex_num; k++) 
	{
		dot_file << "  " << k << "[shape=\"circle\"";
		//output coloring label
		dot_file << ",label=\"" << m_coloring.at(m_inverse_vertex_map.at(k)) << "\"";
		if(inCycle[k]) dot_file << ",color=\"red\"]\n";
		else dot_file << "]\n";
	}//end for

	//output edges
	pair<typename graph_type::edge_iterator, typename graph_type::edge_iterator> edge_range = edges(m_graph);
	for(BOOST_AUTO(itr, edge_range.first); itr != edge_range.second; ++itr) 
	{
		BOOST_AUTO(from, source(*itr, m_graph));
		uint32_t f_ind = m_vertex_map.at(from);
		BOOST_AUTO(to, target(*itr, m_graph));
		uint32_t t_ind = m_vertex_map.at(to);

		if (m_edge_weight_map[*itr] >= 0) // conflict edge 
		{
			if(m_coloring.at(from) != m_coloring.at(to)) 
				dot_file << "  " << f_ind << "--" << t_ind << "[color=\"black\",style=\"solid\",penwidth=3]\n";
			else 
				dot_file << "  " << f_ind << "--" << t_ind << "[color=\"blue\",style=\"solid\",penwidth=3]\n";
		}
		else // stitch edge 
		{
			if(m_coloring.at(from) == m_coloring.at(to)) 
				dot_file << "  " << f_ind << "--" << t_ind << "[color=\"black\",style=\"dashed\",penwidth=3]\n";
			else 
				dot_file << "  " << f_ind << "--" << t_ind << "[color=\"blue\",style=\"dashed\",penwidth=3]\n";
		}
	}
	dot_file << "}";
	dot_file.close();
	system("dot -Tpdf color_graph.dot -o color_graph.pdf");
}

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif

/// @endcond
