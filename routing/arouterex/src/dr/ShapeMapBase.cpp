#include "ShapeMapBase.h"

PROJECT_NAMESPACE_BEGIN

CheckResult ShapeMap::shapeToShapeCheck(const Box<LocType> &rect, IndexType allLayer, ShapeID targetShape)
{
    CheckResult checkResult = CheckResult::LEGAL;
    /// Compute the pin shape
    _shapeComp.computeShape(targetShape);
    
    for (const auto &boxLayer : _shapeComp.result())
    {
        if (allLayer == boxLayer.second)
        {
            /// On the same shape
        }
        if (::klib::idxDif(allLayer, boxLayer.second) == 1)
        {
            /// One is cut one is metal
            /// Should I add to the conflict list?
            if (rect.overlap(boxLayer.first))
            {
                if ( !rect.cover(boxLayer.first) && !boxLayer.first.cover(rect))
                {
                    checkResult = CheckResult::ILLEGAL;
                    _conflictShapes.emplace_back(targetShape);
                }
            }
            
        }
    }
    return checkResult;
}

CheckResult ShapeMap::viaToPinCheck(XY<LocType> loc, NetNodeVIA via, IndexType lowerLayer)
{
    return CheckResult::LEGAL;
    auto bottomMetal = _shapeComp.computeViaTopMetalShape(loc, lowerLayer, via);
    auto cutBBox = _shapeComp.computeViaCutBBox(loc, lowerLayer, via);
    auto topMetal = _shapeComp.computeViaTopMetalShape(loc, lowerLayer, via);
    CheckResult checkResult = CheckResult::LEGAL;
    //updateCheckReult(checkResult, this->shapeToShapeCheck(bottomMetal, DRUtil::routeLayer2AllLayer(lowerLayer), _sourceShape));
    //updateCheckReult(checkResult, this->shapeToShapeCheck(bottomMetal, DRUtil::routeLayer2AllLayer(lowerLayer), _targetShape));
    updateCheckReult(checkResult, this->shapeToShapeCheck(cutBBox, DRUtil::routeLayer2CutLayer(lowerLayer), _sourceShape));
    updateCheckReult(checkResult, this->shapeToShapeCheck(cutBBox, DRUtil::routeLayer2CutLayer(lowerLayer), _targetShape));
    //updateCheckReult(checkResult, this->shapeToShapeCheck(topMetal, DRUtil::routeLayer2AllLayer(lowerLayer + 1), _sourceShape));
    //updateCheckReult(checkResult, this->shapeToShapeCheck(topMetal, DRUtil::routeLayer2AllLayer(lowerLayer + 1), _targetShape));
    if (loc == XY<LocType>(52380, 21820) && lowerLayer == 2)
    {
        DBG("%s: result %s layer %d \n", __FUNCTION__, checkResult2Str(checkResult).c_str(), lowerLayer);
        DBG("bottom %s cut %s, topMetal %s \n", bottomMetal.toStr().c_str(), cutBBox.toStr().c_str(), topMetal.toStr().c_str());
        exit(0);
    }
    return checkResult;
}

CheckResult ShapeMap::wireFastAggrOffTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire)
{
    if (!_sourceShape.valid())
    {
        WRN("%s: must set source shape ID to make the check making sense if routing from a existing shape! \n");
    }
    /// Get the parallel shape search range
    LocType searchRange = _instr.wireParallelNearestAggrRange(wire, layer);
    /// Construct a segment
    ManSegment<LocType> seg(from, to);
    /// Extend the two ends of the segment
    seg.setBegin(seg.begin() - searchRange);
    seg.setEnd(seg.end() + searchRange);
    /// enlarge the searchRange to take consideration of the wire width
    searchRange += wire.halfWidth(); 
    /// Search the parallel region
    ShapeID nearest = segmentNearest(ManSegment<LocType>(from, to), DRUtil::routeLayer2AllLayer(layer), searchRange, _sourceShape, _targetShape);

    /// Shouldn't have any shape excluding the source and target in the search region
    /// Overly constrained?
    if (nearest.valid())
    {
        _conflictShapes.emplace_back(nearest);
        return CheckResult::ILLEGAL;
    }
    return CheckResult::LEGAL;
}

