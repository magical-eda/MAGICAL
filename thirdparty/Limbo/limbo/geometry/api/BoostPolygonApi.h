/**
 * @file   BoostPolygonApi.h
 * @brief  Geometry traits for Boost.Polygon, include this file when you use Boost.Polygon geometric types 
 *
 * if your point class setting is different from that in the default point_traits, please create a specialization
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _LIMBO_GEOMETRY_API_BOOSTPOLYGONAPI_H
#define _LIMBO_GEOMETRY_API_BOOSTPOLYGONAPI_H

#include <limbo/geometry/Geometry.h>

/// @brief namespace for Limbo
namespace limbo 
{ 
/// @brief namespace for Limbo.Geometry
namespace geometry 
{

/// @brief specialization of @ref limbo::geometry::point_traits for boost::polygon::point_data
template <typename T>
struct point_traits<boost::polygon::point_data<T> >
{
    /// @nowarn
	typedef T coordinate_type;
	typedef boost::polygon::point_data<T> point_type;
    /// @endnowarn

    /// @brief get coordinate from point 
    /// @param point a point object 
    /// @param orient orientation 
    /// @return coordinate 
	static coordinate_type get(const point_type& point, orientation_2d const& orient) 
	{
		if (orient == HORIZONTAL) return point.x();
		else if (orient == VERTICAL) return point.y();
		else assert(0);
	}
    /// @brief set coordinate for point 
    /// @param point a point object 
    /// @param orient orientation 
    /// @param value coordinate value 
	static void set(point_type& point, orientation_2d const& orient, coordinate_type const& value) 
	{
		if (orient == HORIZONTAL) return point.set(boost::polygon::HORIZONTAL, value);
		else if (orient == VERTICAL) return point.set(boost::polygon::VERTICAL, value);
		else assert(0);
	}
    /// @brief construct point from coordinates 
    /// @param x coordinate 
    /// @param y coordinate 
    /// @return point object 
	static point_type construct(coordinate_type const& x, coordinate_type const& y) 
	{
		return point_type(x, y);
	}
};

/// @brief specialization of @ref limbo::geometry::rectangle_traits for boost::polygon::rectangle_data
template <typename T>
struct rectangle_traits<boost::polygon::rectangle_data<T> >
{
    /// @nowarn
	typedef T coordinate_type;
	typedef boost::polygon::rectangle_data<T> rectangle_type;
    /// @endnowarn

    /// @brief get coordinate from rectangle 
    /// @param rect a rectangle object 
    /// @param dir direction 
    /// @return coordinate 
	static coordinate_type get(const rectangle_type& rect, direction_2d const& dir) 
	{
		switch (dir)
		{
			case LEFT: return boost::polygon::xl(rect);
			case BOTTOM: return boost::polygon::yl(rect);
			case RIGHT: return boost::polygon::xh(rect);
			case TOP: return boost::polygon::yh(rect);
			default: assert(0);
		}
	}
    /// @brief set coordinate for rectangle 
    /// @param rect a rectangle object 
    /// @param dir direction 
    /// @param value coordinate 
	static void set(rectangle_type& rect, direction_2d const& dir, coordinate_type const& value) 
	{
		switch (dir)
		{
			case LEFT: boost::polygon::xl(rect, value); break;
			case BOTTOM: boost::polygon::yl(rect, value); break;
			case RIGHT: boost::polygon::xh(rect, value); break;
			case TOP: boost::polygon::yh(rect, value); break;
			default: assert(0);
		}
	}
    /// @brief construct rectangle from coordinates 
    /// @param xl, yl, xh, yh coordinates 
    /// @return rectangle object 
	static rectangle_type construct(coordinate_type const& xl, coordinate_type const& yl, 
			coordinate_type const& xh, coordinate_type const& yh) 
	{
		return rectangle_type(xl, yl, xh, yh); 
	}
};

} // namespace geometry
} // namespace limbo

#endif 
