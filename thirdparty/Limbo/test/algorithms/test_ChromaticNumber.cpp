/**
 * @file   test_ChromaticNumber.cpp
 * @brief  test chromatic number algorithm 
 * @author Yibo Lin
 * @date   Feb 2015
 */

#include <iostream>
#include <fstream>
#include <string>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <limbo/algorithms/coloring/ChromaticNumber.h>
#include <limbo/algorithms/coloring/GreedyColoring.h>
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
using std::ofstream;
using std::string;
using namespace boost;

/// maximum independent set visitor 
/// @tparam GraphType graph type 
template <typename GraphType>
struct MisVisitor
{
	GraphType& g; ///< graph 

    /// constructor 
    /// @param g_ graph 
	MisVisitor(GraphType& g_) : g(g_) {}
    /// copy constructor 
    /// @param rhs a MisVisitor object  
	MisVisitor(MisVisitor const& rhs) : g(rhs.g) {}

    /// required callback for @ref limbo::algorithms::MaxIndependentSetByMaxClique
    /// @tparam MisType a container type, default is std::deque 
    /// @param is independent set 
	template <typename MisType>
	void mis(MisType const& is)
	{
		typename property_map<GraphType, vertex_index_t>::type vertex_index_map = get(vertex_index, g);

#if 1
		if (num_vertices(g) < 20)
		{
			cout << "independent sets" << endl;
			for (typename MisType::const_iterator it = is.begin(); 
					it != is.end(); ++it)
			{
				cout << vertex_index_map[*it] << " ";
			}
			cout << endl;
		}
#endif
	}
};

/// main function \n
/// generate a random graph, then \n 
/// - test 1: find maximum independent set by @ref limbo::algorithms::MaxIndependentSetByMaxClique
/// - test 2: compute chromatic number by @ref limbo::algorithms::coloring::LawlerChromaticNumber
/// - test 3: greedy coloring by @ref limbo::algorithms::coloring::DsatColoring
int main()
{
	// do not use setS, it does not compile for subgraph
	// do not use custom property tags, it does not compile for most utilities
	typedef adjacency_list<vecS, vecS, undirectedS, 
			property<vertex_index_t, std::size_t>, 
			property<edge_index_t, std::size_t, property<edge_weight_t, int> >,
			property<graph_name_t, string> > graph_type;
	typedef subgraph<graph_type> subgraph_type;
	typedef property<vertex_index_t, std::size_t> VertexId;
	typedef property<edge_index_t, std::size_t> EdgeID;
	typedef typename boost::graph_traits<subgraph_type>::vertex_descriptor vertex_descriptor; 

#if 1
	mt19937 gen;
    subgraph_type g;
    int N = 40;
    std::vector<vertex_descriptor> vertex_set;
    std::vector< std::pair<vertex_descriptor, vertex_descriptor> > edge_set;
    generate_random_graph(g, N, N * 2, gen,
                          std::back_inserter(vertex_set),
                          std::back_inserter(edge_set));
#else
	subgraph_type g;

	{
		vertex_descriptor v1 = add_vertex(g);
		vertex_descriptor v2 = add_vertex(g);
		vertex_descriptor v3 = add_vertex(g);
		vertex_descriptor v4 = add_vertex(g);
		vertex_descriptor v5 = add_vertex(g);
		add_edge(v1, v2, g);
		add_edge(v1, v3, g);
		add_edge(v1, v4, g);
		add_edge(v2, v3, g);
		add_edge(v2, v4, g);
		add_edge(v3, v4, g);
		add_edge(v3, v5, g);
	}

#endif

	property_map<subgraph_type, vertex_index_t>::type vertex_index_map = get(vertex_index, g);
	property_map<subgraph_type, edge_index_t>::type edge_index_map = get(edge_index, g);

	cout << "original graph" << endl;
	print_edges2(g, vertex_index_map, edge_index_map);
	print_graph(g, vertex_index_map);

	dynamic_properties dp;
	//dp.property("style", edge_style);
	//dp.property("pos", vertex_pos);
	//dp.property("len", weight);
	dp.property("node_id", get(vertex_index, g));
	ofstream out("graph.gv");
	write_graphviz_dp(out, g, dp);
	system("dot -Tpdf graph.gv -o graph.pdf");
	
	// test complement_graph
	{
		subgraph_type gp;
		std::map<vertex_descriptor, vertex_descriptor> mCompG2G;
		limbo::algorithms::complement_graph(g, gp, mCompG2G);

		cout << "complement graph" << endl;
		BGL_FORALL_VERTICES_T(v, gp, subgraph_type)
		{
			cout << vertex_index_map[mCompG2G[v]] << " <--> ";
			boost::graph_traits<subgraph_type>::out_edge_iterator e, e_end;
			for (boost::tie(e, e_end) = out_edges(v, gp); e != e_end; ++e)
				cout << vertex_index_map[mCompG2G[target(*e, gp)]] << " ";
			cout << endl;
		}
	}

	// test max_independent_set
	{
		limbo::algorithms::max_independent_set(g, MisVisitor<subgraph_type>(g), limbo::algorithms::MaxIndependentSetByMaxClique());
	}

	// test LawlerChromaticNumber
	{
		limbo::algorithms::coloring::LawlerChromaticNumber<graph_type> lcn;
		cout << "chromatic number = " << lcn(g) << endl;
	}

	// test DsatColoring
	{
		limbo::algorithms::coloring::DsatColoring<subgraph_type> dc (g);
		cout << "dsat coloring number = " << dc() << endl;
	}
	
	return 0;
}
