/**
 * @file SymNet.h
 * @brief Class for useful information of SymNet
 * @author Keren Zhu
 * @date 10/14/2018
 */

#ifndef AROUTER_SYMNET_H_
#define AROUTER_SYMNET_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN


/// @class AROUTER::SymNet
/// @brief use information of pairs of symmetric nets
class SymNet
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit SymNet() = default;
        /// @brief constructor with net indices
        /// @param firstNetIdx: the first net index of the symmetry pair. secondNetIdx: the second index of the symmetry pair
        explicit SymNet(IndexType firstNetIdx, IndexType secondNetIdx) : _firstNetIdx(firstNetIdx), _secondNetIdx(secondNetIdx) {}
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
        /// @brief get _firstNetIdx
        /// @return the first net index of this symmetry pair
        IndexType firstNetIdx() const { return _firstNetIdx; }
        /// @brief get _secondNetIdx
        /// @return the second net index of this symmetry pair
        IndexType secondNetIdx() const { return _secondNetIdx; }
        /// @brief get _TerminalPairs
        /// @return the vector of pairs representing the pair of terminals in this symmetry pair
        const std::vector<std::pair<IndexType, IndexType>> & terminalPairs() const { return _terminalPairs; }
        /// @brief get _terminalPairs
        /// @return the vector of pairs representing the pair of terminals in this symmetry pair
        std::vector<std::pair<IndexType, IndexType>> & terminalPairs() { return _terminalPairs; }
        /// @brief get whether this Symnet is valid
        /// @return whether this Symnet is valid
        bool valid() const { return _symType != SymType::NONE; }
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
        /// @brief set _firstNetIdx
        /// @param the first net index of this symmetry pair
        void setFirstNetIdx(IndexType firstNetIdx) { _firstNetIdx = firstNetIdx; }
        /// @brief set _secondNetIdx
        /// @param the second net index of this symmetry pair
        void setSecondNetIdx(IndexType secondNetIdx) { _secondNetIdx = secondNetIdx; }
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
        
    private:
        SymType _symType = SymType::NONE; ///< The symmetry type of this pair of nets
        LocType _symAxis = LOC_TYPE_MIN; ///< the symmetric axis of this pair of nets (for horizontal and vertical symmetry)
        IndexType _symAxisGC = INDEX_TYPE_MAX; ///< the symmetric axis of this pair of nets in unit of global routing grid cells (for horizontal and vertical symmetry)
        IndexType _firstNetIdx = INDEX_TYPE_MAX; ///< the first net index
        IndexType _secondNetIdx = INDEX_TYPE_MAX; ///< the second net index
        std::vector<std::pair<IndexType, IndexType>> _terminalPairs; ///< _terminalPairs[index].first = terminal in the first net. _terminalPairs[index].second = terminal in the second net
};

inline XY<LocType> SymNet::symLoc(XY<LocType> origin) const 
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

inline XY<IndexType> SymNet::symGC(XY<IndexType> origin) const 
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

#endif ///AROUTER_SYMNET_H_
