/*************************************************************************
    > File Name: WellGen.h
    > Author: anoynmous anoynmous
    > Mail: anoynmous@anoynmous
    > Created Time: Tue 06 Nov 2018 09:43:00 PM CST
 ************************************************************************/

#ifndef __WELLGEN_H_INCLUDED__
#define __WELLGEN_H_INCLUDED__

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <boost/geometry/geometries/adapted/boost_polygon.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using GdsParser::GdsDB::GdsCell;
using GdsParser::GdsDB::GdsObject;
using GdsParser::GdsDB::GdsPolygon;
using GdsParser::GdsDB::GdsRectangle;
using GdsParser::GdsDB::GdsCellReference;

namespace gtl = boost::polygon;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace trans = boost::geometry::strategy::transform;
typedef GdsPolygon::base_ext_type	polygon_type;
typedef GdsPolygon::coordinate_type coordinate_type;
typedef GdsPolygon::point_type		point_type;
typedef gtl::rectangle_data<int>	rectangle_t;
typedef gtl::polygon_90_data<int>	polygon_90_t;
typedef gtl::polygon_90_set_data<int>	polygon_90_set_t;
typedef gtl::polygon_traits<polygon_90_t>::point_type point_t;
typedef std::pair<rectangle_t, size_t> rtree_value;

class mCoveredBy {
	polygon_90_t m_polygon;
public:
	mCoveredBy(polygon_90_t const& p) : m_polygon(p) {}
	bool operator()(rtree_value const& v) const {
		/*
		polygon_90_t vp;
		int xmin = gtl::xl(v.first), ymin = gtl::yl(v.first), xmax = gtl::xh(v.first), ymax = gtl::yh(v.first);
		point_t pts[5] = {
			point_t(xmin, ymin), point_t(xmin, ymax), point_t(xmax, ymax), point_t(xmax, ymin), point_t(xmin, ymin)
		};
		gtl::set_points(vp, pts, pts+5);
		return bg::covered_by(vp, m_polygon);
		*/
		// NOTE: since adapted polygon is not working, I just use heuristics to decide coverage
		int xmin = gtl::xl(v.first), ymin = gtl::yl(v.first), xmax = gtl::xh(v.first), ymax = gtl::yh(v.first);
		point_t pts[4] = {
			point_t(xmin, ymin), point_t(xmin, ymax), point_t(xmax, ymax), point_t(xmax, ymin)
		};
		bool contain_flag = true;
		for(size_t i=0; i<4; ++i) {
			if( !gtl::contains(m_polygon, pts[i]) ) {
				contain_flag = false;
			}
		}
		return contain_flag;
	}
};

class WellGen {
public:
	string cell_name;
	string gds_file_name;
	string out_gds_file_name;
	string train_gds_file_name;

	// size_t img_size = 5000; // a clip size is 5x5 micron
	// size_t num_pixel = 256; // image resolution is 256x256

	std::map< int, vector<polygon_90_t > > polygons;
	std::map< int, polygon_90_set_t > polygon_sets;

public:
	void parse_gds_file();
	void process_polygons(GdsCell&);
	
};

#endif
