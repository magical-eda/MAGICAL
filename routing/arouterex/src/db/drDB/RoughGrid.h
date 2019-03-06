/**
 * @file RoughGrid.h
 * @brief The rough grid, could be the same as the global routing grid, and could be not
 * @author anoynmous anoynmous
 * @date 10/26/2018
 */

#ifndef AROUTER_ROUGHGRID_H_
#define AROUTER_ROUGHGRID_H_

#include "global/global.h"
#include "util/Vector2D.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::RoughGridNode
/// @brief the grid node to be stored in the rough grid
class RoughGridNode
{
    public:
        /// @brief default constructor
        explicit RoughGridNode() = default;
        /*------------------------------*/ 
        /* Setting up                   */
        /*------------------------------*/ 
        /// @brief add net in global routing guide, keep the ordering but don't check the penalty
        /// @param net index 
        void addGlobalRoutingGuide(IndexType netIdx);
        /// @brief sort the global routing guide
        void sortGlobalRoutingGuide() { std::sort(_netGR.begin(), _netGR.end()); }
        /// @brief add net to penalty list
        /// @param net index
        void addGlobalRoutingPenalty(IndexType netIdx);
        /// @brief add a net in global routing guide, checking the existence in routing guide and routing guide penalty, and maintain the order in the list
        /// @param the index of net
        void addGlobalRoutingGuideSafe(IndexType netIdx);
        /// @brief sort the global routing penalty list
        void sortGlobalRoutingPenalty() { std::sort(_netGRPenalty.begin(), _netGRPenalty.end()); }
        /*------------------------------*/ 
        /* Operations                   */
        /*------------------------------*/ 
        /// @brief find if the net is assigned to this grid in global routing
        /// @oaran the index of net
        /// @return if this net is assigned to this grid in global routing guide
        bool netInGlobalRoutingGuide(IndexType netIdx) const;
        /// @brief find if the net is assigned in the penalty list in global routing
        /// @param the index of net
        /// @return find if the net is assigned in the penalty list in global routing
        bool netInGlobalRoutingPenalty(IndexType netIdx) const;
    private:
        std::vector<IndexType> _netGR; ///< The nets that are assigned to this grid in global routing guide. Keep it sorted
        std::vector<IndexType> _netGRPenalty; ///< The nets that can goes here, but should be penaltied
};

inanoynmouse bool RoughGridNode::netInGlobalRoutingGuide(IndexType netIdx) const 
{
    return std::binary_search(_netGR.begin(), _netGR.end(), netIdx);
}

inanoynmouse bool RoughGridNode::netInGlobalRoutingPenalty(IndexType netIdx) const
{
    return std::binary_search(_netGRPenalty.begin(), _netGRPenalty.end(), netIdx);
}

inanoynmouse void RoughGridNode::addGlobalRoutingPenalty(IndexType netIdx)
{
    if (!netInGlobalRoutingGuide(netIdx))
    {
        
        // using find and iterator may be dangerous (not sure?)
        /// Use anoynmousear search, and maintain the ordering
        IndexType idx = 0;
        for (; idx < _netGRPenalty.size();)
        {
            if (_netGRPenalty.at(idx) > netIdx)
            {
                // Not found, and should insert here
                break;
            }
            if (_netGRPenalty.at(idx) == netIdx)
            {
                // Do not insert anything
                return;
            }
            idx ++; 
        }
        // If enter here then did not find the netIdx, insert the netIdx here
        _netGRPenalty.insert(_netGRPenalty.begin() + idx, netIdx);
    }
}

inanoynmouse void RoughGridNode::addGlobalRoutingGuide(IndexType netIdx)
{
    // using find and iterator may be dangerous (not sure?)
    /// Use anoynmousear search, and maintain the ordering
    IndexType idx = 0;
    for (; idx < _netGR.size();)
    {
        if (_netGR.at(idx) > netIdx)
        {
            // Not found, and should insert here
            break;
        }
        if (_netGR.at(idx) == netIdx)
        {
            // Do not insert anything
            return;
        }
        idx ++; 
    }
    // If enter here then did not find the netIdx, insert the netIdx here
    _netGR.insert(_netGR.begin() + idx, netIdx);
}

inanoynmouse void RoughGridNode::addGlobalRoutingGuideSafe(IndexType netIdx)
{
    /// First search the global routing guide and keep the order
    this->addGlobalRoutingGuide(netIdx);
    /// Then check the penalty, if existed, then remove it
    auto iter = std::find(_netGRPenalty.begin(), _netGRPenalty.end(), netIdx);
    if (iter != _netGRPenalty.end())
    {
        // erase it (maintain the remaining order)
        _netGRPenalty.erase(iter);
    }
}

