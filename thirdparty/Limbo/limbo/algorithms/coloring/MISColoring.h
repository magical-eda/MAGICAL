/**
 * @file   MISColoring.h
 * @author Yibo Lin
 * @date   May 2017
 */
#ifndef LIMBO_ALGORITHMS_COLORING_MISCOLORING
#define LIMBO_ALGORITHMS_COLORING_MISCOLORING

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
#if GUROBI == 1
#include <limbo/solvers/Solvers.h>
#include <limbo/solvers/api/GurobiApi.h>
#endif

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

/// @class limbo::algorithms::coloring::MISColoring
/// MIS based graph coloring, which means iteratively search for the maximum independent set.  
/// Edge weight is used to differentiate conflict edge and stitch edge. 
/// Non-negative weight implies conflict edge, 
/// while negative weight implies stitch edge. 
/// 
/// @tparam GraphType graph type 
template <typename GraphType>
class MISColoring : public Coloring<GraphType>
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
#if GUROBI == 1
        typedef limbo::solvers::LinearModel<float, int32_t> model_type; 
        typedef limbo::solvers::GurobiLinearApi<model_type::coefficient_value_type, model_type::variable_value_type> solver_type; 
#endif
        /// @endnowarn
        
		/// constructor
        /// @param g graph 
		MISColoring(graph_type const& g) 
			: base_type(g)
		{}
		/// destructor
		virtual ~MISColoring() {}

		/// set precolored vertex, not supported  
        /// param v vertex 
        /// param c color 
		virtual void precolor(graph_vertex_type /*v*/, int8_t /*c*/) {limboAssert("precoloring is not supported");}

        /// write raw solution of MIS 
        /// @param filename output file name 
        /// @param vVertexExcludeMark marked true for vertices to exclude from graph 
        /// @param vMISVertex vertices of maximum independent set 
        void write_graph_sol(string const& filename, std::vector<int8_t> const& vVertexExcludeMark, std::vector<graph_vertex_type> const& vMISVertex) const
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
                if (vVertexExcludeMark[k]) // excluded
                    out << ",label=\"" << k << "\",style=filled,fillcolor=\"grey\"";
                else if (std::find(vMISVertex.begin(), vMISVertex.end(), k) != vMISVertex.end())
                    out << ",label=\"" << k << "\",style=filled,fillcolor=\"red\"";
                else 
                    out << ",label=\"" << k << "\"";
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
                    if (vVertexExcludeMark[s] || vVertexExcludeMark[t]) // skipped 
                        out << "  " << s << "--" << t << "[color=\"grey\",style=\"solid\",penwidth=3]\n";
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

	protected:
        /// kernel coloring algorithm 
		/// @return objective value 
		virtual double coloring()
        {
            std::vector<int8_t> vVertexExcludeMark (boost::num_vertices(this->m_graph), 0); 
            std::vector<graph_vertex_type> vMISVertex; 

            for (int8_t c = 0; c < this->color_num(); ++c)
            {
                vMISVertex.clear(); 
                // compute maximum independent set to the residual graph 
#if GUROBI == 1
                // find maximum independent set with maximum number of vertices 
                computeMWISILP(vVertexExcludeMark, vMISVertex); 
#else 
                // only find maximum independent set 
                computeMIS(vVertexExcludeMark, vMISVertex); 
#endif
                // apply color 
                for (typename std::vector<graph_vertex_type>::const_iterator vi = vMISVertex.begin(); vi != vMISVertex.end(); ++vi)
                {
                    this->m_vColor[*vi] = c; 
                    vVertexExcludeMark[*vi] = true; 
                }
            }

            // greedy coloring for the rest 
            vertex_iterator_type vi, vie;
            for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
            {
                graph_vertex_type v = *vi; 
                if (!vVertexExcludeMark[v]) // skip excluded vertices 
                {
                    int8_t bestColor = 0; 
                    edge_weight_type bestCost = std::numeric_limits<edge_weight_type>::max(); 
                    for (int8_t c = 0; c < this->color_num(); ++c)
                    {
                        edge_weight_type curCost = 0; 
                        typename boost::graph_traits<graph_type>::adjacency_iterator ui, uie;
                        for (boost::tie(ui, uie) = boost::adjacent_vertices(v, this->m_graph); ui != uie; ++ui)
                        {
                            graph_vertex_type u = *ui; 
                            if (this->m_vColor[u] >= 0 && this->m_vColor[u] < this->color_num())
                            {
                                if (c == this->m_vColor[u])
                                {
                                    std::pair<graph_edge_type, bool> eU2v = boost::edge(v, u, this->m_graph);
#ifdef DEBUG_MISCOLORING
                                    assert(eU2v.second);
#endif
                                    // edge weight is important since we may deal with merged graphs
                                    // this is just for generality purpose 
                                    curCost += std::max((edge_weight_type)1, boost::get(boost::edge_weight, this->m_graph, eU2v.first));
                                }
                            }
                        }
                        if (curCost < bestCost)
                        {
                            bestCost = curCost;
                            bestColor = c;
                        }
                    }
                    this->m_vColor[v] = bestColor; 
                }
            }

#ifdef DEBUG_MISCOLORING
            this->write_graph("final_output");
#endif
            return this->calc_cost(this->m_vColor); 
        }

