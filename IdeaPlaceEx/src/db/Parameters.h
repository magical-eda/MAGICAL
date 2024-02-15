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
        explicit Parameters();
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
        /// @brief set the number of threads
        void setNumThreads(IndexType numThreads) { _numThreads = numThreads; }
        /// @brief set the grid step constraint
        void setGridStep(LocType gridStep) { _gridStep = gridStep; } 
        /// @brief set the virutal boundary extension. The extension of boundary to io pin with respect to the cell placement
        void setVirtualBoundaryExtension(LocType virtualBoundaryExtension) { _virtualBoundaryExtension = virtualBoundaryExtension; _layoutOffset = 2 * virtualBoundaryExtension; }
        /// @brief set the pin interval 
        void setVirtualPinInterval(LocType virtualPinInterval) { _virtualPinInterval  = virtualPinInterval; }
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
        /// @brief get the number of thread
        IndexType numThreads() const { return _numThreads; }
        /// @brief get the grid step
        LocType gridStep() const { return _gridStep; }
        /// @brief get wether there is grid step constraint
        bool hasGridStep() const { return _gridStep > 0; }
        /// @brief get the extension of virtual boundary of cell placement boundary
        LocType virtualBoundaryExtension() const { return _virtualBoundaryExtension; }
        /// @brief get the interval of virtual io pins
        LocType virtualPinInterval() const { return _virtualPinInterval; }
        /// @brief get the layout offset
        LocType layoutOffset() const { return _layoutOffset; }
        /// @brief get the default aspect ratio for the global placement
        RealType defaultAspectRatio() const { return _defaultAspectRatio; }
        /// @brief get the default max white space
        RealType maxWhiteSpace() const { return _maxWhiteSpace; }
        /// @brief get the default signal flow operator weight
        RealType defaultSignalFlowWeight() const { return _defaultSignalFlowWeight; }
        /// @brief get the default current flow operator weight
        RealType defaultCurrentFlowWeight() const { return _defaultCurrentFlowWeight; }
        /// @@brief get the  weighing ratio of power to regular net
        RealType defaultRelativeRatioOfPowerNet() const { return _defaultRelativeRatioOfPowerNet; }
    private:
        Box<LocType> _boundaryConstraint = Box<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN);
        bool _ifUsePinAssignment; ///< If do pin assignment
        IndexType _numThreads;
        LocType _gridStep;
        LocType _virtualBoundaryExtension; ///< The extension of current virtual boundary to the bounding box of placement
        LocType _virtualPinInterval; ///< The interval between each virtual pin
        LocType _layoutOffset; ///< The default offset for the placement
        RealType _defaultAspectRatio; ///< The defaut aspect ratio for global placement
        RealType _maxWhiteSpace; ///< The default maximum white space target
        RealType _defaultSignalFlowWeight; ///< The default weight for signal flow operators
        RealType _defaultCurrentFlowWeight; ///< The default weight for current flow operators
        RealType _defaultRelativeRatioOfPowerNet; ///< The weighing ratio of power to regular net

};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PARAMETERS_H_
