/**
 * @file SelfSymNet.h
 * @brief Class for useful information of Self symmetry net
 * @author Keren Zhu
 * @date 10/21/2018
 */

#ifndef AROUTER_SELF_SYMNET_H_
#define AROUTER_SELF_SYMNET_H_

#include "SymNet.h"

PROJECT_NAMESPACE_BEGIN


/// @class AROUTER::SelfSymNet
/// @brief use information of pairs of symmetric nets
class SelfSymNet
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit SelfSymNet() = default;
        /// @brief constructor with net indices
        /// @param firstNetIdx: the first net index of the symmetry pair. secondNetIdx: the second index of the symmetry pair
        explicit SelfSymNet(IndexType netIdx) : _netIdx(netIdx) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _symType
        /// @return the type of symmetry
        SymType symType() const { return _symType; }
        /// @brief get _symAxis. For horizontal and vertical symmetry
        /// @return the symmetric axis of this pair of net. For horizontal and vertical symmetry
        LocType symAxis() const { return _symAxis; }
        /// @brief get _symAxisGC. For horizontal and vertical symmetry
        /// @return the symmetry axis of the pair in unit of grid cell indices. For horizontal and vertical symmetry
        IndexType symAxisGC() const { return _symAxisGC; }
        /// @brief get netIdx
        /// @return the net index of this symmetry pair
        IndexType netIdx() const { return _netIdx; }
        /// @brief get the middle terminal index of the net
        /// @return the middle terminal index of the net
        IndexType midTermIdx() const { return _middleTermIdx; }
        /// @brief get _TerminalPairs
        /// @return the vector of pairs representing the pair of terminals in this symmetry pair
        const std::vector<std::pair<IndexType, IndexType>> & terminalPairs() const { return _terminalPairs; }
        /// @brief get _terminalPairs
        /// @return the vector of pairs representing the pair of terminals in this symmetry pair
        std::vector<std::pair<IndexType, IndexType>> & terminalPairs() { return _terminalPairs; }
        /// @brief get whether this Symnet is valid
        /// @return whether this Symnet is valid
        bool valid() const { return _symType != SymType::NONE; }
        /// @brief get the 2-pin net splitting branches
        /// @return 2-pin net splitting branches
        const std::vector<std::pair<IndexType, IndexType>> & branches() const { return _branches; }
        /// @brief get the 2-pin net splitting branches
        /// @return 2-pin net splitting branches
        std::vector<std::pair<IndexType, IndexType>> & branches() { return _branches; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set _symType
        /// @param the type of symmetry
        void setSymType(SymType symType) { _symType = symType; }
        /// @brief set _symAxis
        /// @param the symmetry axis of this pair (For horizontal and vertical symmetry).
        void setSymAxis(LocType symAxis) { _symAxis = symAxis; }
        /// @brief set _symAxisGC
        /// @param the symmetry axis of the pair in unit of grid cell (for horizontal and vertical symmetry)
        void setSymAxisGC(IndexType symAxisGC) { _symAxisGC = symAxisGC; }
        /// @brief set _netIdx
        /// @param the net index of this symmetry pair
        void setNetIdx(IndexType netIdx) { _netIdx = netIdx; }
        /// @brief set the middle terminal index
        /// @param the terminal index at the middle
        void setMiddle(IndexType middleTermIdx) { _middleTermIdx = middleTermIdx; }
        /// @breif set this symnet as invalid
        void setInvalid() { _symType = SymType::NONE; }
        /*------------------------------*/ 
        /* Useful quick functions       */
        /*------------------------------*/ 
        /// @brief get the symmetric point from the origin
        /// @return the point location corresponding to the symmetry and the origin point
        XY<LocType> symLoc(XY<LocType> origin) const;
        /// @brief get the symmetric global routing grid cells from the origin
        /// @return the indices of grid cell corresponding to the symmetry of the origin point
        XY<IndexType> symGC(XY<IndexType> orgin) const;
        /// @brief find the corresponding terminal index
        /// @param a terminal index
        /// @return the corresponding terminal index. If INDEX_TYPE_MAX, then the terminal is not inside the pairs
        IndexType pairedTerminal(IndexType terminalIdx) const
        {
            // Directly return itself if the one is the middle
            if (terminalIdx == _middleTermIdx)
            {
                return terminalIdx;
            } 
            for (auto &pair: _terminalPairs)
            {
                if (pair.first == terminalIdx)
                {
                    return pair.second;
                }
                if (pair.second == terminalIdx)
                {
                    return pair.first;
                }
            }
            return INDEX_TYPE_MAX;
        }
        /// @brief create a dummy SymNet that reflecting the information of this SelfSymNet
        /// @return a Symnet that can providing the terminal 
        SymNet dummySymNet() const 
        {
            SymNet sym;
            sym.setSymType(_symType);
            sym.setSymAxis(_symAxis);
            sym.setSymAxisGC(_symAxisGC);
            sym.setFirstNetIdx(_netIdx);
            sym.setSecondNetIdx(_netIdx);
            sym.terminalPairs() = _terminalPairs;
            sym.terminalPairs().emplace_back(std::make_pair(_middleTermIdx, _middleTermIdx));
            return sym;
        }
        
    private:
        SymType _symType = SymType::NONE; ///< The symmetry type of this pair of nets
        LocType _symAxis = LOC_TYPE_MIN; ///< the symmetric axis of this pair of nets (for horizontal and vertical symmetry)
        IndexType _symAxisGC = INDEX_TYPE_MAX; ///< the symmetric axis of this pair of nets in unit of global routing grid cells (for horizontal and vertical symmetry)
        IndexType _netIdx = INDEX_TYPE_MAX; ///< the net index
        std::vector<std::pair<IndexType, IndexType>> _terminalPairs; ///< _terminalPairs[index].first = terminal in the first net. _terminalPairs[index].second = terminal in the second net
        IndexType _middleTermIdx = INDEX_TYPE_MAX; ///< The terminal in the middle
        std::vector<std::pair<IndexType, IndexType>> _branches; ///< The branches for 2 pin net spliting
};

