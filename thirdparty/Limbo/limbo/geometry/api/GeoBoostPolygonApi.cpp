/**
 @file   GeoBoostPolygonApi.cpp
 @author Yibo Lin
 @date   Sep 2015
 @brief  A simple API of @ref limbo::geometry::polygon2rectangle using Boost.Polygon API 
 */

// for better potability 
// I do not include from limbo dir 
//#include <limbo/geometry/api/GeoBoostPolygonApi.h>
#include "GeoBoostPolygonApi.h"
#include <limbo/geometry/Polygon2Rectangle.h>
#include <limbo/geometry/api/BoostPolygonApi.h>

namespace limbo { namespace geometry {

namespace gtl = boost::polygon;

bool polygon2RectangleBoost(std::vector<gtl::point_data<int> > const& vPoint, std::vector<gtl::rectangle_data<int> >& vRectangle)
{
    typedef gtl::point_data<int> point_type;

    return limbo::geometry::polygon2rectangle(
                vPoint.begin(), vPoint.end(), 
                //set<yLib::Point, limbo::geometry::point_compare_type>(), 
                vector<point_type>(), 
                //list<yLib::Point>(), 
                vRectangle, 
                HOR_VER_SLICING);
}

bool polygon2RectangleBoost(gtl::polygon_90_data<int> const& polygon, std::vector<gtl::rectangle_data<int> >& vRectangle)
{
    typedef gtl::point_data<int> point_type;

    // after experiment, I found use vector as container is fastest
    // for s.gds, set takes 22s, vector takes 2.5s, and list takes 16s
    return limbo::geometry::polygon2rectangle(
                polygon.begin(), polygon.end(), 
                //set<yLib::Point, limbo::geometry::point_compare_type>(), 
                vector<point_type>(), 
                //list<yLib::Point>(), 
                vRectangle, 
                HOR_VER_SLICING);
}

}} // namespace limbo // namespace geometry

