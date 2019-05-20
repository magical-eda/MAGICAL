/**
 * @file   Coloring.h
 * @brief  base class for all graph coloring algorithms 
 * @author Yibo Lin
 * @date   Jun 2015
 */

#ifndef LIMBO_ALGORITHMS_COLORING_COLORING
#define LIMBO_ALGORITHMS_COLORING_COLORING

#include <fstream>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/functional/hash.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <limbo/preprocessor/AssertMsg.h>
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

using boost::uint32_t;
using boost::int32_t;
using boost::int8_t;

namespace la = limbo::algorithms;

/// function object to write vertex label 
/// @tparam GraphType graph type 
template <typename GraphType>
struct ColoringVertexLabelWriter : public la::VertexLabelWriter<GraphType>
{
    /// @nowarn 
    typedef GraphType graph_type;
    typedef la::VertexLabelWriter<graph_type> base_type;
    typedef typename base_type::vertex_descriptor vertex_descriptor;
    /// @endnowarn
    
    std::vector<int8_t> const& vColor; ///< coloring solutions 

    /// constructor 
    /// @param g graph 
    /// @param vc coloring solutions 
    ColoringVertexLabelWriter(graph_type const& g, std::vector<int8_t> const& vc) : base_type(g), vColor(vc) {}

    /// get label of vertex 
    /// @param v vertex 
    /// @return label of vertex 
    std::string label(vertex_descriptor v) const 
    {
        std::ostringstream oss; 
        oss << v << ":" << (int32_t)vColor[v];
        return oss.str();
    }
};

/// function object to write edge label 
/// @tparam GraphType graph type 
template <typename GraphType>
struct ColoringEdgeLabelWriter : public la::EdgeLabelWriter<GraphType>
{
    /// @nowarn
    typedef GraphType graph_type;
    typedef la::EdgeLabelWriter<graph_type> base_type;
    typedef typename base_type::edge_descriptor edge_descriptor;
    typedef typename base_type::edge_weight_type edge_weight_type;
    typedef typename boost::graph_traits<graph_type>::vertex_descriptor vertex_descriptor;
    /// @endnowarn

    std::vector<int8_t> const& vColor; ///< coloring solutions 

    /// constructor 
    /// @param g graph 
    /// @param vc coloring solutions 
    ColoringEdgeLabelWriter(graph_type const& g, std::vector<int8_t> const& vc) : base_type(g), vColor(vc) {}

    /// get label of edge 
    /// @param e edge 
    /// @return label of edge 
    edge_weight_type label(edge_descriptor e) const {return boost::get(boost::edge_weight, this->g, e);}
    /// get color of edge 
    /// @param e edge 
    /// @return color of edge 
    std::string color(edge_descriptor e) const 
    {
		vertex_descriptor s = boost::source(e, this->g);
		vertex_descriptor t = boost::target(e, this->g);
		edge_weight_type w = boost::get(boost::edge_weight, this->g, e);
        bool conflict_flag = (vColor[s] >= 0 && vColor[s] == vColor[t]);
        if (w >= 0 && conflict_flag) // conflict 
            return "red";
        else return "black";
    }
    /// get style of edge 
    /// @param e edge 
    /// @return style of edge 
    std::string style(edge_descriptor e) const {return (boost::get(boost::edge_weight, this->g, e) >= 0)? "solid" : "dashed";}
};

/// @class limbo::algorithms::coloring::Coloring
/// Base class for all coloring algorithms.  
/// All coloring algorithms support 3 and 4 colors. 
/// @tparam GraphType graph type 
template <typename GraphType>
class Coloring 
{
	public:
        /// @nowarn 
		typedef GraphType graph_type;
		typedef typename boost::graph_traits<graph_type>::vertex_descriptor graph_vertex_type;
		typedef typename boost::graph_traits<graph_type>::edge_descriptor graph_edge_type;
		typedef typename boost::graph_traits<graph_type>::vertex_iterator vertex_iterator_type;
		typedef typename boost::graph_traits<graph_type>::edge_iterator edge_iterator_type;
        // value type for edge weight, integer or double...
        typedef typename boost::property_traits<typename boost::property_map<graph_type, boost::edge_weight_t>::const_type>::value_type edge_weight_type;
		// edge weight is used to differentiate conflict edge and stitch edge 
		// non-negative weight implies conflict edge 
		// negative weight implies stitch edge 
        typedef typename boost::property_traits<typename boost::property_map<graph_type, boost::edge_index_t>::const_type>::value_type edge_index_type;
        /// @endnowarn

