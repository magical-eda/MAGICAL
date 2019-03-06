//
// Created by anoynmous on 09/19/2018
//

#include "FixedViaSelector.h"

PROJECT_NAMESPACE_BEGIN

/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
/// Public
/// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 

//////////////////
/// Init
//////////////////

bool FixedViaSelector::init()
{
    /// Init arrays
    IndexType accessLayerSize = _db.macroDB().fixedViaVec2D().size() - 1; /// The last is Ap, typically no via from AP to higher, so size - 1
    _minAreaArray.resize(accessLayerSize);
    _minWidthArray.resize(accessLayerSize);
    _minHeightArray.resize(accessLayerSize);
    _minResMinAreaArray.resize(accessLayerSize);
    _minResMinWidthArray.resize(accessLayerSize);
    _minResMinHeightArray.resize(accessLayerSize);

    /// init the selected vias
    for (IndexType layerIdx = 0; layerIdx < accessLayerSize; ++ layerIdx)
    {
        if (!initMinArea(layerIdx)) { return false; }
        if (!initMinWidth(layerIdx)) { return false; }
        if (!initMinHeight(layerIdx)) { return false; }
        if (!initMinResMinArea(layerIdx)) { return false; }
        if (!initMinResMinWidth(layerIdx)) { return false; }
        if (!initMinResMinHeight(layerIdx)) { return false; }
    }
    return true;
}


/// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 
/// Private
/// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 

IndexType FixedViaSelector::findSmallestVia(IndexType layerIdx, std::function<bool(const LefFixedVia &, const LefFixedVia &)> comp)
{
    AssertMsg(_db.macroDB().fixedViaVec2D().at(layerIdx).size()>0, "%s: Access Layer %d %s has no fixed via \n", __FUNCTION__, layerIdx, _db.techDB().accessLayerName(layerIdx).c_str());

    /// Then o(n) bruteforcely find the layer wanted
    auto & fixedViaArray = _db.macroDB().fixedViaVec2D().at(layerIdx);
    IndexType optViaIdx = 0; /// the index of optimal via
    for (IndexType viaIdx = 1; viaIdx < fixedViaArray.size(); ++viaIdx)
    {
        if (!comp(fixedViaArray.at(optViaIdx), fixedViaArray.at(viaIdx)))
        {
            optViaIdx = viaIdx;
        }
    }
    return optViaIdx;
}

FixedViaSelected FixedViaSelector::buildSelect(IndexType layerIdx, IndexType viaIdx)
{
    FixedViaSelected built;
    built.setViaType(viaIdx);
    return built;
}

//////////////////////////////
/// Init each individual types
//////////////////////////////
bool FixedViaSelector::initMinArea(IndexType layerIdx)
{
    auto func = [&] (const LefFixedVia lhs, const LefFixedVia rhs) 
    {
        if (lhs.isDefault() && !rhs.isDefault()) { return true; }
        if (!lhs.isDefault() && rhs.isDefault()) { return false; }
        if (lhs.cutRectArray().size() == 1 && rhs.cutRectArray().size() > 1) { return true; }
        if (lhs.cutRectArray().size() > 1 && rhs.cutRectArray().size() == 1) { return false; }
        return lhs.boundingBox().area() <rhs.boundingBox().area();
    };
    
    /// Find the optimal via
    IndexType viaIdx = findSmallestVia(layerIdx, func);
    _minAreaArray.at(layerIdx) = buildSelect(layerIdx, viaIdx);

    /// Check whether only multiple cut via exist
    auto & fixedViaArray = _db.macroDB().fixedViaVec2D().at(layerIdx);
    const auto &via = fixedViaArray.at(viaIdx);
    if (via.cutRectArray().size() > 1)
    {
        WRN("%s: layer %d does not has cut == 1 case \n", __FUNCTION__, layerIdx);
    }
    return true;
}

