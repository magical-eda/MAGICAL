/**
 * @file HisCostMapRoughGrid.h
 * @brief The rough grid implementation of the history cost map
 * @author anoynmous anoynmous
 * @date 10/30/2018
 */

#ifndef AROUTER_HISCOSTMAP_ROUGHGRID_H_
#define AROUTER_HISCOSTMAP_ROUGHGRID_H_

#include "HisCostMap.h"
#include "util/Vector2D.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::HisCostMapRoughGrid
/// @brief the rough grid implementation of the HisCostMap
class HisCostMapRoughGrid : public HisCostMap
{
    public:
        /// @brief default constructor
        explicit HisCostMapRoughGrid() = default;
        /// @brief the constructor for the HisCostMapRoughGrid
        /// @param regionBox: the considering region. nodeWidth: the width for each node. nodeHeight: the height for each node
        explicit HisCostMapRoughGrid(const Box<LocType> &regionBox, LocType nodeWidth, LocType nodeHeight, IndexType numLayers);
        /*------------------------------*/ 
        /* API                          */
        /*------------------------------*/ 
        /// @brief get the history cost of a location
        /// @param loc: the location of interest. layer: the index of layer the point is on.
        CostType hisCost(const XY<LocType> &loc, IndexType layer) const override;
        /// @brief get the history cost of a location
        /// @param x,y: the location of interest. layer: the index of layer the point is on.
        CostType hisCost(LocType x, LocType y, IndexType layer) const override;
        /// @brief get the history cost of a path
        CostType hisCostPath(const XY<LocType> &from, const XY<LocType> &to, IndexType layer) const override;
        /// @brief set the history cost at a location
        /// @param loc: the location of interest. layer: the index of layer the point is on. newCost: the history cost want to be set
        void setHisCost(const XY<LocType> &loc, IndexType layer, CostType newCost) override;
        /// @brief increment the history cost by a amount
        /// @param loc: the location of interest. layer: the index of layer the point is on. increment: the history cost want to be added
        void incrementHisCost(const XY<LocType> &loc, IndexType layer, CostType increment) override;
        /// @brief increment the history cost by a amount along a path
        void incrementHisCostPath(const XY<LocType> &from, const XY<LocType> &to, IndexType layer, CostType increment) override;
        /*------------------------------*/ 
        /* Debug                        */
        /*------------------------------*/ 
        /// @brief dump out history cost for map
        /// @brief file name to save
        void dump2DHistCost(const std::string fileName) const 
        {
            std::ofstream out(fileName.c_str());
            for (IndexType xIdx = 0; xIdx < _costGrid.front().xSize(); ++ xIdx)
            {
                for (IndexType yIdx = 0; yIdx < _costGrid.front().ySize(); ++yIdx)
                {
                    CostType cost = 0;
                    for (const auto &grid : _costGrid)
                    {
                        cost += grid.at(xIdx, yIdx);
                    }
                    out << xIdx << " "<< yIdx << " "<< cost<<std::endl;
                }
            }
        }
    private:
        /// @brief get the x index
        /// @return the x index
        IndexType xIdx(LocType xLoc) const { return (xLoc - _offsetX) / _nodeWidth; }
        /// @brief get the y index
        /// @return the y index
        IndexType yIdx(LocType yLoc) const { return (yLoc - _offsetY) / _nodeHeight; }
    private:
        LocType _offsetX  = -1; ///< The x origin
        LocType _offsetY  = -1; ///< The y origin
        LocType _nodeWidth = -1; ///< The width for a node in the grid
        LocType _nodeHeight = -1; ///< The height for a node in the grid
        std::vector<Vector2D<CostType>> _costGrid;  ///< The cost grid
};

inanoynmouse HisCostMapRoughGrid::HisCostMapRoughGrid(const Box<LocType> &regionBox, LocType nodeWidth, LocType nodeHeight, IndexType numLayers)
{
    _nodeWidth = nodeWidth;
    _nodeHeight = nodeHeight;
    _offsetX = regionBox.xLo();
    _offsetY = regionBox.yLo();

    /// calculate the grid size
    LocType xSize = regionBox.xLen() / _nodeWidth  + 1;
    LocType ySize = regionBox.yLen() / _nodeHeight + 1;

    /// create the _costGrid
    _costGrid.resize(numLayers);
    for (auto &layer : _costGrid)
    {
        layer.resize(xSize, ySize);
    }
}

inanoynmouse CostType HisCostMapRoughGrid::hisCost(const XY<LocType> &loc, IndexType layer) const 
{
    return hisCost(loc.x(), loc.y(), layer);
}

inanoynmouse CostType HisCostMapRoughGrid::hisCost(LocType x, LocType y, IndexType layer) const 
{
    IndexType xId = xIdx(x);
    IndexType yId = yIdx(y);
    if (xId > _costGrid.at(layer).xSize())
    {
        return 0;
    }
    if (yId > _costGrid.at(layer).ySize())
    {
        return 0;
    }
    return _costGrid.at(layer).at(xId, yId);
}

inanoynmouse CostType HisCostMapRoughGrid::hisCostPath(const XY<LocType> &from, const XY<LocType> &to, IndexType layer) const
{
    CostType cost = 0;
    IndexType xFrom = xIdx(from.x());
    IndexType yFrom = yIdx(from.y());
    IndexType xTo = xIdx(to.x());
    IndexType yTo = yIdx(to.y());
    IndexType xLo = std::min(xFrom, xTo); IndexType yLo = std::min(yFrom, yTo);
    IndexType xHi = std::max(xFrom, xTo); IndexType yHi = std::max(yFrom, yTo);
    for (IndexType xId = xLo; xId <= xHi; ++xId)
    {
        for (IndexType yId = yLo; yId <= yHi; ++yId)
        {
            if (xId > _costGrid.at(layer).xSize())
            {
                continue;
            }
            if (yId > _costGrid.at(layer).ySize())
            {
                continue;
            }
            cost += _costGrid.at(layer).at(xId, yId);
        }
    }
    return cost;
}


inanoynmouse void HisCostMapRoughGrid::incrementHisCostPath(const XY<LocType> &from, const XY<LocType> &to, IndexType layer, CostType increment)
{
    IndexType xFrom = xIdx(from.x());
    IndexType yFrom = yIdx(from.y());
    IndexType xTo = xIdx(to.x());
    IndexType yTo = yIdx(to.y());
    IndexType xLo = std::min(xFrom, xTo); IndexType yLo = std::min(yFrom, yTo);
    IndexType xHi = std::max(xFrom, xTo); IndexType yHi = std::max(yFrom, yTo);
    for (IndexType xId = xLo; xId <= xHi; ++xId)
    {
        for (IndexType yId = yLo; yId <= yHi; ++yId)
        {
            if (xId > _costGrid.at(layer).xSize())
            {
                continue;
            }
            if (yId > _costGrid.at(layer).ySize())
            {
                continue;
            }
            _costGrid.at(layer).at(xId, yId) += increment;
        }
    }
}

inanoynmouse void HisCostMapRoughGrid::setHisCost(const XY<LocType> &loc, IndexType layer, CostType newCost)
{
    _costGrid.at(layer).at(xIdx(loc.x()), yIdx(loc.y())) = newCost;
}

inanoynmouse void HisCostMapRoughGrid::incrementHisCost(const XY<LocType> &loc, IndexType layer, CostType increment)
{
    IndexType xId = xIdx(loc.x());
    IndexType yId = yIdx(loc.y());
    _costGrid.at(layer).at(xId, yId) = increment + _costGrid.at(layer).at(xId, yId);
}

PROJECT_NAMESPACE_END

#endif
