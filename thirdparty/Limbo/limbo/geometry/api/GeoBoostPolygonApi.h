/**
 * @file   GeoBoostPolygonApi.h
 * @brief  this file extracts polygon-to-rectangle conversion for Boost.Polygon API. 
 * @author Yibo Lin
 * @date   Sep 2015
 */

#ifndef LIMBO_GEOMETRY_GEOBOOSTPOLYGONAPI_H
#define LIMBO_GEOMETRY_GEOBOOSTPOLYGONAPI_H

#include <boost/polygon/polygon.hpp>

/// @brief namespace for Limbo
namespace limbo 
{ 
/// @brief namespace for Limbo.Geometry
namespace geometry 
{

namespace gtl = boost::polygon;

/// @brief this function takes a set of points describing a rectilinear polygon and decomposes it into rectangles 
/// @param vPoint a set of points as input 
/// @param vRectangle a set of rectangles as output 
/// @return true if succeeded 
bool polygon2RectangleBoost(std::vector<gtl::point_data<int> > const& vPoint, std::vector<gtl::rectangle_data<int> >& vRectangle);
/// @brief this function takes a rectilinear polygon and decomposes it into rectangles 
/// @param polygon rectilinear polygon as input 
/// @param vRectangle a set of rectangles as output 
/// @return true if succeeded 
bool polygon2RectangleBoost(gtl::polygon_90_data<int> const& polygon, std::vector<gtl::rectangle_data<int> >& vRectangle);

} // namespace geometry
} // namespace limbo

#endif