bool FixedViaSelector::initMinWidth(IndexType layerIdx)
{
    auto func = [&] (const LefFixedVia lhs, const LefFixedVia rhs) 
    {
        if (lhs.isDefault() && !rhs.isDefault()) { return true; }
        if (!lhs.isDefault() && rhs.isDefault()) { return false; }
        if (lhs.cutRectArray().size() == 1 && rhs.cutRectArray().size() > 1) { return true; }
        if (lhs.cutRectArray().size() > 1 && rhs.cutRectArray().size() == 1) { return false; }
        return lhs.boundingBox().xLen() <rhs.boundingBox().xLen();
    };
    
    /// Find the optimal via
    IndexType viaIdx = findSmallestVia(layerIdx, func);
    _minWidthArray.at(layerIdx) = buildSelect(layerIdx, viaIdx);
    return true;
}

bool FixedViaSelector::initMinHeight(IndexType layerIdx)
{
    auto func = [&] (const LefFixedVia lhs, const LefFixedVia rhs) 
    {
        if (lhs.isDefault() && !rhs.isDefault()) { return true; }
        if (!lhs.isDefault() && rhs.isDefault()) { return false; }
        if (lhs.cutRectArray().size() == 1 && rhs.cutRectArray().size() > 1) { return true; }
        if (lhs.cutRectArray().size() > 1 && rhs.cutRectArray().size() == 1) { return false; }
        return lhs.boundingBox().yLen() <rhs.boundingBox().yLen();
    };
    
    /// Find the optimal via
    IndexType viaIdx = findSmallestVia(layerIdx, func);
    _minHeightArray.at(layerIdx) = buildSelect(layerIdx, viaIdx);
    return true;
}

bool FixedViaSelector::initMinResMinArea(IndexType layerIdx)
{
    auto func = [&] (const LefFixedVia lhs, const LefFixedVia rhs)
    {
        if (lhs.isDefault() && !rhs.isDefault()) { return true; }
        if (!lhs.isDefault() && rhs.isDefault()) { return false; }
        if (lhs.hasResistance() && rhs.hasResistance())
        {
            /// Resistance has been converted to int, directly compare them
            if (lhs.resistance() != rhs.resistance())
            {
                return lhs.resistance() < rhs.resistance();
            }
        }
        /// either no resistance, or have equal resistance
        return lhs.boundingBox().area() < rhs.boundingBox().area();
    };
    
    /// Find the optimal via
    IndexType viaIdx = findSmallestVia(layerIdx, func);
    _minResMinAreaArray.at(layerIdx) = buildSelect(layerIdx, viaIdx);
    return true;
}

bool FixedViaSelector::initMinResMinWidth(IndexType layerIdx)
{
    auto func = [&] (const LefFixedVia lhs, const LefFixedVia rhs)
    {
        if (lhs.isDefault() && !rhs.isDefault()) { return true; }
        if (!lhs.isDefault() && rhs.isDefault()) { return false; }
        if (lhs.hasResistance() && rhs.hasResistance())
        {
            /// Resistance has been converted to int, directly compare them
            if (lhs.resistance() != rhs.resistance())
            {
                return lhs.resistance() < rhs.resistance();
            }
        }
        /// either no resistance, or have equal resistance
        return lhs.boundingBox().xLen() < rhs.boundingBox().xLen();
    };
    
    /// Find the optimal via
    IndexType viaIdx = findSmallestVia(layerIdx, func);
    _minResMinWidthArray.at(layerIdx) = buildSelect(layerIdx, viaIdx);
    return true;
}

bool FixedViaSelector::initMinResMinHeight(IndexType layerIdx)
{
    auto func = [&] (const LefFixedVia lhs, const LefFixedVia rhs)
    {
        if (lhs.isDefault() && !rhs.isDefault()) { return true; }
        if (!lhs.isDefault() && rhs.isDefault()) { return false; }
        if (lhs.hasResistance() && rhs.hasResistance())
        {
            /// Resistance has been converted to int, directly compare them
            if (lhs.resistance() != rhs.resistance())
            {
                return lhs.resistance() < rhs.resistance();
            }
        }
        /// either no resistance, or have equal resistance
        return lhs.boundingBox().yLen() < rhs.boundingBox().yLen();
    };
    
    /// Find the optimal via
    IndexType viaIdx = findSmallestVia(layerIdx, func);
    _minResMinHeightArray.at(layerIdx) = buildSelect(layerIdx, viaIdx);
    return true;
}

PROJECT_NAMESPACE_END
