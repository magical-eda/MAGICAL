/**
 * @file   Geometry.h
 * @brief  Contains utilities for geometric types, such as type traits, area calculator. 
 * I'm trying to make these setting as similar as Boost.Polygon, which will lead to easier embedding
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _LIMBO_GEOMETRY_GEOMETRY_H
#define _LIMBO_GEOMETRY_GEOMETRY_H

#include <vector>
#include <list>
#include <ostream>

/// @brief namespace for Limbo
namespace limbo 
{
/// @brief namespace for Limbo.Geometry
namespace geometry 
{

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;

/// @enum limbo::geometry::orientation_2d_enum
/// @brief orientation type for lines 
enum orientation_2d_enum
{
	HORIZONTAL = 0, 
	VERTICAL = 1
};

/// @enum limbo::geometry::slicing_orientation_2d
/// @brief orientation type for slicing  
enum slicing_orientation_2d
{
	HORIZONTAL_SLICING = 1,
	VERTICAL_SLICING = 2,
	HOR_VER_SLICING = 3, ///< horizontal/vertical slicing and choose rectangle with larger area every time 
    HOR_VER_SA_SLICING = 4, ///< horizontal/vertical slicing and choose rectangle with smaller area every time 
    HOR_VER_AR_SLICING = 5 ///< horizontal/vertical slicing and choose rectangle with better aspect ratio every time 
};

/// @brief convert enum type of slicing orientation to string 
/// @param slicing_orient slicing orientation 
/// @return string denoting slicing orientation 
inline std::string to_string(slicing_orientation_2d slicing_orient)
{
    switch (slicing_orient)
    {
        case HORIZONTAL_SLICING:
            return "HORIZONTAL_SLICING"; 
        case VERTICAL_SLICING:
            return "VERTICAL_SLICING"; 
        case HOR_VER_SLICING:
            return "HOR_VER_SLICING"; 
        case HOR_VER_SA_SLICING:
            return "HOR_VER_SA_SLICING"; 
        case HOR_VER_AR_SLICING:
            return "HOR_VER_AR_SLICING"; 
        default:
            return "UNKNOWN";
    }
}

/// @enum limbo::geometry::direction_2d
/// @brief direction type for rectangles 
enum direction_2d
{
	LEFT = 0, 
	BOTTOM = 1, 
	RIGHT = 2, 
	TOP = 3
};

/// @enum limbo::geometry::winding_direction
/// @brief winding direction type 
/// CLOCKWISE and COUNTERCLOCKWISE refers to winding direction of a polygon 
enum winding_direction
{
	CLOCKWISE = 0, 
	COUNTERCLOCKWISE = 1, 
	UNKNOWN_WINDING = 2 
};

/// @class limbo::geometry::orientation_2d 
/// @brief a class denoting orientation of lines 
class orientation_2d
{
	public:
        /// @brief constructor 
		orientation_2d() {m_orient = -1;}
        /// @brief constructor
        /// @param ori enum type of orientation 
		orientation_2d(orientation_2d_enum const& ori)
			: m_orient(ori) {}
        /// @brief copy constructor
        /// @param ori an orientation_2d object 
		inline orientation_2d(const orientation_2d& ori) : m_orient(ori.m_orient) {}
        /// @brief assignment 
        /// @param ori an orientation_2d object 
        /// @return reference to the object 
		inline orientation_2d& operator=(const orientation_2d& ori) 
		{m_orient = ori.m_orient; return * this; }
        /// @brief equality operator 
        /// @param rhs object at right hand side 
        /// @return true if they are equal 
		inline bool operator==(orientation_2d const& rhs) const { return (m_orient == rhs.m_orient); }
        /// @brief inequality operator 
        /// @param rhs object at right hand side 
        /// @return true if they are inequal 
		inline bool operator!=(orientation_2d const& rhs) const { return (m_orient != rhs.m_orient); }
        /// @brief convert orientation to integer 
        /// @return integer representation of the orientation 
		inline int to_int() const { return m_orient; }
        /// @brief turn 90 degree 
		inline void turn_90() { m_orient = m_orient^ 1; }
        /// @brief get perpendicular orientation 
        /// @return perpendicular orientation 
		inline orientation_2d get_perpendicular() const 
		{
			orientation_2d retval = *this;
			retval.turn_90();
			return retval;
		}
        /// @brief check whether the orientation is valid 
        /// @return true if it is valid 
		inline bool valid() const {return m_orient == 0 || m_orient == 1;}
        /// @brief less than operator 
        /// @param ori1 orientation 
        /// @param ori2 orientation 
        /// @return true if ori1 is less than ori2 
		friend bool operator<(orientation_2d const& ori1, orientation_2d const& ori2) 
		{return ori1.m_orient < ori2.m_orient;}
	protected:
		int m_orient; ///< enum type of orientation in integer representation 
};

/// @brief type traits for coordinates 
/// @tparam T coordinate type
template <typename T>
struct coordinate_traits; 

/// @brief specialization for default integer type 
template <>
struct coordinate_traits<int> 
{
    /// @nowarn
	typedef int coordinate_type;
	typedef long area_type;

	typedef long manhattan_area_type;
	typedef unsigned long unsigned_area_type;
	typedef long coordinate_difference;

	typedef long coordinate_distance;
    /// @endnowarn
};

/// @class limbo::geometry::point_traits
/// @brief type traits for point 
/// @tparam PointType point type 
template <typename PointType>
struct point_traits 
{
    /// @nowarn
	typedef PointType point_type;
	typedef typename point_type::coordinate_type coordinate_type;
    /// @endnowarn

    /// @brief get coordinate from point 
    /// @param point a point object 
    /// @param orient orientation 
    /// @return coordinate 
	static coordinate_type get(const point_type& point, orientation_2d const& orient) 
	{return point.get(orient);}
    /// @brief set coordinate for point 
    /// @param point a point object 
    /// @param orient orientation 
    /// @param value coordinate value 
	static void set(point_type& point, orientation_2d const& orient, coordinate_type const& value) 
	{point.set(orient, value);}
    /// @brief construct point from coordinates 
    /// @param x coordinate 
    /// @param y coordinate 
    /// @return point object 
	static point_type construct(coordinate_type const& x, coordinate_type const& y) 
	{return point_type(x, y);}
};

/// @class limbo::geometry::rectangle_traits
/// @brief type traits for rectangle 
/// @tparam RectType rectangle type 
template <typename RectType>
struct rectangle_traits 
{
    /// @nowarn
	typedef RectType rectangle_type;
	typedef typename rectangle_type::coordinate_type coordinate_type;
    /// @endnowarn

    /// @brief get coordinate from rectangle 
    /// @param rectangle a rectangle object 
    /// @param direct direction 
    /// @return coordinate 
	static inline coordinate_type get(rectangle_type const& rectangle, direction_2d const& direct) 
	{return rectangle.get(direct);}
    /// @brief set coordinate for rectangle 
    /// @param rectangle a rectangle object 
    /// @param direct direction 
    /// @param v coordinate 
	static inline void set(rectangle_type& rectangle, direction_2d const& direct, coordinate_type const& v)
	{rectangle.set(direct, v);}
    /// @brief construct rectangle from coordinates 
    /// @param xl, yl, xh, yh coordinates 
    /// @return rectangle object 
	static inline rectangle_type construct(coordinate_type const& xl, coordinate_type const& yl, 
			coordinate_type const& xh, coordinate_type const& yh)
	{return rectangle_type(xl, yl, xh, yh);}
};

#if 0
/// @brief type traits for polygon 
template <typename PolygonType>
struct polygon_90_traits 
{
	typedef PolygonType polygon_type;
	typedef typename polygon_type::point_type point_type;
	typedef typename point_type::coordinate_type coordinate_type;
	typedef typename polygon_type::vertex_iterator_type vertex_iterator_type;

	// Get the begin iterator
	static inline vertex_iterator_type begin_vertex(polygon_type const& p)
	{return p.begin_vertex();}

	// Get the end iterator
	static inline vertex_iterator_type end_vertex(polygon_type const& p) 
	{return p.end_vertex();}

	// Get the winding direction of the polygon
	static inline winding_direction winding(polygon_type const&) 
	{return UNKNOWN_WINDING;}
};
#endif 

/// @class limbo::geometry::container_traits
/// @brief type traits for containers such as vector, list, multiset 
/// @tparam ContainerType container type 
template <typename ContainerType>
struct container_traits
{
    /// @nowarn
	typedef ContainerType container_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::const_iterator const_iterator_type;
	typedef typename container_type::iterator iterator_type;
    /// @endnowarn

    /// @brief insert value to container 
	static void insert(container_type& container, value_type const& v) {container.insert(v);}
    /// @brief insert value to container with hint of iterator 
	static void insert(container_type& container, iterator_type it, value_type const& v) {container.insert(it, v);}
    /// @brief erase value from iterator 
	static void erase(container_type& container, value_type const& v) {container.erase(v);}
    /// @brief erase an element from container with iterator 
	static void erase(container_type& container, iterator_type it) {container.erase(it);}
	/// @brief a default construct function is necessary, because std::set needs a compare function, but vector does not 
	template <typename PointCompareType>
	static container_type construct(PointCompareType const& comp)
	{return container_type(comp);}
};

/// @brief partial specialization of @ref limbo::geometry::container_traits for commonly used data structures like std::vector 
/// @tparam T data type 
template <typename T>
struct container_traits<std::vector<T> >
{
    /// @nowarn
	typedef std::vector<T> container_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::const_iterator const_iterator_type;
	typedef typename container_type::iterator iterator_type;
    /// @endnowarn

    /// @brief insert value to container 
	static void insert(container_type& container, value_type const& v) {container.push_back(v);}
    /// @brief insert value to container with hint of iterator 
	static void insert(container_type& container, iterator_type it, value_type const& v) {container.insert(it, v);}
    /// @brief erase an element from container with iterator 
	static void erase(container_type& container, iterator_type it) {container.erase(it);}
	/// @brief a default construct function 
    /// @tparam PointCompareType comparison type for point, not used here 
	template <typename PointCompareType>
	static container_type construct(PointCompareType const&)
	{return container_type();}
};
/// @brief partial specialization of @ref limbo::geometry::container_traits for commonly used data structures like std::list 
/// @tparam T data type 
template <typename T>
struct container_traits<std::list<T> >
{
    /// @nowarn
	typedef std::list<T> container_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::const_iterator const_iterator_type;
	typedef typename container_type::iterator iterator_type;
    /// endnowarn

    /// @brief insert value to container 
	static void insert(container_type& container, value_type const& v) {container.push_back(v);}
    /// @brief insert value to container with hint of iterator 
	static void insert(container_type& container, iterator_type it, value_type const& v) {container.insert(it, v);}
    /// @brief erase an element from container with iterator 
	static void erase(container_type& container, iterator_type it) {container.erase(it);}
	/// @brief a default construct function 
    /// @tparam PointCompareType comparison type for point, not used here 
	template <typename PointCompareType>
	static container_type construct(PointCompareType const&)
	{return container_type();}
};

/// @brief calculate signed area of a polygon, the result is positive if its winding is CLOCKWISE
/// @tparam PointSet point set type 
template <typename PointSet>
inline coordinate_traits<typename point_traits<typename container_traits<PointSet>::value_type>::area_type>
area(PointSet const& vPoint)
{
	typedef typename container_traits<PointSet>::const_iterator_type const_iterator_type;
	typedef typename container_traits<PointSet>::value_type point_type;
	typedef typename point_traits<point_type>::coordinate_type coordinate_type;
	typedef typename coordinate_traits<coordinate_type>::area_type area_type;

	area_type a = 0;
	for (const_iterator_type itCur = vPoint.begin(); itCur != vPoint.end(); ++itCur)
	{
		const_iterator_type itNext = itCur;
		++itNext;
		if (itNext == vPoint.end()) itNext = vPoint.begin();

		a += (area_type)(point_traits<point_type>::x(*itNext) - point_traits<point_type>::x(*itCur)) * 
			(area_type)(point_traits<point_type>::y(*itNext) + point_traits<point_type>::y(*itCur)); 
	}
	return a/2;
}

} // namespace geometry
} // namespace limbo

#endif 
