/**
 * @file GridMapGR.h
 * @brief Map of grid cell and grid edge for global routing
 * @author anoynmous anoynmous
 * @date 10/06/2018
 */

#ifndef AROUTER_GRIDMAPGR_H_
#define AROUTER_GRIDMAPGR_H_

#include "GridCellGR.h"
#include "GridEdgeGR.h"
#include "util/Vector3D.h"
#include "util/Vector2D.h"
#include "util/Interval.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::GridMapGR
/// @brief Map of grid cells and grid edges for global routing
class GridMapGR
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit GridMapGR() = default;
        /// @brief constructor with x, y, and layer size
        /// @param the number of x grid cells, the number of y grid cells, and the number of layers
        explicit GridMapGR(IndexType numX, IndexType numY, IndexType numLayers);
        /*------------------------------*/ 
        /* Initialization               */
        /*------------------------------*/ 
        /// @brief initialize the capacity for horizontal edges
        /// @param layerIdx: the layer. totalCap: the capacity of the horizontal edges
        void initHCap(IndexType layerIdx, IntType totalCap);
        /// @brief initialize the capacity for vertical edges
        /// @param layerIdx: the layer. totalCap: the capacity of the horizontal edges
        void initVCap(IndexType layerIdx, IntType totalCap);
        /// @brief initialize the capacity for via edges
        /// @param layerIdx: the lower layer. totalCap: the capacity of the horizontal edges
        void initViaCap(IndexType layerIdx, IntType totalCap);
        /// @brief initialize the 2D maps
        void init2D();
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _gridCellMap
        /// @return const reference to the grid cells
        const Vector3D<GridCellGR> &        gridCells() const         { return  _gridCellMap; }
        /// @brief get _gridCellMap
        /// @return reference to the grid cells
        Vector3D<GridCellGR> &              gridCells()               { return  _gridCellMap; }
        /// @brief get _viaEdgeMap
        /// @return const reference to the via edges between the layers
        const Vector3D<GridEdgeGR> &        viaEdges() const          { return _viaEdgeMap; }
        /// @brief get _viaEdgeMap
        /// @return reference to the via edges between the layers
        Vector3D<GridEdgeGR> &              viaEdges()                { return _viaEdgeMap; }
        /// @brief get _horizontalEdgeMap
        /// @return const reference to the horizontal edges on the same layer
        const Vector3D<GridEdgeGR> &        horizontalEdges() const   { return _horizontalEdgeMap; }
        /// @brief get _horizontalEdgeMap
        /// @return reference to the horizontal edges on the same layer
        Vector3D<GridEdgeGR> &              horizontalEdges()         { return _horizontalEdgeMap; }
        /// @brief get _verticalEdgeMap
        /// @return const reference to the vertical edges on the same layer
        const Vector3D<GridEdgeGR> &        verticalEdges() const     { return _verticalEdgeMap; }
        /// @brief get _verticalEdgeMap
        /// @return reference to the vertical edges on the same layer
        Vector3D<GridEdgeGR> &              verticalEdges()           { return _verticalEdgeMap; }
        /// @brief get _gridCellMap2D
        /// @return the 2D gridcell map
        const Vector2D<GridCellGR> &        gridCells2D() const       { return  _gridCellMap2D; }
        /// @brief get _gridCellMap2D
        /// @return the 2D gridcell map
        Vector2D<GridCellGR> &              gridCells2D()             { return  _gridCellMap2D; }
        /// @brief get _hEdgeMap2D
        /// @return the horizontal edges on the 2D grid
        const Vector2D<GridEdgeGR> &        horizontalEdges2D() const { return _hEdgeMap2D; }
        /// @brief get _hEdgeMap2D
        /// @return the horizontal edges on the 2D grid
        Vector2D<GridEdgeGR> &              horizontalEdges2D()       { return _hEdgeMap2D; }
        /// @brief get _vEdgeMap2D
        /// @return the vertical edges on the 2D grid
        const Vector2D<GridEdgeGR> &        verticalEdges2D() const   { return _vEdgeMap2D; }
        /// @brief get _vEdgeMap2D
        /// @return the vertical edges on the 2D grid
        Vector2D<GridEdgeGR> &              verticalEdges2D()         { return _vEdgeMap2D; }
        /// @brief get _gridWidth
        /// @return the width of a grid
        LocType gridWidth() const { return _gridWidth; }
        /// @brief get _gridHeight
        /// @return the height of a grid
        LocType gridHeight() const { return _gridHeight; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set whether in 2D mode
        /// @param bool: whether in 2D mode
        void setMode2D(bool mode2D) { _mode2D = mode2D; }
        /// @brief set _gridWidth
        /// @param width of a grid
        void setGridWidth(LocType gridWidth) { _gridWidth = gridWidth; }
        /// @brief get _gridHeight
        /// @param height of a grid
        void setGridHeight(LocType gridHeight) { _gridHeight =gridHeight; }
        /*------------------------------*/ 
        /* sizes...                     */
        /*------------------------------*/ 
        /// @brief number of layers
        /// @return number of metal/masterslice layers
        IndexType numLayers() const   { return _gridCellMap.zSize(); }
        /// @brief number of x grid cells
        /// @return the number of x grid cells on a layer
        IndexType numX() const        { return _gridCellMap.xSize(); }
        /// @brief the number of y grid cells
        /// @return the number of y grid cells on a layer
        IndexType numY() const        { return _gridCellMap.ySize(); }
        /// @brief the number of grid cells
        /// @return the number of grid cells
        IndexType numGCs() const      { return _gridCellMap.size(); }
        /// @brief the number of grid cells on a layer
        /// @return the number of grid cells on a layer
        IndexType numLayerGCs() const { return numX() * numY(); }
        /*------------------------------*/ 
        /* Single cell/edge acess       */
        /*------------------------------*/ 
        /// @brief get the grid cell
        /// @param the index of the gridCell XYZ<IndexType>(xIdx, yIdx, zIdx)
        /// @return const reference to grid cell
        const GridCellGR &              gridCell(const XYZ<IndexType> &xyzIdx) const         { if (_mode2D) {return _gridCellMap2D.at(xyzIdx.xy()); } return _gridCellMap.at(xyzIdx); }
        /// @brief get the grid cell
        /// @param the index of the gridCell XYZ<IndexType>(xIdx, yIdx, zIdx)
        /// @return reference to grid cell
        GridCellGR &                    gridCell(const XYZ<IndexType> &xyzIdx)               { if (_mode2D) {return _gridCellMap2D.at(xyzIdx.xy()); } return _gridCellMap.at(xyzIdx); }
        /// @brief get the grid cell 2D version
        /// @param the index of the gridCell XY<IndexType>(xIdx, yIdx)
        /// @return const reference to grid cell
        const GridCellGR &              gridCell(const XY<IndexType> &xyIdx) const           { Assert(_mode2D); return _gridCellMap2D.at(xyIdx);   }
        /// @brief get the grid cell 2D version
        /// @param the index of the gridCell XY<IndexType>(xIdx, yIdx, zIdx)
        /// @return reference to grid cell
        GridCellGR &                    gridCell(const XY<IndexType> &xyIdx)                 { Assert(_mode2D); return _gridCellMap2D.at(xyIdx);   }
        /// @brief get the via edge
        /// @param the index of the via edge XYZ<IndexType>(xIdx, yIdx, lower layerIdx)
        /// @return const referece to the viaEdge
        const GridEdgeGR &              viaEdge(const XYZ<IndexType> &xyzIdx) const          { Assert(!_mode2D); return _viaEdgeMap.at(xyzIdx); }
        /// @brief get the via edge
        /// @param the index of the via edge XYZ<IndexType>(xIdx, yIdx, lower layerIdx)
        /// @return referece to the viaEdge
        GridEdgeGR &                    viaEdge(const XYZ<IndexType> &xyzIdx)                { Assert(!_mode2D); return _viaEdgeMap.at(xyzIdx); }
        /// @brief get the horizontal grid edge
        /// @param the index of the horizontal grid edge XYZ<IndexType>(lower x index, y index, layer index)
        /// @return const reference to the horizontal grid edge
        const GridEdgeGR &              horizontalEdge(const XYZ<IndexType> &xyzIdx) const   { if (_mode2D) { return _hEdgeMap2D.at(xyzIdx.xy()); } return _horizontalEdgeMap.at(xyzIdx); }
        /// @brief get the horizontal grid edge
        /// @param the index of the horizontal grid edge XYZ<IndexType>(lower x index, y index, layer index)
        /// @return reference to the horizontal grid edge
        GridEdgeGR &                    horizontalEdge(const XYZ<IndexType> &xyzIdx)         { if (_mode2D) { return _hEdgeMap2D.at(xyzIdx.xy()); } return _horizontalEdgeMap.at(xyzIdx); }
        /// @brief get the horizontal grid edge (2D version)
        /// @param the index of the horizontal grid edge XY<IndexType>(lower x index, y index)
        /// @return const reference to the horizontal grid edge
        const GridEdgeGR &              horizontalEdge(const XY<IndexType> &xyIdx) const     { Assert(_mode2D);  return _hEdgeMap2D.at(xyIdx);  }
        /// @brief get the horizontal grid edge (2D version)
        /// @param the index of the horizontal grid edge XY<IndexType>(lower x index, y index)
        /// @return reference to the horizontal grid edge
        GridEdgeGR &                    horizontalEdge(const XY<IndexType> &xyIdx)           { Assert(_mode2D);  return _hEdgeMap2D.at(xyIdx);  }
        /// @brief get the vertical grid edge
        /// @param the index of the vertical grid edge XYZ<IndexType>(x index, lower y index, layer index)
        /// @return const reference to the vertical grid edge
        const GridEdgeGR &              verticalEdge(const XYZ<IndexType> &xyzIdx) const     { if (_mode2D) { return _vEdgeMap2D.at(xyzIdx.xy()); } return _verticalEdgeMap.at(xyzIdx); }
        /// @brief get the vertical grid edge
        /// @param the index of the vertical grid edge XYZ<IndexType>(x index, lower y index, layer index)
        /// @return const reference to the vertical grid edge
        GridEdgeGR &                    verticalEdge(const XYZ<IndexType> &xyzIdx)           { if (_mode2D) { return _vEdgeMap2D.at(xyzIdx.xy()); } return _verticalEdgeMap.at(xyzIdx); }
        /// @brief get the vertical grid edge (2D version)
        /// @param the index of the vertical grid edge XY<IndexType>(x index, lower y index)
        /// @return const reference to the vertical grid edge
        const GridEdgeGR &              verticalEdge(const XY<IndexType> &xyIdx) const       { Assert(_mode2D);  return _vEdgeMap2D.at(xyIdx); }
        /// @brief get the vertical grid edge (2D version)
        /// @param the index of the vertical grid edge XY<IndexType>(x index, lower y index)
        /// @return reference to the vertical grid edge
        GridEdgeGR &                    verticalEdge(const XY<IndexType> &xyIdx)             { Assert(_mode2D);  return _vEdgeMap2D.at(xyIdx); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return const reference tothe grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const Interval<XYZ<IndexType>> &edge) const { return gridEdge(edge.begin(), edge.end()); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const Interval<XYZ<IndexType>> &edge)       { return gridEdge(edge.begin(), edge.end()); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return const reference tothe grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const Interval<XY<IndexType>> &edge) const  { return gridEdge(edge.begin(), edge.end()); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return const reference tothe grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const Interval<XY<IndexType>> &edge)        { return gridEdge(edge.begin(), edge.end()); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param two XYZ<IndexType> represent the index of the two terminals of the grid cells
        /// @return const reference to the grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const XYZ<IndexType> &node1, const XYZ<IndexType> &node2) const;
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param two XYZ<IndexType> represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const XYZ<IndexType> &node1, const XYZ<IndexType> &node2);
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param two XY<IndexType> represent the index of the two terminals of the grid cells
        /// @return const reference to the grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const XY<IndexType> &node1, const XY<IndexType> &node2) const;
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param two XY<IndexType> represent the index of the two terminals of the grid cells
        /// @return const reference to the grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const XY<IndexType> &node1, const XY<IndexType> &node2);
        /*------------------------------*/ 
        /* Routing operations           */
        /*------------------------------*/ 
        /// @brief drop a subnet on a edge
        /// @param subNetIdx: the index of subnet to be dropped. IntType useCap: the amount of capacity it use node1, node2: a XYZ<IndexType> to determine the edge.
        /// @return bool. true: the insert success, false: the subNet already exists in the edge
        bool dropSubNet(IndexType subNetIdx, IntType useCap, const XYZ<IndexType> &node1, const XYZ<IndexType> &node2);
        /// @brief drop a subnet on a edge (2D version)
        /// @param subNetIdx: the index of subnet to be dropped. IntType useCap: the amount of capacity it use node1, node2: a XY<IndexType> to determine the edge.
        /// @return bool. true: the insert success, false: the subNet already exists in the edge
        bool dropSubNet(IndexType subNetIdx, IntType useCap, const XY<IndexType> &node1, const XY<IndexType> &node2);
        /// @brief drop a subnet on a edge
        /// @param subNetIdx: the index of subnet to be dropped. IntType useCap: the amount of capacity it use edge: a Interval<XYZ<IndexType>> to determine the edge.
        /// @return bool. true: the insert success, false: the subNet already exists in the edge
        bool dropSubNet(IndexType subNetIdx, IntType useCap, const Interval<XYZ<IndexType>> &edge)  { return dropSubNet(subNetIdx, useCap, edge.begin(), edge.end()); }
        /// @brief drop a subnet on a edge (2D version)
        /// @param subNetIdx: the index of subnet to be dropped. IntType useCap: the amount of capacity it use edge: a Interval<XY<IndexType>> to determine the edge.
        /// @return bool. true: the insert success, false: the subNet already exists in the edge
        bool dropSubNet(IndexType subNetIdx, IntType useCap, const Interval<XY<IndexType>> &edge)   { return dropSubNet(subNetIdx, useCap, edge.begin(), edge.end()); }
        /// @brief drop a pin on a gridcell
        /// @param pinIdx: the index of a pin. node: XYZ<IndexType> the index of the grid cell
        /// @return bool. true: the insert success. false: the pinIdx already exists in the grid cell
        bool dropPin(IndexType pinIdx, const XYZ<IndexType> &node)                                  { return this->gridCell(node).dropPin(pinIdx); }
        /// @brief drop a pin on a gridcell (2D version)
        /// @param pinIdx: the index of a pin. node: XY<IndexType> the index of the grid cell
        /// @return bool. true: the insert success. false: the pinIdx already exists in the grid cell
        bool dropPin(IndexType pinIdx, const XY<IndexType> &node)                                   { return this->gridCell(node).dropPin(pinIdx); }
        /// @brief drop a blockage on a gridcell
        /// @param  node: XYZ<IndexType> the index of the grid cell
        /// @return bool. true: the insert success. false: the grid cell has already been blocked
        bool dropBlockage(const XYZ<IndexType> &node)                                               { return this->gridCell(node).dropBlockage(); }
        /// @brief drop a blockage on a gridcell (2D version)
        /// @param  node: XY<IndexType> the index of the grid cell
        /// @return bool. true: the insert success. false: the grid cell has already been blocked
        bool dropBlockage(const XY<IndexType> &node)                                                { return this->gridCell(node).dropBlockage(); }
        /// @brief erase a subnet on a edge
        /// @param subNetIdx: the index of the subnet to be erased. useCap: the amount of the capacity the subnet use. node1, node2: a XYZ<IndexType> to determine the edge.
        /// @return bool. true: the erase success. false: the subnet wasn't in the edge
        bool eraseSubNet(IndexType subNetIdx, IntType useCap, const XYZ<IndexType> &node1, const XYZ<IndexType> &node2);
        /// @brief erase a subnet on a edge (2D version)
        /// @param subNetIdx: the index of the subnet to be erased. useCap: the amount of the capacity the subnet use. node1, node2: a XY<IndexType> to determine the edge.
        /// @return bool. true: the erase success. false: the subnet wasn't in the edge
        bool eraseSubNet(IndexType subNetIdx, IntType useCap, const XY<IndexType> &node1, const XY<IndexType> &node2);
        /// @brief erase a subnet on a edge
        /// @param subNetIdx: the index of the subnet to be erased. useCap: the amount of the capacity the subnet use. edge: a Interval<XYZ<IndexType>> to determine the edge.
        /// @return bool. true: the erase success. false: the subnet wasn't in the edge 
        bool eraseSubNet(IndexType subNetIdx, IntType useCap, const Interval<XYZ<IndexType>> &edge) { return eraseSubNet(subNetIdx, useCap, edge.begin(), edge.end()); }
        /// @brief erase a subnet on a edge (2D version)
        /// @param subNetIdx: the index of the subnet to be erased. useCap: the amount of the capacity the subnet use. edge: a Interval<XY<IndexType>> to determine the edge.
        /// @return bool. true: the erase success. false: the subnet wasn't in the edge 
        bool eraseSubNet(IndexType subNetIdx, IntType useCap, const Interval<XY<IndexType>> &edge)  { return eraseSubNet(subNetIdx, useCap, edge.begin(), edge.end()); }
        /// @brief erase a pin from a grid cell
        /// @param node: XYZ<IndexType> the index of the grid cell
        /// @return bool. True: the erase success. False: the pin wasn't in the gridcell
        bool erasePin(IndexType pinIdx, const XYZ<IndexType> &node)                                 { return this->gridCell(node).erasePin(pinIdx); }
        /// @brief erase a pin from a grid cell (2D version)
        /// @param node: XYZ<IndexType> the index of the grid cell
        /// @return bool. True: the erase success. False: the pin wasn't in the gridcell
        bool erasePin(IndexType pinIdx, const XY<IndexType> &node)                                  { return this->gridCell(node).erasePin(pinIdx); }
        /// @brief erase a blockage from a grid cell
        /// @param node: XY<IndexType>: the index of the grid cell
        /// @return bool. True: the erase success. False: the gridcell wasn't being blocked
        bool eraseBlockage(const XYZ<IndexType> &node)                                              { return this->gridCell(node).eraseBlockage(); }
        /// @brief erase a blockage from a grid cell (2D version)
        /// @param node: XY<IndexType>: the index of the grid cell
        /// @return bool. True: the erase success. False: the gridcell wasn't being blocked
        bool eraseBlockage(const XY<IndexType> &node)                                               { return this->gridCell(node).eraseBlockage(); }

    private:
        Vector3D<GridCellGR> _gridCellMap; ///< the map for grid cells 
        Vector3D<GridEdgeGR> _viaEdgeMap; ///< the map for the viaEdge _viaEdgeMap[lower layer][x index][y index] = GridEdgeGR()
        Vector3D<GridEdgeGR> _horizontalEdgeMap; ///< the map for the horizontal edge on a layer _horizontalEdgeMap[layer index][lower x index][y index] = GridEdgeGR()
        Vector3D<GridEdgeGR> _verticalEdgeMap; ///< the map for the vertical edge on a layer _verticalEdgeMap[layer index][x index][lower y index] = GridEdgeGR()
        bool _mode2D = false;  /// True: return the 2D cells and edges
        Vector2D<GridCellGR> _gridCellMap2D; ///< the version for 2D map
        Vector2D<GridEdgeGR> _hEdgeMap2D; ///< the horizontal edges in 2D mode
        Vector2D<GridEdgeGR> _vEdgeMap2D; ///< the vertical edges in 2D mode
        LocType _gridHeight = 1; ///< height of a grid
        LocType _gridWidth = 1; ///< width of a grid
};