/// @class AROUTER::RoughGridNode
/// @brief a much rougher grid for store some information for the detailed router
class RoughGrid
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        explicit RoughGrid() = default;
        /// @brief constructor
        /// @param dieBox: the boundary of the die nodeWidth: the width of each grid node. nodeHeight: the height of each grid node. numLayers: number of layers
        explicit RoughGrid(const Box<LocType> &dieBox, LocType nodeWidth, LocType nodeHeight, IndexType numLayers);
        /*------------------------------*/ 
        /* Design information           */
        /*------------------------------*/ 
        /// @brief add one routing layer preferred direction 
        /// @param the routing direction
        void addRouteDir(RouteDirType routeDir) { _routeDirArray.emplace_back(routeDir); }
        /// @brief add the pitch size for one layer
        /// @param the pitch size 
        void addPitch(LocType pitch) { _pitchArray.emplace_back(pitch); } 
        /*------------------------------*/ 
        /* Setting up                   */
        /*------------------------------*/ 
        /// @brief finalize the things need to be set up
        void init();
        /*------------------------------*/ 
        /* Global routing guide         */
        /*------------------------------*/ 
        /// @brief return if the net is assigned to a routing guide
        /// @param netIdx: the index of the net. loc: the location that want to query. layer: the index of the layer
        /// @return if the net is assigned to the given location
        bool netInGlobalRoutingGuide(IndexType netIdx, XY<LocType> loc, IndexType layer) const;
        /// @brief return if the net is assigned to a penalty list
        /// @param netIdx: the index of the net. loc: the location that want to query. layer: the index of the layer
        /// @return if the net is assigned to the given location
        bool netInGlobalRoutingPenalty(IndexType netIdx, XY<LocType> loc, IndexType layer) const;
        /// @brief add a point to global routing solution for one net
        /// @param netIdx: the index of net. loc: location in database unit. layer: routable layer index
        void addLocationToGlobalRouting(IndexType netIdx, XY<LocType> loc, IndexType layer);
        /*------------------------------*/ 
        /* Getter                       */
        /*------------------------------*/ 
        /// @brief get _grid
        /// @return the 3D grid
        const std::vector<Vector2D<RoughGridNode>> &    grid() const                                        { return _grid; }
        /// @brief get _grid
        /// @return the 3D grid
        std::vector<Vector2D<RoughGridNode>> &          grid()                                              { return _grid; }
        /// @brief get a RoughGridNode
        /// @param the index of the node
        /// @return a node
        const RoughGridNode &                           at(IndexType x, IndexType y, IndexType layer) const { return  _grid.at(layer).at(x, y); }
        /// @brief get a RoughGridNode
        /// @param the index of the node
        /// @return a node
        RoughGridNode &                                 at(IndexType x, IndexType y, IndexType layer)       { return  _grid.at(layer).at(x, y); }
        /// @brief get a RoughGridNode
        /// @param the index of the node
        /// @return a node
        const RoughGridNode &                           at(const XY<IndexType> &xy, IndexType layer) const  { return  _grid.at(layer).at(xy); }
        /// @brief get a RoughGridNode
        /// @param the index of the node
        /// @return a node
        RoughGridNode &                                 at(const XY<IndexType> &xy, IndexType layer)        { return  _grid.at(layer).at(xy); }
        /// @brief get a RoughGridNode
        /// @param the index of the node
        /// @return a node
        const RoughGridNode &                           at(const XYZ<IndexType> &xyz) const                 { return  _grid.at(xyz.z()).at(xyz.xy()); }
        /// @brief get a RoughGridNode
        /// @param the index of the node
        /// @return a node
        RoughGridNode &                                 at(const XYZ<IndexType> &xyz)                       { return  _grid.at(xyz.z()).at(xyz.xy()); }
        /// @brief get _dieBox
        /// @return the boundary of the die
        const Box<LocType> &                            dieBox() const                                      { return _dieBox; }
        /// @brief get _dieBox
        /// @return the boundary of the die
        Box<LocType> &                                  dieBox()                                            { return _dieBox; }
        /// @brief get _tileWidth
        /// @return the width of a node
        LocType                                         tileWidth() const                                   { return _tileWidth; }
        /// @brief get _tileHeight
        /// @return the height of a node
        LocType                                         tileHeight() const                                  { return _tileHeight; }
        /// @brief get the xSize of the rough grid size
        /// @return the xSize of the rough grid size
        IndexType                                       sizeX() const                                       { return _grid.at(0).xSize(); }
        /// @brief get the ySize of the rough grid size
        /// @return the ySize of the rough grid size
        IndexType                                       sizeY() const                                       { return _grid.at(0).ySize(); }
        /// @brief get the number of layer
        /// @return the number of layers
        IndexType                                       numLayers() const                                   { return _grid.size(); }
        /// @brief get _routeDir
        /// @return the vector of the routing direction for each layer
        const std::vector<RouteDirType> &               routeDirArray() const                               { return _routeDirArray; }
        /// @brief get the routing direction of one layer
        /// @param the index of layer (routable layer, masterslice and metal)
        /// @return the preferred routing direction of that layer
        RouteDirType                                    routeDir(IndexType layerIdx) const                  { return _routeDirArray.at(layerIdx); }
        /// @brief get _pitchArray
        /// @return the vector of the pitch width for each routable layer
        const std::vector<LocType> &                    pitchArray() const                                  { return _pitchArray; }
        /// @brief get pitch width for one layer
        /// @return the pitch width of the layer of interest
        LocType                                         pitch(IndexType layerIdx) const                     { return _pitchArray.at(layerIdx); }
        /// @brief get the offset of the tracks for one layer
        /// @param index of routable layer
        /// @return the offset of the tracks of one layer
        LocType                                         trackOffset(IndexType routeLayer) const             { return _trackOffSetArray.at(routeLayer); }
        /// @brief get the track offset array for setting
        /// @return the track offset array
        void                                            addTrackOffset(LocType trackOffset)                 { _trackOffSetArray.emplace_back(trackOffset); }
        /*------------------------------*/ 
        /* Location to indices          */
        /*------------------------------*/ 
        /// @brief get the box as location of a node
        /// @param the indices of the node
        /// @return the location/shape of a node
        Box<LocType> nodeBox(IndexType x, IndexType y) const;
        /// @brief get the box as location of a node
        /// @param the indices of the node
        /// @return the location/shape of a node
        Box<LocType> nodeBox(XY<IndexType> xy) const { return nodeBox(xy.x(), xy.y()); }
        /// @brief get the indices of the node corresponding to a point
        /// @param the location of the point
        /// @return the index of the node containning the point
        XY<IndexType> nodeIdx(LocType x, LocType y) const;
        /// @brief get the indices of the node corresponding to a point
        /// @param the location of the point
        /// @return the index of the node containning the point
        XY<IndexType> nodeIdx(const XY<LocType> &xyLoc) const { return nodeIdx(xyLoc.x(), xyLoc.y()); }


    private:
        std::vector<Vector2D<RoughGridNode>> _grid; ///< the 3D grid map
        Box<LocType> _dieBox = Box<LocType>(-1, -1, -1 ,-1); ///< The boundary of the die
        LocType _tileWidth = -1; ///< width of each node
        LocType _tileHeight = -1; ///< height of each node
        std::vector<RouteDirType> _routeDirArray; ///< The routing direction
        std::vector<LocType> _pitchArray; ///< the pitch size for each layer
        std::vector<LocType> _trackOffSetArray; ///< The offset for the tracks
};

