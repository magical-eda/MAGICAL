/**
 * @file   ILPColoringLemonCbc.h
 * @brief  coloring algorithm based on integer linear programming (ILP) with Cbc as ILP solver. 
 *
 * Cbc is accessed with the API provided by Lemon, so both Cbc and Lemon should be installed properly. 
 * It is necessary for Lemon to bind CBC when installing. 
 *
 * @author Yibo Lin
 * @date   Jun 2015
 */

#ifndef LIMBO_ALGORITHMS_COLORING_ILPCOLORINGLEMONCBC
#define LIMBO_ALGORITHMS_COLORING_ILPCOLORINGLEMONCBC

#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <limits>
#include <cassert>
#include <cmath>
#include <stdlib.h>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <boost/cstdint.hpp>
#include <boost/unordered_map.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/property_map/property_map.hpp>
//#include <boost/graph/bipartite.hpp>
//#include <boost/graph/kruskal_min_spanning_tree.hpp>
//#include <boost/graph/prim_minimum_spanning_tree.hpp>
//#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <limbo/string/String.h>
#include <limbo/algorithms/coloring/Coloring.h>
#include <lemon/cbc.h>
#include <lemon/lp.h>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

using std::ostringstream;
using boost::unordered_map;

/// @class limbo::algorithms::coloring::ILPColoringLemonCbc
/// ILP coloring algorithm using Cbc as ILP solver and Lemon as API to Cbc, 
/// i.e. this class has dependency to Cbc and Lemon. 
/// Lemon should be installed with Cbc flag enabled 
/// 
/// Edge weight is used to differentiate conflict edge and stitch edge.
/// Non-negative weight implies conflict edge. 
/// Negative weight implies stitch edge. 
/// Use vertex color to save vertex stitch candidate number.
/// 
/// @tparam GraphType graph type 
template <typename GraphType>
class ILPColoringLemonCbc : public Coloring<GraphType>
{
	public:
        /// @nowarn 
		typedef Coloring<GraphType> base_type;
		using typename base_type::graph_type;
		using typename base_type::graph_vertex_type;
		using typename base_type::graph_edge_type;
		using typename base_type::vertex_iterator_type;
		using typename base_type::edge_iterator_type;
        using typename base_type::edge_weight_type;
		using typename base_type::ColorNumType;
        /// @endnowarn

		/// hasher class for graph_edge_type
		struct edge_hash_type : std::unary_function<graph_edge_type, std::size_t>
		{
            /// get hash value for an edge 
            /// @param e edge 
            /// @return hash value 
			std::size_t operator()(graph_edge_type const& e) const 
			{
				std::size_t seed = 0;
				boost::hash_combine(seed, e.m_source);
				boost::hash_combine(seed, e.m_target);
				return seed;
			}
		};

		/// constructor
        /// @param g graph 
		ILPColoringLemonCbc(graph_type const& g) 
			: base_type(g)
		{}
		/// destructor
		virtual ~ILPColoringLemonCbc() {}

	protected:
		/// @return objective value 
		virtual double coloring();
};

