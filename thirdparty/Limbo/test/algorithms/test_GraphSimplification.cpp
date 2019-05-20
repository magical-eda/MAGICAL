/**
 * @file   test_GraphSimplification.cpp
 * @brief  test graph simplification algorithms @ref limbo::algorithms::coloring::GraphSimplification
 * @author Yibo Lin
 * @date   May 19 01:15:09 2015
 */

#include <iostream>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <limbo/algorithms/coloring/GraphSimplification.h>
#include <boost/version.hpp>
#if BOOST_VERSION <= 14601
#include <boost/graph/detail/is_same.hpp>
#else
#include <boost/type_traits/is_same.hpp>
#endif

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::pair;
using namespace boost;

/// @nowarn 
// do not use setS, it does not compile for subgraph
// do not use custom property tags, it does not compile for most utilities
typedef adjacency_list<vecS, vecS, undirectedS, 
		property<vertex_index_t, std::size_t, property<vertex_color_t, int> >, 
		property<edge_index_t, std::size_t, property<edge_weight_t, int> >,
		property<graph_name_t, string> > graph_type;
typedef subgraph<graph_type> subgraph_type;
typedef property<vertex_index_t, std::size_t> VertexId;
typedef property<edge_index_t, std::size_t> EdgeID;
typedef graph_traits<graph_type>::vertex_descriptor vertex_descriptor; 
typedef graph_traits<graph_type>::edge_descriptor edge_descriptor; 
typedef property_map<graph_type, edge_weight_t>::type edge_weight_map_type;
typedef property_map<graph_type, vertex_color_t>::type vertex_color_map_type;
/// @endnowarn

/// test 1: a real graph from input 
/// @param filename input file in graphviz format 
void realGraph(string const& filename)
{
	ifstream in (filename.c_str());

	// the graphviz reader in boost cannot specify vertex_index_t
	// I have to create a temporary graph and then construct the real graph 
	typedef adjacency_list<vecS, vecS, undirectedS, 
			property<vertex_index_t, std::size_t, property<vertex_color_t, int, property<vertex_name_t, std::size_t> > >, 
			property<edge_index_t, std::size_t, property<edge_weight_t, int> >,
			property<graph_name_t, string> > tmp_graph_type;
	tmp_graph_type tmpg;
	dynamic_properties tmpdp;
	tmpdp.property("node_id", get(vertex_name, tmpg));
	tmpdp.property("label", get(vertex_name, tmpg));
	tmpdp.property("weight", get(edge_weight, tmpg));
	tmpdp.property("label", get(edge_weight, tmpg));
	assert(read_graphviz(in, tmpg, tmpdp, "node_id"));

	// real graph 
	graph_type g (num_vertices(tmpg));
	graph_traits<tmp_graph_type>::vertex_iterator vit, vit_end;
	for (tie(vit, vit_end) = vertices(tmpg); vit != vit_end; ++vit)
	{
		size_t name  = get(vertex_name, tmpg, *vit);
		int color = get(vertex_color, tmpg, *vit);
		put(vertex_color, g, (vertex_descriptor)name, color);
	}

	graph_traits<tmp_graph_type>::edge_iterator eit, eit_end;
	for (tie(eit, eit_end) = edges(tmpg); eit != eit_end; ++eit)
	{
		size_t s_name = get(vertex_name, tmpg, source(*eit, tmpg));
		size_t t_name = get(vertex_name, tmpg, target(*eit, tmpg));
		pair<edge_descriptor, bool> pe = add_edge(s_name, t_name, g);
		assert(pe.second);
		int weight = get(edge_weight, g, *eit);
		put(edge_weight, g, pe.first, weight);
	}

#ifdef DEBUG_GRAPHSIMPLIFICATION
	dynamic_properties dp;
	dp.property("id", get(vertex_index, g));
	dp.property("node_id", get(vertex_index, g));
	dp.property("label", get(vertex_index, g));
	dp.property("weight", get(edge_weight, g));
	dp.property("label", get(edge_weight, g));
	ofstream out ("graph_init.gv");
	write_graphviz_dp(out, g, dp, string("id"));
	out.close();
	system("dot -Tpdf graph_init.gv -o graph_init.pdf");
#endif

	//test relaxed LP based coloring
	typedef limbo::algorithms::coloring::GraphSimplification<graph_type> simplification_type;
	simplification_type gs (g, 3); 
	std::vector<int> vPrecolor (num_vertices(g), -1);
	if (vPrecolor.size() > 0) vPrecolor[0] = 0;
	if (vPrecolor.size() > 3) vPrecolor[3] = 0;
	if (vPrecolor.size() > 4) vPrecolor[4] = 0;
	gs.precolor(vPrecolor.begin(), vPrecolor.end());
#if 0
	gs.hide_small_degree();
	gs.write_graph_dot("graph_simpl1");
	//gs.merge_subK4();
	gs.biconnected_component();
	//gs.connected_component();
#else 
	gs.simplify(simplification_type::HIDE_SMALL_DEGREE | simplification_type::BICONNECTED_COMPONENT);
#endif
	gs.write_graph_dot("graph_simpl3");
	gs.write_simplified_graph_dot("graph_simpl_merge");

	in.close();
}

/// main function \n
/// test either on real graph 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 
int main(int argc, char** argv)
{
	assert(argc >= 2);
	realGraph(argv[1]);

	return 0;
}
