/**
 * @file DatabaseDR.h
 * @brief Database for detailed router
 * @author anoynmous anoynmous
 * @date 10/25/2018
 */

#ifndef AROUTER_DATABASEDR_H_
#define AROUTER_DATABASEDR_H_

#include "db/Pin.h"
#include "db/drDB/NetDR.h"
#include "db/drDB/RoughGrid.h"
#include "db/Terminal.h"

PROJECT_NAMESPACE_BEGIN

namespace DRUtil
{

    /// @brief conversion from routable layer (masterslice + metal) to all layers (masterslice + metal + cut)
    /// @param the index of the layer in routable layer
    /// @return index in all layer
    inanoynmouse IndexType routeLayer2AllLayer(IndexType routingLayerIdx)
    {
        return routingLayerIdx * 2;
    }

    /// @brief conversion from all layers(masterslice + metal + cut) to routable layer 
    /// @param the index of layer in all layers
    /// @return the index of all routable layer
    inanoynmouse IndexType allLayer2RouteLayer(IndexType allLayerIdx)
    {
        Assert(allLayerIdx % 2 == 0);
        return allLayerIdx / 2;
    }
    /// @brief conversion from the bottom metal layer (routble layer index) to the index of the CUT layer
    /// @param the routable layer index of the bottom metal layer of a CUT layer
    /// @return the index of the CUT layar in cut Layers
    inanoynmouse IndexType routeLayer2CutLayer(IndexType lowerMetalLayer)
    {
        return lowerMetalLayer;
    }
    /// @brief the combinition of box spacing, parallel run length, and width
    /// @param two boxes
    /// @return .first.first: spacing, .first.second: parallel run length, .second: pair of box width
    inanoynmouse std::pair<std::pair<LocType, LocType>, std::pair<LocType, LocType>> spacingCheckBundle(const Box<LocType> &lhs, const Box<LocType> &rhs)
    {
        LocType paraLen = 0;
        LocType spacing = 0;
        LocType width1 = 0;
        LocType width2 = 0;
        if (lhs.xLo() >= rhs.xHi() || rhs.xLo() >= lhs.xHi())
        {
            if (lhs.yLo() < rhs.yHi() && rhs.yLo() < lhs.yHi())
            {
                 paraLen = std::min(lhs.yHi(), rhs.yHi()) - std::max(lhs.yLo(), rhs.yLo());
                 spacing = std::max(lhs.xLo(), rhs.xLo()) - std::min(lhs.xHi(), rhs.xHi());
                 width1 = lhs.xLen();
                 width2 = rhs.xLen();
            }
            else 
            {
                /// If no parallel run and two shapes are disjointed, then count the spacing as the distance between one _ur and one _ll
                LocType xDif = std::min(lhs.xHi(), rhs.xHi()) - std::max(lhs.xLo(), rhs.xLo());
                LocType yDif = std::min(lhs.yHi(), rhs.yHi()) - std::max(lhs.yLo(), rhs.yLo());
                spacing =  std::hypot(xDif, yDif) + 1; ///< Conservatively add one
            }
        }
        if (lhs.yLo() >= rhs.yHi() || rhs.yLo() >= lhs.yHi())
        {
            if (lhs.xLo() < rhs.xHi() && rhs.xLo() < lhs.xHi())
            {
                paraLen = std::min(lhs.xHi(), rhs.xHi()) - std::max(lhs.xLo(), rhs.xLo());
                spacing =  std::max(lhs.yLo(), rhs.yLo()) - std::min(lhs.yHi(), rhs.yHi());
                width1 = lhs.yLen();
                width2 = rhs.yLen();
            }
            //Assert(0);
        }
        return std::make_pair(
                std::make_pair(spacing, paraLen),
                std::make_pair(width1, width2));
    }

} ///namespace DRUTil

