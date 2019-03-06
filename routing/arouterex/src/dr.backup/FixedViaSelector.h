//
// Created by anoynmous on 09/19/2018
//

#ifndef AROUTER_FIXEDVIASELECTOR_H_
#define AROUTER_FIXEDVIASELECTOR_H_

#include <functional> // std::function
#include "db/Database.h"

/// ================================================================================ 
/// FixedViaSelector.h
///
/// Preprocess the available fixed cells from input.
/// Provide the interface for selecting the available vias on different layer
///
/// ================================================================================ 


PROJECT_NAMESPACE_BEGIN

class FixedViaSelected
{
    public:
        explicit FixedViaSelected() = default; 
        IndexType viaIdx = 0;
};

class FixedViaSelector
{
    public:
        explicit FixedViaSelector(Database &db) : _db(db) {}

        ///////////////////
        /// Getters
        ///////////////////
        const FixedViaSelected & minArea(IndexType layerIdx) const         { return _minAreaArray.at(layerIdx); }
        const FixedViaSelected & minWidth(IndexType layerIdx) const        { return _minWidthArray.at(layerIdx); }
        const FixedViaSelected & minHeight(IndexType layerIdx) const       { return _minHeightArray.at(layerIdx); }
        const FixedViaSelected & minResMinArea(IndexType layerIdx) const   { return _minResMinAreaArray.at(layerIdx); }
        const FixedViaSelected & minResMinWidth(IndexType layerIdx) const  { return _minResMinWidthArray.at(layerIdx); }
        const FixedViaSelected & minResMinHeight(IndexType layerIdx) const { return _minResMinHeightArray.at(layerIdx); }

        ////////////////////
        /// Setup
        ////////////////////
        /// Wrapper for init functions
        bool init();

    private:
        /// Given an bool function as function, find the smallest (with respect to the given comp) and return the corresponding index of the fixedVia in the layer
        IndexType findSmallestVia(IndexType layerIdx, std::function<bool(const LefFixedVia &, const LefFixedVia &)> comp);
        /// Given the index of a fixedVia, build the FixedViaSelected
        FixedViaSelected buildSelect(IndexType layerIdx, IndexType viaIdx);

        /////////////////////
        /// Individual init
        /////////////////////
        /// minArea
        bool initMinArea(IndexType layerIdx);
        bool initMinWidth(IndexType layerIdx);
        bool initMinHeight(IndexType layerIdx);
        bool initMinResMinArea(IndexType layerIdx);
        bool initMinResMinWidth(IndexType layerIdx);
        bool initMinResMinHeight(IndexType layerIdx);
    private:
        Database &_db;
        /// Depending on bounding box of the vias
        std::vector<FixedViaSelected> _minAreaArray;
        std::vector<FixedViaSelected> _minWidthArray;
        std::vector<FixedViaSelected> _minHeightArray;
        std::vector<FixedViaSelected> _minResMinAreaArray;
        std::vector<FixedViaSelected> _minResMinWidthArray;
        std::vector<FixedViaSelected> _minResMinHeightArray;
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_FIXEDVIASELECTOR_H_
