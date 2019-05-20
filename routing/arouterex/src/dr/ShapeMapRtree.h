/**
 * @file ShapeMapRtree.h
 * @brief The Boost Rtree implementation of ShapeMap
 * @author Keren Zhu
 * @date 10/31/2018
 */

#ifndef AROUTER_SHAPEMAP_RTREE_H_
#define AROUTER_SHAPEMAP_RTREE_H_

#include "db/RTree.h"

#include "ShapeMapBase.h"
#include "ShapeConstructor.h"

PROJECT_NAMESPACE_BEGIN

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
        typedef bgi::rtree<rectangle_type, bgi::linear<16, 4> > rtree_type;

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
        /// @brief Construct the shape map for the region. Need to be called before any querying. Multiple regions should be able to be maintained by calling this function in sequence
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
