/**
 * @file   test_SDPColoring.cpp
 * @brief  test SDP coloring algorithm 
 * @author Yibo Lin
 * @date   Sep 2015
 */

#include <iostream>
#include <fstream>
#include <string>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <limbo/algorithms/coloring/SDPColoringCsdp.h>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/iteration_macros.hpp>

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
		property<edge_index_t, std::size_t, property<edge_weight_t, double> >,
		property<graph_name_t, string> > graph_type;
typedef subgraph<graph_type> subgraph_type;
typedef property<vertex_index_t, std::size_t> VertexId;
typedef property<edge_index_t, std::size_t> EdgeID;
typedef graph_traits<graph_type>::vertex_descriptor vertex_descriptor; 
typedef graph_traits<graph_type>::edge_descriptor edge_descriptor; 
typedef property_map<graph_type, edge_weight_t>::type edge_weight_map_type;
typedef property_map<graph_type, vertex_color_t>::type vertex_color_map_type;
/// @endnowarn

/// test 1: a simple graph 
double simple_graph() 
{
	graph_type g;
	vertex_descriptor a = boost::add_vertex(g);
	vertex_descriptor b = boost::add_vertex(g);
	vertex_descriptor c = boost::add_vertex(g);
	vertex_descriptor d = boost::add_vertex(g);
	vertex_descriptor e = boost::add_vertex(g);
	boost::add_edge(a, b, g);
	boost::add_edge(a, c, g);
	boost::add_edge(a, d, g);
	boost::add_edge(a, e, g);
	boost::add_edge(b, c, g);
	boost::add_edge(b, e, g);
	boost::add_edge(c, d, g);
	boost::add_edge(d, e, g);

	BOOST_AUTO(edge_weight_map, get(edge_weight, g));
	graph_traits<graph_type>::edge_iterator eit, eit_end;
	for (tie(eit, eit_end) = edges(g); eit != eit_end; ++eit)
	{
        if (source(*eit, g) != a || target(*eit, g) != d)
            edge_weight_map[*eit] = 1;
        else 
            edge_weight_map[*eit] = -1;
	}

	//test relaxed LP based coloring
	limbo::algorithms::coloring::SDPColoringCsdp<graph_type> lc (g); 
	lc.stitch_weight(0.1);
	// THREE or FOUR 
	lc.color_num(limbo::algorithms::coloring::SDPColoringCsdp<graph_type>::THREE);
	return lc();
}

/// test 2: a random graph 
double random_graph() 
{
	mt19937 gen;
	graph_type g;
	int N = 40;
	std::vector<vertex_descriptor> vertex_set;
	std::vector< std::pair<vertex_descriptor, vertex_descriptor> > edge_set;
	generate_random_graph(g, N, N * 2, gen,
			std::back_inserter(vertex_set),
			std::back_inserter(edge_set));
	BOOST_AUTO(edge_weight_map, get(edge_weight, g));
	unsigned int i = 0; 
	graph_traits<graph_type>::edge_iterator eit, eit_end;
	for (tie(eit, eit_end) = edges(g); eit != eit_end; ++eit, ++i)
	{
#if 1
		if (i%10 == 0) // generate stitch 
			edge_weight_map[*eit] = -1;
		else // generate conflict 
#endif
			edge_weight_map[*eit] = 1;
	}

	//test relaxed LP based coloring
	limbo::algorithms::coloring::SDPColoringCsdp<graph_type> lc (g); 
	lc.stitch_weight(0.1);
	// THREE or FOUR 
	lc.color_num(limbo::algorithms::coloring::SDPColoringCsdp<graph_type>::THREE);
    return lc();
}

/// test 3: a real graph from input 
/// @param filename input file in graphviz format  
double real_graph(string const& filename)
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

#ifdef DEBUG_SDPCOLORING
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
	limbo::algorithms::coloring::SDPColoringCsdp<graph_type> lc (g); 
	lc.stitch_weight(0.1);
	// THREE or FOUR 
	lc.color_num(limbo::algorithms::coloring::SDPColoringCsdp<graph_type>::THREE);
	double cost = lc();

	in.close();
    return cost;
}

/// main function 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0
int main(int argc, char** argv)
{
    double cost;
	if (argc < 2)
	{
		cost = simple_graph();
		//cost = random_graph();
	}
	else cost = real_graph(argv[1]);

    cout << "cost = " << cost << endl;

	return 0;
}
