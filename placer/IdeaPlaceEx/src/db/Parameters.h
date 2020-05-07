/**
 * @file Parameters.h
 * @brief The placement parameters
 * @author Keren Zhu
 * @date 10/16/2019
 */

#ifndef IDEAPLACE_PARAMETERS_H_
#define IDEAPLACE_PARAMETERS_H_

#include "global/global.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::Parameters
/// @brief The placement eingie parameters
class Parameters
{
    public:
        /// @brief default constrcutor
        explicit Parameters() = default;
        /*------------------------------*/ 
        /* Set the parameters           */
        /*------------------------------*/ 
        /// @brief set the boundry constraints
        /// @param the placement boundry
        void setBoundaryConstraint(const Box<LocType> &boundaryConstraint) { _boundaryConstraint = boundaryConstraint; }
        /// @brief open the functionality of virtual pin assignment
        void openVirtualPinAssignment() { _ifUsePinAssignment = true; }
        /// @brief close the functionality of virtual pin assignment 
        void closeVirtualPinAssignment() { _ifUsePinAssignment = false; }
        /*------------------------------*/ 
        /* Query the parameters         */
        /*------------------------------*/ 
        /// @brief whether the boundry constraint is set
        /// @return whether the boundry constraint is set
        bool isBoundaryConstraintSet() const { return _boundaryConstraint.valid(); }
        /// @brief get the boundry constraint
        /// @return the boundry constraint
        const Box<LocType> & boundaryConstraint() const { return _boundaryConstraint; }
        /// @brief get whether to use the virtual pin assignment functionality
        bool ifUsePinAssignment() const { return _ifUsePinAssignment; }
    private:
        Box<LocType> _boundaryConstraint = Box<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN);
        bool _ifUsePinAssignment = false; ///< If do pin assignment
};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PARAMETERS_H_