        /// number of colors 
		enum ColorNumType
		{
			THREE = 3, 
			FOUR = 4
		};
		/// hasher class for graph_edge_type
		struct EdgeHashType : std::unary_function<graph_edge_type, std::size_t>
		{
            /// API to get edge hash value 
            /// @param e edge 
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
		Coloring(graph_type const& g);
		/// destructor
		virtual ~Coloring() {};

		// member functions
		/// top API 
		/// @return objective value 
		virtual double operator()(); 

		/// set number of colors 
        /// @param cn number of colors 
		virtual void color_num(ColorNumType cn) {m_color_num = cn;} 
		/// set number of colors 
        /// @param cn number of colors 
		virtual void color_num(int8_t cn) {m_color_num = (cn == 3)? THREE : FOUR;}
        /// @return number of colors 
        virtual ColorNumType color_num() const {return m_color_num;}

		/// set precolored vertex 
        /// @param v vertex 
        /// @param c color 
		virtual void precolor(graph_vertex_type v, int8_t c) {m_vColor[v] = c; m_has_precolored = true;}

        /// @return true if contain precolored vertices 
        virtual bool has_precolored() const {return m_has_precolored;}

		/// @return stitch weight 
		virtual double stitch_weight() const {return m_stitch_weight;}
        /// set weight of stitch 
        /// @param w weight 
		virtual void stitch_weight(double w) {m_stitch_weight = w;}

		/// set number of threads 
        /// @param t number of threads 
		virtual void threads(int32_t t) {m_threads = t;}

        /// retrieve coloring solution 
        /// @param v vertex 
		/// @return coloring solution 
		virtual int8_t color(graph_vertex_type v) const {return m_vColor[v];}

        // helper functions 
        /// get edge weight 
        /// @param e edge 
        /// @return edge weight 
        inline virtual edge_weight_type edge_weight(graph_edge_type const& e) const {return boost::get(boost::edge_weight, m_graph, e);}

        /// compute cost of coloring solutions
        /// @param vColor coloring solutions 
		/// @return cost with a coloring solution 
		virtual edge_weight_type calc_cost(std::vector<int8_t> const& vColor) const;

        /// check edge weight within \a lb and \a ub 
        /// @param g graph 
        /// @param lb lower bound 
        /// @param ub upper bound 
        void check_edge_weight(graph_type const& g, edge_weight_type lb, edge_weight_type ub) const;

        /// print edge weight 
        /// @param g graph 
        void print_edge_weight(graph_type const& g) const;

		// for debug 
        /// write graph in graphviz format 
        /// @param filename output file name 
		virtual void write_graph(std::string const& filename) const;
        /// write graph in graphviz format 
        /// @param filename output file name 
        /// @param g graph 
        /// @param vColor coloring solutions 
        virtual void write_graph(std::string const& filename, graph_type const& g, std::vector<int8_t> const& vColor) const;
	protected:
		/// @return objective value 
		virtual double coloring() = 0;

		graph_type const& m_graph; ///< initial graph 
		std::vector<int8_t> m_vColor; ///< coloring solutions 

