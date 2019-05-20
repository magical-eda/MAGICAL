/**
 * @file HisCostMap.h
 * @brief Base class for history cost
 * @author Keren Zhu
 * @date 10/29/2018
 */

#ifndef AROUTER_HISCOSTMAP_H_
#define AROUTER_HISCOSTMAP_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::HisCostMap
/// @brief base class for the history cost map
class HisCostMap
{
    public:
        /// @brief default constructor
        explicit HisCostMap() = default;
        /// @brief get the history cost of a location
        /// @param loc: the location of interest. layer: the index of layer the point is on.
        virtual CostType hisCost(const XY<LocType> &loc, IndexType layer) const = 0;
        /// @brief get the history cost of a path
        virtual CostType hisCostPath(const XY<LocType> &from, const XY<LocType> &to, IndexType layer) const = 0;
        /// @brief get the history cost of a location
        /// @param x,y: the location of interest. layer: the index of layer the point is on.
        virtual CostType hisCost(LocType x, LocType y, IndexType layer) const = 0;
        /// @brief set the history cost at a location
        /// @param loc: the location of interest. layer: the index of layer the point is on. newCost: the history cost want to be set
        virtual void setHisCost(const XY<LocType> &loc, IndexType layer, CostType newCost) = 0;
        /// @brief increment the history cost by a amount
        /// @param loc: the location of interest. layer: the index of layer the point is on. increment: the history cost want to be added
        virtual void incrementHisCost(const XY<LocType> &loc, IndexType layer, CostType increment) = 0;
        /// @brief increment the history cost by a amount along a path
        virtual void incrementHisCostPath(const XY<LocType> &from, const XY<LocType> &to, IndexType layer, CostType increment) = 0;
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_HISCOSTMAP_H_
