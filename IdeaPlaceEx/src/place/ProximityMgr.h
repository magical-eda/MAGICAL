/**
 * @file ProximityMgr.h
 * @brief Proximity Maneger
 * @author Keren Zhu
 * @date 02/22/2020
 */

#ifndef IDEAPLACE_PROXIMITY_MGR_H_
#define IDEAPLACE_PROXIMITY_MGR_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN


/// @class IDEAPLACE::ProximityMgr
/// @brief the proximity manager
class ProximityMgr
{
    static constexpr IntType DEFAULT_DUMMY_NET_WEIGHT = 10;
    /// @brief quick shortcut for recording the pin idx in cell 
    struct PinInCellIdx
    {
        PinInCellIdx(IndexType cellIdx_, IndexType pinIdx_) : cellIdx(cellIdx_), pinIdx(pinIdx_) {}
        IndexType cellIdx;
        IndexType pinIdx;
    };
    public:
        /// @brief default constructor
        explicit ProximityMgr(Database &db) : _db(db) {}
        /// @brief restore all the changes
        void restore();
        /// @brief configure the proximity with adding dummy nets
        void applyProximityWithDummyNets();
    private:
        /// @brief add dummy net for a proximity group
        /// @param a ProximityGroup
        void addDummyNetForProximityGroup(const ProximityGroup &pg);
    private:
        Database &_db; ///< The placement engine database
        std::vector<IndexType> _dummyPins; ///< The indices of dummy pins
        std::vector<IndexType> _dummyNets; ///< The indices of dummy nets
        std::vector<PinInCellIdx> _dummyCells; ///< The indices for dummy cells
};


PROJECT_NAMESPACE_END

#endif //IDEAPLACE_PROXIMITY_MGR_H_