CheckResult ShapeMap::wireFastConservOffTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire)
{
    WRN("%s: return the result of aggressive mode \n", __FUNCTION__);
    return wireFastAggrOffTrackCheck(from, to, layer, wire);
}

CheckResult ShapeMap::wireCompleteOffTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire)
{
    CheckResult checkResult = CheckResult::LEGAL;
    std::vector<ShapeID> queryResults;
    /// Check the spacing rules
    Box<LocType> path = _shapeComp.computeWireShape(from, to, wire); ///< The shape of the input wire
    // Dummy
    this->queryBox(path, this->numLayers() - 1, queryResults);
    if (queryResults.size() > 0)
    {
        return CheckResult::ILLEGAL;
    }
    /// Parallel spacing checking
    /// Get the largest spacing with the possible parallel run length
    LocType searchRange = _instr.wireParallelNearestAggrRange(wire, layer);
    /// Enlarge the searchRange to take consideration of the wire width
    searchRange += wire.halfWidth();
    Box<LocType> searchBox;
    if (from.x() == to.x())
    {
        /// Vertical
        searchBox = Box<LocType>(from.x() - searchRange, std::min(from.y(), to.y()), from.x() + searchRange, std::max(from.y(), to.y()));
        AssertMsg(from.y() != to.y(), "from %s to %s, layer %d \n", from.toStr().c_str(), to.toStr().c_str(), layer);
    }
    else 
    {
        /// horizontal
        Assert(from.y() == to.y());
        searchBox = Box<LocType>(std::min(from.x(), to.x()), from.y() - searchRange, std::max(from.x(), to.x()), from.y() + searchRange);
    }
    this->queryBox(searchBox, DRUtil::routeLayer2AllLayer(layer), queryResults, _sourceShape, _targetShape);

    /// Check the spacing rule between each box
    for (ShapeID shapeID : queryResults)
    {
#if 0
        if (to == XY<LocType>(380, 12078) && layer == 1)
        {
            DBG("%s: from %s to %s shape %s \n", __FUNCTION__, from.toStr().c_str(), to.toStr().c_str(), shapeID.toStr().c_str());
        }
#endif
        /// Skip off track spacing check if the shapes are on-track
        if (_shapeComp.onTrack(shapeID, from, layer, to, layer))
        {
            /// Make sure the ontrack shapes are on the same net
            //if (shapeID.net() != _netIdx || shapeID == _sourceShape || shapeID == _targetShape)
            if (shapeID.isRouteShape())
            {
                if (shapeID.net() == _netIdx)
                {
                    continue;
                }
            }
            else if (shapeID.isTerminal())
            {
                if (shapeID == _sourceShape || shapeID == _targetShape)
                {
                    continue;
                }
            }
        }
        _shapeComp.computeShape(shapeID);
        for (const auto &rect : _shapeComp.result())
        {
            /// Possibly that the shapeID is a via, also constraint that only check the shapes on the same layer
            if (rect.second == DRUtil::routeLayer2AllLayer(layer))
            {
                CheckResult spacingCheck = this->metalSpacingChecking(path, rect.first, layer);
                if (spacingCheck == CheckResult::ILLEGAL)
                {
                    _conflictShapes.emplace_back(shapeID);
                }
                this->updateCheckReult(checkResult, spacingCheck);
            }
        }
        //if (checkResult == CheckResult::ILLEGAL) { return CheckResult::ILLEGAL; }
    }

    /// TODO: end of anoynmouse checks

    return checkResult;

}

