/**
 * @file Constraints.h
 * @brief The placement database data structure
 * @author Keren Zhu
 * @date 10/02/2019
 */

#ifndef IDEAPLACE_DATABASE_CONSTRAINTS_H_
#define IDEAPLACE_DATABASE_CONSTRAINTS_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @brief the data structures for symmetric pairs
class SymPair
{
    public:
        /// @brief default constructor
        explicit SymPair() = default;
        /// @brief constructor
        /// @param one cell index
        /// @param another cell index
        explicit SymPair(IndexType cellIdx1, IndexType cellIdx2) { this->setCellIdx(cellIdx1, cellIdx2); }
        /// @brief set the cell indices. The order stored is always left is lower in index
        /// @param one cell index
        /// @param another cell index
        void setCellIdx(IndexType cellIdx1, IndexType cellIdx2)
        {
            if (cellIdx1 < cellIdx2)
            {
                _firstCell = cellIdx1;
                _secondCell = cellIdx2;
            }
            else
            {
                _secondCell = cellIdx1;
                _firstCell = cellIdx2;
            }
        }
        /// @brief get the first cell index
        /// @return the first cell index
        IndexType firstCell() const { Assert(_firstCell != INDEX_TYPE_MAX);  return _firstCell; }
        /// @brief get the second cell index
        /// @return the second cell index
        IndexType secondCell() const { Assert(_secondCell != INDEX_TYPE_MAX);  return _secondCell; }
    private:
        IndexType _firstCell = INDEX_TYPE_MAX; ///< The first cell
        IndexType _secondCell = INDEX_TYPE_MAX; ///< The second cell
};

/// @brief the data structure for the symmetric group
class SymGroup
{
    public:
        /// @brief default constructor
        explicit SymGroup() = default;
        /// @brief add self symmetric cell
        /// @param cell index
        void addSelfSym(IndexType cellIdx) { _selfSyms.emplace_back(cellIdx); }
        /// @brief get the number of self symmetric cells
        /// @return the number of self symmetric cells in this group
        IndexType numSelfSyms() const { return _selfSyms.size(); }
        /// @brief get one cell index of the self symmetric constraint in this group
        /// @param the index of self symmetric constraint in this group
        /// @return a cell index
        IndexType selfSym(IndexType idx) const { return _selfSyms.at(idx); }
        /// @brief add symmetric pair. The parameters order does not matter
        /// @param one cell index
        /// @param another cell index
        void addSymPair(IndexType cellIdx1, IndexType cellIdx2) { _symPairs.emplace_back(SymPair(cellIdx1, cellIdx2)); }
        /// @brief get the number of symmetric pairs in this group
        /// @return the number of symmetric pairs in this group
        IndexType numSymPairs() const { return _symPairs.size(); }
        /// @brief get a symmetric pair in this group
        /// @param the index of the pair in this group
        /// @return a symmetric pair
        const SymPair & symPair(IndexType idx) const { return _symPairs.at(idx); }
        IndexType numConstraints() const { return numSelfSyms() + numSymPairs(); }
        const std::vector<SymPair> & vSymPairs() const { return _symPairs; }
        const std::vector<IndexType> & vSelfSyms() const { return _selfSyms; }
    private:
        std::vector<SymPair> _symPairs; ///< The symmetric pairs
        std::vector<IndexType> _selfSyms; ///< The self symmetric cells
};

/// @brief simple proximity
class ProximityGroup
{
    public:
      /// @brief default constructor
      explicit ProximityGroup() = default;
      /// @brief add a cell to this proximity group
      /// @param the cell index
      void addCell(IndexType cellIdx) { _cells.emplace_back(cellIdx); }
      /// @brief set the weight of the proximity group
      /// @param the weight of this proximity group
      void setWeight(IntType weight) { _weight = weight; }
      /// @brief get the vector of the cell indices
      /// @return get the vector of the cell indices
      const std::vector<IndexType> &cells() const { return _cells; }

    private:
      std::vector<IndexType> _cells; ///< The cells belonging to this proximity group
      IntType _weight; ///< The weight of this proximity group
};

/// @brief the signal/current path
class SignalPath
{
    public:
        /// @brief default constructor
        explicit SignalPath() = default;
        const std::vector<IndexType> & vPinIdxArray() const { return _pinIdxArray; }
        std::vector<IndexType> & vPinIdxArray() { return _pinIdxArray; }
        void addPinIdx(IndexType pinIdx) { _pinIdxArray.emplace_back(pinIdx); }
        void markAsPower() { _isPower = true; }
        BoolType isPower() const { return _isPower; }
        /// @brief copy the settings of another SigalPath
        void copySettings(const SignalPath & other)
        {
            this->_isPower = other._isPower;
        }
    private:
        std::vector<IndexType> _pinIdxArray; ///< The indices of pins composing the path
        BoolType _isPower = false;
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_DBASE_CONSTRAINTS_H.at(
