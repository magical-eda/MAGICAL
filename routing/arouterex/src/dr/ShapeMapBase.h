/**
 * @file ShapeMapBase.h
 * @brief The base class for querying the existing shapes
 * @author Keren Zhu
 * @date 10/25/2018
 */

#ifndef AROUTER_SHAPEMAPBASE_H_
#define AROUTER_SHAPEMAPBASE_H_

#include "util/ManSegment.h"
#include "SearchInstruct.h"
#include "ShapeConstructor.h"
#include "DRChecker.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::ShapeMap
/// @brief the base class for shape map interface
class ShapeMap
{
    public:
        explicit ShapeMap(Database &db, SearchInstruct &instr) : _db(db), _instr(instr),  _shapeComp(db), _shapeChecker(instr) {}
        /*------------------------------*/ 
        /* Public set-up interface      */
        /*------------------------------*/ 
        /// @brief Construct the shape map for the region. Need to be called before any querying. Multiple regions should be able to be maintained by calling this function in sequence
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
        /// @brief check the design rule for two wires on the same net
        CheckResult wiresSameNetCheck(XY<LocType> from1, XY<LocType> to1, XY<LocType> from2, XY<LocType> to2, IndexType layer, NetNodeWire wire);
        /// @brief check the design rules for two vias on the same net
        CheckResult viasSameNetCheck(XY<LocType> loc1, XY<LocType> loc2, IndexType lowerLayer, NetNodeVIA via);
        /// @brief check the design rule for two wires on the different nets
        CheckResult wiresDifNetCheck(XY<LocType> from1, XY<LocType> to1, XY<LocType> from2, XY<LocType> to2, IndexType layer, NetNodeWire wire);
        /// @brief check the design rules for two vias on the different nets
        CheckResult viasDifNetCheck(XY<LocType> loc1, XY<LocType> loc2, IndexType lowerLayer, NetNodeVIA via);
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
        /// @brief get the shape-level design rule checker
        const ShapeChecker & shapeRuleChecker() const { return _shapeChecker; }
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
        ShapeChecker _shapeChecker; ///< Used to check the design rules in the contexts of shapes
        ShapeID _sourceShape = ShapeID(INDEX_TYPE_MAX, INT_TYPE_MIN); ///< the source of the search 
        ShapeID _targetShape = ShapeID(INDEX_TYPE_MAX, INT_TYPE_MIN); ///< the target of the search
        bool _ignoreRoutedShape = false; ///< If ignore the routed shapes and only care about the fixed objects
        IndexType _netIdx; ///< The current net
        std::vector<ShapeID> _conflictShapes; ///< The shapes that detected as conflicts in the checking
};

inline void ShapeMap::updateCheckReult(CheckResult &original, CheckResult updated) const 
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

inline CheckResult ShapeMap::wiresSameNetCheck(XY<LocType> from1, XY<LocType> to1, XY<LocType> from2, XY<LocType> to2, IndexType layer, NetNodeWire wire)
{
    auto wire1 = _shapeComp.computeWireShape(from1, to1, wire);
    auto wire2 = _shapeComp.computeWireShape(from2, to2, wire);
    return _shapeChecker.sameNetMetalCheck(wire1, wire2, layer);
}


inline CheckResult ShapeMap::viasSameNetCheck(XY<LocType> loc1, XY<LocType> loc2, IndexType lowerLayer, NetNodeVIA via)
{
    CheckResult checkResult = CheckResult::LEGAL;
    auto bottom1 = _shapeComp.computeViaBottomMetalShape(loc1, lowerLayer, via);
    auto bottom2 = _shapeComp.computeViaBottomMetalShape(loc2, lowerLayer, via);
    updateCheckReult(checkResult, _shapeChecker.sameNetMetalCheck(bottom1, bottom2, lowerLayer));
    if (checkResult == CheckResult::ILLEGAL)
    {
        return checkResult;
    }
    auto top1 = _shapeComp.computeViaTopMetalShape(loc1, lowerLayer, via);
    auto top2 = _shapeComp.computeViaTopMetalShape(loc2, lowerLayer, via);
    updateCheckReult(checkResult, _shapeChecker.sameNetMetalCheck(top1, top2, lowerLayer + 1));
    if (checkResult == CheckResult::ILLEGAL)
    {
        return checkResult;
    }
    /// TODO: realize multi cut check
    auto cutBBox1 = _shapeComp.computeViaCutBBox(loc1, lowerLayer, via);
    auto cutBBox2 = _shapeComp.computeViaCutBBox(loc2, lowerLayer, via);
    updateCheckReult(checkResult, _shapeChecker.cutSpacingSameNetChecking(cutBBox1, cutBBox2, lowerLayer));
    return checkResult;
}

inline CheckResult ShapeMap::wiresDifNetCheck(XY<LocType> from1, XY<LocType> to1, XY<LocType> from2, XY<LocType> to2, IndexType layer, NetNodeWire wire)
{
    auto wire1 = _shapeComp.computeWireShape(from1, to1, wire);
    auto wire2 = _shapeComp.computeWireShape(from2, to2, wire);
    return _shapeChecker.metalSpacingChecking(wire1, wire2, layer);
}


inline CheckResult ShapeMap::viasDifNetCheck(XY<LocType> loc1, XY<LocType> loc2, IndexType lowerLayer, NetNodeVIA via)
{
    CheckResult checkResult = CheckResult::LEGAL;
    auto bottom1 = _shapeComp.computeViaBottomMetalShape(loc1, lowerLayer, via);
    auto bottom2 = _shapeComp.computeViaBottomMetalShape(loc2, lowerLayer, via);
    updateCheckReult(checkResult, _shapeChecker.metalSpacingChecking(bottom1, bottom2, lowerLayer));
    if (checkResult == CheckResult::ILLEGAL)
    {
        return checkResult;
    }
    auto top1 = _shapeComp.computeViaTopMetalShape(loc1, lowerLayer, via);
    auto top2 = _shapeComp.computeViaTopMetalShape(loc2, lowerLayer, via);
    updateCheckReult(checkResult, _shapeChecker.metalSpacingChecking(top1, top2, lowerLayer + 1));
    if (checkResult == CheckResult::ILLEGAL)
    {
        return checkResult;
    }
    /// TODO: realize multi cut check
    auto cutBBox1 = _shapeComp.computeViaCutBBox(loc1, lowerLayer, via);
    auto cutBBox2 = _shapeComp.computeViaCutBBox(loc2, lowerLayer, via);
    updateCheckReult(checkResult, _shapeChecker.cutSpacingDifNetChecking(cutBBox1, cutBBox2, lowerLayer));
    return checkResult;
}
PROJECT_NAMESPACE_END


/// inlcude the ShapeMapDesignRuleCheck
//#include "ShapeMapDesignRuleCheck.h"

#endif ///AROUTER_SHAPEMAPBASE_H_
