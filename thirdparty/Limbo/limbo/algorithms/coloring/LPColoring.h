/**
 * @file   LPColoring.h
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

#ifndef LIMBO_ALGORITHMS_COLORING_LP
#define LIMBO_ALGORITHMS_COLORING_LP

#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <cassert>
#include <cmath>
#include <stdlib.h>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <boost/cstdint.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/dynamic_bitset.hpp>
#include <limbo/string/String.h>
#include <limbo/algorithms/coloring/Coloring.h>
#include "gurobi_c++.h"

#ifdef DEBUG_NONINTEGERS
extern std::vector<unsigned int> vLP1NonInteger; 
extern std::vector<unsigned int> vLP1HalfInteger; 
extern std::vector<unsigned int> vLP2NonInteger; 
extern std::vector<unsigned int> vLP2HalfInteger; 
extern std::vector<unsigned int> vLPEndNonInteger; 
extern std::vector<unsigned int> vLPEndHalfInteger; 
extern std::vector<unsigned int> vLPNumIter; 
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

using std::vector;
using std::queue;
using std::set;
using std::cout;
using std::endl;
using std::string;

/// @class limbo::algorithms::coloring::LPColoring
/// LP based graph coloring.  
/// 
/// @tparam GraphType graph type 
template <typename GraphType>
class LPColoring : public Coloring<GraphType>
{
	public:
        /// @nowarn
        typedef Coloring<GraphType> base_type;
		typedef typename base_type::graph_type graph_type;
		typedef typename base_type::graph_vertex_type graph_vertex_type;
		typedef typename base_type::graph_edge_type graph_edge_type;
		typedef typename base_type::vertex_iterator_type vertex_iterator_type;
		typedef typename base_type::edge_iterator_type edge_iterator_type;
        typedef typename base_type::edge_weight_type edge_weight_type;
        typedef typename base_type::EdgeHashType edge_hash_type;
        /// @endnowarn

        /// records the information of non-integer values 
        struct NonIntegerInfo
        {
            uint32_t vertex_non_integer_num; ///< number of vertices with non-integer solutions 
            uint32_t edge_non_integer_num; ///< number of edges with non-integer solutions 
            uint32_t vertex_half_integer_num; ///< number of vertices with half-integer solutions 
            uint32_t edge_half_integer_num; ///< number of edges with half-integer solutions 

            /// constructor 
            NonIntegerInfo() 
                : vertex_non_integer_num(std::numeric_limits<uint32_t>::max())
                , edge_non_integer_num(std::numeric_limits<uint32_t>::max())
                , vertex_half_integer_num(std::numeric_limits<uint32_t>::max())
                , edge_half_integer_num(std::numeric_limits<uint32_t>::max())
            {}
        };
        /// information for a variable of a constraint 
        struct ConstrVariableInfo
        {
            double coeff; ///< coefficient
            char sense; ///< '>' or '<'

            /// constructor 
            ConstrVariableInfo() 
                : coeff(0.0)
                , sense('>')
            {}
            /// set data 
            /// @param c coefficient 
            /// @param s sense 
            void set(double c, char s)
            {
                coeff = c;
                sense = s;
            }
            /// check whether two constraints have the same direction 
            /// @param rhs constraint 
            /// @return true if two constraints have the same direction 
            bool same_direction(ConstrVariableInfo const& rhs) const 
            {
                if (coeff == 0.0 || rhs.coeff == 0.0)
                    return true;
                else if (sense == rhs.sense)
                    return (coeff > 0 && rhs.coeff > 0) || (coeff < 0 && rhs.coeff < 0);
                else 
                    return (coeff > 0 && rhs.coeff < 0) || (coeff < 0 && rhs.coeff > 0);
            }
        };

		// member functions
		/// constructor
        /// @param g graph 
		LPColoring(graph_type const& g);
		/// destructor
		~LPColoring() {};

        /// @return number of LP iterations 
        uint32_t lp_iters() const {return m_lp_iters;}

        // for debug 
        /// print solution 
        /// @param vColorBits LP solutions 
        void print_solution(vector<GRBVar> const& vColorBits) const;
	protected:
		/// kernel coloring algorithm; 
        /// relaxed linear programming based coloring for the conflict graph (this->m_graph)
		/// @return objective value 
		double coloring(); 

		/// apply coloring solution 
        /// @param vColorBits LP solutions 
		void apply_solution(vector<GRBVar> const& vColorBits);

		/// create the NoStitchGraph (this->m_graph) from the (m_conflict_graph)
		void initialize();
        /// create basic variables, objective and constraints
        /// @param vColorBits vertex bits of LP 
        /// @param vEdgeBits edge bits of LP 
        /// @param obj objective of LP 
        /// @param optModel LP model 
        void set_optimize_model(vector<GRBVar>& vColorBits, vector<GRBVar>& vEdgeBits, GRBLinExpr& obj, GRBModel& optModel);
        /// set anchor vertex 
        /// @param vColorBits vertex bits of LP 
        void set_anchor(vector<GRBVar>& vColorBits) const; 
        /// for each color bit pair of a vertex 
        /// @param vColorBits vertex bits of LP 
        /// @param obj objective of LP 
        void adjust_variable_pair_in_objective(vector<GRBVar> const& vColorBits, GRBLinExpr& obj) const; 
        /// for each color bit pair of vertices of an edge 
        /// @param vColorBits vertex bits of LP 
        /// @param obj objective of LP 
        void adjust_conflict_edge_vertices_in_objective(vector<GRBVar> const& vColorBits, GRBLinExpr& obj) const;
        /// odd cycle constraints from Prof. Baldick
        /// @param vColorBits vertex bits of LP 
        /// @param optModel LP model 
        void add_odd_cycle_constraints(vector<GRBVar> const& vColorBits, GRBModel& optModel); 
        /// solve model 
        /// @param optModel LP model 
        void solve_model(GRBModel& optModel);

		/// DFS to search for the odd cycles, stored in m_odd_cycles
        /// @param v vertex 
        /// @param vOddCyle container to store odd cycles 
		void get_odd_cycles(graph_vertex_type const& v, vector<vector<graph_vertex_type> >& vOddCyle);
        /// @return the vertex with the largest degree
		graph_vertex_type max_degree_vertex() const;

		/// Optimal rounding based on binding constraints
        /// @param optModel LP model 
        /// @param vColorBits vertex bits of LP 
        /// @param vEdgeBits edge bits of LP 
		void rounding_with_binding_analysis(GRBModel& optModel, vector<GRBVar>& vColorBits, vector<GRBVar>& vEdgeBits);

		/// greedy final coloring refinement
		uint32_t post_refinement();
        /// refine coloring solution of an edge 
        /// @param e edge 
		bool refine_color(graph_edge_type const& e);

        /// compute number of non-integers and half-integers for both vertex variables and edge variables 
        /// @param vColorBits vertex bits of LP 
        /// @param vEdgeBits edge bits of LP 
        /// @param info non-integer information 
        void non_integer_num(vector<GRBVar> const& vColorBits, vector<GRBVar> const& vEdgeBits, NonIntegerInfo& info) const;
        /// compute number of non-integers and half-integers with given variable array 
        /// @param vVariables variables 
        /// @param nonIntegerNum number of non-integers 
        /// @param halfIntegerNum number of half-integers 
        void non_integer_num(vector<GRBVar> const& vVariables, uint32_t& nonIntegerNum, uint32_t& halfIntegerNum) const;

		/// check if a variable is integer or not 
        /// @param value target value to be checked 
        /// @return true if value is an integer 
		bool is_integer(double value) const {return value == floor(value);}

        /// check number of precolored vertices 
        /// @param vVertex vertices of the graph 
        /// @return number of precolored vertices 
        uint32_t check_precolored_num(vector<LPColoring<GraphType>::graph_vertex_type> const& vVertex) const; 

		/// members
        boost::dynamic_bitset<> m_vVertexHandledByOddCycle; ///< record whether a vertex has already been handled by an odd cycle 
		uint32_t m_constrs_num; ///< record number of constraints 
        uint32_t m_lp_iters; ///< record lp iterations 
};

/// constructor
template <typename GraphType>
LPColoring<GraphType>::LPColoring(graph_type const& g) 
    : LPColoring<GraphType>::base_type(g)
    , m_vVertexHandledByOddCycle(boost::num_vertices(g), false)
    , m_constrs_num(0)
    , m_lp_iters(0)
{
}

//DFS to search for the odd cycles, stored in m_odd_cycles
template <typename GraphType>
void LPColoring<GraphType>::get_odd_cycles(graph_vertex_type const& v, vector<vector<LPColoring<GraphType>::graph_vertex_type> >& vOddCyle) 
{
	//odd_cycle results
	vOddCyle.clear();

    // do not search for odd cycles for the same vertex again 
    if (m_vVertexHandledByOddCycle[v]) return;
    else m_vVertexHandledByOddCycle[v] = true;

	//the array denoting the distance/visiting of the graph 
	uint32_t numVertices = boost::num_vertices(this->m_graph);
	vector<int32_t> vNodeDistColor (numVertices, -1);
	vector<bool> vNodeVisited (numVertices, false);

	//inCycle flag
	vector<bool> vInCycle (numVertices, false);

	// stack for DFS
    // use std::vector instead of std::stack for better memory control
	vector<graph_vertex_type> vVertexStack;
	vVertexStack.reserve(numVertices);
	vNodeVisited[v] = true;
	vNodeDistColor[v] = 0;
	vVertexStack.push_back(v);
	while (!vVertexStack.empty()) 
	{
		//determine whether the top element needs to be popped
		bool popFlag = true;
		//access the top element on the dfs_stack
        // current vertex 
		graph_vertex_type cv = vVertexStack.back();
		//access the neighbors 
		typename boost::graph_traits<graph_type>::adjacency_iterator ui, uie;
		for(boost::tie(ui, uie) = adjacent_vertices(cv, this->m_graph); ui != uie; ++ui) 
		{
            graph_vertex_type u = *ui;

			if(vNodeDistColor[u] == -1) 
			{
				vNodeDistColor[u] = 1 - vNodeDistColor[cv];
				vNodeVisited[u] = true;
				//push to the stack
				vVertexStack.push_back(u);
				popFlag = false;
				break;
			}
		} //end for 

		if (true == popFlag) 
		{
            vector<graph_vertex_type> cycle;
			//detect the odd cycle
            for (boost::tie(ui, uie) = adjacent_vertices(cv, this->m_graph); ui != uie; ++ui)
			{
                graph_vertex_type u = *ui;
				if (vNodeVisited[u] && (vNodeDistColor[u] == vNodeDistColor[cv])) 
				//if (*vi == v && (nodeDist[next_index] == nodeDist[curr_index])) // we only care about odd cycle related to v 
				{
					//suppose v is not in the current cycle 
					vInCycle[v] = false;
					//detect the cycle between curr_v and *vi
					int32_t cnt = vVertexStack.size();
					for(int32_t k = cnt - 1; k >= 0; --k) // back trace cycle 
					{
						cycle.push_back(vVertexStack[k]);
						//flag the nodes in cycle
						vInCycle[vVertexStack[k]] = true;
						if(vVertexStack[k] == u) break;
					}
					//store the cycle, when v is in cycle
                    // if contain precolored vertices, avoid cycle with a lot precolored vertices
					if (!cycle.empty() && vInCycle[v] && !(this->has_precolored() && check_precolored_num(cycle)+2 > cycle.size())) 
					{
                        vOddCyle.push_back(vector<graph_vertex_type>());
                        vOddCyle.back().swap(cycle);
                        // if we find a small cycle, mark all vertices as handled to reduce the number of duplicate constraints
                        if (cnt == 3)
                            for (int32_t k = 0; k != cnt; ++k)
                                m_vVertexHandledByOddCycle[vVertexStack[k]] = true;
					} 
                    else cycle.clear(); // remember to clear cycle 
				}
			}

			//pop the top element
			vVertexStack.pop_back();
            vNodeVisited[cv] = false;
		}//end if popFlag
	}//end while
}

// the vertex with the largest degree
template <typename GraphType>
typename LPColoring<GraphType>::graph_vertex_type LPColoring<GraphType>::max_degree_vertex() const
{
    graph_vertex_type u = 0;
    uint32_t maxDegree = 0;
    vertex_iterator_type vi, vie;
    for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
    {
        uint32_t d = boost::degree(*vi, this->m_graph);
        if (d > maxDegree)
        {
            u = *vi;
            maxDegree = d;
        }
    }
    return u;
}

template <typename GraphType>
void LPColoring<GraphType>::set_optimize_model(vector<GRBVar>& vColorBits, vector<GRBVar>& /*vEdgeBits*/, GRBLinExpr& obj, GRBModel& optModel)
{
	uint32_t numVertices = boost::num_vertices(this->m_graph);
	//uint32_t numEdges = boost::num_edges(this->m_graph);
    uint32_t numColorBits = numVertices<<1;

	//set up the LP variables
	vColorBits.reserve(numColorBits);
	//vEdgeBits.reserve(numEdges);
	// vertex and edge variables 
    char buf[64];
	for (uint32_t i = 0; i != numColorBits; ++i)
	{
        sprintf(buf, "v%u", i);
        // check whether precolored 
        uint32_t vertexIdx = i>>1;
        int8_t color = this->m_vColor[vertexIdx];
        if (color < 0) // not precolored
            vColorBits.push_back(optModel.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, buf));
        else // precolored
        {
            int8_t colorBit = (i&1)? (color&1) : (color>>1); 
#ifdef DEBUG_LPCOLORING
            assert(colorBit >= 0 && colorBit <= 1);
#endif
            vColorBits.push_back(optModel.addVar(colorBit, colorBit, colorBit, GRB_CONTINUOUS, buf));
        }
	}
	//for (uint32_t i = 0; i != numEdges; ++i)
	//{
	//	// some variables here may not be used 
    //    sprintf(buf, "e%u", i);
	//	vEdgeBits.push_back(optModel.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, buf));
	//}
	//Integrate new variables
	optModel.update();

    // set up objective
    obj.clear(); // set to 0 
    optModel.setObjective(obj, GRB_MINIMIZE);

	//set up the LP constraints
    edge_iterator_type ei, eie; 
	for(boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei) 
	{
        graph_edge_type e = *ei;
        graph_vertex_type s = boost::source(e, this->m_graph);
        graph_vertex_type t = boost::target(e, this->m_graph);

        // if both vertices of an edge is precolored, skip 
        // otherwise, it may crash due to intrinsic conflict 
        if (this->has_precolored() && this->m_vColor[s] >= 0 && this->m_vColor[t] >= 0)
            continue;

        uint32_t bitIdxS = s<<1;
        uint32_t bitIdxT = t<<1;

        edge_weight_type w = this->edge_weight(e);
        assert_msg(w > 0, "no stitch edge allowed, positive edge weight expected: " << w);

        sprintf(buf, "R%u", m_constrs_num++);  
        optModel.addConstr(
                vColorBits[bitIdxS] + vColorBits[bitIdxS+1] 
                + vColorBits[bitIdxT] + vColorBits[bitIdxT+1] >= 1
                , buf);

        sprintf(buf, "R%u", m_constrs_num++);  
        optModel.addConstr(
                1 - vColorBits[bitIdxS] + vColorBits[bitIdxS+1] 
                + 1 - vColorBits[bitIdxT] + vColorBits[bitIdxT+1] >= 1
                , buf);

        sprintf(buf, "R%u", m_constrs_num++);  
        optModel.addConstr(
                vColorBits[bitIdxS] + 1 - vColorBits[bitIdxS+1] 
                + vColorBits[bitIdxT] + 1 - vColorBits[bitIdxT+1] >= 1
                , buf);

        sprintf(buf, "R%u", m_constrs_num++);  
        optModel.addConstr(
                1 - vColorBits[bitIdxS] + 1 - vColorBits[bitIdxS+1] 
                + 1 - vColorBits[bitIdxT] + 1 - vColorBits[bitIdxT+1] >= 1
                , buf);
	}

	if (this->color_num() == base_type::THREE)
	{
		for(uint32_t k = 0; k < numColorBits; k += 2) 
		{
			sprintf(buf, "R%u", m_constrs_num++);  
			optModel.addConstr(
                    vColorBits[k] + vColorBits[k+1] <= 1, 
                    buf);
		}
	}

	//Integrate new variables
	optModel.update();
}

