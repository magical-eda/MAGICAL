/**
 * @file   ILPColoring.h
 * @brief  coloring algorithm based on integer linear programming (ILP) with Gurobi as ILP solver.
 * @author Yibo Lin
 * @date   May 2015
 */

#ifndef LIMBO_ALGORITHMS_COLORING_ILPCOLORING
#define LIMBO_ALGORITHMS_COLORING_ILPCOLORING

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
#include <limbo/solvers/Solvers.h>
#include <limbo/solvers/api/GurobiApi.h>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

/// @class limbo::algorithms::coloring::ILPColoring
/// ILP based graph coloring.  
/// Edge weight is used to differentiate conflict edge and stitch edge. 
/// Non-negative weight implies conflict edge, 
/// while negative weight implies stitch edge. 
/// 
/// @tparam GraphType graph type 
template <typename GraphType>
class ILPColoring : public Coloring<GraphType>
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
        typedef typename base_type::EdgeHashType edge_hash_type;
        typedef limbo::solvers::LinearModel<float, int32_t> model_type; 
        typedef limbo::solvers::GurobiLinearApi<model_type::coefficient_value_type, model_type::variable_value_type> solver_type; 
        /// @endnowarn
        
		/// constructor
        /// @param g graph 
		ILPColoring(graph_type const& g) 
			: base_type(g)
		{}
		/// destructor
		virtual ~ILPColoring() {}

        /// write raw solution of ILP 
        /// @param filename output file name 
        /// @param opt_model problem model 
        /// @param vVertexBit array of vertex bits that indicate coloring solutions; each vertex corresponds to two bits 
        void write_graph_sol(string const& filename, model_type const& opt_model, std::vector<model_type::variable_type> const& vVertexBit) const;

	protected:
        /// kernel coloring algorithm 
		/// @return objective value 
		virtual double coloring();
};

template <typename GraphType>
double ILPColoring<GraphType>::coloring()
{
	uint32_t vertex_num = boost::num_vertices(this->m_graph);
	uint32_t edge_num = boost::num_edges(this->m_graph);
	uint32_t vertex_variable_num = vertex_num<<1;

	boost::unordered_map<graph_edge_type, uint32_t, edge_hash_type> hEdgeIdx; // edge index 

	uint32_t cnt = 0;
	edge_iterator_type ei, eie;
	for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei, ++cnt)
		hEdgeIdx[*ei] = cnt;

	/// ILP model 
    model_type opt_model;
    limbo::solvers::GurobiParameters gurobiParams; 
    gurobiParams.setOutputFlag(0); 
    gurobiParams.setNumThreads(this->m_threads);
	//set up the ILP variables
    std::vector<model_type::variable_type> vVertexBit;
    std::vector<model_type::variable_type> vEdgeBit;

    opt_model.reserveVariables(vertex_variable_num+edge_num); 
    if (this->m_color_num == base_type::THREE)
        opt_model.reserveConstraints(edge_num*4+vertex_num);
    else 
        opt_model.reserveConstraints(edge_num*4);

	// vertex variables 
	vVertexBit.reserve(vertex_variable_num); 
	for (uint32_t i = 0; i != vertex_variable_num; ++i)
	{
		uint32_t vertex_idx = (i>>1);
		std::ostringstream oss; 
		oss << "v" << i;
		if (this->m_vColor[vertex_idx] >= 0 && this->m_vColor[vertex_idx] < this->m_color_num) // precolored 
		{
			int8_t color_bit;
			if ((i&1) == 0) color_bit = (this->m_vColor[vertex_idx]>>1)&1;
			else color_bit = this->m_vColor[vertex_idx]&1;
			vVertexBit.push_back(opt_model.addVariable(color_bit, color_bit, limbo::solvers::INTEGER, oss.str()));
		}
		else // uncolored 
			vVertexBit.push_back(opt_model.addVariable(0, 1, limbo::solvers::INTEGER, oss.str()));
	}

	// edge variables 
	vEdgeBit.reserve(edge_num);
	for (uint32_t i = 0; i != edge_num; ++i)
	{
		std::ostringstream oss;
		oss << "e" << i;
		vEdgeBit.push_back(opt_model.addVariable(0, 1, limbo::solvers::INTEGER, oss.str()));
	}

	// set up the objective 
    model_type::expression_type obj;
	for (boost::tie(ei, eie) = edges(this->m_graph); ei != eie; ++ei)
	{
		edge_weight_type w = boost::get(boost::edge_weight, this->m_graph, *ei);
		if (w > 0) // weighted conflict 
			obj += w*vEdgeBit[hEdgeIdx[*ei]];
		else if (w < 0) // weighted stitch 
			obj += this->m_stitch_weight*(-w)*vEdgeBit[hEdgeIdx[*ei]];
	}
	opt_model.setObjective(obj);
    opt_model.setOptimizeType(limbo::solvers::MIN); 

	// set up the constraints
	uint32_t constr_num = 0;
	for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
	{
		graph_vertex_type s = boost::source(*ei, this->m_graph);
		graph_vertex_type t = boost::target(*ei, this->m_graph);

		uint32_t vertex_idx1 = s<<1;
		uint32_t vertex_idx2 = t<<1;

		edge_weight_type w = boost::get(boost::edge_weight, this->m_graph, *ei);
		uint32_t edge_idx = hEdgeIdx[*ei];

		char buf[100];
		string tmpConstr_name;
		if (w >= 0) // constraints for conflict edges 
		{
			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					vVertexBit[vertex_idx1] + vVertexBit[vertex_idx1+1] 
					+ vVertexBit[vertex_idx2] + vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= 1
					, buf);

			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					- vVertexBit[vertex_idx1] + vVertexBit[vertex_idx1+1] 
					- vVertexBit[vertex_idx2] + vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= -1
					, buf);

			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					vVertexBit[vertex_idx1] - vVertexBit[vertex_idx1+1] 
					+ vVertexBit[vertex_idx2] - vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= -1
					, buf);

			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					- vVertexBit[vertex_idx1] - vVertexBit[vertex_idx1+1] 
					- vVertexBit[vertex_idx2] - vVertexBit[vertex_idx2+1] 
					+ vEdgeBit[edge_idx] >= -3
					, buf);

		}
		else // constraints for stitch edges 
		{
			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					vVertexBit[vertex_idx1] - vVertexBit[vertex_idx2] - vEdgeBit[edge_idx] <= 0
					, buf);

			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					vVertexBit[vertex_idx2] - vVertexBit[vertex_idx1] - vEdgeBit[edge_idx] <= 0
					, buf);

			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					vVertexBit[vertex_idx1+1] - vVertexBit[vertex_idx2+1] - vEdgeBit[edge_idx] <= 0
					, buf);      

			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(
					vVertexBit[vertex_idx2+1] - vVertexBit[vertex_idx1+1] - vEdgeBit[edge_idx] <= 0
					, buf);
		}
	}

	// additional constraints for 3-coloring 
	if (this->m_color_num == base_type::THREE)
	{
		char buf[100];
		for(uint32_t k = 0; k != vertex_variable_num; k += 2) 
		{
			sprintf(buf, "R%u", constr_num++);  
			opt_model.addConstraint(vVertexBit[k] + vVertexBit[k+1] <= 1, buf);
		}
	}

	//optimize model 
    solver_type solver (&opt_model); 
    int32_t opt_status = solver(&gurobiParams); 