#if GUROBI == 1
        /// @brief compute maximum independent set with ILP assuming weight of vertices is 1. 
        /// Other weight is not supported yet.  
        /// The objective is to minimize the overall weight of edges in the residual graph.  
        /// @param vVertexExcludeMark marked true for vertices to exclude from graph 
        /// @param vMISVertex store vertices of maximum independent set 
        /// @return size of maximum independent set 
        double computeMWISILP(std::vector<int8_t> const& vVertexExcludeMark, std::vector<graph_vertex_type>& vMISVertex)
        {
            uint32_t vertex_num = boost::num_vertices(this->m_graph);
            uint32_t edge_num = boost::num_edges(this->m_graph); 
            uint32_t exclude_vertex_num = std::count(vVertexExcludeMark.begin(), vVertexExcludeMark.end(), 1); 

            /// ILP model 
            model_type opt_model;
            limbo::solvers::GurobiParameters gurobiParams; 
            gurobiParams.setOutputFlag(0); 
            gurobiParams.setNumThreads(this->m_threads);
            //set up the ILP variables
            // this is a map of length vertex_num 
            std::vector<model_type::variable_type> vVertex2Var (vertex_num);
            boost::unordered_map<graph_edge_type, model_type::variable_type, edge_hash_type> hEdge2Var; // edge index 

            // vertex variables 
            opt_model.reserveVariables(vertex_num-exclude_vertex_num+edge_num); 
            vertex_iterator_type vi, vie;
            for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
            {
                graph_vertex_type v = *vi; 
                if (!vVertexExcludeMark[v]) // skip excluded vertices 
                {
                    std::ostringstream oss; 
                    oss << "v" << v; 
                    vVertex2Var[v] = opt_model.addVariable(0, 1, limbo::solvers::INTEGER, oss.str()); 
                }
            }
            // edge variables 
            edge_iterator_type ei, eie; 
            for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
            {
                graph_vertex_type s = boost::source(*ei, this->m_graph);
                graph_vertex_type t = boost::target(*ei, this->m_graph);

                if (!vVertexExcludeMark[s] && !vVertexExcludeMark[t]) // skip any edge connected to excluded vertices 
                {
                    std::ostringstream oss; 
                    oss << "e" << s << "_" << t; 
                    hEdge2Var.insert(std::make_pair(*ei, opt_model.addVariable(0, 1, limbo::solvers::INTEGER, oss.str()))); 
                }
            }

            // set up the objective 
            // maximize the edge weight selected, which is equivalent to minimize the edge weight in the residual graph 
            // max. sum e
            model_type::expression_type obj;
            for (typename boost::unordered_map<graph_edge_type, model_type::variable_type, edge_hash_type>::const_iterator it = hEdge2Var.begin(); it != hEdge2Var.end(); ++it)
                obj += it->second; 
            opt_model.setObjective(obj);
            opt_model.setOptimizeType(limbo::solvers::MAX); 

            // conflict constraints 
            char buf[100];
            uint32_t constr_num = 0; 
            for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
            {
                graph_vertex_type s = boost::source(*ei, this->m_graph);
                graph_vertex_type t = boost::target(*ei, this->m_graph);

                if (!vVertexExcludeMark[s] && !vVertexExcludeMark[t]) // skip any edge connected to excluded vertices 
                {
                    sprintf(buf, "R%u", constr_num++);  
                    opt_model.addConstraint(
                            vVertex2Var[s] + vVertex2Var[t] <= 1
                            , buf);
                }
            }
            // edge selected constraints 
            // the edge variable will be 1 if any of its vertices is selected 
            // e <= vs + vt 
            // if vs = vt = 0, e = 0
            // else e can be any value between 0 and 1 
            // consider that we are maximizing e in the objective 
            for (typename boost::unordered_map<graph_edge_type, model_type::variable_type, edge_hash_type>::const_iterator it = hEdge2Var.begin(); it != hEdge2Var.end(); ++it)
            {
                graph_edge_type const& e = it->first; 
                graph_vertex_type s = boost::source(e, this->m_graph);
                graph_vertex_type t = boost::target(e, this->m_graph);

                sprintf(buf, "E%u", constr_num++);  
                opt_model.addConstraint(
                        it->second - vVertex2Var[s] - vVertex2Var[t] <= 0
                        , buf);
            }

            //optimize model 
            solver_type solver (&opt_model); 
            int32_t opt_status = solver(&gurobiParams); 
#ifdef DEBUG_MISCOLORING
            opt_model.print("graph.lp");
            opt_model.printSolution("graph.sol");
#endif 
            if(opt_status == limbo::solvers::INFEASIBLE) 
            {
                cout << "ERROR: The model is infeasible... EXIT" << endl;
                exit(1);
            }

            // collect maximum independent set vertices 
            for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
            {
                graph_vertex_type v = *vi; 
                if (!vVertexExcludeMark[v]) // skip excluded vertices 
                {
                    int8_t value = opt_model.variableSolution(vVertex2Var[v]); 
                    if (value)
                        vMISVertex.push_back(v); 
                }
            }

#ifdef DEBUG_MISCOLORING
            this->write_graph_sol("graph_sol", vVertexExcludeMark, vMISVertex); // dump solution figure 
#endif

            // return objective value 
            return opt_model.evaluateObjective();
        }