template <typename GraphType>
void LPColoring<GraphType>::set_anchor(vector<GRBVar>& vColorBits) const 
{
    if (this->has_precolored()) // no anchor if containing precolored vertices 
        return;
	//Anchoring the coloring of the vertex with largest degree
	graph_vertex_type anchorVertex = max_degree_vertex();
	uint32_t bitIdxAnchor = anchorVertex<<1;
	vColorBits[bitIdxAnchor].set(GRB_DoubleAttr_UB, 0.0);
	vColorBits[bitIdxAnchor].set(GRB_DoubleAttr_LB, 0.0);
	vColorBits[bitIdxAnchor+1].set(GRB_DoubleAttr_UB, 0.0);
	vColorBits[bitIdxAnchor+1].set(GRB_DoubleAttr_LB, 0.0);
}

/// tune objective for each color bit pair of vertex  
template <typename GraphType>
void LPColoring<GraphType>::adjust_variable_pair_in_objective(vector<GRBVar> const& vColorBits, GRBLinExpr& obj) const 
{
    for(uint32_t k = 0, ke = vColorBits.size(); k < ke; k += 2) 
    {
        double value1 = vColorBits[k].get(GRB_DoubleAttr_X);
        double value2 = vColorBits[k+1].get(GRB_DoubleAttr_X);
        if (!is_integer(value1) || !is_integer(value2))
        {
            if (value1 > value2)
                obj += vColorBits[k+1]-vColorBits[k];
            else if (value1 < value2)
                obj += vColorBits[k]-vColorBits[k+1];
        }
    }//end for 
}