CheckResult ShapeMap::viaFastAggrCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via)
{
    /// Just check whether there is shapes in the spacing range of via
    CheckResult checkResult = CheckResult::LEGAL; 
    /// Bottom and top metal
    /// Check the spacing between the bottom/top metals with other metals, excluding the on-track metals
    Box<LocType> bottomMetal = _shapeComp.computeViaBottomMetalShape(loc, lowerLayer, via);
    Box<LocType> topMetal = _shapeComp.computeViaTopMetalShape(loc, lowerLayer, via);
    /// Calculate the needed query range
    LocType searchRange = _instr.metalMinSpacing(lowerLayer);
    Box<LocType> bottomSearch = bottomMetal;
    bottomSearch.enlargeBy(searchRange);
    searchRange = _instr.metalMinSpacing(lowerLayer + 1);
    Box<LocType> topSearch = topMetal;
    topSearch.enlargeBy(searchRange);
    /// Query the bottom
    std::vector<ShapeID> queryResults;
    this->queryBox(bottomSearch, DRUtil::routeLayer2AllLayer(lowerLayer), queryResults, _sourceShape, _targetShape);
    /// Just make sure no shapes nearby
    if (queryResults.size() > 0) 
    { 
        for (const auto &shape : queryResults)
        {
            /// If they are on track, then they are connected shapes
            /// Should not be declared as illegal here
            if (!_shapeComp.onTrack(shape, loc, lowerLayer, loc, lowerLayer + 1))
            {
                _conflictShapes.emplace_back(shape);
                checkResult = CheckResult::ILLEGAL;
            }
            else
            {
                /// Make sure the ontrack shapes are on the same net
                if (shape.net() != _netIdx)
                {
                    _conflictShapes.emplace_back(shape);
                    checkResult = CheckResult::ILLEGAL;
                    //return CheckResult::ILLEGAL;
                }
            }
        }
    }
    /// Query the top
    queryResults.clear();
    //this->queryBox(topSearch, DRUtil::routeLayer2AllLayer(lowerLayer + 1), queryResults, _sourceShape, _targetShape);
    /// Just make sure no shapes nearby
    if (queryResults.size() > 0) 
    { 
        for (const auto &shape : queryResults)
        {
            /// If they are on track, then they are connected shapes
            /// Should not be declared as illegal here
            if (!_shapeComp.onTrack(shape, loc, lowerLayer, loc, lowerLayer + 1))
            {
                _conflictShapes.emplace_back(shape);
                checkResult = CheckResult::ILLEGAL;
                //return CheckResult::ILLEGAL; 
            }
            else
            {
                /// Make sure the ontrack shapes are on the same net
                if (shape.net() != _netIdx)
                {
                    _conflictShapes.emplace_back(shape);
                    checkResult = CheckResult::ILLEGAL;
                    //return CheckResult::ILLEGAL;
                }
            }
        }
    }


    /// Cut
    /// Get the conservative search range for the cut spacing
    searchRange  = _instr.cutSpacing(lowerLayer);
    Box<LocType> cutBBox = _shapeComp.computeViaCutBBox(loc, lowerLayer, via);
    /// Enlarge the cut bounding box as the query range
    cutBBox.enlargeBy(searchRange);
    /// calculate the target cut layer in all layers
    IndexType cutLayerIdx = DRUtil::routeLayer2AllLayer(lowerLayer) + 1;
    /// Query the cut shapes
    queryResults.clear();
    this->queryBox(cutBBox, cutLayerIdx, queryResults, _sourceShape, _targetShape);
    /// Just make sure no shapes nearby
    if (queryResults.size() > 0) 
    { 
        checkResult = CheckResult::ILLEGAL; 
        for (auto shape : queryResults)
        {
            _conflictShapes.emplace_back(shape);
        }
    }

    return checkResult;
}

