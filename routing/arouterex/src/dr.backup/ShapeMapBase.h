/**
 * @file ShapeMapBase.h
 * @brief The base class for querying the existing shapes
 * @author anoynmous anoynmous
 * @date 10/25/2018
 */

#ifndef AROUTER_SHAPEMAPBASE_H_
#define AROUTER_SHAPEMAPBASE_H_

#include "db/Database.h"
#include "util/ManSegment.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::ShapeID
/// @brief used to identify which segnment this shape belongs to 
struct ShapeID
{
    /// @brief default constructor
    explicit ShapeID() = default;
    /// @brief whether this is a valid shape ID. Could return an invalid shape ID if the query does not have any result
    /// @return if the ShapeID is valid
    bool valid() const { return netIdx != INDEX_TYPE_MAX && nodeIdx != INDEX_TYPE_MAX; }
    IndexType netIdx = INDEX_TYPE_MAX; ///< The net index
    IndexType nodeIdx = INDEX_TYPE_MAX; ///< The node index, the node of the child side of the shape
};

/// @class AROUTER::ShapeMap
/// @brief the base class for shape map interface
class ShapeMap
{
    public:
        explicit ShapeMap(Database &db) : _db(db) {}
        /*------------------------------*/ 
        /* Public set-up interface      */
        /*------------------------------*/ 
        /// @brief Construct the shape map for the region. Need to be called before any querying. Multiple regions should be able to be maintained by calanoynmousg this function in sequence
        /// @param region: the region that want the shape map to be constructed. layer: the layer want to construct the map
        virtual void constructRegion(const Box<LocType> &region, IndexType layer);
        /// @brief Remove a shape from the constructed map
        /// @param shapeID: the ID of the shape
        /// @return bool: if the shape existing in the map
        virtual bool removeShpe(ShapeID shapeID);
        /// @brief add a shape to the constructed map
        /// @param shapeID: the ID of the shape
        /// @return boof if successful
        virtual bool insertShape(ShapeID shapeID);
        /// @brief rename a shape ID
        /// @param the old shapeID and new shapeID
        /// @return if sucessful/ the shape is existing
        virtual bool renameShape(ShapeID oldShapeID, ShapeID newShapeID);
        /// @brief clear a region of the constructed map
        /// @param region: the region that was constructed but now want to be cleared
        /// @return if the clear successs
        virtual bool clearRegion(const Box<LocType> &region);
        /// @brief clear everything in the constructed map
        virtual void clear();
        /*------------------------------*/ 
        /* Public query interface       */
        /*------------------------------*/ 
        /// @brief query the nearest shape to the input point
        /// @param loc: the point want to start querying. layer: the layer for querying 
        virtual ShapeID nearest(const XY<LocType> &loc, IndexType layer) const;
        /// @brief query the nearest shape to the input point with hint of the routing direction
        /// @param loc: the point want to start querying. layerL the layer for querying. searchDir: the searching direction; if horizontal, then query horizontally vice versa
        virtual ShapeID nearest(const XY<LocType> &loc, IndexType layer, RouteDirType searchDir) const;
        /// @brief search the parallel region along a segment, and find the nearest shape
        /// @param segment: the sengment want to start querying. layer: the layer. width: the search region width
        virtual ShapeID segmentNearest(const ManSegment<LocType> &segment, IndexType layer, LocType width) const;
        /// @brief query a rectangle box region
        /// @param box: the search region. layer: which layer it is on. queryResults: a reference of vector to back_insert the result in
        virtual void queryBox(const Box<LocType> &box, IndexType layer, std::vector<ShapeID> &results);
    protected:
        Database &_db; ///< The database for the Arouter
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_SHAPEMAPBASE_H_