/// tune objective for each color bit pair along conflict edges 
template <typename GraphType>
void LPColoring<GraphType>::adjust_conflict_edge_vertices_in_objective(vector<GRBVar> const& vColorBits, GRBLinExpr& obj) const 
{
    edge_iterator_type ei, eie; 
	for(boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei) 
    {
        graph_edge_type e = *ei;
        graph_vertex_type s = boost::source(e, this->m_graph);
        graph_vertex_type t = boost::target(e, this->m_graph);

        for (uint32_t i = 0; i != 2; ++i)
        {
            uint32_t bitIdxS = (s<<1)+i;
            uint32_t bitIdxT = (t<<1)+i;

            double value1 = vColorBits[bitIdxS].get(GRB_DoubleAttr_X);
            double value2 = vColorBits[bitIdxT].get(GRB_DoubleAttr_X);

            if (value1 > value2)
                obj += vColorBits[bitIdxT]-vColorBits[bitIdxS]; // reverse, as we minimize objective
            else if (value1 < value2)
                obj += vColorBits[bitIdxS]-vColorBits[bitIdxT]; // reverse, as we minimize objective
        }
    }//end for 
}

/// odd cycle trick from Prof. Baldick
template <typename GraphType>
void LPColoring<GraphType>::add_odd_cycle_constraints(vector<GRBVar> const& vColorBits, GRBModel& optModel) 
{
    char buf[64];
    vector<vector<graph_vertex_type> > vOddCyle;
    for(uint32_t k = 0, ke = vColorBits.size(); k < ke; k += 2) 
    {
        // only add odd cycle for half integer 
        if (vColorBits[k].get(GRB_DoubleAttr_X) != 0.5 && vColorBits[k+1].get(GRB_DoubleAttr_X) != 0.5)
            continue;

        graph_vertex_type v = k>>1;
        this->get_odd_cycles(v, vOddCyle);

        for (typename vector<vector<graph_vertex_type> >::const_iterator it1 = vOddCyle.begin(), it1e = vOddCyle.end(); it1 != it1e; ++it1)
        {
            vector<graph_vertex_type> const& oddCycle = *it1;
            int32_t cycleLength = oddCycle.size(); // safer to use integer as we do minus afterward
            GRBLinExpr constraint1 = 0;
            GRBLinExpr constraint2 = 0;

            for (typename vector<graph_vertex_type>::const_iterator it2 = oddCycle.begin(), it2e = oddCycle.end(); it2 != it2e; ++it2)
            {
                graph_vertex_type u = *it2;
                constraint1 += vColorBits[u<<1];
                constraint2 += vColorBits[(u<<1)+1];
            }

            sprintf(buf, "ODD%lu_%u", v, m_constrs_num++);
            optModel.addConstr(constraint1 >= 1, buf);
            sprintf(buf, "ODD%lu_%u", v, m_constrs_num++);
            optModel.addConstr(constraint1 <= cycleLength-1, buf);
            sprintf(buf, "ODD%lu_%u", v, m_constrs_num++);
            optModel.addConstr(constraint2 >= 1, buf);
            sprintf(buf, "ODD%lu_%u", v, m_constrs_num++);
            optModel.addConstr(constraint2 <= cycleLength-1, buf);
        }
    }//end for k
}

