/**
 * @file   bLibApi.h
 * @brief  Geometry traits for bLib, include this file when bLib is used 
 * if your point class setting is different from that in the default point_traits,
 * please create a specialization
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _LIMBO_GEOMETRY_API_BLIBAPI_H
#define _LIMBO_GEOMETRY_API_BLIBAPI_H

#include <limbo/geometry/Geometry.h>

/// @brief namespace for Limbo
namespace limbo 
{ 
/// @brief namespace for Limbo.Geometry
namespace geometry 
{

/// @brief specialization of @ref limbo::geometry::point_traits for bLib::bPoint 
template <>
struct point_traits<bLib::bPoint>
{
    /// @nowarn
	typedef bLib::bPoint point_type;
	typedef int coordinate_type;
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
		if (orient == HORIZONTAL) return point.set(value, point.y());
		else if (orient == VERTICAL) return point.set(point.x(), value);
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

/// @brief specialization of @ref limbo::geometry::rectangle_traits for bLib::bBox 
template <>
struct rectangle_traits<bLib::bBox>
{
    /// @nowarn
	typedef bLib::bBox rectangle_type;
	typedef int coordinate_type;
    /// @endnowarn

    /// @brief get coordinate from rectangle 
    /// @param rect a rectangle object 
    /// @param dir direction 
    /// @return coordinate 
	static coordinate_type get(const rectangle_type& rect, direction_2d const& dir) 
	{
		switch (dir)
		{
			case LEFT: return rect.x1();
			case BOTTOM: return rect.y1();
			case RIGHT: return rect.x2();
			case TOP: return rect.y2();
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
			case LEFT: rect.set(value, rect.y1(), rect.x2(), rect.y2()); break;
			case BOTTOM: rect.set(rect.x1(), value, rect.x2(), rect.y2()); break;
			case RIGHT: rect.set(rect.x1(), rect.y1(), value, rect.y2()); break;
			case TOP: rect.set(rect.x1(), rect.y1(), rect.x2(), value); break;
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

}}// namespace limbo // namespace geometry

#endif 
