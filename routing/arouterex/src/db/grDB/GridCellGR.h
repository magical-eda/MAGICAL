/**
 * @file GridCellGR.h
 * @brief Grid cell for global routing. Updated with single layer
 * @author Keren Zhu
 * @date 10/06/2018
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
        explicit GridCellGR() = default;

        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _netIdxSet const reference
        /// @return the nets covered by this grid
        //const std::set<IndexType> &        subNets() const      { return _subNetIdxSet; }
        /// @brief get _netIdxSetArray reference
        /// @return the nets covered by this grid
        //std::set<IndexType> &              subNets()            { return _subNetIdxSet; }
        /// @brief get _pinIdxSetArray const reference
        /// @return the pins covered by this grid
        const std::set<IndexType> &        pins() const      { return _pinIdxSet; }
        /// @brief get _pinIdxSetArray reference
        /// @return the pins covered by this grid
        std::set<IndexType> &              pins()            { return _pinIdxSet; }
        /// @brief get the terminal indices dropped to this gridcell
        /// @return the terminal indices array
        const std::vector<IndexType> &     terminals() const { return _termIdxArray; }
        /// @brief get the terminal indices dropped to this gridcell
        /// @return the terminal indices array
        std::vector<IndexType> &           terminals()       { return _termIdxArray; }
        /// @brief get _blockedArray const reference
        /// @return whether the grid is blocked or not
        bool                               isBlocked() const { return _blocked; }
        /// @brief get _hisCostArray const reference
        /// @param the history cost
        IntType                            hisCost() const   { return _hisCost + _penalty  * GR_CELL_PENALTY_COST; }
        /// @brief determine whether certain terminal is inside this cell
        /// @param the index of terminal query for
        /// @return whether the terminal is dropped to this cell
        bool existTerm(IndexType termIdx) const 
        {
            for (IndexType term : _termIdxArray)
            {
                if (term == termIdx)
                {
                    return true;
                }
            }
            return false;
        }

        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set isBlocked on a layer
        /// @param isBlocked
        void                                            setBlocked(bool blocked)    { _blocked = blocked; }
        /// @brief set history cost on a layer
        /// @param history cost
        void                                            setHisCost(IntType hisCost) { _hisCost = hisCost; }
        /// @brief set penalty term
        /// @param the penalty term
        void setPenalty(IntType penalty) { _penalty = penalty; }
        /// @brief increment penalty term
        void increPenalty() { _penalty++; }


        /*------------------------------*/ 
        /* Routing operations           */
        /*------------------------------*/ 
        /// @brief drop a pin to grid cell
        /// @param index of the pin
        /// @return true if inserted, false if pin already exist
        bool         dropPin(IndexType pinIdx)  { return _pinIdxSet.insert(pinIdx).second; }
        /// @brief drop a terminal to grid cell
        /// @param the index of the terminal
        void dropTerminal(IndexType termIdx) { return _termIdxArray.emplace_back(termIdx); }
        //r @brief erase a pin from the grid cell
        /// @param the index of the pin
        /// @return true if erased, false if pin does not exist
        bool         erasePin(IndexType pinIdx) { return _pinIdxSet.erase(pinIdx); }
        /// @brief drop a net to grid cell
        /// @param index of the net, and index of the layer to drop on
        /// @return true if inserted, false if net already exist
        //bool        dropNet(IndexType netIdx)   { return _subNetIdxSet.insert(netIdx).second; }
        /// @brief erase a net from the grid cell
        /// @param the index of the net, and the index of the layer to erase from
        /// @return true if erased, false if net does not exist
        //bool        eraseNet(IndexType netIdx)  { return _subNetIdxSet.erase(netIdx); }
        /// @brief clear the nets on one layer
        /// @param which layer to be cleared
        /// @return void
        //void        clearUsage()                { _subNetIdxSet.clear(); }
        /// @brief drop a blockage on one layer
        /// @return true if inserted, false if already blocked
        bool        dropBlockage();
        /// @brief erase a blockage on one layer
        /// @return true if erased, false if was no blockage
        bool        eraseBlockage();
        /// @brief the usage of the grid cell. Essentially the number of nets using this gridcell
        /// @return number of nets using the gridcell
        //IndexType   usage() const               { return _subNetIdxSet.size(); }
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

    private:
        //std::set<IndexType> _subNetIdxSet; ///< _subNetIdxSetArray[layerIdx] = {subNetIdx}
                                                          ///< if netIdx == INDEX_TYPE_MAX, it is a blockage
        std::set<IndexType> _pinIdxSet; ///< _pinIdxSetArray[layerIdx] = {pinIdx}
        bool _blocked; ///< _blockedArray[layerIdx] = {grid blocked or not}
        IntType _hisCost; ///< History costs for negotiation congestion based routing _hisCostArray[layerIdx] = {history_cost}
        IntType _penalty = 0; ///< The penalty term
        std::vector<IndexType> _termIdxArray; ///< The terminals belongs to this
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_GRIDCELLGR_H_
