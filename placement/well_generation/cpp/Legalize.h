/*************************************************************************
    > File Name: Legalize.h
    > Author: anoynmous anoynmous
    > Mail: anoynmous@anoynmous
    > Created Time: Thu 13 Dec 2018 05:53:16 PM CST
 ************************************************************************/

#ifndef __LEGALIZE_H_INCLUDED__
#define __LEGALIZE_H_INCLUDED__

#include <iostream>
#include <set>
#include <map>
#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <boost/geometry/geometries/adapted/boost_polygon.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using std::cout;
using std::endl;
using std::vector;
using std::set;
using std::map;
using std::pair;
using GdsParser::GdsDB::GdsCell;
using GdsParser::GdsDB::GdsObject;
using GdsParser::GdsDB::GdsPolygon;
using GdsParser::GdsDB::GdsRectangle;
using GdsParser::GdsDB::GdsCellReference;

namespace gtl = boost::polygon;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef GdsPolygon::base_ext_type	polygon_type;
typedef GdsPolygon::coordinate_type coordinate_type;
typedef GdsPolygon::point_type point_type;
typedef gtl::rectangle_data<int>	rectangle_t;
typedef gtl::polygon_90_data<int>	polygon_90_t;
typedef gtl::polygon_90_set_data<int>	polygon_90_set_t;
typedef gtl::polygon_traits<polygon_90_t>::point_type point_t;
typedef gtl::segment_data<int>		segment_t;
typedef gtl::interval_data<int>		interval_t;

typedef gtl::rectangle_data<double> rectangle_type_d;
typedef std::pair<rectangle_t, size_t> rtree_value;
// typedef std::pair<rectangle_type_d, size_t> rtree_value;

class mSegmentIntersect {
	segment_t m_segment;
public:
	mSegmentIntersect(segment_t const& intv) : m_segment(intv) {}
	bool operator()(rtree_value const& v) const {
		segment_t segl(gtl::ll(v.first), gtl::ul(v.first));
		segment_t segr(gtl::lr(v.first), gtl::ur(v.first));
		segment_t segb(gtl::ll(v.first), gtl::lr(v.first));
		segment_t segt(gtl::ul(v.first), gtl::ur(v.first));
		point_t pt_low = m_segment.low();
		point_t pt_high = m_segment.high();
		return gtl::intersects(m_segment, segl, false) || gtl::intersects(m_segment, segr, false) || gtl::intersects(m_segment, segb, false) || gtl::intersects(m_segment, segt, false) || gtl::contains(v.first, rectangle_t(gtl::x(pt_low), gtl::y(pt_low), gtl::x(pt_high), gtl::y(pt_high)));
	}
};

class mSegmentInside {
	segment_t m_segment;
public:
	mSegmentInside(segment_t const& intv) : m_segment(intv) {}
	bool operator()(rtree_value const& v) const {
		point_t pt_low = m_segment.low();
		point_t pt_high = m_segment.high();
		return gtl::contains(v.first, rectangle_t(gtl::x(pt_low), gtl::y(pt_low), gtl::x(pt_high), gtl::y(pt_high)));
	}
};

/*
class sEdge {
public:
	size_t device_id;
	int value;
	bool is_horizontal;
};
*/

class dEdge {
public:
	dEdge() {}
	dEdge(size_t s, size_t t, bool h) : src(s), tgt(t), is_horizontal(h) {}
	size_t src;
	size_t tgt;
	int value;
	bool is_horizontal;
};

template <class T>
class Loc {
public:
	// typedef double T;
	Loc() {}
	Loc(T _x, T _y) : m_x(_x), m_y(_y) {}
	T m_x;
	T m_y;

	// T const& x() const { return m_x; }
	// T const& y() const { return m_y; }
	T x() const { return m_x; }
	T y() const { return m_y; }
	T & x() { return m_x; }
	T & y() { return m_y; }
	// T const& w() const { return m_x; }
	// T const& h() const { return m_y; }
	T w() const { return m_x; }
	T h() const { return m_y; }
	T & w() { return m_x; }
	T & h() { return m_y; }
};

class LayerShape {
public:
	LayerShape(int lay, rectangle_t const& shp) : layer(lay), shape(shp) {}
	int layer;
	rectangle_t shape;
}; 

