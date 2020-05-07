/**
 * @file Net.h
 * @brief The placement net data structure
 * @author Keren Zhu
 * @date 09/30/2019
 */

#ifndef IDEAPLACE_NET_H_
#define IDEAPLACE_NET_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::Net
/// @brief the net class
class Net
{
    public:
        /// @brief default constructor
        explicit Net() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the weight of the net
        /// @return the weight of this net
        IntType weight() const { return _weight; }
        /// @brief get the name for the net
        /// @return the name of this net
        const std::string & name() const { return _name; }
        /// @brief get whether this pin is an IO pin
        bool isIo() const { return _isIo; }
        /// @brief get the virtual pin location
        /// @return the location for the virtual pin
        const XY<LocType> &virtualPinLoc() const { return _virtualPin; }
        /// @brief get whether need to consider the virtual pin: If not IO net, or if no vitual pin assigned
        bool isValidVirtualPin() const { return _isIo && _virtualPin.x() != LOC_TYPE_MIN; }
        /// @brief get whether this net is a dummy net
        /// @return whether this net is a dummy net
        bool isDummyNet() const { return _isDummy; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the weight of the net
        /// @param the weight of the net
        void setWeight(IntType weight) { _weight = weight; }
        /// @brief set the name of the net
        /// @param the name of the net
        void setName(const std::string &name) { _name = name; }
        /// @brief set whether this net is an IO net
        void setIsIo(bool isIo) { _isIo =isIo; }
        /// @brief set the virtual pin location of this net
        void setVirtualPin(const XY<LocType> &virtualPinLocation) { _virtualPin = virtualPinLocation; }
        /// @brief invalidate the virtual pin
        void invalidateVirtualPin() { _virtualPin = XY<LocType>(LOC_TYPE_MIN, LOC_TYPE_MIN); }
        /// @brief mark this net as a dummy net
        void markAsDummyNet() { _isDummy = true; }
        /*------------------------------*/ 
        /* Vector operations            */
        /*------------------------------*/ 
        /// @brief get the number of pins
        /// @return the number of pins this cell has
        IndexType numPinIdx() const { return _pinIdxArray.size(); }
        /// @brief add one pin to the cell
        /// @param the index of pin that want to be added
        void addPin(IndexType pinIdx) { _pinIdxArray.emplace_back(pinIdx); }
        /// @brief get the database pin index
        /// @param the index of pin array
        /// @return the databse pin index
        IndexType pinIdx(IndexType idx) const { return AT(_pinIdxArray, idx); }
        const std::vector<IndexType> & pinIdxArray() const { return _pinIdxArray; }
    private:
        std::string _name = ""; ///< The name for the net
        std::vector<IndexType> _pinIdxArray; ///< The index to the pins belonging to the net
        IntType _weight = 1; ///< The weight of this net
        bool _isIo = true; ///< Whether thisnet is an IO net 
        XY<LocType> _virtualPin = XY<LocType>(LOC_TYPE_MIN, LOC_TYPE_MIN); ///< The virtual pin location
        bool _isDummy = false; ///< Whether this is a dummy net
};

PROJECT_NAMESPACE_END

#endif /// IDEAPLACE_NET_H_