/// solve model 
template <typename GraphType>
void LPColoring<GraphType>::solve_model(GRBModel& optModel) 
{
    char buf[64];
    //optimize the new model
    optModel.update();
#ifdef DEBUG_LPCOLORING
    sprintf(buf, "%u.lp", m_lp_iters);
    optModel.write(buf);
#endif
    optModel.optimize();
    int32_t optStatus = optModel.get(GRB_IntAttr_Status);
    if (optStatus == GRB_INFEASIBLE)
    {
        // write lp 
        sprintf(buf, "%u.lp", m_lp_iters);
        optModel.write(buf);
        // write iis 
        optModel.computeIIS();
        sprintf(buf, "%u.ilp", m_lp_iters);
        optModel.write(buf);
    }
    assert_msg(optStatus != GRB_INFEASIBLE, "model is infeasible");
    ++m_lp_iters;
}

//relaxed linear programming based coloring for the conflict graph (this->m_graph)
template <typename GraphType> 
double LPColoring<GraphType>::coloring() 
{
#ifdef DEBUG_LPCOLORING
    this->write_graph("initial_input");
#endif
    vector<GRBVar> vColorBits;
    vector<GRBVar> vEdgeBits;
    GRBLinExpr obj;

	//set up the LP environment
	GRBEnv env;
	//mute the log from the LP solver
	env.set(GRB_IntParam_OutputFlag, 0);
    // set number of threads 
	if (this->m_threads > 0 && this->m_threads < std::numeric_limits<int32_t>::max())
		env.set(GRB_IntParam_Threads, this->m_threads);
    // default algorithm 
    env.set(GRB_IntParam_Method, -1);
    // since GRBModel does not allow default constructor, we have to setup GRBEnv before it
	GRBModel optModel = GRBModel(env);

    // initialize model and set anchor vertex 
    set_optimize_model(vColorBits, vEdgeBits, obj, optModel);
#ifndef DEBUG_NOANCHOR
    set_anchor(vColorBits);
#endif

    solve_model(optModel);

#ifdef DEBUG_LPCOLORING
    printf("\nLP %u solution: ", m_lp_iters);
    print_solution(vColorBits);
#endif

    NonIntegerInfo prevInfo; // initialize to numeric max 
    NonIntegerInfo curInfo;
    non_integer_num(vColorBits, vEdgeBits, curInfo);
#ifdef DEBUG_NONINTEGERS
    vLP1NonInteger.push_back(curInfo.vertex_non_integer_num); 
    vLP1HalfInteger.push_back(curInfo.vertex_half_integer_num); 
#endif
	//iteratively scheme
	while(curInfo.vertex_non_integer_num > 0 && curInfo.vertex_non_integer_num < prevInfo.vertex_non_integer_num) 
	{
		//set the new objective
		//push the non-half_integer to 0/1
		// tune objective for a pair of values 
        adjust_variable_pair_in_objective(vColorBits, obj);
		// tune objective for a pair of value along conflict edges 
        // disabled because it has conflicts with odd cycle constraints 
        //adjust_conflict_edge_vertices_in_objective(vColorBits, obj);

		optModel.setObjective(obj);

		//add new constraints
		//odd cycle trick from Prof. Baldick
        add_odd_cycle_constraints(vColorBits, optModel);

        solve_model(optModel);

#ifdef DEBUG_LPCOLORING
        printf("LP %u solution: ", m_lp_iters);
        print_solution(vColorBits);
#endif

        prevInfo = curInfo;
        non_integer_num(vColorBits, vEdgeBits, curInfo);
#ifdef DEBUG_NONINTEGERS
        if (m_lp_iters == 2)
        {
            vLP2NonInteger.push_back(curInfo.vertex_non_integer_num); 
            vLP2HalfInteger.push_back(curInfo.vertex_half_integer_num); 
        }
#endif
	}

#ifdef DEBUG_NONINTEGERS
    vLPEndNonInteger.push_back(curInfo.vertex_non_integer_num); 
    vLPEndHalfInteger.push_back(curInfo.vertex_half_integer_num);
    vLPNumIter.push_back(m_lp_iters); 
#endif

	// binding analysis
    //rounding_with_binding_analysis(optModel, vColorBits, vEdgeBits);
    // apply coloring solution 
    apply_solution(vColorBits);
    // post refinement 
	post_refinement();

#ifdef DEBUG_LPCOLORING
    this->write_graph("final_output");
#endif
    return this->calc_cost(this->m_vColor);
}

