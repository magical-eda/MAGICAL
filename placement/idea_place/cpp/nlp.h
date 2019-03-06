/*************************************************************************
    > File Name: nlp.h
    > Author: anoynmous anoynmous
    > Mail: anoynmous@anoynmous
    > Created Time: Fri 01 Jun 2018 07:31:46 PM CDT
 ************************************************************************/

#ifndef __NLP_H_INCLUDED__
#define __NLP_H_INCLUDED__

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
 
#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/timer/timer.hpp>

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>
#ifdef USE_GUROBI
#include <limbo/solvers/api/GurobiApi.h>
#endif
#include <limbo/solvers/api/LPSolveApi.h>

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <set>
#include <map>
#include <utility>
#include <cassert>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include "wnnlp.h"
#include "wnconj.h"

#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/index/rtree.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::set;
using std::pair;
using std::map;
namespace gtl = boost::polygon;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace trans = boost::geometry::strategy::transform;

using namespace boost::polygon::operators;

using GdsParser::GdsDB::GdsCell;
using GdsParser::GdsDB::GdsObject;
using GdsParser::GdsDB::GdsPolygon;
using GdsParser::GdsDB::GdsRectangle;
using GdsParser::GdsDB::GdsCellReference;
typedef GdsPolygon::base_ext_type polygon_type;
typedef GdsPolygon::coordinate_type coordinate_type;
typedef GdsPolygon::point_type point_type;
typedef gtl::rectangle_data<double> rectangle_type_d;
typedef gtl::polygon_data<double> polygon_type_d;
typedef gtl::polygon_90_data<double> polygon_90_t;
typedef gtl::polygon_90_set_data<double> polygon_90_set_t;
typedef gtl::polygon_traits<polygon_90_t>::point_type point;

// typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
// typedef bg::model::box<point_t> box_t;
// typedef bg::model::polygon<point_t> polygon_t;
typedef std::pair<rectangle_type_d, size_t> rtree_value;

class mIntersect {
	rectangle_type_d m_rect;
public:
	mIntersect(rectangle_type_d const& r) : m_rect(r) {}
	bool operator()(rtree_value const& v) const {
		return gtl::intersects(v.first, m_rect, false);
	}
};

class mDifferentID {
	size_t idx;
public:
	mDifferentID(size_t i) : idx(i) {}
	bool operator()(rtree_value const& v) const {
		return idx != v.second; 
	}
};

class mPointTouch {
	rectangle_type_d m_rect;
public:
	mPointTouch(rectangle_type_d const& r) : m_rect(r) {}
	// only corner poing touching
	bool operator()(rtree_value const& v) const {
		return (gtl::ll(v.first)==gtl::ur(m_rect) || gtl::ur(v.first)==gtl::ll(m_rect) || gtl::lr(v.first)==gtl::ul(m_rect) || gtl::ul(v.first)==gtl::lr(m_rect)); 
	}
};

class Event {
public:
	Event(double ts, size_t idx, size_t ridx, bool edge_type) : time_stamp(ts), index(idx), real_index(ridx), edge_type(edge_type) {}

	double time_stamp;
	size_t index;
	size_t real_index;
	bool edge_type; // 0: bottom edge; 1: top edge

	bool operator < (const Event& that) const {
		if (time_stamp < that.time_stamp) {
			return true;
		} else if (time_stamp == that.time_stamp) {
			return edge_type > that.edge_type;
		} else {
			return false;
		}
	}
};

class Vertex {
public:
	Vertex(size_t i, string n) : index(i), name(n) {}

	void add_dimension(size_t w, size_t h) {
		dimensions.push_back(std::make_pair(w, h));
	}

	size_t index;
	string name;
	string type;
	vector<pair<size_t, size_t> > dimensions; // this version considers variants
	pair<double, double> scal_dimension; // the actual dimension used in the program 
	pair<double,double> offset;
	// vector< pair<int, gtl::rectangle_data<double> > > feol_layers; 
	vector< pair<int, rectangle_type_d > > feol_layers; 
	GdsCell* gds_cell;
};

class dBox {
public:
	dBox(size_t _index, double _x, double _y, double _w, double _h) : index(_index), x(_x), y(_y), w(_w), h(_h) {}

	size_t index;
	double x;
	double y;
	double w;
	double h;

	bool operator==(dBox const& that) const {
		return index==that.index;
	}

	bool hor_overlapping(dBox const& that) const {
		if (x+w <= that.x || that.x+that.w <= x) {
			return false;
		}
		return true;
	}

	bool ver_overlapping(dBox const& that) const {
		if (y+h <= that.y || that.y+that.h <= y) {
			return false;
		}
		return true;
	}

	int overlapping(dBox const& that) const {
		if (!(hor_overlapping(that) && ver_overlapping(that))) {
			return 0;
		}
		else {
			// calculate displacement
			double disp_l = that.x + that.w - x;
			double disp_r = x + w - that.x;
			double disp_b = that.y + that.h - y;
			double disp_t = y + h - that.y;
			double min_disp = std::min(std::min(std::min(disp_l, disp_r), disp_b), disp_t);
			if (min_disp == disp_l) { return 1; }
			else if (min_disp == disp_r) { return 2; }
			else if (min_disp == disp_b) { return 3; }
			else if (min_disp == disp_t) { return 4; }
			else { assert(0); } // should not be here
		}
	}
	
