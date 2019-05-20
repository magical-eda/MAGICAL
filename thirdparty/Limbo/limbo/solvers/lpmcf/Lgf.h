/**
 * @file   Lgf.h
 * @brief  solve linear programming with min-cost flow 
 *
 * [Lemon](https://lemon.cs.elte.hu) is used as min-cost flow solver 
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _LIMBO_SOLVERS_LPMCF_LGF_H
#define _LIMBO_SOLVERS_LPMCF_LGF_H

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>
#include <cctype>
#include <ctime>

#include <lemon/network_simplex.h>
#include <lemon/cost_scaling.h>
//#include <lemon/capacity_scaling.h>
//#include <lemon/cycle_canceling.h>

#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>

#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/graph_to_eps.h>
#include <lemon/math.h>

#include <limbo/preprocessor/AssertMsg.h>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Solvers 
namespace solvers 
{ 
/// namespace for Limbo.Solvers.lpmcf 
namespace lpmcf 
{

using std::cout;
using std::endl;
using std::string;

/// @class limbo::solvers::lpmcf::Lgf 
/// @brief solve network flow graph with min-cost flow 
/// @tparam T data type 
template <typename T>
class Lgf 
{
	public:
        /// @nowarn
		// value_type can only be integer types 
		typedef T value_type;
		typedef value_type cost_type;
		typedef lemon::SmartDigraph graph_type;
		typedef graph_type::Node node_type;
		typedef graph_type::Arc arc_type;
		typedef graph_type::NodeMap<value_type> node_value_map_type;
		typedef graph_type::NodeMap<string> node_name_map_type;
		typedef graph_type::ArcMap<value_type> arc_value_map_type;
		typedef graph_type::ArcMap<value_type> arc_cost_map_type;
		typedef graph_type::ArcMap<value_type> arc_flow_map_type;
		typedef graph_type::NodeMap<value_type> node_pot_map_type; // potential of each node 

		// four kinds of algrithms for min-cost flow 
		typedef lemon::NetworkSimplex<graph_type, value_type, cost_type> alg_network_simplex_type;
		typedef lemon::CostScaling<graph_type, value_type, cost_type> alg_cost_scaling_type;
//		typedef lemon::CapacityScaling<graph_type, value_type, cost_type> alg_capacity_scaling_type;
//		typedef lemon::CycleCanceling<graph_type, value_type, cost_type> alg_cycle_canceling_type;

//		typedef alg_network_simplex_type alg_type;
		typedef alg_cost_scaling_type alg_type;
        /// @endnowarn

        /// @brief constructor
		Lgf() 
			: m_graph(), 
			m_hLower(m_graph), 
			m_hUpper(m_graph), 
			m_hCost(m_graph), 
			m_hSupply(m_graph), 
			m_hName(m_graph),
			m_totalcost(std::numeric_limits<cost_type>::max()),
			m_hFlow(m_graph), 
			m_hPot(m_graph)
		{
		}
        /// @brief destructor 
		virtual ~Lgf() {}

        /// @brief API to run the algorithm 
        /// @param filename input file in .lgf format 
		typename alg_type::ProblemType operator() (string const& filename)
		{
			this->read_lgf(filename);

			typename alg_type::ProblemType status = this->run();
			if (status == alg_type::OPTIMAL)
				this->print_solution(filename+".sol");

			return status;
		}

        /// @brief print graph 
		/// @param filename output file in .lgf format 
		virtual void print_graph(string const& filename) const 
		{
			typedef lemon::dim2::Point<int> Point;
			
			lemon::Palette palette; 

			graph_type::NodeMap<Point> coords(m_graph);
			graph_type::NodeMap<double> sizes(m_graph);
			graph_type::NodeMap<int> colors(m_graph);
			graph_type::NodeMap<int> shapes(m_graph);
			graph_type::ArcMap<int> acolors(m_graph);
			graph_type::ArcMap<int> widths(m_graph);
			//lemon::IdMap<graph_type, node_type> id(m_graph);

			srand(1000);
			int64_t i = 0;
			for (graph_type::NodeIt v(m_graph); v != lemon::INVALID; ++v, ++i)
			{
				coords[v] = Point(
						10*(i+rand()%(m_graph.maxNodeId()<<2)), 
						10*(i+rand()%(m_graph.maxNodeId()<<2))
						);
				sizes[v] = 1;
				colors[v] = 1;
				shapes[v] = 0;
			}
			i = 0;
			for (graph_type::ArcIt a(m_graph); a != lemon::INVALID; ++a, ++i)
			{
				acolors[a] = 0;
				widths[a] = 1;
			}

			// dump eps figure 
			lemon::graphToEps(m_graph, filename+".eps")
				.title("LpMcfD EPS figure")
				.coords(coords)
				.absoluteNodeSizes().absoluteArcWidths()
				.nodeScale(2).nodeSizes(sizes)
				.nodeShapes(shapes)
				.nodeColors(lemon::composeMap(palette,colors))
				.arcColors(lemon::composeMap(palette,acolors))
				.arcWidthScale(.4).arcWidths(widths)
				.nodeTexts(m_hName).nodeTextSize(3)
				.enableParallel().parArcDist(1)
				.drawArrows().arrowWidth(1).arrowLength(1)
				.run();
			// dump lgf file 
			lemon::DigraphWriter<graph_type>(m_graph, filename+".lgf")
				.nodeMap("name", m_hName)
				.nodeMap("supply", m_hSupply)
				.arcMap("capacity_lower", m_hLower)
				.arcMap("capacity_upper", m_hUpper)
				.arcMap("cost", m_hCost)
				.run();
		}
        /// @brief print solution 
        /// @param filename output file name 
		virtual void print_solution(string const& filename) const 
		{
			std::ofstream out (filename.c_str());
			if (!out.good())
			{
				cout << "failed to open " << filename << endl;
				return;
			}

			out << "total cost: " << m_totalcost << endl;
			out << "############# MCF Flow #############" << endl;
			for (graph_type::ArcIt a(m_graph); a != lemon::INVALID; ++a)
			{
				out << m_graph.id(a) << ": " 
					<< m_hName[m_graph.source(a)] << "->" << m_hName[m_graph.target(a)] << ": " 
					<< m_hFlow[a] << endl;
			}
			out << "############# MCF Potential #############" << endl;
			for (graph_type::NodeIt v(m_graph); v != lemon::INVALID; ++v)
			{
				out << m_hName[v] << ": " << m_hPot[v] << endl; 
			}

			out.close();
		}
		/// @brief read input file in .lgf format and initialize graph 
        /// @param lgfFile input file name 
		void read_lgf(string const& lgfFile)
		{
			lemon::DigraphReader<graph_type>(m_graph, lgfFile)
				.nodeMap("supply", m_hSupply)
				.nodeMap("name", m_hName)
				.arcMap("capacity_lower", m_hLower)
				.arcMap("capacity_upper", m_hUpper)
				.arcMap("cost", m_hCost)
				.run();
		}
	protected:
        /// @brief run algorithm 
        /// @return solving status: OPTIMAL, INFEASIBLE, or UNBOUNDED
		typename alg_type::ProblemType run()
		{
			// 1. choose algorithm 
			alg_type alg (m_graph);

			// 2. run 
			typename alg_type::ProblemType status = alg.reset().resetParams()
				.lowerMap(m_hLower)
				.upperMap(m_hUpper)
				.costMap(m_hCost)
				.supplyMap(m_hSupply)
//				.stSupply(m_st, m_st, m_st_supply)
				.run();

			// 3. check results 
#ifdef DEBUG
			switch (status)
			{
				case alg_type::OPTIMAL:
					cout << "OPTIMAL" << endl;
					break;
				case alg_type::INFEASIBLE:
					cout << "INFEASIBLE" << endl;
					break;
				case alg_type::UNBOUNDED:
					cout << "UNBOUNDED" << endl;
					break;
			}

			assert_msg(status == alg_type::OPTIMAL, "failed to achieve OPTIMAL solution");
#endif 
			// 4. update solution 
			if (status == alg_type::OPTIMAL)
			{
				m_totalcost = alg.template totalCost<cost_type>();
				// get dual solution of LP, which is the flow of mcf, skip this if not necessary
				alg.flowMap(m_hFlow);
				// get solution of LP, which is the dual solution of mcf 
				alg.potentialMap(m_hPot);
			}

			return status;
		}

		graph_type m_graph; ///< input graph 
		arc_value_map_type m_hLower; ///< lower bound of flow, usually zero  
		arc_value_map_type m_hUpper; ///< upper bound of flow, arc capacity in mcf  
		arc_cost_map_type m_hCost; ///< arc cost in mcf 
		node_value_map_type m_hSupply; ///< node supply in mcf 
		node_name_map_type m_hName; ///< node name in mcf 
		cost_type m_totalcost; ///< total cost after solving 

		arc_flow_map_type m_hFlow; ///< solution of min-cost flow, which is the dual solution of LP 
		node_pot_map_type m_hPot; ///< dual solution of min-cost flow, which is the solution of LP 
};

} // namespace lpmcf
} // namespace solvers
} // limbo 

#endif 