inanoynmouse bool GridMapGR::dropSubNet(IndexType subNetIdx, IntType useCap, const XYZ<IndexType> &node1, const XYZ<IndexType> &node2)
{
    auto &edge = this->gridEdge(node1, node2);
    return edge.occupy(subNetIdx, useCap);
}

inanoynmouse bool GridMapGR::dropSubNet(IndexType subNetIdx, IntType useCap, const XY<IndexType> &node1, const XY<IndexType> &node2)
{
    auto &edge = this->gridEdge(node1, node2); // mode2D should be asserted inside gridEdge()
    return edge.occupy(subNetIdx, useCap);
}

inanoynmouse bool GridMapGR::eraseSubNet(IndexType subNetIdx, IntType useCap, const XYZ<IndexType> &node1, const XYZ<IndexType> &node2)
{
    auto &edge = this->gridEdge(node1, node2);
    return edge.remove(subNetIdx, useCap);
}

inanoynmouse bool GridMapGR::eraseSubNet(IndexType subNetIdx, IntType useCap, const XY<IndexType> &node1, const XY<IndexType> &node2)
{
    auto &edge = this->gridEdge(node1, node2); // mode2D should be asserted inside gridEdge()
    return edge.remove(subNetIdx, useCap);
}
PROJECT_NAMESPACE_END

#endif /// AROUTER_GRIDMAPGR_H_