template <typename GraphType>
void LPColoring<GraphType>::apply_solution(vector<GRBVar> const& vColorBits)
{
    for (uint32_t i = 0, ie = this->m_vColor.size(); i != ie; ++i)
    {
        GRBVar const& var1 = vColorBits[i<<1];
        GRBVar const& var2 = vColorBits[(i<<1)+1];
        int8_t b1 = round(var1.get(GRB_DoubleAttr_X));
        int8_t b2 = round(var2.get(GRB_DoubleAttr_X));
        // exclude (1, 1) for three coloring 
        this->m_vColor[i] = std::min((b1<<1)+b2, (int8_t)this->color_num()-1);
    }
}

/// optimal rounding based on the binding analysis
/// but only a part of all vertices can be rounded 
template <typename GraphType>
void LPColoring<GraphType>::rounding_with_binding_analysis(GRBModel& optModel, vector<GRBVar>& vColorBits, vector<GRBVar>& vEdgeBits) 
{
    NonIntegerInfo prevInfo; // initialize to numeric max 
    NonIntegerInfo curInfo;
    non_integer_num(vColorBits, vEdgeBits, curInfo);
	//iteratively scheme
	while(curInfo.vertex_non_integer_num > 0 && curInfo.vertex_non_integer_num < prevInfo.vertex_non_integer_num) 
    {
        bool modifyFlag = false; // whether binding analysis causes any change 
        for (uint32_t i = 0, ie = vColorBits.size(); i < ie; i += 2)
        {
            GRBVar const& var1 = vColorBits[i];
            GRBVar const& var2 = vColorBits[i+1];
            double value1 = var1.get(GRB_DoubleAttr_X);
            double value2 = var2.get(GRB_DoubleAttr_X);

            if (!(value1 == 0.5 && value2 == 0.5))
                continue;

            GRBColumn column[2] = {
                optModel.getCol(var1),
                optModel.getCol(var2)
            };

            ConstrVariableInfo prevConstrInfo[2];
            ConstrVariableInfo curConstrInfo[2];

            // (0, 0), (0, 1), (1, 0), (1, 1)
            bool mValidColorBits[2][2] = {{true, true}, {true, true}};
            if (this->color_num() == base_type::THREE)
                mValidColorBits[1][1] = false;

            uint32_t invalidCount = 0;
            bool failFlag = false; // whether optimal rounding is impossible 
            // check all corresponding constraints 
            for (uint32_t j = 0; j != 2 && !failFlag; ++j)
                for (uint32_t k = 0, ke = column[j].size(); k != ke; ++k)
                {
                    GRBConstr constr = column[j].getConstr(k);
                    // skip non-binding constraint 
                    //if (constr.get(GRB_DoubleAttr_Slack) != 0.0) 
                    //    continue;
                    char sense = constr.get(GRB_CharAttr_Sense);
                    curConstrInfo[0].set(optModel.getCoeff(constr, var1), sense);
                    curConstrInfo[1].set(optModel.getCoeff(constr, var2), sense);

                    // conflict sensitivity detected 
                    if (!curConstrInfo[0].same_direction(prevConstrInfo[0]) || !curConstrInfo[1].same_direction(prevConstrInfo[1]))
                    {
                        failFlag = true;
                        break;
                    }

                    // check all coloring solutions 
                    for (int32_t b1 = 0; b1 != 2; ++b1)
                        for (int32_t b2 = 0; b2 != 2; ++b2)
                        {
                            if (mValidColorBits[b1][b2])
                            {
                                double delta = curConstrInfo[0].coeff*(b1-value1) + curConstrInfo[1].coeff*(b2-value2);
                                if ((sense == '>' && delta < 0.0) || (sense == '<' && delta > 0.0)) // fail
                                {
                                    mValidColorBits[b1][b2] = false;
                                    ++invalidCount;
                                }
                            }
                        }

                    // no valid rounding solution 
                    if (invalidCount == 4)
                    {
                        failFlag = true;
                        break;
                    }

                    prevConstrInfo[0] = curConstrInfo[0];
                    prevConstrInfo[1] = curConstrInfo[1];
                }

            // apply 
            if (!failFlag)
            {
                for (int32_t b1 = 0; b1 != 2; ++b1)
                    for (int32_t b2 = 0; b2 != 2; ++b2)
                        if (mValidColorBits[b1][b2])
                        {
                            vColorBits[i].set(GRB_DoubleAttr_UB, b1);
                            vColorBits[i].set(GRB_DoubleAttr_LB, b1);
                            vColorBits[i+1].set(GRB_DoubleAttr_UB, b2);
                            vColorBits[i+1].set(GRB_DoubleAttr_LB, b2);
                            modifyFlag = true;
                        }
            }
        }

        // exit if nothing changed 
        if (!modifyFlag) break;

        solve_model(optModel);

        prevInfo = curInfo;
        non_integer_num(vColorBits, vEdgeBits, curInfo);
    }
}

