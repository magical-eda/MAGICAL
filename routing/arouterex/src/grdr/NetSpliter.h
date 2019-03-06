/**
 * @file TwoPinNetSpliter.h
 * @brief Split the nets into 2-pin nets
 * @author anoynmous anoynmous
 * @date 10/09/2018
 */

#ifndef AROUTER_TWOPINSPLITER_H_
#define AROUTER_TWOPINSPLITER_H_

#include "db/Terminal.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::NetSpliter
class NetSpliter
{
    public:
        explicit NetSpliter() = default;

        /// Setters
        void setPins(std::vector<XY<LocType>> &pins) { _pinsPtr = &pins; }
        void setNetPins(const std::vector<IndexType> & pinIdxArray) { _pinIdxArray = pinIdxArray; }
        void clear() { _pinIdxArray.clear(); _splitedPairs.clear(); }

        /// Getters
        const std::vector<std::pair<IndexType, IndexType>> & results() const { return _splitedPairs; }
        std::vector<std::pair<IndexType, IndexType>> & results() { return _splitedPairs; }

        /// Solve
        /// @brief Just connect 1 to 2, 2 to 3 etc.
        bool solveEndToEnd();
        /// @brief Spliting using MST
        bool solveMST();
        /// @brief Spliting using FLUTE
        bool solveFlute();
    private:
        /// @brief reorder the pairs to make sure the splited subNets are end to end
        void reorder();
    private:
        std::vector<XY<LocType>>* _pinsPtr = NULL; ///< Pointer to the pin vector/ problem input
        std::vector<IndexType> _pinIdxArray; ///< the index of pins want to consider
        std::vector<std::pair<IndexType, IndexType>> _splitedPairs; ///< The solved result of the the two pin nets

};

PROJECT_NAMESPACE_END

#endif ///AROUTER_TWOPINSPLITER_H_
