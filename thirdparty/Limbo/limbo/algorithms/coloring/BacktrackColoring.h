/**
 * @file   BacktrackColoring.h
 * @brief  graph coloring by backtracking 
 * @author Yibo Lin
 * @date   Jun 2015
 */

#ifndef LIMBO_ALGORITHMS_COLORING_BACKTRACKCOLORING
#define LIMBO_ALGORITHMS_COLORING_BACKTRACKCOLORING

#include <limbo/algorithms/coloring/Coloring.h>
#include <limbo/algorithms/coloring/GreedyColoring.h>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Algorithms 
namespace algorithms 
{ 
/// namespace for Limbo.Algorithms.Coloring 
namespace coloring 
{

/// @class limbo::algorithms::coloring::BacktrackColoring
/// Solve graph coloring with backtracking 
/// @tparam GraphType graph type 
template <typename GraphType>
class BacktrackColoring : public Coloring<GraphType>
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
		typedef typename boost::graph_traits<graph_type>::adjacency_iterator adjacency_iterator_type;
        /// @endnowarn

		/// constructor
        /// @param g graph 
		BacktrackColoring(graph_type const& g) 
			: base_type(g)
		{}
		/// destructor
		virtual ~BacktrackColoring() {}
	protected:
		/// @return objective value 
		virtual double coloring();
		/// initial color assignment by greedy approach 
        /// @param vColor array to store coloring solution 
		/// @return cost 
		double init_coloring(vector<int8_t>& vColor) const;
		/// kernel function for recursive backtracking 
        /// @param vBestColor best coloring solution assignment 
        /// @param vColor current coloring solution assignment 
        /// @param best_cost best cost 
        /// @param cur_cost current cost 
        /// @param v current vertex 
        /// @param cost_lb cost lower bound 
        /// @param cost_ub cost upper bound 
		void coloring_kernel(vector<int8_t>& vBestColor, vector<int8_t>& vColor, double& best_cost, double& cur_cost, graph_vertex_type v, double cost_lb, double cost_ub) const;
};

template <typename GraphType>
double BacktrackColoring<GraphType>::coloring()
{
	vector<int8_t> vBestColor(this->m_vColor.begin(), this->m_vColor.end());
	vector<int8_t> vColor (this->m_vColor.begin(), this->m_vColor.end());
	double best_cost = this->init_coloring(vBestColor);
	double cur_cost = 0;
	double actual_cost;

    //std::cout << "best cost from dsat = " << best_cost << std::endl;

	// solve coloring problem 
	// heuristic for speedup when graph is large 
	if (boost::num_vertices(this->m_graph) > 50 && best_cost > 0)
	{
		double cur_best_cost = best_cost;
		//double best_cost_lb = 0;
		for (double tmp_best_cost = 0; tmp_best_cost < cur_best_cost; ++tmp_best_cost)
		{
			//best_cost = tmp_best_cost;
            //best_cost = std::numeric_limits<double>::max();
			this->coloring_kernel(vBestColor, vColor, best_cost, cur_cost, 0, tmp_best_cost, tmp_best_cost);
			//actual_cost = this->calc_cost(vBestColor);
            //std::cout << "tmp_best_cost = " << tmp_best_cost << " best_cost = " << best_cost << " actual_cost = " << actual_cost 
                //<< " best_cost_lb = " << best_cost_lb 
                //<< std::endl;
			//if (best_cost == actual_cost)
            if (best_cost <= tmp_best_cost)
				break;
			//else best_cost_lb += 1;
            // reset 
            vColor.assign(this->m_vColor.begin(), this->m_vColor.end());
            cur_cost = 0;
		}
	}
	else if (best_cost > 0)
        this->coloring_kernel(vBestColor, vColor, best_cost, cur_cost, 0, 0, best_cost);

	// apply coloring solution 
	this->m_vColor.swap(vBestColor);

	// verify solution  
	actual_cost = this->calc_cost(this->m_vColor);
	assert_msg(best_cost == actual_cost, "best_cost = " << best_cost << ", actual cost = " << actual_cost);

	return best_cost;
}

template <typename GraphType>
double BacktrackColoring<GraphType>::init_coloring(vector<int8_t>& vColor) const 
{
	DsatColoring<graph_type> dc (this->m_graph);
	dc();

	vertex_iterator_type vi, vie;
	for (boost::tie(vi, vie) = boost::vertices(this->m_graph); vi != vie; ++vi)
	{
		graph_vertex_type v = *vi;
		int8_t color = dc.color(v);
		if (vColor[v] >= 0 && vColor[v] < this->m_color_num) // precolored 
			continue;
		else if (color >= this->m_color_num) 
			vColor[v] = this->m_color_num-1;
		else vColor[v] = color;
		assert(vColor[v] >= 0 && vColor[v] < this->m_color_num);
	}
	// calculate cost 
	double cost = this->calc_cost(vColor);
	return cost;
}

template <typename GraphType>
void BacktrackColoring<GraphType>::coloring_kernel(vector<int8_t>& vBestColor, vector<int8_t>& vColor, double& best_cost, double& cur_cost, 
        BacktrackColoring<GraphType>::graph_vertex_type v, double cost_lb, double cost_ub) const 
{
	if (best_cost <= cost_lb) // no conflict or reach to lower bound cost  
		return;
	if (cur_cost >= best_cost || cur_cost > cost_ub) // branch and bound 
		return; 
	if (v == boost::num_vertices(this->m_graph)) // leaf node in the recursion tree 
	{
		if (cur_cost < best_cost) 
		{
			best_cost = cur_cost;
			vBestColor.assign(vColor.begin(), vColor.end());
		}
		return;
	}

	int8_t color_begin = 0;
	int8_t color_end = this->m_color_num;
	if (this->m_vColor[v] >= 0 && this->m_vColor[v] < this->m_color_num) // precolored vertex 
	{
		color_begin = this->m_vColor[v];
		color_end = color_begin+1;
	}
	for (int8_t c = color_begin; c < color_end; ++c)
	{
		vColor[v] = c;
		double delta_cost = 0;
		adjacency_iterator_type vi, vie;
		for (boost::tie(vi, vie) = boost::adjacent_vertices(v, this->m_graph); vi != vie; ++vi)
		{
			graph_vertex_type u = *vi;
			if (u < v) // only check parent node in the recursion tree 
			{
				if (vColor[u] == c) // conflict 
				{
					std::pair<graph_edge_type, bool> e = boost::edge(u, v, this->m_graph);
					assert_msg(e.second, "failed to find edge with " << u << "--" << v);
					edge_weight_type w = boost::get(boost::edge_weight, this->m_graph, e.first);
					assert_msg(w > 0, "only support conflict edges with positive cost"); // only support conflict edges 
					delta_cost += w;
				}
			}
		}
		cur_cost += delta_cost;
		this->coloring_kernel(vBestColor, vColor, best_cost, cur_cost, v+1, cost_lb, cost_ub); // recursion 
		cur_cost -= delta_cost;
	}
}

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