template <class T>
class Device {
public:
	// typedef double T;
	// typedef int T;
	Device() {}
	Device(string n, T x, T y, T w, T h) : name(n) {
		loc = Loc<T>(x, y);
		size = Loc<T>(w, h);
	}
	string name;
	int type = 0; // 0: device without NW; 1: device with NW
	Loc<T> loc;
	Loc<T> size;
	Loc<T> offset;
	vector< LayerShape > layers_shapes;
	GdsCell* gds_cell;
};

enum class OptimizerType 
{
    GUROBI, 
    LPSOLVE
};

class Legalize {
public:
	typedef int data_type; // data type for device location, etc
	
	Legalize() {}
	Legalize(string const& name, OptimizerType const& optimizer) : ckt_name(name), optimizer_type(optimizer) {}
	double	gds_scal_fac	= 0.001;
	int uu2gdsu = 1000;
	int		grid_size		= 340; // the NW minimum width value
	// double scale = 0.06; // gds unit to pixel unit
	int		scale			= 60; // gds unit to pixel unit
	double	device_in_well_thresh = 0.5; // intersection area ratio
	// Loc<data_type> gds_ll; // FIXME: set the lower left corner from the GDS parser
	// Loc<data_type> gds_ur; // FIXME: set the lower left corner from the GDS parser
	Loc<data_type> well_contact_size;
	rectangle_t well_contact_rect;
	rectangle_t well_contact_pin_rect;
	Loc<data_type> contour_offset;

	string ckt_name;
	string result_dir;
	string benchmark_dir;
	// string tech_dir;
	// string placement_result_file_name;
	// string wcon_gds_dir;
	string in_offset_file_name;
	string in_gds_file_prefix;
	// string in_ll_ur_file_name;
	string in_simple_techfile_name;
	string in_spacing_rules_file_name;
	string in_enclosure_rules_file_name;
	string in_symmetry_file_name;
	string loc_file_name;
	string contour_file_name;
	string contour_offset_file_name;
	string in_well_contact_gds_name;
	vector< Device<data_type> > devices; 
	std::map<string, size_t>	m_vn_ids;
    OptimizerType optimizer_type;
	
	vector< pair<int, string> > layers; 
	map<int, size_t> layer_num2id; 
	vector< vector<int> > spacing_rules; // need to scale by 1000 (gds_scal_fac)
	vector< vector<int> > enclosure_rules; // need to scale by 1000 (gds_scal_fac)
	
	typedef bgi::rtree< rtree_value, bgi::quadratic<16> > rtree_type;
	typedef vector< vector< Loc<data_type> > >			cand_loc_type;
	typedef vector< vector< int > >				cost_type;
	typedef map< pair<size_t, size_t>, int > conflict_cost_type;
	
	typedef pair<size_t, size_t> symmetric_pair_t; // vertex ID
	typedef set< symmetric_pair_t > symmetric_pairs_t;
	typedef set<size_t> self_symmetric_t;
	typedef pair< symmetric_pairs_t, self_symmetric_t > symmetric_group_t;
	
	vector< symmetric_group_t > symmetric_groups;

	GdsCell* wcon_gds_cell;

public:
	void legalize();
	// void align_to_grid(polygon_90_t const&, point_t const&, vector< polygon_90_t >&);
	void align_to_grid_legalize(polygon_90_t const& contour, point_t const& point, Loc<data_type> const& wcontact_loc, set<size_t> const& dset, polygon_90_t& aligned_contour, vector< polygon_90_t >&);
	int	 get_grid_value(int, int);
	void read_contour_file(string const&, vector< polygon_90_t > &);
	void read_contour_offset_file(string const& file_name);
	void read_pts_from_file(vector< point_t >& pts, string const& file_name);
	
	void read_symmetry_file();
	void read_techfile_simple();
	void read_spacing_rules_file();
	void read_enclosure_rules_file();