CheckResult ShapeMap::viaFastConservCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via)
{
    /// Just check whether there is shapes in the spacing range of via
    CheckResult checkResult = CheckResult::LEGAL; 
    /// Bottom and top metal
    /// Check the spacing between the bottom/top metals with other metals, excluding the on-track metals
    Box<LocType> bottomMetal = _shapeComp.computeViaBottomMetalShape(loc, lowerLayer, via);
    Box<LocType> topMetal = _shapeComp.computeViaTopMetalShape(loc, lowerLayer, via);
    /// Calculate the needed query range
    LocType searchRange = _instr.metalLargestSpacing(lowerLayer);
    Box<LocType> bottomSearch = bottomMetal;
    bottomSearch.enlargeBy(searchRange);
    searchRange = _instr.metalLargestSpacing(lowerLayer + 1);
    Box<LocType> topSearch = topMetal;
    topSearch.enlargeBy(searchRange);
    /// Query the bottom
    std::vector<ShapeID> queryResults;
    this->queryBox(bottomSearch, DRUtil::routeLayer2AllLayer(lowerLayer), queryResults, _sourceShape, _targetShape);
    /// Just make sure no shapes nearby
    if (queryResults.size() > 0) 
    { 
        for (const auto &shape : queryResults)
        {
            /// If they are on track, then they are connected shapes
            /// Should not be declared as illegal here
            if (!_shapeComp.onTrack(shape, loc, lowerLayer, loc, lowerLayer + 1))
            {
                _conflictShapes.emplace_back(shape);
                checkResult = CheckResult::ILLEGAL;
                //return CheckResult::ILLEGAL; 
            }
        }
    }
    /// Query the top
    queryResults.clear();
    this->queryBox(topSearch, DRUtil::routeLayer2AllLayer(lowerLayer + 1), queryResults, _sourceShape, _targetShape);
    /// Just make sure no shapes nearby
    if (queryResults.size() > 0) 
    { 
        for (const auto &shape : queryResults)
        {
            /// If they are on track, then they are connected shapes
            /// Should not be declared as illegal here
            if (!_shapeComp.onTrack(shape, loc, lowerLayer, loc, lowerLayer + 1))
            {
                _conflictShapes.emplace_back(shape);
                checkResult = CheckResult::ILLEGAL;
            }
            else
            {
                /// Make sure the ontrack shapes are on the same net
                if (shape.net() != _netIdx)
                {
                    _conflictShapes.emplace_back(shape);
                    checkResult = CheckResult::ILLEGAL;
                }
            }
        }
    }


    /// Cut
    /// Get the conservative search range for the cut spacing
    searchRange  = _instr.cutConservativeSpacing(lowerLayer);
    Box<LocType> cutBBox = _shapeComp.computeViaCutBBox(loc, lowerLayer, via);
    /// Enlarge the cut bounding box as the query range
    cutBBox.enlargeBy(searchRange);
    /// calculate the target cut layer in all layers
    IndexType cutLayerIdx = DRUtil::routeLayer2AllLayer(lowerLayer) + 1;
    /// Query the cut shapes
    queryResults.clear();
    this->queryBox(cutBBox, cutLayerIdx, queryResults, _sourceShape, _targetShape);
    /// Just make sure no shapes nearby
    if (queryResults.size() > 0) 
    { 
        checkResult = CheckResult::ILLEGAL; 
        for (auto shape : queryResults)
        {
            _conflictShapes.emplace_back(shape);
        }
    }

    return checkResult;
}

CheckResult ShapeMap::wireCompleteCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire)
{
    CheckResult checkResult = CheckResult::LEGAL;

    updateCheckReult(checkResult, this->wireCompleteOffTrackCheck(from, to, layer, wire));
    if (to == XY<LocType>(380, 12078) && layer == 1)
    {
        //DBG("%s From %s \n", __FUNCTION__, from.toStr().c_str());
        //DBG("%s: offtrack check %s \n", __FUNCTION__, checkResult2Str(this->wireCompleteOffTrackCheck(from, to, layer, wire)).c_str());
    }
    updateCheckReult(checkResult, this->wireOnTrackCheck(from, to, layer, wire));
#if 0
    if (to == XY<LocType>(380, 12078) && layer == 1)
    {
        DBG("%s: ontrack check %s \n", __FUNCTION__, checkResult2Str(this->wireOnTrackCheck(from, to, layer, wire)).c_str());
    }
#endif


    return checkResult;
}

CheckResult ShapeMap::viaCompleteCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via, IndexType netIdx)
{
    CheckResult checkResult = CheckResult::LEGAL;

    updateCheckReult(checkResult, this->viaOnTrackCheck(loc, lowerLayer, via));
    updateCheckReult(checkResult, this->viaOffTrackCheck(loc, lowerLayer, via, netIdx));
    updateCheckReult(checkResult, this->viaToPinCheck(loc, via, lowerLayer));

    return checkResult;
}

