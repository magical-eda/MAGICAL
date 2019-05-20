/**
 * @file Rtree.h
 * @brief The Boost Rtree implementation
 * @author Keren Zhu
 * @date 03/09/2019
 */

#ifndef AROUTER_RTREE_H_
#define AROUTER_RTREE_H_
#include <boost/version.hpp>
#include <cmath>


#if (BOOST_VERSION/100)%1000 > 55
// this is to fix the problem in boost 1.57.0 (1.55.0 works fine)
// it reports problem to find abs 
namespace boost { namespace polygon {
	using std::abs;
}} // namespace boost // namespace polygon
#endif

#include <boost/cstdint.hpp>
//#include <boost/polygon/polygon.hpp>
#include <boost/geometry.hpp>
// use adapted boost.polygon in boost.geometry, which is compatible to rtree
#include <boost/geometry/geometries/adapted/boost_polygon.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "GeometryAPI.h"
#include "db/NetNode.h"

PROJECT_NAMESPACE_BEGIN

namespace gtl = boost::polygon;
namespace bg = boost::geometry;
namespace bgi = bg::index;
using boost::int32_t;
using boost::int64_t;
using gtl::rectangle_concept;
using gtl::polygon_90_concept;
using gtl::polygon_90_set_concept;
using gtl::point_data;
using gtl::segment_data;
using gtl::rectangle_data;
using gtl::polygon_90_data;
using gtl::polygon_90_set_data;

using namespace gtl::operators;


class Shape 
{
    public:
		/// default constructor 
		Shape() {this->initialize();}
		/// copy constructor
		Shape(Shape const& rhs) {this->copy(rhs);}
		/// assignment 
		Shape& operator=(Shape const& rhs)
		{
			if (this != &rhs)
				this->copy(rhs);
			return *this;
		}
        /// whether should I add virtual here 
        /// a trade-off between safety and memory 
		~Shape() {}

        /// @brief get _shapeID
        /// @return the ID of this shape
        ShapeID  shapeID() const { return _shapeID; }
        /// @brief get _shapeID
        /// @return the ID of this shape
        void setShapeID(ShapeID shapeID) { _shapeID = shapeID; }


    private:
		void initialize()
		{
            _shapeID = ShapeID(INDEX_TYPE_MAX, INDEX_TYPE_MAX);
		}
		void copy(Shape const& rhs)
		{
            _shapeID = rhs.shapeID();
		}


	protected:
        ShapeID _shapeID;
};

/// @class AROUTER::Rectangle
/// @brief the interface of boost rectangle
template <typename T>
class Rectangle : public rectangle_data<T>, public Shape
{
	public:
		typedef T coordinate_type;
		typedef rectangle_data<coordinate_type> base_type;
        typedef Shape shape_base_type;
		typedef rectangle_concept geometry_type; // important 
		typedef point_data<coordinate_type> point_type;
		using typename base_type::interval_type;

		/// default constructor 
		Rectangle() : base_type(), shape_base_type() {}
		Rectangle(interval_type const& hor, interval_type const& ver) : base_type(hor, ver), shape_base_type() {}
		Rectangle(coordinate_type xl, coordinate_type yl, coordinate_type xh, coordinate_type yh) : base_type(xl, yl, xh, yh), shape_base_type() {}
		/// copy constructor
		Rectangle(Rectangle const& rhs) : base_type(rhs), shape_base_type(rhs) {}
		/// assignment 
		Rectangle& operator=(Rectangle const& rhs)
		{
			if (this != &rhs)
			{
				this->base_type::operator=(rhs);
                this->shape_base_type::operator=(rhs);
			}
			return *this;
		}
        /// convertion to std::string 
        operator std::string() const 
        {
            std::ostringstream oss;
            print(oss);
            return oss.str();
        }
        /// whether should I add virtual here 
        /// a trade-off between safety and memory 
		~Rectangle() {}

        void print(std::ostream& os) const 
        {
			os << "(" << gtl::xl(*this) << ", " << gtl::yl(*this) << ", " << gtl::xh(*this) << ", " << gtl::yh(*this) << ")";
        }
		friend std::ostream& operator<<(std::ostream& os, Rectangle const& rhs)
		{
            rhs.print(os);
			return os;
		}

};

PROJECT_NAMESPACE_END

#endif // AROUTER_RTREE_H_
