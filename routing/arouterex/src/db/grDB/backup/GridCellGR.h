/**
 * @file GridCellGR.h
 * @brief Grid cell for global routing
 * @author Keren Zhu
 * @date 10/03/2018
 */

#ifndef AROUTER_GRIDCELLGR_H_
#define AROUTER_GRIDCELLGR_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::GridCellGR
/// @brief Grid Cell for global router. 
class GridCellGR
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief constructor
        /// @param the grid indices and the number of layers
        explicit GridCellGR(XY<IndexType> gridIdx = XY<IndexType>(INDEX_TYPE_MAX, INDEX_TYPE_MAX), IndexType numLayers = 0);

        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _gridIdx
        /// @return the grid indices (by value)
        XY<IndexType>                                   gridIdx() const                                 { return _gridIdx; }
        /// @brief get _netIdxSetArray const reference
        /// @return the nets covered by this grid
        const std::vector<std::set<IndexType>> &        netIdxSetArray() const                          { return _netIdxSetArray; }
        /// @brief get _netIdxSetArray reference
        /// @return the nets covered by this grid
        std::vector<std::set<IndexType>> &              netIdxSetArray()                                { return _netIdxSetArray; }
        /// @brief get _pinIdxSetArray const reference
        /// @return the pins covered by this grid
        const std::vector<std::set<IndexType>> &        pinIdxSetArray() const                          { return _pinIdxSetArray; }
        /// @brief get _pinIdxSetArray reference
        /// @return the pins covered by this grid
        std::vector<std::set<IndexType>> &              pinIdxSetArray()                                { return _pinIdxSetArray; }
        /// @brief get _blockedArray const reference
        /// @return whether the grid[layer] is blocked or not
        const std::vector<bool> &                       blockedArray() const                            { return _blockedArray; }
        /// @brief get _blockedArray reference
        /// @return whether the grid[layer] is blocked or not
        std::vector<bool> &                             blockedArray()                                  { return _blockedArray; }
        /// @brief get _hisCostArray const reference
        /// @param the history costs
        const std::vector<IntType> &                    hisCostArray() const                            { return _hisCostArray; }
        /// @brief get _hisCostArray reference
        /// @param the history costs
        std::vector<IntType> &                          hisCostArray()                                  { return _hisCostArray; }

        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set grid indices
        /// @param XY<Index> as x and y indices
        void                                            setIdx(XY<IndexType> gridIdx)                   { _gridIdx = gridIdx; }

        /*------------------------------*/ 
        /* Single layer wrappers        */
        /*------------------------------*/ 
        /// @brief get single layer nets
        /// @return set of nets on single layer
        const std::set<IndexType> &                     nets(IndexType layerIdx) const                  { return _netIdxSetArray.at(layerIdx); }
        /// @brief get single layer nets
        /// @return set of nets on single layer
        std::set<IndexType> &                           nets(IndexType layerIdx)                        { return _netIdxSetArray.at(layerIdx); }
        /// @brief get single layer pins
        /// @return set of pins on single layer
        const std::set<IndexType> &                     pins(IndexType layerIdx) const                  { return _pinIdxSetArray.at(layerIdx); }
        /// @brief get single layer pins
        /// @return set of pins on single layer
        std::set<IndexType> &                           pins(IndexType layerIdx)                        { return _pinIdxSetArray.at(layerIdx); }
        /// @brief set isBlocked on a layer
        /// @param layerIdx and isBlocked
        void                                            setBlocked(IndexType layerIdx, bool blocked)    { _blockedArray.at(layerIdx) = blocked; }
        /// @brief is blocked on a layer
        /// @param layer index
        /// @return the target layer whether is blocked
        bool                                            isBlocked(IndexType layerIdx) const             { return _blockedArray.at(layerIdx); }
        /// @brief set history cost on a layer
        /// @param layerIdx and cost
        void                                            setHisCost(IndexType layerIdx, IntType hisCost) { _hisCostArray.at(layerIdx) = hisCost; }
        /// @brief history cost on a layer
        /// @param layer index
        /// @return the history cost on the target layer 
        IntType                                         hisCost(IndexType layerIdx) const               { return _hisCostArray.at(layerIdx); }


        /*------------------------------*/ 
        /* Routing operations           */
        /*------------------------------*/ 
        /// @brief drop a pin to grid cell
        /// @param index of the pin, and index of the layer to drop on
        /// @return true if inserted, false if pin already exist
        bool         dropPin(IndexType pinIdx, IndexType layerIdx)  { return _pinIdxSetArray.at(layerIdx).insert(pinIdx).second; }
        /// @brief erase a pin from the grid cell
        /// @param the index of the pin, and the index of the layer to erase from
        /// @return true if erased, false if pin does not exist
        bool         erasePin(IndexType pinIdx, IndexType layerIdx) { return _pinIdxSetArray.at(layerIdx).erase(pinIdx); }
        /// @brief drop a net to grid cell
        /// @param index of the net, and index of the layer to drop on
        /// @return true if inserted, false if net already exist
        bool        dropNet(IndexType netIdx, IndexType layerIdx)   { return _netIdxSetArray.at(layerIdx).insert(netIdx).second; }
        /// @brief erase a net from the grid cell
        /// @param the index of the net, and the index of the layer to erase from
        /// @return true if erased, false if net does not exist
        bool        eraseNet(IndexType netIdx, IndexType layerIdx)  { return _netIdxSetArray.at(layerIdx).erase(netIdx); }
        /// @brief clear the nets on one layer
        /// @param which layer to be cleared
        /// @return void
        void        clearUsage(IndexType layerIdx)                  { _netIdxSetArray.at(layerIdx).clear(); }
        /// @brief drop a blockage on one layer
        /// @param which layer to be blocked
        /// @return true if inserted, false if already blocked
        bool        dropBlockage(IndexType layerIdx);
        /// @brief erase a blockage on one layer
        /// @param which layer to be erased
        /// @return true if erased, false if was no blockage
        bool        eraseBlockage(IndexType layerIdx);
        /// @brief the usage of the grid cell. Essentially the number of nets using this gridcell
        /// @param the layer 
        /// @return number of nets using the gridcell
        IndexType   usage(IndexType layerIdx) const                 { return _netIdxSetArray.at(layerIdx).size(); }
        /* 
         * I probably want to keep the history cost update in the routing routine instead of gridcell class
         */
        /// @brief update history cost and clear usage
        /// @param the layer index
        /// @return overflow of history cost
        //IndexType updateHisCostClearUsage(IndexType layerIdx);
        /// @brief update history cost
        /// @param the layer index
        /// @return overflow of history cost
        //IndexType overflowHistory(IndexType layerIdx, IntType historyWeight)/

        /*------------------------------*/ 
        /* Operator                     */
        /*------------------------------*/ 
        bool operator<(const GridCellGR &rhs) const { return _gridIdx < rhs.gridIdx(); }

    private:
        XY<IndexType> _gridIdx; ///< grid indices
        std::vector<std::set<IndexType>> _netIdxSetArray; ///< _netIdxSetArray[layerIdx] = {netIdx}
                                                          ///< if netIdx == INDEX_TYPE_MAX, it is a blockage
        std::vector<std::set<IndexType>> _pinIdxSetArray; ///< _pinIdxSetArray[layerIdx] = {pinIdx}
        std::vector<bool> _blockedArray; ///< _blockedArray[layerIdx] = {grid blocked or not}
        std::vector<IntType> _hisCostArray; ///< History costs for negotiation congestion based routing _hisCostArray[layerIdx] = {history_cost}
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_GRIDCELLGR_H_