#endif

        /// @brief compute maximum independent set, weight is not supported yet  
        /// @param vVertexExcludeMark marked true for vertices to exclude from graph 
        /// @param vMISVertex store vertices of maximum independent set 
        /// @return size of maximum independent set 
        double computeMIS(std::vector<int8_t> const& vVertexExcludeMark, std::vector<graph_vertex_type>& vMISVertex)
        {
            uint32_t vertex_num = boost::num_vertices(this->m_graph);

            // this is a map of length vertex_num 
            std::set<graph_vertex_type> sVertex;

            for (uint32_t i = 0; i < vertex_num; ++i)
            {
                if (!vVertexExcludeMark[i]) // skip excluded vertices 
                    sVertex.insert(i); 
            }

            // while sVertex is not empty 
            //     choose v \in V 
            //     add v to vMISVertex 
            //     remove neighbors of v from V 
            while (!sVertex.empty())
            {
                graph_vertex_type v = *sVertex.begin(); 
                vMISVertex.push_back(v); 

                sVertex.erase(v);
                typename boost::graph_traits<graph_type>::adjacency_iterator ui, uie;
                for (boost::tie(ui, uie) = boost::adjacent_vertices(v, this->m_graph); ui != uie; ++ui)
                {
                    graph_vertex_type u = *ui; 
                    sVertex.erase(u); 
                }
            }

#ifdef DEBUG_MISCOLORING
            this->write_graph_sol("graph_sol", vVertexExcludeMark, vMISVertex); // dump solution figure 
#endif

            // return size of independent set  
            return vMISVertex.size();
        }
};

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