		ColorNumType m_color_num; ///< number of colors 
		double m_stitch_weight; ///< stitch weight 
		int32_t m_threads; ///< control number of threads for ILP solver 
        bool m_has_precolored; ///< whether contain precolored vertices 
};

template <typename GraphType>
Coloring<GraphType>::Coloring(Coloring<GraphType>::graph_type const& g) 
    : m_graph(g)
    , m_vColor(boost::num_vertices(g), -1)
    , m_color_num(THREE)
    , m_stitch_weight(0.1)
    , m_threads(std::numeric_limits<int32_t>::max())
    , m_has_precolored(false)
{}

template <typename GraphType>
double Coloring<GraphType>::operator()()
{
    if (boost::num_vertices(m_graph) <= color_num()) // if vertex number is no larger than color number, directly assign color
    {
        // need to consider precolored vertices
        bool unusedColors[4] = {true, true, true, true};
        if (color_num() == THREE)
            unusedColors[3] = false;
        for (int32_t i = 0, ie = m_vColor.size(); i != ie; ++i)
        {
            if (m_vColor[i] < 0) // if not precolored, assign to an unused color
            {
                for (int8_t c = 0; c != 4; ++c)
                    if (unusedColors[c])
                    {
                        m_vColor[i] = c;
                        break;
                    }
            }
            // must have valid color after assignment 
            assert(m_vColor[i] >= 0);
            unusedColors[m_vColor[i]] = false;
        }
        return calc_cost(m_vColor);
    }
    else // perform coloring algorithm 
        return this->coloring();
}

template <typename GraphType>
typename Coloring<GraphType>::edge_weight_type Coloring<GraphType>::calc_cost(std::vector<int8_t> const& vColor) const 
{
	assert(vColor.size() == boost::num_vertices(this->m_graph));
	double cost = 0;
	edge_iterator_type ei, eie;
	for (boost::tie(ei, eie) = boost::edges(m_graph); ei != eie; ++ei)
	{
		edge_weight_type w = boost::get(boost::edge_weight, m_graph, *ei);
		graph_vertex_type s = boost::source(*ei, m_graph);
		graph_vertex_type t = boost::target(*ei, m_graph);
        if (s == t) // skip self edges 
            continue; 
		if (w >= 0) // conflict edge 
			cost += (vColor[s] == vColor[t])*w;
		else // stitch edge 
			cost += (vColor[s] != vColor[t])*w;
	}
	return cost;
}

template <typename GraphType>
void Coloring<GraphType>::check_edge_weight(typename Coloring<GraphType>::graph_type const& g, typename Coloring<GraphType>::edge_weight_type lb, typename Coloring<GraphType>::edge_weight_type ub) const 
{
	edge_iterator_type ei, eie;
	for (boost::tie(ei, eie) = boost::edges(g); ei != eie; ++ei)
	{
		edge_weight_type w = boost::get(boost::edge_weight, m_graph, *ei);
        assert_msg(w >= lb && w <= ub, "edge weight out of range: " << w);
    }
}

template <typename GraphType>
void Coloring<GraphType>::print_edge_weight(typename Coloring<GraphType>::graph_type const& g) const 
{
	edge_iterator_type ei, eie;
	for (boost::tie(ei, eie) = boost::edges(g); ei != eie; ++ei)
	{
		edge_weight_type w = boost::get(boost::edge_weight, m_graph, *ei);
        std::cout << w << " ";
    }
    std::cout << "\n";
}

// it seems doxygen cannot handle template functions with the same name correctly 
/// @cond
template <typename GraphType>
void Coloring<GraphType>::write_graph(std::string const& filename) const 
{
    write_graph(filename, m_graph, m_vColor);
}

template <typename GraphType>
void Coloring<GraphType>::write_graph(std::string const& filename, Coloring<GraphType>::graph_type const& g, std::vector<int8_t> const& vColor) const
{
    std::ofstream out ((filename+".gv").c_str());
    la::write_graph(out, g, ColoringVertexLabelWriter<graph_type>(g, vColor), ColoringEdgeLabelWriter<graph_type>(g, vColor));
    out.close();
    la::graphviz2pdf(filename);
}
/// @endcond

} // namespace coloring
} // namespace algorithms
} // namespace limbo

#endif