CheckResult ShapeMap::viaOffTrackCheck(XY<LocType> loc, IndexType lowerLayer, NetNodeVIA via, IndexType netIdx)
{
    CheckResult checkResult = CheckResult::LEGAL;
    /// Need to check 1. off track metal rules. 2. cut spacing
    
    /// Check the cut layer spacing
    updateCheckReult(checkResult, this->viaCutSpacingChecking(loc, via, lowerLayer, netIdx));

#if 0
    if (loc == XY<LocType>(380, 12078) && lowerLayer == 1)
    {
        DBG("%s: location %s layer %d \n", __FUNCTION__, loc.toStr().c_str(), lowerLayer);
        DBG("%s: cut check check %s \n", __FUNCTION__, checkResult2Str(checkResult).c_str());
    }
#endif
    /// Check the via metals
    updateCheckReult(checkResult, this->viaMetalChecking(loc, via, lowerLayer));
    
#if 0
    if (loc == XY<LocType>(380, 12078) && lowerLayer == 1)
    {
        DBG("%s: metal check check %s \n", __FUNCTION__, checkResult2Str(this->viaMetalChecking(loc, via, lowerLayer)).c_str());
    }
#endif
    return checkResult;
}


CheckResult ShapeMap::viaCutSpacingChecking(XY<LocType> loc, NetNodeVIA via, IndexType lowerLayer, IndexType netIdx)
{
    CheckResult checkResult = CheckResult::LEGAL;
    /// First check cut spacing
    /// Get the conservative search range for the cut spacing
    LocType searchRange  = _instr.cutConservativeSpacing(lowerLayer);
    Box<LocType> cutBBox = _shapeComp.computeViaCutBBox(loc, lowerLayer, via);
    /// Enlarge the cut bounding box as the query range
    cutBBox.enlargeBy(searchRange);
    /// calculate the target cut layer in all layers
    IndexType cutLayerIdx = DRUtil::routeLayer2AllLayer(lowerLayer) + 1;
    /// Query the cut shapes
    std::vector<ShapeID> queryResults;
    this->queryBox(cutBBox, cutLayerIdx, queryResults, _sourceShape, _targetShape);
    if (_db.macroDB().viaMacro(lowerLayer, via.viaType()).cutRectArray().size() == 1)
    {
        Box<LocType> cutBox = _db.macroDB().viaMacro(lowerLayer, via.viaType()).cutRectArray().front();
        cutBox.offsetBy(loc);
        for (ShapeID shapeID : queryResults)
        {
            /// Only check offTrack spacing
            if (_shapeComp.onTrack(shapeID, loc, lowerLayer, loc, lowerLayer + 1))
            {
                /// Make sure the ontrack shapes are on the same net
                //if (shapeID.net() != _netIdx || shapeID == _sourceShape || shapeID == _targetShape)
                if (shapeID.isRouteShape())
                {
                    if (shapeID.net() == _netIdx)
                    {
                        continue;
                    }
                }
                else if (shapeID.isTerminal())
                {
                    if (shapeID == _sourceShape || shapeID == _targetShape)
                    {
                        continue;
                    }
                }
            }
            if (shapeID.isRouteShape())
            {
                std::vector<Box<LocType>> rects;
                _shapeComp.computeViaCutShape(shapeID, rects);
                if (shapeID.net() == netIdx)
                {
                    for (const auto &otherCut : rects)
                    {
                        /// Check same net rule
                        auto sameNetCheck = this->cutSpacingSameNetChecking(cutBox, otherCut, lowerLayer);
                        if (sameNetCheck == CheckResult::ILLEGAL)
                        {
                            _conflictShapes.emplace_back(shapeID);
                        }
                        updateCheckReult(checkResult, sameNetCheck);
                    }
                }
                else 
                {
                    for (const auto &otherCut : rects)
                    {
                        /// Check different net rule
                        auto cutCheck = this->cutSpacingDifNetChecking(cutBox, otherCut, lowerLayer);
                        if (cutCheck == CheckResult::ILLEGAL)
                        {
                            _conflictShapes.emplace_back(shapeID);
                        }
                        updateCheckReult(checkResult, cutCheck);
                    }
                }
            }
            else
            {
                _shapeComp.computeFixedShape(shapeID);
                for (auto & rect :  _shapeComp.result())
                {
                    if (rect.second == cutLayerIdx)
                    {
                        auto cutCheck = this->cutSpacingDifNetChecking(cutBox, rect.first, lowerLayer);
                        if (cutCheck == CheckResult::ILLEGAL)
                        {
                            _conflictShapes.emplace_back(shapeID);
                        }
                        updateCheckReult(checkResult, cutCheck);
                    }
                }
            }
        }
    }
    else
    {
        /// TODO:
        ERR("%s: The cases of multiple cut shapes have not been implemented! \n", __FUNCTION__);
        return CheckResult::LEGAL;
    }

    return checkResult;
}

