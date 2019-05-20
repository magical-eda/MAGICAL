/**
 * @file ProcessPin.h
 * @brief Store the processed pin information for the detailed router
 * @author Keren Zhu
 * @date 12/05/2018
 */

#ifndef AROUTER_PROCESS_PIN_H_
#define AROUTER_PROCESS_PIN_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::ProcessPin
/// @brief the processed pin information for the detailed router
class ProcessPin
{
    public:
        explicit ProcessPin() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the set of points for seaching start points
        /// @return A set of points inside the pin shapes
        const std::vector<XY<LocType>> & searchPoints() const { return _searchPoints; }
        /// @brief get the set of points for seaching start points
        /// @return A set of points inside the pin shapes
        std::vector<XY<LocType>> & searchPoints() { return _searchPoints; }
    private:
        std::vector<XY<LocType>> _searchPoints; ///< A set of points in the pin that can be used for seaching start point
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_PROCESS_PIN_H_
