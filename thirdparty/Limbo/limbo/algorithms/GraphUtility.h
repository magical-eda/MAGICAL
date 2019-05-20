/**
 * @file   GraphUtility.h
 * @brief  some graph utilities such as compute complement graph and graphviz writer. 
 *
 * These are add-ons for Boost.Graph library. 
 *
 * @author Yibo Lin
 * @date   Feb 2015
 */

#ifndef LIMBO_ALGORITHMS_GRAPHUTILITY_H
#define LIMBO_ALGORITHMS_GRAPHUTILITY_H

#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/iteration_macros.hpp>

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.algorithms
namespace algorithms 
{

/// @brief get the complement graph of original graph 
/// @tparam GraphType graph type 
/// @param g original graph 
/// @param gp complement graph 
/// @param mCompG2G a vertex mapping from complement graph to original graph 
template <typename GraphType>
void complement_graph(GraphType const& g, GraphType& gp, 
		std::map<typename boost::graph_traits<GraphType>::vertex_descriptor, 
		typename boost::graph_traits<GraphType>::vertex_descriptor>& mCompG2G) 
{ 
	typedef typename boost::graph_traits<GraphType>::vertex_descriptor vertex_descriptor; 
	std::map<vertex_descriptor, vertex_descriptor> vmap; 

	BGL_FORALL_VERTICES_T(v, g, GraphType) 
	{
		vertex_descriptor u = boost::add_vertex(gp); 
		vmap[v] = u;
		mCompG2G[u] = v;
	}

	BGL_FORALL_VERTICES_T(u, g, GraphType) 
	{ 
		std::vector<vertex_descriptor> neighbors(
				adjacent_vertices(u, g).first, 
				adjacent_vertices(u, g).second
				); 
		std::sort(neighbors.begin(), neighbors.end()); 
		BGL_FORALL_VERTICES_T(v, g, GraphType) 
		{ 
			// Might want to check for self-loops 
			if (v != u && !std::binary_search(neighbors.begin(), neighbors.end(), v)) 
				boost::add_edge(vmap[u], vmap[v], gp); 
		} 
	} 
} 

/// @brief default VertexLabelWriter for write_graph
/// @tparam GraphType graph type 
template <typename GraphType>
struct VertexLabelWriter
{
    /// @nowarn 
    typedef GraphType graph_type;
    typedef typename boost::graph_traits<graph_type>::vertex_descriptor vertex_descriptor;
    /// @endnowarn

    graph_type const& g; ///< bind graph object 

    /// constructor 
    /// @param _g graph 
    VertexLabelWriter(graph_type const& _g) : g(_g) {}
    /// @param v vertex 
    /// @return label of vertex 
    vertex_descriptor label(vertex_descriptor v) const {return v;}
};

/// @brief default EdgeLabelWriter for write_graph 
/// @tparam GraphType graph type 
template <typename GraphType>
struct EdgeLabelWriter 
{
    /// @nowarn 
    typedef GraphType graph_type;
    typedef typename boost::graph_traits<graph_type>::edge_descriptor edge_descriptor;
    typedef typename boost::property_traits<typename boost::property_map<graph_type, boost::edge_weight_t>::const_type>::value_type edge_weight_type;
    /// @endnowarn 

    graph_type const& g; ///< bind graph object 

    /// constructor 
    /// @param _g graph 
    EdgeLabelWriter(graph_type const& _g) : g(_g) {}
    /// @param e edge 
    /// @return label of edge 
    edge_weight_type label(edge_descriptor e) const {return boost::get(boost::edge_weight, g, e);}
    /// @return color 
    std::string color(edge_descriptor ) const {return "black";}
    /// @return style 
    std::string style(edge_descriptor ) const {return "solid";}
};

/// @brief write graph to graphviz format and convert to pdf. 
/// Although Boost.Graph has write_graphviz component, it is not easy to use. 
/// @tparam GraphType graph type 
/// @tparam VertexLabelType must provide label() member function 
/// @tparam EdgeLabelType must provide label(), color(), and style() member function
/// @param out output stream 
/// @param g graph 
/// @param vl function object for vertex label
/// @param el function object for edge label 
template <typename GraphType, typename VertexLabelType, typename EdgeLabelType>
void write_graph(std::ofstream& out, GraphType const& g, VertexLabelType const& vl, EdgeLabelType const& el) 
{
	out << "graph D { \n"
		<< "  randir = LR\n"
		<< "  size=\"4, 3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\",fontsize=200]\n" 
		<< "  node[shape=\"circle\",fontsize=200]\n";

	//output nodes 
	uint32_t vertex_num = boost::num_vertices(g);
	for(uint32_t k = 0; k < vertex_num; ++k) 
	{
		// output vertex label
		out << "  " << k << "[shape=\"circle\""<< ",label=\"" << vl.label(k) << "\"]\n";
	}//end for

	//output edges
    typename boost::graph_traits<GraphType>::edge_iterator ei, eie;
	for (boost::tie(ei, eie) = boost::edges(g); ei != eie; ++ei)
    {
        out << "  " << boost::source(*ei, g) << "--" << boost::target(*ei, g) 
            << "[label=" << el.label(*ei) << ",color=\"" << el.color(*ei) << "\",style=\"" << el.style(*ei) << "\",penwidth=3]\n";
    }
	out << "}";
}

/// @brief convert graphviz format to pdf.  
/// The input filename should be filename+suffix
/// @param filename output file name 
/// @param suffix file suffix 
inline void graphviz2pdf(std::string const& filename, const char* suffix = ".gv")
{
	char cmd[100];
	sprintf(cmd, "dot -Tpdf %s%s -o %s.pdf", filename.c_str(), suffix, filename.c_str());
	system(cmd);
}

} // namespace algorithms
} // namespace limbo

#endif