/// @class AROUTER::DatabaseDR
/// @brief the database for detailed router
class DatabaseDR
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit DatabaseDR() = default; 
        /*------------------------------*/ 
        /* Init                         */
        /*------------------------------*/ 
        /// @brief init the data structures once the information has been parsed in
        bool init();
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _netArray
        /// @return the vector for the nets
        const std::vector<NetDR> &      nets() const { return _netArray; }
        /// @brief get _netArray
        /// @return the vector for the nets
        std::vector<NetDR> &            nets() { return _netArray; }
        /// @brief get _pinArray
        /// @return the vector for the pins
        const std::vector<Pin> &        pins() const { return _pinArray; }
        /// @brief get _pinArray
        /// @return the vector for the pins
        std::vector<Pin> &              pins() { return _pinArray; }
        /// @brief get _roughGrid
        /// @return the rough grid for the detailed router
        const RoughGrid &               roughGrid() const { return _roughGrid; }
        /// @brief get _roughGrid
        /// @return the rough grid for the detailed router
        RoughGrid &                     roughGrid() { return _roughGrid; }
        /// @brief get _terminalArray
        /// @return the vector for the terminals
        //const std::vector<Terminal> &   terminalArray() const { return _terminalArray; }
        /// @brief get _terminalArray
        /// @return the vector for the terminals
        //std::vector<Terminal> &   terminalArray() { return _terminalArray; }
        /// @brief get a net
        /// @param the index of the net
        /// @return a net
        const NetDR &                   net(IndexType netIdx) const { return _netArray.at(netIdx); }
        /// @brief get a net
        /// @param the index of the net
        /// @return a net
        NetDR &                         net(IndexType netIdx) { return _netArray.at(netIdx); }
        /// @brief get a pin
        /// @param the index of the pin
        /// @return a pin
        const Pin &                     pin(IndexType pinIdx) const { return _pinArray.at(pinIdx); }
        /// @brief get a pin
        /// @param the index of the pin
        /// @return a pin
        Pin &                           pin(IndexType pinIdx) { return _pinArray.at(pinIdx); }
        /// @brief get a terminal
        /// @param the idnex of the terminal
        /// @return a terminal
        //const Terminal &                terminal(IndexType termIdx) const { return _terminalArray.at(termIdx); }
        /// @brief get a terminal
        /// @param the idnex of the terminal
        /// @return a terminal
        //Terminal &                      terminal(IndexType termIdx) { return _terminalArray.at(termIdx); }
        /// @brief get a rough grid node
        /// @param the index of the node
        /// @return a rough grid node
        const RoughGridNode &           gridNode(IndexType x, IndexType y, IndexType layer) const { return _roughGrid.at(x, y, layer); }
        /// @brief get a rough grid node
        /// @param the index of the node
        /// @return a rough grid node
        RoughGridNode &                 gridNode(IndexType x, IndexType y, IndexType layer) { return _roughGrid.at(x, y, layer); }
        /// @brief get a rough grid node
        /// @param the index of the node
        /// @return a rough grid node
        const RoughGridNode &           gridNode(const XY<IndexType> &xy, IndexType layer) const { return _roughGrid.at(xy, layer); }
        /// @brief get a rough grid node
        /// @param the index of the node
        /// @return a rough grid node
        RoughGridNode &                 gridNode(const XY<IndexType> &xy, IndexType layer) { return _roughGrid.at(xy, layer); }
        /// @brief get a rough grid node
        /// @param the index of the node
        /// @return a rough grid node
        const RoughGridNode &           gridNode(const XYZ<IndexType> &xyz) const { return _roughGrid.at(xyz); }
        /// @brief get a rough grid node
        /// @param the index of the node
        /// @return a rough grid node
        RoughGridNode &                 gridNode(const XYZ<IndexType> &xyz) { return _roughGrid.at(xyz); }
        /// @brief get net node
        /// @param the index of node
        const NetNode &netNode(ShapeID shapeID) const { return _netArray.at(shapeID.netIdx).node(shapeID.nodeIdx); }
        /// @brief get net node
        /// @param the index of node
        NetNode &netNode(ShapeID shapeID) { return _netArray.at(shapeID.netIdx).node(shapeID.nodeIdx); }
        /// @brief get the parent node
        /// @param the net node
        /// @return its parent node
        const NetNode &parentNetNode(ShapeID shapeID) const { return _netArray.at(shapeID.netIdx).node(netNode(shapeID).parent()); }
        /// @brief get the parent node
        /// @param the net node
        /// @return its parent node
        NetNode &parentNetNode(ShapeID shapeID) { return _netArray.at(shapeID.netIdx).node(netNode(shapeID).parent()); }
        /*------------------------------*/ 
        /* Information of the design    */
        /*------------------------------*/ 
        /// @brief get the region for the die
        /// @return the region of the die
        const Box<LocType> & dieBox() const { return _roughGrid.dieBox(); }
        /// @brief get the preferred routing direction for the routable layers
        /// @return the routable layers' preferred routing direction
        const std::vector<RouteDirType> & routeDirArray() const  { return _roughGrid.routeDirArray();}
        /// @brief get the preferred routing direction for routable layer
        /// @param the index of the interested layer in the routable layer (masterslice or metal)
        /// @return the preferred routing layer of the layer of interest
        RouteDirType routeDir(IndexType routableLayerIdx) const { return _roughGrid.routeDir(routableLayerIdx); }
        /// @brief get the pitch width for the routable layers
        /// @return the routable layers' preferred routing direction
        const std::vector<LocType> & pitchArray() const { return _roughGrid.pitchArray(); }
        /// @brief get the min pitch width for routable layer
        /// @param the index of the interested layer in the routable layer (masterslice or metal)
        /// @return the min pitch width of the layer of interest
        LocType pitch(IndexType routableLayerIdx) const { return _roughGrid.pitch(routableLayerIdx); }
        /// @brief number of routable layer
        /// @return the number of masterslice + metal layers
        IndexType numLayers() const { return _roughGrid.numLayers(); }
        /// @brief number of nets
        /// @return the number of nets
        IndexType numNets() const { return _netArray.size(); }
        /// @brief number of pins
        /// @return the number of pins
        IndexType numPins() const { return _pinArray.size(); }
        /// @brief using the numNet as the index for the fixed objects not belonging to any net. determine whether the shapeID is such a object
        /// @return whether the shape is not belonging to any net
        bool isShapeBlockage(ShapeID shapeID) const { return shapeID.netIdx ==  this->numNets(); }
        /*------------------------------*/ 
        /* Global routing guide         */
        /*------------------------------*/ 
        /// @brief return if the net is assigned to a routing guide
        /// @param netIdx: the index of the net. loc: the location that want to query. layer: the index of the layer
        /// @return if the net is assigned to the given location
        bool netInGlobalRoutingGuide(IndexType netIdx, XY<LocType> loc, IndexType layer) const { return _roughGrid.netInGlobalRoutingGuide(netIdx, loc, layer); }
        /*------------------------------*/ 
        /* Query the net                */
        /*------------------------------*/ 
        /// @brief determine whether the node is existing in the node, and return the index of the node
        /// @return INDEX_TYPE_MAX if not existing, the index of the node otherwise
        IndexType existNode(IndexType netIdx, XY<LocType> loc, IndexType layer) const { return this->net(netIdx).existNode(loc, layer); }
        /// @brief determine if the input node is somewhere between two connected nodes. Return the parent and child indices
        /// @return .first = parent node index, .second = child node index. If INDEX_TYPE_MAX, then not found
        std::pair<IndexType, IndexType> betweenNodes(IndexType netIdx, XY<LocType> loc, IndexType layer) const { return this->net(netIdx).betweenNodes(loc, layer); }
        /*------------------------------*/ 
        /* Query the Shape              */
        /*------------------------------*/ 
        /// @brief is a shape a wire?
        bool shapeIsWire(ShapeID shapeID) const;
        /// @brief is a shape a VIA?
        bool shapeIsVia(ShapeID shapeID) const;
        /// @brief is a shape a fixed object?
        bool shapeIsFixed(ShapeID shapeID) const;

    private:
        std::vector<NetDR> _netArray; ///< The vector for nets
        std::vector<Pin> _pinArray;  ///< The vector for physical pins
        RoughGrid _roughGrid; ///< A rough grid. Global routing results and other geometric information are transform into this
        //std::vector<Terminal> _terminalArray; ///< The vector for terminal (collection of pins)
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_DATABASEDR_H_
