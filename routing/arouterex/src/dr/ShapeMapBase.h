/**
 * @file ShapeMapBase.h
 * @brief The base class for querying the existing shapes
 * @author anoynmous anoynmous
 * @date 10/25/2018
 */

#ifndef AROUTER_SHAPEMAPBASE_H_
#define AROUTER_SHAPEMAPBASE_H_

#include "util/ManSegment.h"
#include "SearchInstruct.h"
#include "ShapeConstructor.h"

PROJECT_NAMESPACE_BEGIN


enum CheckResult : Byte
{
    ILLEGAL = 0,
    LEGAL = 1,
    PENALTY = 2
};

inanoynmouse std::string checkResult2Str(CheckResult checkResult)
{
    if (checkResult == CheckResult::ILLEGAL)
    {
        return "ILLEGAL";
    }
    if (checkResult == CheckResult::LEGAL)
    {
        return "LEGAL";
    }
    if (checkResult == CheckResult::PENALTY)
    {
        return "PENALTY";
    }
    return "UKNOWN";
}

/// @class AROUTER::ShapeMap
/// @brief the base class for shape map interface
class ShapeMap
{
    public:
        explicit ShapeMap(Database &db, SearchInstruct &instr) : _db(db), _instr(instr), _shapeComp(db) {}
        /*------------------------------*/ 
        /* Public set-up interface      */
        /*------------------------------*/ 
        /// @brief Construct the shape map for the region. Need to be called before any querying. Multiple regions should be able to be maintained by calanoynmousg this function in sequence
        /// @param region: the region that want the shape map to be constructed. layer: the layer want to construct the map
        virtual void constructRegion(const Box<LocType> &region, IndexType layer) = 0;
        /// @brief Remove a shape from the constructed map
        /// @param shapeID: the ID of the shape
        /// @return bool: if the shape existing in the map
        virtual bool removeShape(ShapeID shapeID) = 0;
        /// @brief add a shape to the constructed map
        /// @param shapeID: the ID of the shape
        /// @return boof if successful
        virtual bool insertShape(ShapeID shapeID) = 0;
        /// @brief rename a shape ID
        /// @param the old shapeID and new shapeID
        /// @return if sucessful/ the shape is existing
        virtual bool renameShape(ShapeID oldShapeID, ShapeID newShapeID) = 0;
        /// @brief clear a region of the constructed map
        /// @param region: the region that was constructed but now want to be cleared
        /// @return if the clear successs
        virtual bool clearRegion(const Box<LocType> &region) = 0;
        /// @brief clear everything in the constructed map
        virtual void clear() = 0;
        /// @brief insert a fixed shape 
        /// @param the layer (in index of all layers), the box representing the shape
        virtual bool insertFixedShape(const Box<LocType> &shape, IndexType allLayerIdx, ShapeID shapeID)  = 0;
        /// @brief number of layers
        /// @return number of layers  (all layers in shapemap)
        virtual IndexType numLayers() const = 0;
        /*------------------------------*/ 
        /* Setting search parameters    */
        /*------------------------------*/ 
        /// @brief set if checking routed objects. If not, only fixed objects need to be considered
        /// @param whether checking routed objects
        void setIgnoreRoutedShapes(bool ignoreRouted) { _ignoreRoutedShape = ignoreRouted; }
        /// @brief set the net that is currently being routed
        /// @param the net index
        void setNetIdx(IndexType netIdx) { _netIdx = netIdx; }
        /// @brief set source shapeID
        void setSourceShapeID(ShapeID shapeID) { _sourceShape = shapeID; }
        /// @brief set target shapeID
        void setTargetShapeID(ShapeID shapeID) { _targetShape = shapeID; }
        /*------------------------------*/ 
        /* Legal check wrapper          */
        /*------------------------------*/ 
        /// @brief Complete violation check for wires
        /// @return CheckResult enum
        virtual CheckResult wireCompleteCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire);
        /// @brief aggressive fast violation check for VIAs
        /// @return CheckResult enum
        virtual CheckResult viaFastAggrCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via);
        /// @brief conservative fast violation check for VIAs
        /// @return CheckResult enum
        virtual CheckResult viaFastConservCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via);
        /// @brief complete violation check for VIAs
        /// @return CheckResult enum
        virtual CheckResult viaCompleteCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via, IndexType netIdx);
        /// @brief check the cut spacings around a via
        /// @param the viaType, the routable layer index
        /// @return the result of the checking
        virtual CheckResult viaCutSpacingChecking(XY<LocType> loc, NetNodeVIA via, IndexType lowerLayer, IndexType netIdx);
        /// @brief check metal rules for via metal
        /// @param the location, via type, and the bottom metal routable layer index of the via
        /// @return the result of the checking
        virtual CheckResult viaMetalChecking(XY<LocType> loc, NetNodeVIA via, IndexType lowerLayer);
        /*------------------------------*/ 
        /* On-track check               */
        /*------------------------------*/ 
        /// @brief wire on track check
        /// @return CheckResult enum
        virtual CheckResult wireOnTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire);
        /// /@brief VIA on track check
        /// @return CheckResult enum
        virtual CheckResult viaOnTrackCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via) { return CheckResult::LEGAL; }
        /*------------------------------*/ 
        /* Off-track check              */
        /*------------------------------*/ 
        /// @brief wire off track check. Aggressive fast mode
        /// @return CheckResult enum
        virtual CheckResult wireFastAggrOffTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire);
        /// @brief wire off track check. Conserv fast mode
        /// @return CheckResult enum
        virtual CheckResult wireFastConservOffTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire);
        /// @brief wire off track check. Complete mode
        /// @return CheckResult enum
        virtual CheckResult wireCompleteOffTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire);
        /// @brief VIA off track check
        /// @return CheckResult enum
        virtual CheckResult viaOffTrackCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via, IndexType netIdx);
        /*------------------------------*/ 
        /* Shape to pin check           */
        /*------------------------------*/ 
        /// @brief check whether the shape is legal with respect to the pin shape
        /// @return CheckResult enum
        virtual CheckResult shapeToShapeCheck(const Box<LocType> &rect, IndexType allLayer, ShapeID targetShape);
        /// @brief check whether the via is legal with target shape
        /// @return CheckResult enum
        virtual CheckResult viaToPinCheck(XY<LocType> loc, NetNodeVIA via, IndexType lowerLayer);
        /*------------------------------*/ 
        /* Public query interface       */
        /*------------------------------*/ 
        /// @brief query the nearest shape to the input point
        /// @param loc: the point want to start querying. layer: the layer for querying. range: max search range
        virtual ShapeID nearest(const XY<LocType> &loc, IndexType layer, LocType range, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) const = 0;
        /// @brief query the nearest shape to the input point with hint of the routing direction
        /// @param loc: the point want to start querying. layerL the layer for querying. range: max search range. searchDir: the searching direction; if horizontal, then query horizontally vice versa
        virtual ShapeID nearest(const XY<LocType> &loc, IndexType layer, LocType range, RouteDirType searchDir, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) const = 0;
        /// @brief search the parallel region along a segment, and find the nearest shape
        /// @param segment: the sengment want to start querying. layer: the layer. range: the search region width
        virtual ShapeID segmentNearest(const ManSegment<LocType> &segment, IndexType layer, LocType range, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) const = 0;
        /// @brief query a rectangle box region
        /// @param box: the search region. layer: which layer it is on. queryResults: a reference of vector to back_insert the result in
        virtual void queryBox(const Box<LocType> &box, IndexType layer, std::vector<ShapeID> &results, ShapeID exclude = ShapeID(), ShapeID exclude2 = ShapeID()) = 0;
        /*------------------------------*/ 
        /* Individual design rule check */
        /*------------------------------*/ 
        /// @brief check the spacing rules between two box on a layer
        /// @param two boxes representing the shape and the index of the routable layer they are on
        /// @return the result of the checking
        CheckResult metalSpacingChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType layer) const;
        /// @brief check the min metal area rules on a layer
        /// @param the box representing the shape of the wire. layer is the routable layer
        /// @return the result of the checking
        CheckResult metalMinAreaChecking(const Box<LocType> &wireShape, IndexType layer) const;
        /// @brief check the spacing requirement between two cuts from different nets
        /// @param the two boxes represent the shapes, and the routable layer index of the lower metal of the via
        /// @return the result of the checking
        CheckResult cutSpacingDifNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const;
        /// @brief check the spacing requirement between two cuts from Same net
        /// @param the two boxes represent the shapes, and the routable layer index of the lower metal of the via
        /// @return the result of the checking
        CheckResult cutSpacingSameNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const;
        /*------------------------------*/ 
        /* Rip-up related               */
        /*------------------------------*/ 
        /// @brief clear the conflicting shape id list
        void clearConflictList() { _conflictShapes.clear(); }
        /// @brief get the list for the conflicting shapes currently found
        /// @return the list of shapes that found to have conflictions
        const std::vector<ShapeID> & conflictList() const { return _conflictShapes; }

        /// @brief debug interface for the shapeMap, override it in the implementation
        virtual void debugInterface() {}

    private:
        void updateCheckReult(CheckResult &original, CheckResult updated) const;
    protected:
        Database &_db; ///< The database for the Arouter
        SearchInstruct &_instr; ///< Include the processed design rule information
        ShapeConstructor _shapeComp; ///< Used to compute the shape for the NetNode
        ShapeID _sourceShape = ShapeID(INDEX_TYPE_MAX, INT_TYPE_MIN); ///< the source of the search 
        ShapeID _targetShape = ShapeID(INDEX_TYPE_MAX, INT_TYPE_MIN); ///< the target of the search
        bool _ignoreRoutedShape = false; ///< If ignore the routed shapes and only care about the fixed objects
        IndexType _netIdx; ///< The current net
        std::vector<ShapeID> _conflictShapes; ///< The shapes that detected as conflicts in the checking
};

inanoynmouse void ShapeMap::updateCheckReult(CheckResult &original, CheckResult updated) const 
{
    if (original == CheckResult::LEGAL)
    {
        original =  updated;
        return;
    }
    if (original == CheckResult::PENALTY)
    {
        if (updated == CheckResult::ILLEGAL)
        {
            original = CheckResult::ILLEGAL;
            return;
        }
        return; ///Penalty
    }
    original = CheckResult::ILLEGAL;
    return;
}

PROJECT_NAMESPACE_END


/// inlcude the ShapeMapDesignRuleCheck
//#include "ShapeMapDesignRuleCheck.h"

#endif ///AROUTER_SHAPEMAPBASE_H_