	double get_displacement_l(dBox const& that) const {
		return (that.x+that.w-x);
	}

	double get_hor_overlap(dBox const& that) const {
		double o_l = std::max(x, that.x);
		double o_r = std::min(x+w, that.x+that.w);
		assert(o_r > o_l); // if overlap
		return (o_r - o_l);
	}
	
	double get_ver_overlap(dBox const& that) const {
		double o_b = std::max(y, that.y);
		double o_t = std::min(y+h, that.y+that.h);
		assert(o_t > o_b); // if overlap
		return (o_t - o_b);
	}

};

class dSegment {
public:
	dSegment() {}
	dSegment(double first, double second) {
		segs.push_back(std::make_pair(first, second));
	}

	vector<pair<double, double> > segs;

    struct {
        bool operator()(pair<double, double> a, pair<double, double> b) const {   
			return a.first < b.first;
        }   
    } comp;
	
	bool trim(double first, double second) {
		bool overlap_flag = false;
		vector<pair<double, double> > segs_to_add;
		vector<size_t> segs_to_erase;
		for (size_t i=0; i<segs.size(); ++i) {
			if (second <= segs[i].first || segs[i].second <= first) {
				continue;
			}
			overlap_flag = true;
			if (second > segs[i].first && second < segs[i].second) {
				if (first <= segs[i].first) {
					segs[i].first = second;
				}
				else {
					segs_to_add.push_back(std::make_pair(second, segs[i].second));
					segs[i].second = first;
				}
			}
			else if (second >= segs[i].second) {
				if (first <= segs[i].first) {
					segs_to_erase.push_back(i);
				}
				else {
					segs[i].second = first;
				}
			}
		}
		// first delete
		std::sort(segs_to_erase.begin(), segs_to_erase.end(), std::greater<size_t>());
		for (auto i : segs_to_erase) {
			segs.erase(segs.begin()+i);
		}
		// then add
		segs.insert(segs.end(), segs_to_add.begin(), segs_to_add.end());
		std::sort(segs.begin(), segs.end(), comp); // maintain order
		return overlap_flag;
	} // end trime
};

enum class OptimizerType 
{
    GUROBI, 
    LPSOLVE
};

class NLPlace {
public:
	NLPlace() {}
	NLPlace(string const& techfile, string const& spacing_rules_path, string const& width_area_rules_path, string const& path, string const& dir, OptimizerType const& optimizer) 
    {
        in_simple_techfile_name = techfile; 
        in_spacing_rules_file_name = spacing_rules_path; 
        in_width_area_rules_file_name = width_area_rules_path; 
        files_path = path; 
        results_dir = dir; 
        optimizer_type = optimizer; 
    }
	~NLPlace() {}

	typedef pair<size_t, size_t> symmetric_pair_t; // vertex ID
	typedef set< symmetric_pair_t > symmetric_pairs_t;
	typedef set<size_t> self_symmetric_t;
	typedef pair< symmetric_pairs_t, self_symmetric_t > symmetric_group_t;
	typedef boost::adjacency_list < boost::listS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property < boost::edge_weight_t, double > > graph_t;
    typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits < graph_t >::edge_descriptor edge_descriptor;
    typedef boost::graph_traits < graph_t >::edge_iterator edge_iterator;
	typedef boost::property_map<graph_t, boost::vertex_index_t>::type IndexMap;
	typedef boost::graph_traits < graph_t >::adjacency_iterator adjacency_iterator;
    typedef std::pair<size_t, size_t> Edge;
	