#ifdef DEBUG_ILPCOLORING
	opt_model.print("graph.lp");
	opt_model.printSolution("graph.sol");
#endif 
	if(opt_status == limbo::solvers::INFEASIBLE) 
	{
		cout << "ERROR: The model is infeasible... EXIT" << endl;
		exit(1);
	}

#ifdef DEBUG_ILPCOLORING
    this->write_graph_sol("graph_sol", opt_model, vVertexBit); // dump solution figure 
#endif

	// collect coloring solution 
	for (uint32_t k = 0; k != vertex_variable_num; k += 2)
	{
		int8_t color = (opt_model.variableSolution(vVertexBit[k])*2)+opt_model.variableSolution(vVertexBit[k+1]);
		uint32_t vertex_idx = (k>>1);

		assert(color >= 0 && color < this->m_color_num);
		if (this->m_vColor[vertex_idx] >= 0 && this->m_vColor[vertex_idx] < this->m_color_num) // check precolored vertex 
			assert(this->m_vColor[vertex_idx] == color);
		else // assign color to uncolored vertex 
			this->m_vColor[vertex_idx] = color;
	}

	// return objective value 
	return opt_model.evaluateObjective();
}

template <typename GraphType>
void ILPColoring<GraphType>::write_graph_sol(string const& filename, typename ILPColoring<GraphType>::model_type const& opt_model, 
        std::vector<typename ILPColoring<GraphType>::model_type::variable_type> const& vVertexBit) const
{
	std::ofstream out((filename+".gv").c_str());
	out << "graph D { \n"
		<< "  randir = LR\n"
		<< "  size=\"4, 3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\",fontsize=200]\n" 
		<< "  node[shape=\"circle\",fontsize=200]\n";

	//output nodes 
	uint32_t vertex_num = boost::num_vertices(this->m_graph);
	for(uint32_t k = 0; k < vertex_num; ++k) 
	{
		out << "  " << k << "[shape=\"circle\"";
		//output coloring label
		out << ",label=\"" << k << ":(" << opt_model.variableSolution(vVertexBit[(k<<1)]) << "," << opt_model.variableSolution(vVertexBit[(k<<1)+1]) << ")\"";
		out << "]\n";
	}//end for

	//output edges
	edge_iterator_type ei, eie;
	for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
	{
		edge_weight_type w = boost::get(boost::edge_weight, this->m_graph, *ei);
		graph_vertex_type s = boost::source(*ei, this->m_graph);
		graph_vertex_type t = boost::target(*ei, this->m_graph);
		if (w >= 0) // conflict edge 
		{
            bool conflict_flag = (opt_model.variableSolution(vVertexBit[(s<<1)]) == opt_model.variableSolution(vVertexBit[(t<<1)]) 
                    && opt_model.variableSolution(vVertexBit[(s<<1)+1]) == opt_model.variableSolution(vVertexBit[(t<<1)+1]));

			if(conflict_flag)
				out << "  " << s << "--" << t << "[color=\"red\",style=\"solid\",penwidth=3]\n";
			else 
				out << "  " << s << "--" << t << "[color=\"black\",style=\"solid\",penwidth=3]\n";
		}
		else // stitch edge 
			out << "  " << s << "--" << t << "[color=\"blue\",style=\"dotted\",penwidth=3]\n";
	}
	out << "}";
	out.close();
    la::graphviz2pdf(filename);
}

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
