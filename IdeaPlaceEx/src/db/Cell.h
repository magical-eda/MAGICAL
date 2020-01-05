/**
 * @file Cell.h
 * @brief The placement cell data structure
 * @author Keren Zhu
 * @date 09/30/2019
 */

#ifndef IDEAPLACE_CELL_H_
#define IDEAPLACE_CELL_H_

#include "global/global.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::Cell
/// @brief the cell/device/macro of the placement engine
class Cell
{
    public:
        /// @brief default constructor
        explicit Cell() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the x coordinate of the cell
        /// @return the x coordinate of the cell
        LocType xLoc() const { return _loc.x(); }
        /// @brief get the y coordinate of the cell
        /// @return the y coordinate of the cell
        LocType yLoc() const { return _loc.y(); }
        /// @brief get the x coordinate of the cell
        /// @return the x coordinate of the cell
        LocType xLo() const { return _loc.x() + _cellBBox.xLo(); }
        /// @brief get the y coordinate of the cell
        /// @return the y coordinate of the cell
        LocType yLo() const { return _loc.y() + _cellBBox.yLo(); }
        /// @brief get the xHi of the cell
        /// @return xHi of the cell
        LocType xHi() const { return _loc.x() + _cellBBox.xHi(); }
        /// @brief get the yHi of the cell
        /// @return yHi of the cell
        LocType yHi() const { return _loc.y() + _cellBBox.yHi(); }
        /// @brief get the name of the cell
        /// @return the name of the cell
        const std::string & name() const { return _name; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the x coordinate of the cell
        /// @param the x coordinate of the cell
        void setXLoc(LocType xLoc) { _loc.setX(xLoc); }
        /// @brief set the y coordinate of the cell
        /// @param the y coordinate of the cell
        void setYLoc(LocType yLoc) { _loc.setY(yLoc); }
        /// @brief set the name of the cell
        /// @param the name of the cell
        void setName(const std::string &name) { _name = name; }
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
        /// @brief allocate the bounding box array with the size of layers
        /// @param the number of layers need to be considered
        void allocateLayers(IndexType numLayers) 
        { 
            /// Initialize the bounding box to invalid shape
            _bboxArray.resize(numLayers, Box<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN)); 
        }
        /// @brief check whether there is shape in the layer
        /// @return whether the layer has shape
        bool layerHasShape(IndexType layerIdx) const { return AT(_bboxArray, layerIdx).valid(); }
        /// @brief union a bounding box in a layer
        /// @param first: the layer index
        /// @param second: the shape bounding box
        void unionBBox(IndexType layerIdx, const Box<LocType> & bbox) { AT(_bboxArray, layerIdx).unionBox(bbox); } 
        /// @brief get the bounding box of shapes in a layer
        /// @param first: the layer index
        /// @return the bounding box of the layer before offseted
        const Box<LocType> & bbox(IndexType layerIdx) const
        {
            Assert(this->layerHasShape(layerIdx)); 
            return AT(_bboxArray, layerIdx); 
        }
        /// @brief get the actual bounding box (after being offsetted by the location) of shapes in a layer
        /// @param first: the layer index
        /// @return the bounding box of the layer after offseted
        const Box<LocType> bboxOff(IndexType layerIdx) const
        {
            Assert(this->layerHasShape(layerIdx)); 
            return AT(_bboxArray, layerIdx).offsetBox(_loc);
        }
        /*------------------------------*/ 
        /* Supporting functions         */
        /*------------------------------*/ 
        /// @brief calculate the bounding box of the entire cell (union from each layer)
        void calculateCellBBox()
        {
            _cellBBox = Box<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN);
            for (const auto &bbox : _bboxArray)
            {
                _cellBBox.unionBox(bbox);
            }
            //INF("Enlarge cell boundry for 10 for spacing \n");
            _cellBBox.enlargeBy(0);
        }
        /// @brief get the bounding box of the entire cell
        /// @return the bounding box of the entire cell
        const Box<LocType> & cellBBox() const { return _cellBBox; }
        /// @brief calculate the cellBBox offset by the current location
        /// @return the cellBBox offset by the current location
        Box<LocType> cellBBoxOff() const { return _cellBBox.offsetBox(_loc); }

    private:
        std::string _name; ///< The cell name
        XY<LocType> _loc; ///< The location of the cell
        std::vector<IndexType> _pinIdxArray; ///< The index to the pins belonging to the cell
        std::vector<Box<LocType>> _bboxArray; ///< _shapeArray[layer] = the bounding box of the shapes in the layer
        Box<LocType> _cellBBox = Box<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN); ///< The bounding box of the entire cell
};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_CELL_H_
