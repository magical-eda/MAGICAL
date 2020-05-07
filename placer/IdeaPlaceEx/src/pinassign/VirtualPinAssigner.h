/**
 * @file VirtualPinAssigner.h
 * @brief Kernel for assigning virtual pins on the placement boundary
 * @author Keren Zhu
 * @date 02/21/2020
 */

#ifndef IDEAPLACE_VIRTUAL_PIN_ASSIGNMER_H_
#define IDEAPLACE_VIRTUAL_PIN_ASSIGNMER_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::VirtualPinAssigner
/// @brief The kernel for assigning virtual pins
class VirtualPinAssigner
{
    public:
        explicit VirtualPinAssigner(Database &db) : _db(db) {}
        /* Kernal interface */
        /// @brief cnfigure the virtual boundary based on databse
        void reconfigureVirtualPinLocationFromDB();
        /// @brief reconfigure the virtual boundary and pin locations
        void reconfigureVirtualPinLocations(const Box<LocType> &cellsBBox);
        /// @brief solve pin assignment from information from DB
        void pinAssignmentFromDB();
        /// @brief short cut to solve the problem from databse information
        void solveFromDB();
        /// @brief solve the pin assignment problem. Export the solution to the database
        /// @param a function for query cell location
        void pinAssignment(std::function<XY<LocType>(IndexType)> cellLocQueryFunc);
        /* Parameter settings */
        /// @brief set the extension distance of placement boundary to cell boundary
        void setVirtualBoundaryExtension(LocType ex) { _virtualBoundaryExtension = ex; }
        /// @brief set the interval between pins
        void setVirtualPinInterval(LocType in) { _virtualPinInterval = in; }
    private:
        Database &_db; ///< The placement database
        Box<LocType> _boundary; ///< The virtual boundary of the placement
        std::vector<XY<LocType>> _virtualPins; ///< The locations for virtual pins
        LocType _virtualBoundaryExtension = VIRTUAL_BOUNDARY_EXTENSION; ///< The extension to placement cell bounding box
        LocType _virtualPinInterval = VIRTUAl_PIN_INTERVAL; ///< The interval between virtual pins
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_VIRTUAL_PIN_ASSIGNMER_H_
