/**
 * @file ShapeMapRtree.h
 * @brief The Boost Rtree implementation of ShapeMap
 * @author anoynmous anoynmous
 * @date 10/31/2018
 */

#ifndef AROUTER_SHAPEMAP_RTREE_H_
#define AROUTER_SHAPEMAP_RTREE_H_

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

#include "ShapeMapBase.h"
#include "ShapeConstructor.h"
#include "GeometryAPI.h"

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


/// @class AROUTER::ShapeRtree
/// @brief the rtree storing rectangles and ShapeID
class ShapeMapRtree : public ShapeMap
{
    /*------------------------------*/ 
    /* Types                        */
    /*------------------------------*/ 
    public:
        typedef LocType coordinate_type;
        typedef gtl::coordinate_traits<coordinate_type>::manhattan_area_type area_type;
        typedef gtl::coordinate_traits<coordinate_type>::coordinate_difference coordinate_difference;
        typedef rectangle_data<coordinate_type> base_type;
        typedef point_data<coordinate_type> point_type;
        typedef bg::model::segment<point_type> path_type; /// Use geometic model for the segment query
		typedef rectangle_data<coordinate_type> rectangle_query_type;
        typedef Rectangle<coordinate_type> rectangle_type;

        /// rtree
        typedef bgi::rtree<rectangle_type, bgi::anoynmousear<16, 4> > rtree_type;

    /*------------------------------*/ 
    /* Member functions             */
    /*------------------------------*/ 
    public:
        explicit ShapeMapRtree(Database &db, SearchInstruct &instr) : ShapeMap(db, instr) {}
        /// @brief init the misc. things
        void init();
        /*------------------------------*/ 
        /* Public set-up interface      */
        /*------------------------------*/ 
        /// @brief Construct the shape map for the region. Need to be called before any querying. Multiple regions should be able to be maintained by calanoynmousg this function in sequence
        /// @param region: the region that want the shape map to be constructed. layer: the layer want to construct the map
        void constructRegion(const Box<LocType> &region, IndexType layer) override;
        /// @brief Remove a shape from the constructed map. This should be called BEFORE removing the shape from the net!
        /// @param shapeID: the ID of the shape
        /// @return bool: if the shape existing in the map
        bool removeShape(ShapeID shapeID) override;
        /// @brief add a shape to the constructed map. This should be called AFTER removing the shape from the net!
        /// @param shapeID: the ID of the shape
        /// @return boof if successful
        bool insertShape(ShapeID shapeID) override;
        /// @brief rename a shape ID. This should be called BEFORE actually renaming the shapeID
        /// @param the old shapeID and new shapeID
        /// @return if sucessful/ the shape is existing
        bool renameShape(ShapeID oldShapeID, ShapeID newShapeID) override;
        /// @brief clear a region of the constructed map
        /// @param region: the region that was constructed but now want to be cleared
        /// @return if the clear successs
        bool clearRegion(const Box<LocType> &region) override;
        /// @brief clear everything in the constructed map
        void clear() override;
        /// @brief number of layers (in the map)
        IndexType numLayers() const override { return _rtreeArray.size(); }
        /// @brief insert a fixed shape 
        /// @param the layer (in index of all layers), the box representing the shape
        bool insertFixedShape(const Box<LocType> &shape, IndexType allLayerIdx, ShapeID shapeID) override
        {
            rectangle_type rect(shape.xLo(), shape.yLo(), shape.xHi(), shape.yHi());
            rect.setShapeID(shapeID);
            _rtreeArray.at(allLayerIdx).insert(rect);
#if 0
            DBG("%s: insert %s at %d,shapeID %s \n", __FUNCTION__, shape.toStr().c_str(), allLayerIdx, shapeID.toStr().c_str());
            Box<LocType> bottomSearch = Box<LocType>(-750, 11190, 1510, 13300);
            std::vector<ShapeID> queryResults;
            this->queryBox(bottomSearch, 2, queryResults, ShapeID(), ShapeID());
            for (ShapeID shapeID : queryResults)
            {
                DBG("query shapeID: %s \n", shapeID.toStr().c_str());
            }
#endif
            return true;
        }
        /*------------------------------*/ 
        /* Public query interface       */
        /*------------------------------*/ 
        /// @brief query the nearest shape to the input point
        /// @param loc: the point want to start querying. layer: the layer for querying. range: max search range
        ShapeID nearest(const XY<LocType> &loc, IndexType layer, LocType range, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) const override;
        /// @brief query the nearest shape to the input point with hint of the routing direction
        /// @param loc: the point want to start querying. layerL the layer for querying. range: max search range. searchDir: the searching direction; if horizontal, then query horizontally vice versa
        ShapeID nearest(const XY<LocType> &loc, IndexType layer, LocType range, RouteDirType searchDir, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) const override;
        /// @brief search the parallel region along a segment, and find the nearest shape
        /// @param segment: the sengment want to start querying. layer: the layer. range: the search region width
        ShapeID segmentNearest(const ManSegment<LocType> &segment, IndexType layer, LocType range, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) const override;
        /// @brief query a rectangle box region
        /// @param box: the search region. layer: which layer it is on. queryResults: a reference of vector to back_insert the result in
        void queryBox(const Box<LocType> &box, IndexType layer, std::vector<ShapeID> &results, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) override;
        /*------------------------------*/ 
        /* Debug                        */
        /*------------------------------*/ 
        void debugInterface() override { writeGds("debug/rtreeDebug.gds"); }
        /// @brief write all the shapes in a gds
        /// @param filename: the name of the output gds file name
        bool writeGds(const std::string &filename);
    private:
        /// @brief add the pin shapes into the shape map
        void initPinShapes();
    /*------------------------------*/ 
    /* Member variables             */
    /*------------------------------*/ 
    private:
        std::vector<rtree_type> _rtreeArray; ///< each rtree for each layer
};


PROJECT_NAMESPACE_END

#endif ///AROUTER_SHAPEMAP_RTREE_H_