CheckResult ShapeMap::viaMetalChecking(XY<LocType> loc, NetNodeVIA via, IndexType lowerLayer)
{
    //DBG("%s: check %s, %d, via %d \n", __FUNCTION__, loc.toStr().c_str(), lowerLayer, via.viaType());
    CheckResult checkResult = CheckResult::LEGAL;
    /// Check the spacing between the bottom/top metals with other metals, excluding the on-track metals
    Box<LocType> bottomMetal = _shapeComp.computeViaBottomMetalShape(loc, lowerLayer, via);
    Box<LocType> topMetal = _shapeComp.computeViaTopMetalShape(loc, lowerLayer, via);

    /// Query the bottom
    std::vector<ShapeID> queryResults;
    // Dummy blockage
    this->queryBox(bottomMetal, this->numLayers() - 1, queryResults);
    if (queryResults.size() > 0)
    {
        return CheckResult::ILLEGAL;
    }
    this->queryBox(topMetal, this->numLayers() - 1, queryResults);
    if (queryResults.size() > 0)
    {
        return CheckResult::ILLEGAL;
    }


    

    /// Calculate the needed query range
    LocType searchRange = _instr.metalLargestSpacing(lowerLayer);
    Box<LocType> bottomSearch = bottomMetal;
    bottomSearch.enlargeBy(searchRange);
    //DBG("%s: bottom search %s \n", __FUNCTION__, bottomSearch.toStr().c_str());

    searchRange = _instr.metalLargestSpacing(lowerLayer + 1);
    Box<LocType> topSearch = topMetal;
    topSearch.enlargeBy(searchRange);

    this->queryBox(bottomSearch, DRUtil::routeLayer2AllLayer(lowerLayer), queryResults, _sourceShape, _targetShape);
    //DBG("at layer %d, source %s target %s \n", DRUtil::routeLayer2AllLayer(lowerLayer), _sourceShape.toStr().c_str(), _targetShape.toStr().c_str());

    /// Check the spacing rule between each box
    for (ShapeID shapeID : queryResults)
    {
        /// Only check offTrack spacing
        if (_shapeComp.onTrack(shapeID, loc, lowerLayer, loc, lowerLayer + 1))
        {
            /// Make sure the ontrack shapes are on the same net
            //if (shapeID.net() != _netIdx || shapeID == _sourceShape || shapeID == _targetShape)
            if (shapeID.isRouteShape())
            {
                if (shapeID.net() == _netIdx)
                {
                    continue;
                }
            }
            else if (shapeID.isTerminal())
            {
                if (shapeID == _sourceShape || shapeID == _targetShape)
                {
                    continue;
                }
            }
        }
        _shapeComp.computeShape(shapeID);
        for (const auto &rect : _shapeComp.result())
        {
            /// Possibly that the shapeID is a via, also constraint that only check the shapes on the same layer
            if (rect.second == DRUtil::routeLayer2AllLayer(lowerLayer))
            {
                auto metalCheck = this->metalSpacingChecking(bottomMetal, rect.first, lowerLayer);
                if (metalCheck == CheckResult::ILLEGAL)
                {
                    _conflictShapes.emplace_back(shapeID);
                }
                this->updateCheckReult(checkResult, metalCheck);
            }
        }
        //if (checkResult == CheckResult::ILLEGAL) { return CheckResult::ILLEGAL; }
    }

    /// Query the top
    queryResults.clear();
    this->queryBox(topSearch, DRUtil::routeLayer2AllLayer(lowerLayer + 1), queryResults, _sourceShape, _targetShape);

    /// Check the spacing rule between each box
    for (ShapeID shapeID : queryResults)
    {
        /// Only check offTrack spacing
        if (_shapeComp.onTrack(shapeID, loc, lowerLayer, loc, lowerLayer + 1))
        {
            /// Make sure the ontrack shapes are on the same net
            //if (shapeID.net() != _netIdx || shapeID == _sourceShape || shapeID == _targetShape)
            if (shapeID.isRouteShape())
            {
                if (shapeID.net() == _netIdx)
                {
                    continue;
                }
            }
            else if (shapeID.isTerminal())
            {
                if (shapeID == _sourceShape || shapeID == _targetShape)
                {
                    continue;
                }
            }
        }
        _shapeComp.computeShape(shapeID);
        for (const auto &rect : _shapeComp.result())
        {
            /// Possibly that the shapeID is a via, also constraint that only check the shapes on the same layer
            if (rect.second == DRUtil::routeLayer2AllLayer(lowerLayer + 1))
            {
                auto metalCheck = this->metalSpacingChecking(topMetal, rect.first, lowerLayer + 1);
                if (metalCheck == CheckResult::ILLEGAL)
                {
                    _conflictShapes.emplace_back(shapeID);
                }
                this->updateCheckReult(checkResult, metalCheck);
            }
        }
        //if (checkResult == CheckResult::ILLEGAL) { return CheckResult::ILLEGAL; }
    }

    /// TODO: EOL

    return checkResult;
}

