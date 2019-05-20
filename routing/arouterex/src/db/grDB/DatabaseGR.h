/**
 * @file DatabaseGR.h
 * @brief The databse for global router
 * @author Keren Zhu
 * @date 10/07/2018
 */

#ifndef AROUTER_DATABASEGR_H_
#define AROUTER_DATABASEGR_H_

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include "db/Pin.h"
#include "NetGR.h"
#include "SubNet.h"
#include "GridMapGR.h"



/// @brief hash function for PROJECT_NAMASPACE::Interval
namespace std
{
    template<> 
    struct hash<PROJECT_NAMESPACE::Interval<PROJECT_NAMESPACE::XYZ<PROJECT_NAMESPACE::IndexType>> >
    {
        typedef PROJECT_NAMESPACE::Interval<PROJECT_NAMESPACE::XYZ<PROJECT_NAMESPACE::IndexType>> argumentType;
        typedef std::size_t resultType;
        resultType operator()(argumentType const& s) const noexcept
        {
            resultType seed = 0;
            boost::hash_combine(seed, s.begin().x());
            boost::hash_combine(seed, s.begin().y());
            boost::hash_combine(seed, s.begin().z());
            boost::hash_combine(seed, s.end().x());
            boost::hash_combine(seed, s.end().y());
            boost::hash_combine(seed, s.end().z());
            return seed;
        }
    };
}

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::DatabaseGR
/// @brief the database for the global router
class DatabaseGR
{
    public:
        /// @brief the map for look up the edge usage and which subnets are using it
        /// MapEdge2SubNet[edge] = {subnet indices}
        typedef std::unordered_map<Interval<XYZ<IndexType>>, std::set<IndexType>> MapEdge2SubNet;
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit DatabaseGR() = default; 
        /*------------------------------*/ 
        /* Initialization                */
        /*------------------------------*/ 
        /// @brief wrapper of the initialization functions
        bool initISPD08();
        /// @brief init the pins in gridMap
        void initPinInGrid();
        /// @brief calculate the bounding box for the Net
        /// @param the index of Net
        /// @return if sucessful
        bool initNetBBox(IndexType NetIdx);
    public:
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _dieLL
        /// @return the lower-left corner location of the design
        const XY<LocType> &                     dieLL() const               { return _dieLL; }
        /// @brief get _dieLL
        /// @return the lower-left corner location of the design
        XY<LocType> &                           dieLL()                     { return _dieLL; }
        /// @brief get _dieUR
        /// @return upper-right corner location of the design
        const XY<LocType> &                     dieUR() const               { return _dieUR; }
        /// @brief get _dieUR
        /// @return upper-right corner location of the design
        XY<LocType> &                           dieUR()                     { return _dieUR; }
        /// @brief get _netArray
        /// @return const reference to the vector of nets
        const std::vector<NetGR> &              nets() const                { return  _netArray; }
        /// @brief get _netArray
        /// @return reference to the vector of nets
        std::vector<NetGR> &                    nets()                      { return  _netArray; }
        /// @brief get a net 
        /// @param the index of the net
        /// @return a net
        const NetGR &                           net(IndexType netIdx) const { return _netArray.at(netIdx); }
        /// @brief get a net 
        /// @param the index of the net
        /// @return a net
        NetGR &                                 net(IndexType netIdx)       { return _netArray.at(netIdx); }
        /// @brief get the number of nets
        /// @return number of nets
        IndexType                               numNets() const             {  return _netArray.size(); }
        /// @brief get _subNetVec
        /// @return const reference to subnets
        const std::vector<SubNet> &             subNets() const             { return _subNetVec; }
        /// @brief get _subNetVec
        /// @return reference to subNets
        std::vector<SubNet> &                   subNets()                   { return _subNetVec; }
        /// @brief get _physPinArray
        /// @return const reference to the physical pins
        const std::vector<Pin> &                phyPinArray() const         { return _physPinArray; }
        /// @brief get _phyPinArray
        /// @return reference to the physical pins
        std::vector<Pin> &                      phyPinArray()               { return _physPinArray; }
        /// @brief get _virtualPinVec
        /// @return const reference to the steiner points
        const std::vector<Pin> &                virtualPinVec() const       { return _virtualPinVec; }
        /// @brief get _virtualPinVec
        /// @return reference to the steiner points
        std::vector<Pin> &                      virtualPinVec()             { return _virtualPinVec; }
        /// @brief get a pin
        /// @param the index of the pin, the index is the overall index for both the physical pins and steiner points
        /// @return the physical pin or the steiner point
        const Pin &                             pin(IndexType pinIdx) const;
        /// @brief get a pin
        /// @param the index of the pin, the index is the overall index for both the physical pins and steiner points
        /// @return the physical pin or the steiner point
        Pin &                                   pin(IndexType pinIdx);
        /// @brief get _gridMap
        /// @return const referece to the GridMap
        const GridMapGR &                       gridMap() const       { return _gridMap; }
        /// @brief get _gridMap
        /// @return reference to the gridMap
        GridMapGR &                             gridMap()             { return _gridMap; }
        /// @brief get the width of the grid
        /// @return the width of the grid
        LocType gridWidth() const { return _gridMap.gridWidth(); }
        /// @brief get the height of the grid
        /// @return the height of the grid
        LocType gridHeight() const { return _gridMap.gridHeight(); }
        /// @brief get the number of horizontal cells in grid map
        /// @return the number of horizontal cells in the grid map
        IndexType numGridCellX() const { return _gridMap.numX(); }
        /// @brief get the number of vertical cells in the grid map
        /// @return the number of vertical cells in the grid map
        IndexType numGridCellY() const { return _gridMap.numY(); }
        /// @brief get _gridMap2D
        /// @return const reference to the 2D grid map
        //const GridMapGR &                       gridMap2D() const     { return _gridMap2D; }
        /// @brief get _gridMap2D
        /// @return reference to the 2D grid map
        //GridMapGR &                             gridMap2D()           { return _gridMap2D; }
        /// @brief get _mapEdge2SubnetsArray
        /// @return the vector of the routing edges each net use
        const std::vector<MapEdge2SubNet> &     edgeMap() const       { return _mapEdge2SubnetsArray; }
        /// @brief get _mapEdge2SubnetsArray
        /// @return the vector of the routing edges each net use
        std::vector<MapEdge2SubNet> &           edgeMap()             { return _mapEdge2SubnetsArray; }
        /*------------------------------*/ 
        /* Unit conversion              */
        /*------------------------------*/ 
        /// @brief convert the design unit to the index of global routing grid
        /// @param a design coordinate
        /// @return the indices of the global routing grid
        XY<IndexType> gridIdx(XY<LocType> loc) const 
        { 
            return XY<IndexType> (
                    (loc.x() - _dieLL.x()) / gridWidth(),
                    (loc.y() - _dieLL.y()) / gridHeight());
        } 
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set _gridWidth
        /// @param the width of a gridcell
        void setGridWidth(LocType gridWidth) { _gridMap.setGridWidth(gridWidth); }
        /// @brief set _gridHeight
        /// @param the height of a gridcell
        void setGridHeight(LocType gridHeight) { _gridMap.setGridHeight(gridHeight); }
        /*------------------------------*/ 
        /* Wrappers for grid map        */
        /*------------------------------*/ 
        /// @brief get the grid cell
        /// @param the index of the gridCell XYZ<IndexType>(xIdx, yIdx, zIdx)
        /// @return const reference to grid cell
        const GridCellGR &              gridCell(const XYZ<IndexType> &xyzIdx) const         { return _gridMap.gridCell(xyzIdx); }
        /// @brief get the grid cell
        /// @param the index of the gridCell XYZ<IndexType>(xIdx, yIdx, zIdx)
        /// @return reference to grid cell
        GridCellGR &                    gridCell(const XYZ<IndexType> &xyzIdx)               { return _gridMap.gridCell(xyzIdx); }
        /// @brief get the grid cell (2D version)
        /// @param the index of the gridCell XY<IndexType>(xIdx, yIdx)
        /// @return const reference to grid cell
        const GridCellGR &              gridCell(const XY<IndexType> &xyIdx) const         { return _gridMap.gridCell(xyIdx); }
        /// @brief get the grid cell (2D version)
        /// @param the index of the gridCell XYZ<IndexType>(xIdx, yIdx)
        /// @return reference to grid cell
        GridCellGR &                    gridCell(const XY<IndexType> &xyIdx)               { return _gridMap.gridCell(xyIdx); }
        /// @brief get the via edge
        /// @param the index of the via edge XYZ<IndexType>(xIdx, yIdx, lower layerIdx)
        /// @return const referece to the viaEdge
        const GridEdgeGR &              viaEdge(const XYZ<IndexType> &xyzIdx) const          { return _gridMap.viaEdge(xyzIdx); }
        /// @brief get the via edge
        /// @param the index of the via edge XYZ<IndexType>(xIdx, yIdx, lower layerIdx)
        /// @return referece to the viaEdge
        GridEdgeGR &                    viaEdge(const XYZ<IndexType> &xyzIdx)                { return _gridMap.viaEdge(xyzIdx); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return const reference tothe grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const Interval<XYZ<IndexType>> &edge) const { return _gridMap.gridEdge(edge); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const Interval<XYZ<IndexType>> &edge)       { return _gridMap.gridEdge(edge); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return const reference tothe grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const Interval<XY<IndexType>> &edge) const { return _gridMap.gridEdge(edge); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const Interval<XY<IndexType>> &edge)       { return _gridMap.gridEdge(edge); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param two XYZ<IndexType> represent the index of the two terminals of the grid cells
        /// @return const reference to the grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const XYZ<IndexType> &node1, const XYZ<IndexType> &node2) const { return _gridMap.gridEdge(node1, node2); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param two XYZ<IndexType> represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const XYZ<IndexType> &node1, const XYZ<IndexType> &node2) { return _gridMap.gridEdge(node1, node2); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param two XY<IndexType> represent the index of the two terminals of the grid cells
        /// @return const reference to the grid edge between the input parameter grid nodes
        const GridEdgeGR &              gridEdge(const XY<IndexType> &node1, const XY<IndexType> &node2) const { return _gridMap.gridEdge(node1, node2); }
        /// @brief get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters (2D version)
        /// @param two XYZ<IndexType> represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        GridEdgeGR &                    gridEdge(const XY<IndexType> &node1, const XY<IndexType> &node2) { return _gridMap.gridEdge(node1, node2); }
        /*------------------------------*/ 
        /* Wrappers for 2D grid map     */
        /*------------------------------*/ 
        /// @brief get the grid cell for 2D map
        /// @param the index of the gridCell XY<IndexType>(xIdx, yIdx)
        /// @return const reference to grid cell
        //const GridCellGR &              gridCell2D(const XY<IndexType> &xyIdx) const         { return _gridMap2D.gridCell(XYZ<IndexType>(xyIdx, 0)); }
        /// @brief get the grid cell for 2D map
        /// @param the index of the gridCell XY<IndexType>(xIdx, yIdx, zIdx)
        /// @return reference to grid cell
        //GridCellGR &                    gridCell2D(const XY<IndexType> &xyIdx)               { return _gridMap2D.gridCell(XYZ<IndexType>(xyIdx, 0)); }
        /// @brief For 2D map: get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return const reference tothe grid edge between the input parameter grid nodes
        //const GridEdgeGR &              gridEdge2D(const Interval<XY<IndexType>> &edge) const { return gridEdge2D(edge.begin(), edge.end()); }
        /// @brief For 2D map: get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param the Interval that begin() and end() represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        //GridEdgeGR &                    gridEdge2D(const Interval<XY<IndexType>> &edge)       { return gridEdge2D(edge.begin(), edge.end()); }
        /// @brief For 2D map: get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param two XY<IndexType> represent the index of the two terminals of the grid cells
        /// @return const reference to the grid edge between the input parameter grid nodes
        //const GridEdgeGR &              gridEdge2D(const XY<IndexType> &node1, const XY<IndexType> &node2) const { return _gridMap2D.gridEdge(XYZ<IndexType>(node1, 0), XYZ<IndexType>(node2, 0)); }
        /// @brief For 2D map: get the grid edge, whether it is horizontal and vertical should be determined by the actual parameters
        /// @param two XY<IndexType> represent the index of the two terminals of the grid cells
        /// @return reference to the grid edge between the input parameter grid nodes
        //GridEdgeGR &                    gridEdge2D(const XY<IndexType> &node1, const XY<IndexType> &node2) { return _gridMap2D.gridEdge(XYZ<IndexType>(node1, 0), XYZ<IndexType>(node2, 0)); }
        /*------------------------------*/ 
        /* Routing Information query    */
        /*------------------------------*/ 
        /// @brief whether a net has one edge
        /// @return true if the net has this edge false if not
        bool netHasEdge(IndexType netIdx, const XYZ<IndexType> &node1, const XYZ<IndexType> &node2) 
        { return edgeMap().at(netIdx).find(Interval<XYZ<IndexType>>(node1, node2)) != edgeMap().at(netIdx).end(); }
        /*------------------------------*/ 
        /* Routing operations           */
        /*------------------------------*/ 
        /// @brief add a subnet to a net
        /// @param netIdx: the index of the net. pinIdxFrom: the index of the source pin. pinIdxTo the index of the target pin
        void addSubNet(IndexType netIdx, IndexType pinIdxFrom, IndexType pinIdxTo);
        /// @brief rip-up the routing of a net. Faster than rip-up subnet one by one
        /// @param the index of the net
        void ripupNet(IndexType netIdx);
        /// @brief rip-up a subNet
        /// @param the index of the subNet
        void ripupSubNet(IndexType subNetIdx);
        /// @brief add a edge to a net. will not add to any subnet. And the capacity will not be updated
        /// @param netIdx: the index of the net. edge: Interval representing the edge
        /// @return true if success, false if the edge already exist 
        //bool addEdge2Net(IndexType netIdx, const Interval<XYZ<IndexType>> &edge);
        /// @brief add a edge to a net. will not add to any subnet. And the capacity will not be updated
        /// @param netIdx: the index of the net. node1 node2: Two nodes to represent a edge. The order of the two nodes doesn't matter
        /// @return true if success, false if the edge already exist 
        //bool addEdge2Net(IndexType netIdx, const XYZ<IndexType> &node1, const XYZ<IndexType> &node2) { return this->addEdge2Net(netIdx, Interval<XYZ<IndexType>>(node1, node2)); }
        /// @brief add a edge to subnet. The corresponding _mapEdge2SubnetsArray should also be updated. And the capacity will not be updated
        /// @param subNetIdx: the index of the subnet. edge: Interval representing the edge
        /// @return true: the insert success. false: the subnet already has this edge
        bool addEdge2SubNet(IndexType subNetIdx, const Interval<XYZ<IndexType>> &edge);
        /// @brief add a edge to subnet. The corresponding _mapEdge2SubnetsArray should also be updated. And the capacity will not be updated
        /// @param subNetIdx: the index of the subnet. node1 node2: Two nodes to represent a edge. The order of the two nodes doesn't matter
        /// @return true: the insert success. false: the subnet already has this edge
        bool addEdge2SubNet(IndexType subNetIdx, const XYZ<IndexType> &node1, const XYZ<IndexType> &node2) { return this->addEdge2SubNet(subNetIdx, Interval<XYZ<IndexType>>(node1, node2)); }
        /// @brief add a edge to subnet. The corresponding _mapEdge2SubnetsArray should also be updated. And the capacity will not be updated (2D version)
        /// @param subNetIdx: the index of the subnet. edge: Interval representing the edge
        /// @return true: the insert success. false: the subnet already has this edge
        bool addEdge2SubNet(IndexType subNetIdx, const Interval<XY<IndexType>> &edge);
        /// @brief add a edge to subnet. The corresponding _mapEdge2SubnetsArray should also be updated. And the capacity will not be updated (2D version)
        /// @param subNetIdx: the index of the subnet. node1 node2: Two nodes to represent a edge. The order of the two nodes doesn't matter
        /// @return true: the insert success. false: the subnet already has this edge
        bool addEdge2SubNet(IndexType subNetIdx, const XY<IndexType> &node1, const XY<IndexType> &node2) { return this->addEdge2SubNet(subNetIdx, Interval<XY<IndexType>>(node1, node2)); }
    private:
        XY<LocType> _dieLL; ///< the origin of the die
        XY<LocType> _dieUR; ///< The top right point of the die
        std::vector<NetGR> _netArray; ///< the fixed size vector for the nets
        std::vector<SubNet> _subNetVec; ///< the vector for the subNets(2 pin nets)
        std::vector<Pin> _physPinArray;  ///< the fixed size vector for the physical pins
        std::vector<Pin> _virtualPinVec; ///< the vector for the steiner points
        GridMapGR _gridMap; ///< the grid cells and grid edges
        std::vector<MapEdge2SubNet> _mapEdge2SubnetsArray; ///< a fixed size vector for tracking the routing edges for each net
                                                           ///< _mapEdge2SubNets[index of net][edge] = {indices of subnets occupying edge}
};

inline const Pin & DatabaseGR::pin(IndexType pinIdx) const 
{
    if (pinIdx >= _physPinArray.size())
    {
        // this is steiner point
        return _virtualPinVec.at(pinIdx - _physPinArray.size());
    }
    else
    {
        // this is a physical pin
        return _physPinArray.at(pinIdx);
    }
}

inline Pin & DatabaseGR::pin(IndexType pinIdx) 
{
    if (pinIdx >= _physPinArray.size())
    {
        // this is steiner point
        return _virtualPinVec.at(pinIdx - _physPinArray.size());
    }
    else
    {
        // this is a physical pin
        return _physPinArray.at(pinIdx);
    }
}

inline void DatabaseGR::ripupNet(IndexType netIdx)
{
    for (IndexType subNetIdx :  this->nets().at(netIdx).subNets())
    {
        for (auto &edge : _subNetVec.at(subNetIdx).edges())
        {
            /// Clear the record in grid edge
            this->gridMap().eraseSubNet(subNetIdx, this->nets().at(netIdx).capUse(), edge);
        }
    }
    /// Clear all the subNets
    for (IndexType subNetIdx : this->nets().at(netIdx).subNets())
    {
        this->subNets().at(subNetIdx).ripup();
    }
    /// Clear the overall record for the net
    this->edgeMap().at(netIdx).clear();
}

inline void DatabaseGR::ripupSubNet(IndexType subNetIdx)
{
    IndexType netIdx = this->subNets().at(subNetIdx).netIdx();
    for (auto &edge : _subNetVec.at(subNetIdx).edges())
    {
        /// Clear the record in grid edge
        this->gridMap().eraseSubNet(subNetIdx, this->nets().at(netIdx).capUse(), edge);
        /// Clear the overall record of the net
        auto &edgeRecord = this->edgeMap().at(netIdx).at(edge);
        edgeRecord.erase(subNetIdx);
        /// If the corresponding edge become empty (only this subnet is using the edge in the net), erase that edge
        if (edgeRecord.size() == 0)
        {
            this->edgeMap().at(netIdx).erase(edge);
        }
    }
    /// Clear the subnet
    this->subNets().at(subNetIdx).ripup();
}


inline bool DatabaseGR::addEdge2SubNet(IndexType subNetIdx, const Interval<XYZ<IndexType>> &edge)
{
    IndexType netIdx = this->subNets().at(subNetIdx).netIdx();
    auto &net = this->nets().at(netIdx);
    auto &subNet = this->subNets().at(subNetIdx);
    bool success = true; // Whether the insert is succeeded
    /// update the overall record
    auto edgeRecordIt = this->edgeMap().at(netIdx).find(edge);
    if (edgeRecordIt != this->edgeMap().at(netIdx).end())
    {
        // the net already use this edge. Not neccessarily this subNet
        success = edgeRecordIt->second.insert(subNetIdx).second;
    }
    else
    {
        // the whole net is not using this edge
        this->edgeMap().at(netIdx)[edge].insert(subNetIdx);
    }
    /// If already existing the edge in the subnet, do not try update the other things
    if (!success) { return success; }
    /// update the subnet->route
    subNet.route().addEdge(edge);
    /// update the grid edge
    this->gridMap().dropSubNet(subNetIdx, net.capUse(), edge);    
    return success;
}

inline bool DatabaseGR::addEdge2SubNet(IndexType subNetIdx, const Interval<XY<IndexType>> &edge)
{
    auto edgeXYZ = Interval<XYZ<IndexType>>(XYZ<IndexType>(edge.begin(), 0), XYZ<IndexType>(edge.end(), 0));
    IndexType netIdx = this->subNets().at(subNetIdx).netIdx();
    auto &net = this->nets().at(netIdx);
    auto &subNet = this->subNets().at(subNetIdx);
    bool success = true; // Whether the insert is succeeded
    /// update the overall record
    auto edgeRecordIt = this->edgeMap().at(netIdx).find(edgeXYZ);
    if (edgeRecordIt != this->edgeMap().at(netIdx).end())
    {
        // the net already use this edge. Not neccessarily this subNet
        success = edgeRecordIt->second.insert(subNetIdx).second;
    }
    else
    {
        // the whole net is not using this edge
        this->edgeMap().at(netIdx)[edgeXYZ].insert(subNetIdx);
    }
    /// If already existing, do not try update the other things
    if (!success) { return success; }
    /// update the subnet->route
    subNet.route().addEdge(edgeXYZ);
    /// update the grid edge
    this->gridMap().dropSubNet(subNetIdx, net.capUse(), edge);    
    return success;
}

PROJECT_NAMESPACE_END

#endif ///AROUTER_DATABASEGR_H_