//post coloring refinement
template <typename GraphType>
uint32_t LPColoring<GraphType>::post_refinement() 
{
    uint32_t count = 0;
    if (!this->has_precolored()) // no post refinement if containing precolored vertices 
    {
        // greedy post refinement
        // keep refining until no color changed 
        edge_iterator_type ei, eie;
        do 
        {
            count = 0; 
            for (boost::tie(ei, eie) = boost::edges(this->m_graph); ei != eie; ++ei)
                count += refine_color(*ei);
        } while (count); 
    }

    return count;
}

/// @return true if found a coloring solution to resolve conflicts 
template <typename GraphType>
bool LPColoring<GraphType>::refine_color(LPColoring<GraphType>::graph_edge_type const& e) 
{ 
    graph_vertex_type v[2] = {
        boost::source(e, this->m_graph), 
        boost::target(e, this->m_graph)
    };

    //if (this->m_vColor[v[0]] != this->m_vColor[v[1]])
    //    return false;

    // find coloring solution with best cost 
    int8_t bestColor[2] = {0, 0};
    edge_weight_type bestCost = std::numeric_limits<edge_weight_type>::max();
    int8_t c[2];
    for (c[0] = 0; c[0] != this->color_num(); c[0] += 1)
        for (c[1] = 0; c[1] != this->color_num(); c[1] += 1)
        {
            edge_weight_type curCost = 0;
            typename boost::graph_traits<graph_type>::adjacency_iterator ui, uie;
            for (int32_t i = 0; i != 2; ++i)
            {
                // cv denotes current vertex 
                // ov denotes the other vertex 
                graph_vertex_type cv = v[i], ov = v[!i];
                for (boost::tie(ui, uie) = boost::adjacent_vertices(cv, this->m_graph); ui != uie; ++ui)
                {
                    graph_vertex_type u = *ui;
                    if (u != ov && c[i] == this->m_vColor[u])
                    {
                        std::pair<graph_edge_type, bool> eU2cv = boost::edge(cv, u, this->m_graph);
#ifdef DEBUG_LPCOLORING
                        assert(eU2cv.second);
#endif
                        // edge weight is important since we may deal with merged graphs
                        curCost += std::max((edge_weight_type)1, boost::get(boost::edge_weight, this->m_graph, eU2cv.first));
                    }
                }
            }
            if (c[0] == c[1]) // edge weight is important since we may deal with merged graphs
                curCost += std::max((edge_weight_type)1, boost::get(boost::edge_weight, this->m_graph, e));
            if (curCost < bestCost)
            {
                bestCost = curCost;
                bestColor[0] = c[0];
                bestColor[1] = c[1];
            }
        }
    bool retFlag = false;
    if (this->m_vColor[v[0]] != bestColor[0] || this->m_vColor[v[1]] != bestColor[1])
    {
        this->m_vColor[v[0]] = bestColor[0];
        this->m_vColor[v[1]] = bestColor[1];
        retFlag = true;
    }

    return retFlag;
}