CheckResult ShapeMap::wireOnTrackCheck(XY<LocType> from, XY<LocType> to, IndexType layer, NetNodeWire wire)
{
    CheckResult checkResult = CheckResult::LEGAL;
    /// Check min area rule
    Box<LocType> wireShape = _shapeComp.computeWireShape(from, to, wire); ///< The shape of the input wire
    updateCheckReult(checkResult, this->metalMinAreaChecking(wireShape, layer));

    return checkResult;
}

CheckResult ShapeMap::cutSpacingDifNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const 
{
    CheckResult checkResult = CheckResult::LEGAL;
    LocType spacing = klib::boxSpacing(lhs, rhs);
    if (klib::boxAreParallelOverlap(lhs, rhs))
    {
        if (spacing < _instr.cutParallelOverlapSpacings(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    else
    {
        if (spacing < _instr.cutSpacing(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    return checkResult;
}

CheckResult ShapeMap::cutSpacingSameNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const 
{
    CheckResult checkResult = CheckResult::LEGAL;
    LocType spacing = klib::boxSpacing(lhs, rhs);
    if (klib::boxAreParallelOverlap(lhs, rhs))
    {
        if (spacing < _instr.cutParallelOverlapSpacings(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    else
    {
        if (spacing < _instr.cutSameNetSpacing(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    return checkResult;
}

CheckResult ShapeMap::metalSpacingChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType layer) const
{
    /// Calculate the geometric information of two rects
    auto bundle = DRUtil::spacingCheckBundle(lhs, rhs);
    LocType spacing = bundle.first.first;
    LocType parallel = bundle.first.second;
    LocType width = std::max(bundle.second.first, bundle.second.second);

    /// Calculate the required spacing between two boxes
    LocType spacingRule = _instr.metalSpacing(width, parallel, layer);


    if (spacing < spacingRule) { return CheckResult::ILLEGAL; }
    return CheckResult::LEGAL;
}

CheckResult ShapeMap::metalMinAreaChecking(const Box<LocType> &wireShape, IndexType layer) const
{
    /// Fetch the required area
    LocType minArea = _instr.metalMinArea(layer);
    
    /// Calculate the rect area
    LocType rectArea = wireShape.area();

    if (rectArea < minArea) { return CheckResult::ILLEGAL; }
    return CheckResult::LEGAL;
}

PROJECT_NAMESPACE_END