	void run();
	int global_place();
	void read_params();
	void init_file_names();
	// initialize graph g from the netlist file
	void read_netlist_files();
	void read_block_file_hypergraph();
	void read_net_file_hypergraph();
	void read_symmetry_file();
	void read_overlap_file();
	void read_offset_file();
	void read_techfile_simple();
	void read_spacing_rules_file();
	void read_width_area_rules_file();
	void parse_gds_file(size_t, string);
	void parse_gds_files();
	void scale_vtcs_dimension();
	void write_result_to_file(string);
	void legalize(pair<double, double>&);
	void solve_shortest_paths(vector<dBox> const&, pair<double, double>&); 
	void build_constraint_graphs(vector<dBox> const&);
	void init_edges(vector<dBox> const&, bool, vector< Edge >&, vector<double>&, bool, bool has_symmetry_edges=true);
	void init_irredundant_edges(vector<dBox> const&, bool, vector< Edge >&, vector<double>&); 
	void init_irredundant_edges_insert(vector<dBox> const&, bool, vector<dBox>&, vector<int>&, size_t, size_t);
	void init_irredundant_edges_delete(vector<dBox> const&, bool, vector< Edge >&, vector<double>&, vector<dBox>&, vector<int> const&, size_t, size_t, vector<bool> const&);
	void get_cg_edges(vector<dBox> const&, vector< Edge > &, vector<double> &, vector<bool> &, bool, int, bool);
	void get_cg_edges_no_redundant(vector<dBox> const&, vector< Edge > &, vector<double> &, vector<bool> &, bool, int, bool);
	void constraint_graph(vector< Edge >&, vector<double>&, graph_t&, int);
	void solve_shortest_path(vector<dBox> const&, bool, vector<double> &, double&, graph_t&, vector<double> &);
	double get_total_device_area();
	double get_total_overlap_area(double *);
	void get_total_gp_area(double *, double&, double&, double&, double&);
	double get_total_asym_distance(double *);
	void init_static_variables();
	void detect_negative_cycle();
	std::pair<double, limbo::solvers::SolverProperty> solve_lp(vector< Edge > const&, bool, int, vector<dBox> const&, vector<double>&, double, bool opt_hpwl=true, bool opt_area=false);
	void check_locs_symmetry(vector<dBox> const&);
	// void legal_detail_place();
	limbo::solvers::SolverProperty legal_detail_place(bool infeasible_rerun=false);
	void get_necessary_edges();
	void check_boxes_separable(size_t, size_t);
	void add_edge_greedy(size_t, size_t, graph_t&, graph_t&);
	void dfs_graph(vector< vector<bool> >&, vector<bool>&, size_t, graph_t const&, IndexMap const&);
	void write_graph_dot(graph_t const&, string);
	void dag_transitive_reduction(graph_t&);
	bool dfs_remove_transitive_edge(graph_t&, vector< vector<bool> >&, size_t, vector<bool>&, vector< vector<bool> >&, IndexMap const&);
	void init_locs_from_file(string);
	void reinit_edge_and_weight(graph_t const&, vector< Edge >&, vector<double>&);
	void write_gds();

	// graphs for shortest paths
	graph_t cg_h; // the constraint graph
	vector< Edge > edge_vec_h;
	vector<double> weight_vec_h;
	graph_t cg_v; // the constraint graph
	vector< Edge > edge_vec_v;
	vector<double> weight_vec_v;
	vector<dBox> d_boxes;

	static vector< symmetric_group_t > symmetric_groups;

	string results_dir = "./results/";
	string files_path = "";
	string ckt_name;
	string in_block_file_name;
	string in_net_file_name;
	string in_cnet_file_name;
	string in_symmetry_file_name;
	string in_regularity_file_name;
	string in_overlap_file_name;
	string in_offset_file_name;
	string in_type_file_name;
	string in_simple_techfile_name;
	string in_spacing_rules_file_name;
	string in_width_area_rules_file_name;
	string in_gds_file_prefix;
    OptimizerType optimizer_type;

	// parameters to tune
	vector<double> sfs; // scaanoynmousg factors

	/************ For HPWL and hyper graph **************/
	// NOTE: store vertex ids and edge ids
	map<string, size_t> m_vn_ids; // map vertex name to id 
	set<string> critical_nets; 
	typedef map< string, set<size_t> > m_en_vids_t;
	static m_en_vids_t m_en_vids;

	static vector<Vertex> vtcs;
	static vector<int> dtype; // device types, 0: transistor & resistors; 1: capacitor; 2: others

	// location vector
	vector< pair<double, double> > locs;

	/**** Below are parameters ****/
	double dim_scal_fac = 0.01;
	int uu2gdsu = 1000;
	double gds_scal_fac = 0.001;
	// double gds_scal_fac = 1.0 / (double) uu2gdsu;
	static double lambda1; // the coefficient for x and y overlap penalty
	static double lambda2; // the coefficient for out of boundary penalty
	static double lambda3; // the coefficient for wire length penalty
	static double lambda4; // the coefficient for asymmetry penalty
	// alpha should be a very small value
	static constexpr double alpha = 1;
	// boundaries, assume lb_x, lb_y is 0
	// == automatically set the bounds
	static double ub_x;
	static double ub_y;
	double max_white_space = 0.2;
	double aspect_ratio = 1.0; // width:height
	// double aspect_ratio = 0.6; // width:height
	double total_device_area;
	// == parameters to tune
	double overlap_threshold = 0.005;
	double oob_threshold = 0.05; // out of bound penalty threshold
	double asym_threshold = 0.5; // asymmetry penalty threshold
	
	/************ For design rules handanoynmousg **************/
	vector< pair<int, string> > layers; 
	map<int, size_t> layer_num2id; 
	vector< vector<int> > spacing_rules; // need to scale by 1000 (gds_scal_fac)
	vector<int> width_rules; // need to scale by 1000 (gds_scal_fac)
	vector<int> area_rules; // need to scale by 1000*1000 (gds_scal_fac*gds_scal_fac)
	bgi::rtree< rtree_value, bgi::quadratic<16> > vtcs_rtree;

	vector< vector< rectangle_type_d > > layer_rects;

};

#endif