inanoynmouse RoughGrid::RoughGrid(const Box<LocType> &dieBox, LocType nodeWidth, LocType nodeHeight, IndexType numLayers)
{
    /// Set the basic design geometry
    _dieBox = dieBox;
    _tileWidth = nodeWidth;
    _tileHeight = nodeHeight;

    /// Set up the grid
    _grid.resize(numLayers);
    IndexType xSize = dieBox.xLen() / nodeWidth + 1;
    IndexType ySize = dieBox.yLen() / nodeHeight + 1;
    for (auto &grid : _grid)
    {
        grid.resize(xSize, ySize);
    }
}

inanoynmouse Box<LocType> RoughGrid::nodeBox(IndexType x, IndexType y) const 
{
    LocType xLo = x * _tileWidth + _dieBox.xLo();
    LocType yLo = y  * _tileHeight + _dieBox.yLo();
    return Box<LocType> (xLo, yLo, xLo + _tileWidth, yLo + _tileHeight);
}

inanoynmouse XY<IndexType> RoughGrid::nodeIdx(LocType x, LocType y) const
{
    return XY<IndexType> ((x - _dieBox.xLo()) / _tileWidth, ( y -_dieBox.yLo()) / _tileHeight);
}

inanoynmouse void RoughGrid::init() 
{
    /// Finalizing the global routing guide
    for (auto &gridLayer : _grid)
    {
        for (auto &gridNode : gridLayer)
        {
            /// Sorting the nets so that can do binary search
            gridNode.sortGlobalRoutingGuide();
            gridNode.sortGlobalRoutingPenalty();
        }
    }
}

inanoynmouse bool RoughGrid::netInGlobalRoutingGuide(IndexType netIdx, XY<LocType> loc, IndexType layer) const
{
    if (layer >= this->numLayers())
    {
        return false;
    }
    return this->at(this->nodeIdx(loc), layer).netInGlobalRoutingGuide(netIdx);
}

inanoynmouse bool RoughGrid::netInGlobalRoutingPenalty(IndexType netIdx, XY<LocType> loc, IndexType layer) const
{
    if (layer >= this->numLayers())
    {
        return false;
    }
    return this->at(this->nodeIdx(loc), layer).netInGlobalRoutingPenalty(netIdx);
}

inanoynmouse void RoughGrid::addLocationToGlobalRouting(IndexType netIdx, XY<LocType> loc, IndexType layer)
{
    /// Calculate the indices of grids for this location
    auto nodeIdx = this->nodeIdx(loc);
    /// Add the net to the global routing guide and use the safe operation
    this->grid().at(layer).at(nodeIdx).addGlobalRoutingGuideSafe(netIdx);
}
PROJECT_NAMESPACE_END

#endif ///AROUTER_ROUGHGRID_H_
