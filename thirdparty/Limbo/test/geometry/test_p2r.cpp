/**
 * @file   test_p2r.cpp
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <limbo/geometry/Polygon2Rectangle.h>
using std::cout;
using std::endl;
using std::vector;
using std::list;
using std::set;
using namespace limbo::geometry; 

/**
 * @brief a custom point class 
 */
struct Point
{
    /// coordinate type 
	typedef int value_type;
    /// x coordinate 
	value_type m_x;
    /// y coordinate
	value_type m_y;
    /**
     * @brief access x coordinate
     * @return x coordinate
     */
	value_type const& x() const {return m_x;}
    /**
     * @brief access y coordinate
     * @return y coordinate
     */
	value_type const& y() const {return m_y;}
    /**
     * @brief set x coordinate
     */
	void x(value_type v) {m_x = v;}
    /**
     * @brief set y coordinate
     */
	void y(value_type v) {m_y = v;}

	//bool operator==(Point const& rhs) {return x() == rhs.x() && y() == rhs.y();}
};

namespace limbo { namespace geometry {

/**
 * @brief if your point class setting is different from that in the default point_traits, please create a specialization
 *
 * The specialization should be in the same scope as the original template struct 
 */
template <>
struct point_traits<Point>
{
    /// point type 
	typedef Point point_type;
    /// coordinate type 
	typedef int coordinate_type;

    /**
     * @brief access coordinates 
     * @param point a data point 
     * @param orient orientation can be HORIZONTAL(x) or VERTICAL(y)
     * @return x or y coordinate
     */
	static coordinate_type get(const point_type& point, orientation_2d orient) 
	{
		if (orient == HORIZONTAL) return point.x();
		else if (orient == VERTICAL) return point.y();
		else assert(0);
	}
    /**
     * @brief set coordinates 
     * @param point a data point 
     * @param orient orientation can be HORIZONTAL(x) or VERTICAL(y)
     * @param value data value 
     */
	static void set(point_type& point, orientation_2d orient, coordinate_type value) 
	{
		if (orient == HORIZONTAL) return point.x(value);
		else if (orient == VERTICAL) return point.y(value);
		else assert(0);
	}
    /**
     * @brief construct a point object
     * @param x x coordinate 
     * @param y y coordinate
     * @return the point object
     */
	static point_type construct(coordinate_type x, coordinate_type y) 
	{
		point_type p; 
		p.x(x); p.y(y);
		return p;
	}
};

}}

/**
 * @brief a custom rectangle class 
 */
struct Rectangle 
{
    /// coordinate type 
	typedef int value_type;

    /// left 
	value_type m_xl;
    /// bottom 
	value_type m_yl;
    /// right 
	value_type m_xh;
    /// top
	value_type m_yh;

    /**
     * @brief access left coordinate
     * @return left coordinate 
     */
	value_type const& xl() const {return m_xl;}
    /**
     * @brief access bottom coordinate
     * @return bottom coordinate 
     */
	value_type const& yl() const {return m_yl;}
    /**
     * @brief access right coordinate
     * @return right coordinate 
     */
	value_type const& xh() const {return m_xh;}
    /**
     * @brief access top coordinate
     * @return top coordinate 
     */
	value_type const& yh() const {return m_yh;}
    /**
     * @brief set left coordinate
     * @param v coordinate value
     */
	void xl(value_type v) {m_xl = v;}
    /**
     * @brief set bottom coordinate
     * @param v coordinate value
     */
	void yl(value_type v) {m_yl = v;}
    /**
     * @brief set right coordinate
     * @param v coordinate value
     */
	void xh(value_type v) {m_xh = v;}
    /**
     * @brief set top coordinate
     * @param v coordinate value
     */
	void yh(value_type v) {m_yh = v;}
};

namespace limbo { namespace geometry {

/**
 * @brief if your rectangle class setting is different from that in the default point_traits, please create a specialization
 */
template <>
struct rectangle_traits<Rectangle>
{
    /// rectangle type 
	typedef Rectangle rectangle_type;
    /// coordinate type 
	typedef int coordinate_type;

    /**
     * @brief access coordinates 
     * @param rect a rectangle object
     * @param dir can be LEFT (xl), BOTTOM (yl), RIGHT (xh) or TOP (yh)
     * @return coordinate
     */
	static coordinate_type get(const rectangle_type& rect, direction_2d dir) 
	{
		switch (dir)
		{
			case LEFT: return rect.xl();
			case BOTTOM: return rect.yl();
			case RIGHT: return rect.xh();
			case TOP: return rect.yh();
			default: assert(0);
		}
	}
    /**
     * @brief set coordinates 
     * @param rect a rectangle object
     * @param dir can be LEFT (xl), BOTTOM (yl), RIGHT (xh) or TOP (yh)
     * @param value coordinate value
     */
	static void set(rectangle_type& rect, direction_2d dir, coordinate_type value) 
	{
		switch (dir)
		{
			case LEFT: rect.xl(value); break;
			case BOTTOM: rect.yl(value); break;
			case RIGHT: rect.xh(value); break;
			case TOP: rect.yh(value); break;
			default: assert(0);
		}
	}
    /**
     * @brief construct rectangle 
     * @param xl, yl, xh, yh coordinates of rectangle
     * @return a rectangle object
     */
	static rectangle_type construct(coordinate_type xl, coordinate_type yl, coordinate_type xh, coordinate_type yh) 
	{
		rectangle_type rect; 
		rect.xl(xl); rect.yl(yl);
		rect.xh(xh); rect.yh(yh);
		return rect;
	}
};

}}

/**
 * @brief test polygon-to-rectangle for std::vector
 * @param filename input gnuplot file 
 */
void test1(string const& filename)
{
    vector<Rectangle> vRect; 
	Polygon2Rectangle<vector<Point>, vector<Rectangle> > p2r (vRect, HOR_VER_SLICING);
	assert(p2r.read(filename));
	assert(p2r());
	p2r.print("p2r1.gp");
}

/**
 * @brief test polygon-to-rectangle for std::list
 * @param filename input gnuplot file 
 */
void test2(string const& filename)
{
    vector<Rectangle> vRect; 
	Polygon2Rectangle<list<Point>, vector<Rectangle> > p2r (vRect, HOR_VER_SLICING);
	assert(p2r.read(filename));
	assert(p2r());
	p2r.print("p2r2.gp");
}

/**
 * @brief test polygon-to-rectangle for std::set
 * @param filename input gnuplot file 
 */
void test3(string const& filename)
{
    vector<Rectangle> vRect; 
	Polygon2Rectangle<set<Point, point_compare_type>, vector<Rectangle> > p2r (vRect, HOR_VER_SLICING);
	assert(p2r.read(filename));
	assert(p2r());
	p2r.print("p2r3.gp");
}

/**
 * @brief main function requires an input benchmark in gnuplot format  
 * @param argc number of arguments 
 * @param argv values of arguments 
 * @return 0
 */
int main(int argc, char** argv)
{
	if (argc > 1)
	{
		test1(argv[1]);
		test2(argv[1]);
		test3(argv[1]);
	}
	else cout << "at least 1 argument is required" << endl;

	return 0;
}