inline XY<LocType> SelfSymNet::symLoc(XY<LocType> origin) const 
{
    Assert(_symAxis != INT_TYPE_MIN);
    if (_symType == SymType::HORIZONTAL)
    {
        return XY<LocType>(2 * _symAxis - origin.x(), origin.y());
    }
    else if (_symType == SymType::VERTICAL)
    {
       return XY<LocType>(origin.x(), 2 * _symAxis - origin.y()); 
    }
    /// Should include all the possibility above
    Assert(false);
    return origin;
}

inline XY<IndexType> SelfSymNet::symGC(XY<IndexType> origin) const 
{
    Assert(_symAxisGC != INDEX_TYPE_MAX);
    if (_symType == SymType::HORIZONTAL)
    {
        //AssertMsg(2 * _symAxisGC > origin.x(), "%s: _symAxisGC: %d, origin().x: %d \n", __FUNCTION__, _symAxisGC, origin.x());
        if (2 * _symAxisGC < origin.x()) { return XY<IndexType>(INDEX_TYPE_MAX, INDEX_TYPE_MAX); }
        return XY<IndexType>(2 * _symAxisGC - origin.x(), origin.y());
    }
    else if (_symType == SymType::VERTICAL)
    {
        //Assert(2 * _symAxisGC > origin.y());
        if (2 * _symAxisGC < origin.y()) { return XY<IndexType>(INDEX_TYPE_MAX, INDEX_TYPE_MAX); }
        return XY<IndexType>(origin.x(), 2 * _symAxisGC - origin.y());
    }
    /// Should include all the possibility above
    Assert(false);
    return origin;
}

PROJECT_NAMESPACE_END

#endif ///AROUTER_SELF_SYMNET_H_