// for debug use
// it seems doxygen cannot handle template functions with the same name correctly 
/// @cond 
template <typename GraphType>
void LPColoring<GraphType>::non_integer_num(vector<GRBVar> const& vColorBits, vector<GRBVar> const& /*vEdgeBits*/, LPColoring<GraphType>::NonIntegerInfo& info) const
{
    non_integer_num(vColorBits, info.vertex_non_integer_num, info.vertex_half_integer_num);
    //non_integer_num(vEdgeBits, info.edge_non_integer_num, info.edge_half_integer_num);

#ifdef DEBUG_LPCOLORING
    printf("vertex_non_integer_num = %u, vertex_half_integer_num = %u\n", info.vertex_non_integer_num, info.vertex_half_integer_num); 
    //printf("edge_non_integer_num = %u, edge_half_integer_num = %u\n", info.edge_non_integer_num, info.edge_half_integer_num); 
#endif
}

template <typename GraphType>
void LPColoring<GraphType>::non_integer_num(vector<GRBVar> const& vVariables, uint32_t& nonIntegerNum, uint32_t& halfIntegerNum) const 
{
    nonIntegerNum = 0;
    halfIntegerNum = 0;
    for (vector<GRBVar>::const_iterator it = vVariables.begin(), ite = vVariables.end(); it != ite; ++it)
    {
        double value = it->get(GRB_DoubleAttr_X);
        if (value != 0.0 && value != 1.0)
        {
            ++nonIntegerNum;
            if (value == 0.5)
                ++halfIntegerNum;
        }
    }
}
/// @endcond 

template <typename GraphType>
uint32_t LPColoring<GraphType>::check_precolored_num(vector<LPColoring<GraphType>::graph_vertex_type> const& vVertex) const 
{
    uint32_t precoloredNum = 0;
    for (typename vector<graph_vertex_type>::const_iterator vi = vVertex.begin(), vie = vVertex.end(); vi != vie; ++vi)
        if (this->m_vColor[*vi] >= 0) // precolored vertex 
            ++precoloredNum;
    return precoloredNum;
}

template <typename GraphType>
void LPColoring<GraphType>::print_solution(vector<GRBVar> const& vColorBits) const 
{
    const char* prefix = "";
    for (uint32_t i = 0, ie = vColorBits.size(); i < ie; i += 2)
    {
        printf("%sv%u=(%g,%g)", prefix, i>>1, vColorBits[i].get(GRB_DoubleAttr_X), vColorBits[i+1].get(GRB_DoubleAttr_X));
        prefix = ", ";
    }
    printf("\n");
}

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
