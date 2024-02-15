/**
 * @file Pin.h
 * @brief The placement pin data structure
 * @author Keren Zhu
 * @date 10/01/2019
 */

#ifndef IDEAPLACE_PIN_H_
#define IDEAPLACE_PIN_H_

#include "global/global.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::Pin
/// @brief the Pin class
class Pin
{
    public:
        /// @brief default constructor
        explicit Pin() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the index of the cell this pin belonging to
        /// @return the index of the cell this pin belonging to
        IndexType cellIdx() const { return _cellIdx; }
        /// @brief get the relative location of the pin with respect to the cell
        /// @return the relative location of the pin with respect to the cell
        const Box<LocType> & shape() const { return _shape; }
        /// @brief get the relative location of the pin with respect to the cell
        /// @return the relative location of the pin with respect to the cell
        Box<LocType> & shape() { return _shape; }
        /// @brief get the name of the pin
        /// @return the name of the pin
        const std::string & name() const { return _name; }
        /// @brief get wether this pin is a dummy pin
        /// @return whether this pin is a dummy pin
        bool isDummyPin() const { return _isDummy; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/
        /// @brief set the index of the cell this pin belonging to
        /// @param the index of the cell this pin belonging to
        void setCellIdx(IndexType cellIdx) { _cellIdx = cellIdx; }
        /// @brief set the name of the pin
        /// @param the name of the pin
        void setName(const std::string &name) { _name = name; }
        void markAsDummyPin() { _isDummy = true; }
        /*------------------------------*/ 
        /* Vector operations            */
        /*------------------------------*/ 
        /// @brief get the number of nets
        /// @return the number of nets
        IndexType numNetIdx() const { return _netIdxArray.size(); }
        /// @brief add a net index to the pin
        /// @param the net index to be added
        void addNetIdx(IndexType netIdx) { _netIdxArray.emplace_back(netIdx); }
        /// @brief get one net index that this pin has
        /// @param the index in the net indices array
        /// @return one net index in database
        IndexType netIdx(IndexType idx) const { return _netIdxArray.at(idx); }
        /*------------------------------*/ 
        /* Utils                        */
        /*------------------------------*/ 
        /// @brief get the relative middle point location of the pin with respect to the cell
        /// @return the middle point location of the pin
        XY<LocType> midLoc() const { return _shape.center(); }
    private:
        std::string _name = ""; ///< The name for the pin
        std::vector<IndexType> _netIdxArray; ///< The indices of nets connected to this pin
        IndexType _cellIdx = INDEX_TYPE_MAX; ///< The cell this pin belongs to. Each pin can belong on only one cell
        Box<LocType> _shape = Box<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN); ///< The relative location of this pin with respect to the cell
        bool _isDummy = false; ///< This is a dummy pin
};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PIN_H_
