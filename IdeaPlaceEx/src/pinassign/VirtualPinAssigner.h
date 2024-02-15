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
        explicit VirtualPinAssigner(Database &db) : _db(db) 
        {
            _virtualPinInterval = db.parameters().virtualPinInterval();
            _virtualBoundaryExtension = db.parameters().virtualBoundaryExtension();
        }
        /* Kernal interface */
        /// @brief cnfigure the virtual boundary based on databse
        void reconfigureVirtualPinLocationFromDB(IndexType iter);
        /// @brief reconfigure the virtual boundary and pin locations
        void reconfigureVirtualPinLocations(const Box<LocType> &cellsBBox);
        /// @brief solve pin assignment from information from DB
        bool pinAssignmentFromDB();
        /// @brief short cut to solve the problem from databse information
        bool solveFromDB();
        /// @brief solve the pin assignment problem. Export the solution to the database
        /// @param a function for query cell location
        bool pinAssignment(std::function<XY<LocType>(IndexType)> cellLocQueryFunc);

        /* Parameter settings */
        /// @brief set the extension distance of placement boundary to cell boundary
        void setVirtualBoundaryExtension(LocType ex) { _virtualBoundaryExtension = ex; }
        /// @brief set the interval between pins
        void setVirtualPinInterval(LocType in) { _virtualPinInterval = in; }
        /// @brief use fast mode
        void useFastMode() { _fastMode = true; }
        void useSlowMode() { _fastMode = false; }
    private:
        void assignPowerPin()
        {
#ifdef DEBUG_PINASSIGN
            DBG("Ideaplace: pinassgin: %s\n", __FUNCTION__);
#endif
            for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
            {
                auto & net = _db.net(netIdx);
                if (net.isVdd())
                {
                    _topPin.assign(netIdx);
                    net.setVirtualPin(_topPin);
                    continue;
                }
                if (net.isVss())
                {
                    _botPin.assign(netIdx);
                    net.setVirtualPin(_botPin);
                    continue;
                }
            }
#ifdef DEBUG_PINASSIGN
            DBG("Ideaplace: pinassgin: end %s\n", __FUNCTION__);
#endif
        }
        bool _lpSimplexPinAssignment(
                std::function<bool(IndexType)> isSymNetFunc,
                std::function<bool(IndexType)> isLeftPinFunc,
                std::function<bool(IndexType)> isOtherNetFunc,
                std::function<bool(IndexType)> isOtherPinFunc,
                std::function<LocType(IndexType, IndexType)> symNetToPinCostFunc,
                std::function<LocType(IndexType, IndexType)> otherNetToPinCostFunc,
                std::function<void(IndexType, IndexType)> setSymNetPairToPinFunc,
                std::function<void(IndexType, IndexType)> setOtherNetToPinFunc
                );
        bool _networkSimplexPinAssignment(
                std::function<bool(IndexType)> useNetFunc,
                std::function<bool(IndexType)> usePinFunc,
                std::function<LocType(IndexType, IndexType)> netToPinCostFunc,
                std::function<void(IndexType, IndexType)> setNetToVirtualPinFunc);
    private:
        Database &_db; ///< The placement database
        Box<LocType> _boundary; ///< The virtual boundary of the placement
        std::vector<VirtualPin> _virtualPins; ///< The locations for virtual pins
        VirtualPin _topPin; ///< The pin at top
        VirtualPin _botPin; ///< The pin at bottom
        LocType _virtualBoundaryExtension = -1; ///< The extension to placement cell bounding box
        LocType _virtualPinInterval = -1; ///< The interval between virtual pins
        std::map<IndexType, IndexType> _leftToRightMap; // _leftToRightMap[idx of left] = idx of right
        bool _fastMode = false; ///< True : use two pass MCMF. False: use simplex
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_VIRTUAL_PIN_ASSIGNMER_H_