template <typename GraphType>
double ILPColoringLemonCbc<GraphType>::coloring()
{
	uint32_t vertex_num = boost::num_vertices(this->m_graph);
	uint32_t edge_num = boost::num_edges(this->m_graph);
	uint32_t vertex_variable_num = vertex_num<<1;

	unordered_map<graph_vertex_type, uint32_t> hVertexIdx; // vertex index 
	unordered_map<graph_edge_type, uint32_t, edge_hash_type> hEdgeIdx; // edge index 

	vertex_iterator_type vi, vie;
	uint32_t cnt = 0;
	for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi, ++cnt)
		hVertexIdx[*vi] = cnt;
	edge_iterator_type ei, eie;
	cnt = 0; 
	for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei, ++cnt)
		hEdgeIdx[*ei] = cnt;

	typedef lemon::MipSolver::Row Row; // constraints dimension
	typedef lemon::MipSolver::Col Col; // variable dimension 
	typedef lemon::MipSolver::Expr Expr; // expression 

	// ILP environment
	lemon::CbcMip opt_model;
	//mute the log from the LP solver
	// set threads 
	//set up the ILP variables
	vector<Col> vVertexBit;
	vector<Col> vEdgeBit;

	// vertex variables 
	vVertexBit.reserve(vertex_variable_num); 
	for (uint32_t i = 0; i != vertex_variable_num; ++i)
	{
		uint32_t vertex_idx = (i>>1);
		ostringstream oss; 
		oss << "v" << i;
		if (this->m_vColor[vertex_idx] >= 0 && this->m_vColor[vertex_idx] < this->m_color_num) // precolored 
		{
			int8_t color_bit;
			if ((i&1) == 0) color_bit = (this->m_vColor[vertex_idx]>>1)&1;
			else color_bit = this->m_vColor[vertex_idx]&1;
			vVertexBit.push_back(opt_model.addCol());
			Col const& x = vVertexBit.back();
			opt_model.colBounds(x, color_bit, color_bit);
			opt_model.colType(x, lemon::MipSolver::INTEGER);
			opt_model.colName(x, oss.str());
		}
		else // uncolored 
		{
			vVertexBit.push_back(opt_model.addCol());
			Col const& x = vVertexBit.back();
			opt_model.colBounds(x, 0, 1);
			opt_model.colType(x, lemon::MipSolver::INTEGER);
			opt_model.colName(x, oss.str());
		}
	}

	// edge variables 
	vEdgeBit.reserve(edge_num);
	for (uint32_t i = 0; i != edge_num; ++i)
	{
		ostringstream oss;
		oss << "e" << i;
		vEdgeBit.push_back(opt_model.addCol());
		Col const& x = vEdgeBit.back();
		opt_model.colBounds(x, 0, 1);
		opt_model.colType(x, lemon::MipSolver::REAL);
		opt_model.colName(x, oss.str());
	}

	// update model 

	// set up the objective 
	Expr obj;
	for (boost::tie(ei, eie) = edges(this->m_graph); ei != eie; ++ei)
	{
		edge_weight_type w = boost::get(boost::edge_weight, this->m_graph, *ei);
		if (w > 0) // weighted conflict 
			obj += w*vEdgeBit[hEdgeIdx[*ei]];
		else if (w < 0) // weighted stitch 
			obj += this->m_stitch_weight*(-w)*vEdgeBit[hEdgeIdx[*ei]];
	}
	opt_model.obj(obj);
	opt_model.min();

	// set up the constraints
	//uint32_t constr_num = 0;
	for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
	{
		graph_vertex_type s = boost::source(*ei, this->m_graph);
		graph_vertex_type t = boost::target(*ei, this->m_graph);
		uint32_t sIdx = hVertexIdx[s];
		uint32_t tIdx = hVertexIdx[t];

		uint32_t vertex_idx1 = sIdx<<1;
		uint32_t vertex_idx2 = tIdx<<1;

		edge_weight_type w = boost::get(boost::edge_weight, this->m_graph, *ei);
		uint32_t edge_idx = hEdgeIdx[*ei];

		//char buf[100];
		if (w >= 0) // constraints for conflict edges 
		{
			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					vVertexBit[vertex_idx1] + vVertexBit[vertex_idx1+1] 
					+ vVertexBit[vertex_idx2] + vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= 1
					);

			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					1 - vVertexBit[vertex_idx1] + vVertexBit[vertex_idx1+1] 
					+ 1 - vVertexBit[vertex_idx2] + vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= 1
					);

			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					vVertexBit[vertex_idx1] + 1 - vVertexBit[vertex_idx1+1] 
					+ vVertexBit[vertex_idx2] + 1 - vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= 1
					);

			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					1 - vVertexBit[vertex_idx1] + 1 - vVertexBit[vertex_idx1+1] 
					+ 1 - vVertexBit[vertex_idx2] + 1 - vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= 1
					);

		}
		else // constraints for stitch edges 
		{
			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					vVertexBit[vertex_idx1] - vVertexBit[vertex_idx2] - vEdgeBit[edge_idx] <= 0
					);

			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					vVertexBit[vertex_idx2] - vVertexBit[vertex_idx1] - vEdgeBit[edge_idx] <= 0
					);

			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					vVertexBit[vertex_idx1+1] - vVertexBit[vertex_idx2+1] - vEdgeBit[edge_idx] <= 0
					);      

			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(
					vVertexBit[vertex_idx2+1] - vVertexBit[vertex_idx1+1] - vEdgeBit[edge_idx] <= 0
					);
		}
	}

	// additional constraints for 3-coloring 
	if (this->m_color_num == base_type::THREE)
	{
		//char buf[100];
		for(uint32_t k = 0; k != vertex_variable_num; k += 2) 
		{
			//sprintf(buf, "R%u", constr_num++);  
			opt_model.addRow(vVertexBit[k] + vVertexBit[k+1] <= 1);
		}
	}

	//optimize model 
	opt_model.solve();
	int32_t opt_status = opt_model.type();
	if (opt_status == lemon::MipSolver::INFEASIBLE) 
	{
		cout << "ERROR: The model is infeasible... EXIT" << endl;
		exit(1);
	}

	// collect coloring solution 
	for (uint32_t k = 0; k != vertex_variable_num; k += 2)
	{
		int8_t color = (opt_model.sol(vVertexBit[k])*2) + opt_model.sol(vVertexBit[k+1]);
		uint32_t vertex_idx = (k>>1);

		assert(color >= 0 && color < this->m_color_num);
		if (this->m_vColor[vertex_idx] >= 0 && this->m_vColor[vertex_idx] < this->m_color_num) // check precolored vertex 
			assert(this->m_vColor[vertex_idx] == color);
		else // assign color to uncolored vertex 
			this->m_vColor[vertex_idx] = color;
	}

	// return objective value 
	return opt_model.solValue();
}

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