	void parse_gds_files();
	void parse_gds_file(size_t, string);
	void parse_well_contact_gds_file(string const& file_name);
	void init_locs_and_sizes_from_file(string);
	void read_offset_file();
	// void read_ll_ur_file();
	void init_file_names();
	void init_rtrees(rtree_type&, rtree_type&);
	void assign_devices_to_well(vector< set<size_t> >&, vector< polygon_90_t > const&, rtree_type const&);
	void subtract_and_union(polygon_90_t&, set<size_t> const&);
	void write_contour_to_file(vector< polygon_90_t > const&, string const&);
	void write_contour_to_file_debug(vector< point_t > const&, string const&);
	void get_candidate_locations(cand_loc_type&, cost_type&, conflict_cost_type&, vector< polygon_90_t > const&, vector< set<size_t> > const&, rtree_type const&);
	void decide_well_contact_locations(cand_loc_type const&, cost_type const&, conflict_cost_type const&, vector< Loc<data_type> >&);
	void get_polygon_set_envelop(rectangle_t&, polygon_90_set_t const&);
	void get_polygon_envelop(rectangle_t&, polygon_90_t const&);
	void bloat_polygon(polygon_90_t const&, polygon_90_t&, data_type);
	void get_available_region(polygon_90_t const&, set<size_t> const&, data_type, polygon_90_set_t&);
	int	 calculate_cost(int, int, set<size_t> const&, rtree_type const&);
	int  get_rects_intersect_area(rectangle_t const&, rectangle_t const&);
	/*
	bool get_new_value(int orig_val, int val, int val_a, int val_b, bool incr, rtree_type const&, bool, int&); 
	*/
	bool check_value_legal(int new_val, int val_a, int val_b, rtree_type const& rtree_inside, bool is_horizontal);
	int get_grid_value(int orig_val, int pt_val, int val_a, int val_b, rtree_type const& rtree_inside, bool is_horizontal, bool incr);
	
	void legalize_well(vector< polygon_90_t > const& legalized_contours, vector< set<size_t> > const& devices_in_well, vector< Loc<data_type> > const& wcontact_locs);
	void sort_displacement(rectangle_t const& drect, rectangle_t const& rect, vector<int>& v, vector<size_t>& idx, vector<int> const& spacings);
	void sort_indexes(vector<int> const&v, vector<size_t>& idx_arg);
	void get_overlap_well_edge(rectangle_t const& drect, vector< rectangle_t > const& rects, size_t well_id, size_t device_id, polygon_90_t const& p, vector<dEdge>& edges, point_t const& orig, vector<int> const& spacings);
	void get_overlap_well_edge_new(rectangle_t const& drect, vector< rectangle_t > const& rects, size_t well_id, size_t device_id, polygon_90_t const& p, vector<dEdge>& edges, point_t const& orig, vector<int> const& spacings);
	void get_polygon_edges(polygon_90_t const& p, vector< segment_t >& edges);
	void get_well_edges(vector< dEdge >& edges, polygon_90_t const& p, point_t const& orig, size_t well_id);
	void get_device_edges(vector< dEdge >& device_edges);
	void get_spacings(vector<int>& spacings, size_t device_id);
	void get_spacings_between(vector<int>& spacings, size_t d1, size_t d2);
	void legalize_lp(vector< dEdge > const& well_edges, vector< dEdge > const& device_edges, vector< polygon_90_t > const& legalized_contours, vector<int>& x_values, vector<int>& y_values, vector< pair<int, int> >& well_disp, vector< set<size_t> > const& devices_in_well);
	bool check_devices_in_well(polygon_90_t const& contour, set<size_t> const& dset);
	void write_result_to_file(string out_file_name, vector<int> const& x_values, vector<int> const& y_values);
	void write_wcontact_to_file(vector< Loc<int> > const& wcontacts, string out_file_name);
	void write_wcontact_pin_to_file(vector< Loc<int> > const& wcontacts, string out_file_name);
    void remove_empty_wells(vector< set<size_t> >& devices_in_well, vector< polygon_90_t >& contours);
	GdsParser::GdsDB::GdsCell& write_gds_cell(GdsParser::GdsDB::GdsDB& db, vector<int> const& x_values, vector<int> const& y_values);
	void write_gds_well(GdsCell& cell, vector< polygon_90_t > const& contours);
	void write_gds_well_contact(GdsParser::GdsDB::GdsDB& db, GdsCell& cell, vector< Loc<int> > const& wcontacts);
	void write_gds_guard_ring(GdsParser::GdsDB::GdsDB& db, GdsCell& cell, string out_file_name);
    void rec2poly(std::vector<point_type> &poly, std::vector<point_type> &rec);
};

#endif
