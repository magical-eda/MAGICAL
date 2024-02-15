/**
 * @file Tech.h
 * @brief The placement technology-related data structure
 * @author Keren Zhu
 * @date 09/30/2019
 */

#ifndef IDEAPLACE_TECH_H_
#define IDEAPLACE_TECH_H_

#include <unordered_map>
#include "global/global.h"
#include "util/Vector2D.h"

PROJECT_NAMESPACE_BEGIN

class Tech
{
    public:
        /// @brief default constructor
        explicit Tech() = default;
        /*------------------------------*/ 
        /* Construct the tech           */
        /*------------------------------*/ 
        /// @brief add a gds layer
        /// @param a gds layer ID
        /// @return the placement layer index
        IndexType addGdsLayer(IndexType gdsLayer) 
        {
            IndexType layerIdx = _gdsLayerIdxArray.size();
            _gdsLayerIdxArray.emplace_back(gdsLayer);
            _layerIdxMap[gdsLayer] = layerIdx;
            return layerIdx;
        }
        /// @brief initialize the rule data structures
        void initRuleDataStructure();
        /// @brief set the width rule for a layer
        /// @param first: the layer index
        /// @param second: the width rule for the layer
        void setWidthRule(IndexType layerIdx, LocType width) { _widthRule.at(layerIdx) = width; }
        /// @brief set the area rule for a layer
        /// @param first: the layer index
        /// @param second: the area rule for the layer
        void setAreaRule(IndexType layerIdx, LocType area) { _areaRule.at(layerIdx) = area; }
        /// @brief set the spacing rule for a intra-layer
        /// @param first: the layer 
        void setSpacingRule(IndexType layerIdx, LocType spacing) { _spacingRule.at(layerIdx, layerIdx) = spacing; }
        /// @brief set the spacing for inter-layer case
        /// @param first: the first layer
        /// @param second: the second layer
        /// @param third: the spacing required
        /// For now, just make two layers interchangable
        void setSpacingRule(IndexType layerIdxFirst, IndexType layerIdxSecond, LocType spacing)
        {
            _spacingRule.at(layerIdxFirst, layerIdxSecond) = spacing;
            _spacingRule.at(layerIdxSecond, layerIdxFirst) = spacing;
        }
        /// @brief set the dbu/ database unit
        /// @param the databse unit
        void setDbu(LocType dbu) { _dbu = dbu; }
        /*------------------------------*/ 
        /* Query the tech               */
        /*------------------------------*/ 
        /// @brief get the layer index from GDS techlayer
        /// @param the GDS techlayer ID
        /// @return the corresponding layer index in the placement
        IndexType gdsLayerToLayerIdx(IndexType gdsLayer) const { return _layerIdxMap.at(gdsLayer); }
        /// @brief get whether there is width rule in the given layer
        /// @param the layer index
        /// @return where there is width rule defined in the given layer
        bool hasWidthRule(IndexType layerIdx) const { return _widthRule.at(layerIdx) != -1; }
        /// @brief get the width rule of the given layer
        /// @param the layer index
        /// @return the width rule
        LocType widthRule(IndexType layerIdx) const 
        {
            Assert(this->hasWidthRule(layerIdx));
            return _widthRule.at(layerIdx);
        }
        /// @brief get whether there is area rule defined in the given layer
        /// @param the layer index
        /// @return where there is area rule defined in the given layer
        bool hasAreaRule(IndexType layerIdx) const { return _areaRule.at(layerIdx) != -1; }
        /// @brief get the area rule of the given layer
        /// @param the layer index
        /// @return the area rule
        LocType areaRule(IndexType layerIdx) const 
        {
            Assert(this->hasAreaRule(layerIdx));
            return _areaRule.at(layerIdx);
        }
        /// @brief get whether there is intra spacing rules. The intralayer version
        /// @param first: the layer index 
        /// @return whether there is spacing rule defined
        bool hasSpacingRule(IndexType layerIdxFirst) const
        {
            return _spacingRule.at(layerIdxFirst, layerIdxFirst) != -1;
        }
        /// @brief get the spacing rule of intra-layer
        /// @param the layer index
        /// @return the intra-layer spacing rule
        LocType spacingRule(IndexType layerIdx) const
        {
            Assert(this->hasSpacingRule(layerIdx));
            return _spacingRule.at(layerIdx, layerIdx);
        }
        /// @brief get whether there is intra spacing rules. The interlayer version
        /// @param first: the first layer index 
        /// @param second: the second layer index
        /// @return whether there is spacing rule defined
        bool hasSpacingRule(IndexType layerIdxFirst, IndexType layerIdxSecond) const
        {
            return _spacingRule.at(layerIdxFirst, layerIdxSecond) != -1;
        }
        /// @brief get the inter-spacing rule
        /// @param first: the first layer index
        /// @param second: the second layer index
        /// @return the spacing rule defined
        LocType spacingRule(IndexType layerIdxFirst, IndexType layerIdxSecond) const
        {
            Assert(this->hasSpacingRule(layerIdxFirst, layerIdxSecond));
            return _spacingRule.at(layerIdxFirst, layerIdxSecond);
        }
        /// @brief get the database unit
        /// @return the database unit
        LocType dbu() const { return _dbu; }
        /// @brief get the number of layers
        /// @return the number of layers
        IndexType numLayers() const { return _gdsLayerIdxArray.size(); }
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the map from gds techlayer to IDEAPLACE layer
        /// @return the map from gds techlayer to IDEAPLACE layer
        const std::unordered_map<IndexType, IndexType> & layerIdxMap() const { return _layerIdxMap; }
        /// @brief get the map from gds techlayer to IDEAPLACE layer
        /// @return the map from gds techlayer to IDEAPLACE layer
        std::unordered_map<IndexType, IndexType> & layerIdxMap() { return _layerIdxMap; }
    private:
        LocType _dbu = 1000; ///< 1 um = _dbu database units
        std::unordered_map<IndexType, IndexType> _layerIdxMap; ///< A map from gds layer to IDEAPLACE layer. _layerIdxMap[techlayer] = layer index
        std::vector<IndexType> _gdsLayerIdxArray; ///< The tech layer in GDS. _gdsLayerIdx[idx of layer] = techlayer in GDS
        std::vector<LocType> _widthRule; ///< The width rule of the layer
        std::vector<LocType> _areaRule; ///< The area rule of the layer
        Vector2D<LocType> _spacingRule; ///< The intra/inter-layer spacing rules. _spacingRule[layer1][layer2] = spacing rules btween shapes of layer 1 and layer 2.
};

inline void Tech::initRuleDataStructure()
{
    IndexType numLayers = _gdsLayerIdxArray.size();
    _widthRule.resize(numLayers, -1);
    _areaRule.resize(numLayers, -1);
    _spacingRule.resize(numLayers, numLayers, -1);
}

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_TECH_H_
