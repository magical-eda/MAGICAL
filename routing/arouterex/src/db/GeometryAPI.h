
/*************************************************************************
    > File Name: GeometryApi.h
    > Author: Yibo Lin
    > Mail: yibolin@utexas.edu
    > Created Time: Tue 18 Aug 2015 11:01:34 PM CDT
 ************************************************************************/

#ifndef SIMPLEMPL_GEOMETRYAPI_H
#define SIMPLEMPL_GEOMETRYAPI_H

// for Polygon2Rectangle
#include <limbo/geometry/api/BoostPolygonApi.h>
#include <limbo/geometry/Polygon2Rectangle.h>
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

template <typename T> class Rectangle;
template <typename T> class Polygon;

PROJECT_NAMESPACE_END

/// API for Boost.Geometry 
namespace boost { namespace geometry { namespace index {

template <typename Box>
struct indexable< boost::shared_ptr<Box> >
{
    typedef boost::shared_ptr<Box> V;

    typedef Box const& result_type;
    result_type operator()(V const& v) const { return *v; }
};

template <typename Box>
struct indexable< Box* >
{
    typedef Box* V;

    typedef Box const& result_type;
    result_type operator()(V const& v) const { return *v; }
};

}}} // namespace boost // namespace geometry // namespace index

namespace boost { namespace geometry { namespace traits {

//////// for rectangles ////////
template <typename CoordinateType>
struct tag<PROJECT_NAMESPACE::Rectangle<CoordinateType> > : public tag<typename PROJECT_NAMESPACE::Rectangle<CoordinateType>::base_type>
{};


template <typename CoordinateType>
struct point_type<PROJECT_NAMESPACE::Rectangle<CoordinateType> >
{
    typedef typename PROJECT_NAMESPACE::Rectangle<CoordinateType>::point_type type;
};


template <typename CoordinateType>
struct indexed_access
<
    PROJECT_NAMESPACE::Rectangle<CoordinateType>,
    min_corner, 0
> : public indexed_access<typename PROJECT_NAMESPACE::Rectangle<CoordinateType>::base_type, min_corner, 0>
{};


template <typename CoordinateType>
struct indexed_access
<
    PROJECT_NAMESPACE::Rectangle<CoordinateType>,
    min_corner, 1
> : public indexed_access<typename PROJECT_NAMESPACE::Rectangle<CoordinateType>::base_type, min_corner, 1>
{};


template <typename CoordinateType>
struct indexed_access
<
    PROJECT_NAMESPACE::Rectangle<CoordinateType>,
    max_corner, 0
> : public indexed_access<typename PROJECT_NAMESPACE::Rectangle<CoordinateType>::base_type, max_corner, 0>
{};


template <typename CoordinateType>
struct indexed_access
<
    PROJECT_NAMESPACE::Rectangle<CoordinateType>,
    max_corner, 1
> : public indexed_access<typename PROJECT_NAMESPACE::Rectangle<CoordinateType>::base_type, max_corner, 1>
{};

}}} // namespace boost // namespace geometry // namespace traits

/// API for Boost.Polygon
namespace boost { namespace polygon {

/// necessary for customized rectangle types 
template <typename T>
struct geometry_concept<PROJECT_NAMESPACE::Rectangle<T> > 
{
	typedef rectangle_concept type;
};

/// bug in boost library in the following function
/// function intersects and intersect do not always return the same results (when consider_touch = false)
/// create a specialization to resolve it 
  template <typename T>
  typename enable_if< typename gtl_and_3<y_r_b_intersect3, typename is_mutable_rectangle_concept<typename geometry_concept<PROJECT_NAMESPACE::Rectangle<T> >::type>::type,
                                         typename is_rectangle_concept<typename geometry_concept<PROJECT_NAMESPACE::Rectangle<T> >::type>::type>::type,
                       bool>::type
  intersect(PROJECT_NAMESPACE::Rectangle<T>& rectangle, const PROJECT_NAMESPACE::Rectangle<T>& b, bool consider_touch = true) {
	  // the original version is "intersects(rectangle, b)" without consider_touch 
    if(intersects(rectangle, b, consider_touch)) {
      intersect(rectangle, horizontal(b), HORIZONTAL, consider_touch);
      intersect(rectangle, vertical(b), VERTICAL, consider_touch);
      return true;
    }
    return false;
  }

/// bug in boost library
  template <typename T>
  typename enable_if< typename gtl_and_3<y_r_edist2, typename is_rectangle_concept<typename geometry_concept<PROJECT_NAMESPACE::Rectangle<T> >::type>::type,
                                                          typename is_rectangle_concept<typename geometry_concept<PROJECT_NAMESPACE::Rectangle<T> >::type>::type>::type,
                       typename rectangle_distance_type<PROJECT_NAMESPACE::Rectangle<T> >::type>::type
  euclidean_distance(const PROJECT_NAMESPACE::Rectangle<T> & lvalue, const PROJECT_NAMESPACE::Rectangle<T>& rvalue) {
    typename rectangle_distance_type<PROJECT_NAMESPACE::Rectangle<T> >::type val = square_euclidean_distance(lvalue, rvalue); // originally the result is cast to int, which causes overflow 
    return std::sqrt(val);
  }

}} // namespace boost // namespace polygon

/// API for limbo::geometry
namespace limbo { namespace geometry {

/// \brief specialization for PROJECT_NAMESPACE::Rectangle
template <typename T>
struct rectangle_traits<PROJECT_NAMESPACE::Rectangle<T> >// : public rectangle_traits<boost::polygon::rectangle_data<T> >
{
	typedef T coordinate_type;
	typedef PROJECT_NAMESPACE::Rectangle<coordinate_type> rectangle_type;

	static coordinate_type get(const typename rectangle_type::base_type& rect, direction_2d const& dir) 
	{
		switch (dir)
		{
			case LEFT: return boost::polygon::xl(rect);
			case BOTTOM: return boost::polygon::yl(rect);
			case RIGHT: return boost::polygon::xh(rect);
			case TOP: return boost::polygon::yh(rect);
			default: assert_msg(0, "unknown direction_2d type");
		}
	}
	static void set(typename rectangle_type::base_type& rect, direction_2d const& dir, coordinate_type const& value) 
	{
		switch (dir)
		{
			case LEFT: boost::polygon::xl(rect, value); break;
			case BOTTOM: boost::polygon::yl(rect, value); break;
			case RIGHT: boost::polygon::xh(rect, value); break;
			case TOP: boost::polygon::yh(rect, value); break;
			default: assert_msg(0, "unknown direction_2d type");
		}
	}
	static rectangle_type construct(coordinate_type const& xl, coordinate_type const& yl, 
			coordinate_type const& xh, coordinate_type const& yh) 
	{
		return rectangle_type(xl, yl, xh, yh); 
	}
};

}} // namespace limbo // namespace geometry

#endif
