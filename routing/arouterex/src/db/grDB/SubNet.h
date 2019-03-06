/**
 * @file SubNet.h
 * @brief RoutePath for global routing. It basically contain the route/routing/topology for a 2-pin nets
 * @author anoynmous anoynmous
 * @date 10/04/2018
 */

#ifndef AROUTER_SUBNET_H_
#define AROUTER_SUBNET_H_

#include "util/Box.h"
#include "RoutePath.h"

PROJECT_NAMESPACE_BEGIN

/// @class SubNet
/// @brief a two-pin net for global router
class SubNet
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit SubNet() = default;
        /// @brief constructor
        /// @param netIdx. sourcePinIdx. targetPinIdx
        explicit SubNet(IndexType netIdx, IndexType sourcePinIdx, IndexType targetPinIdx) : _netIdx(netIdx), _fromPinIdx(sourcePinIdx), _toPinIdx(targetPinIdx) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _netIdx
        /// @return the net index this subnet belonging to
        IndexType netIdx() const { return _netIdx; }
        /// @brief get _fromPinIdx
        /// @return the index of "source" pin
        IndexType sourcePinIdx() const { return _fromPinIdx; }
        /// @brief get _toPinIdx
        /// @return the index of "target" pin
        IndexType targetPinIdx() const { return _toPinIdx; }
        /// @brief get _gridBBox
        /// @return const reference to bounding box in unit of grid cell indices
        const Box<IndexType> & gridBBox() const { return _gridBBox; }
        /// @brief get _gridBBox
        /// @return reference to bounding box in unit of grid cell indices
        Box<IndexType> & gridBBox() { return _gridBBox; }
        /// @brief get _route
        /// @return const reference to the routing path of this subnet
        const RoutePath & route() const { return _route; }
        /// @brief get _route
        /// @return reference to the routing path of this subnet
        RoutePath & route() { return _route; }
        /// @brief get the edges of the _route
        /// @return vector of Interval edges this subnet routing through
        const std::vector<Interval<XYZ<IndexType>>> & edges() const { return _route.edgeVec(); }
        /// @brief get the edges of the _route
        /// @return vector of Interval edges this subnet routing through
        std::vector<Interval<XYZ<IndexType>>> & edges() { return _route.edgeVec(); }
        /// @brief is this subNet symmetic
        /// @return sym or not
        bool isSym() const { return  _symSubNet == INDEX_TYPE_MAX; }
        /// @brief if this subnet is routed
        /// @return if the subNet is routed
        bool routed() const { return _routed; } 
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set _netIdx
        /// @param the net index this subnet belonging to
        void setNetIdx(IndexType netIdx) { _netIdx = netIdx; } 
        /// @brief set _fromPinIdx
        /// @param the "source" pin index
        void setSourcePinIdx(IndexType sourceIdx) { _fromPinIdx = sourceIdx; }
        /// @brief set _toPinIdx
        /// @param the "target" pin index
        void setTargetPinIdx(IndexType targetIdx) { _toPinIdx = targetIdx; }
        /// @brief set _gridBBox
        /// @param the lower-left and upper-right grid index of the bounding box
        void setGridBBox(XY<IndexType> ll, XY<IndexType> ur ) { _gridBBox = Box<IndexType>(ll, ur); }
        /// @brief set _routed
        /// @param whether this subNet is routed
        void setRouted(bool routed) { _routed= routed; }
        /*------------------------------*/ 
        /* Routing operations           */
        /*------------------------------*/ 
        /// @brief rip-up the routing path
        void ripup();


    private:
        IndexType _netIdx = INDEX_TYPE_MAX; ///< The net index this subnet belonging to
        IndexType _fromPinIdx = INDEX_TYPE_MAX; ///< The index of the "source" pin
        IndexType _toPinIdx = INDEX_TYPE_MAX; ///< The index of the "target" pin
        Box<IndexType> _gridBBox = Box<IndexType>(INDEX_TYPE_MAX, INDEX_TYPE_MAX, INDEX_TYPE_MAX, INDEX_TYPE_MAX); ///< The bounding box of this subNet in terms of the grid cell index
        RoutePath _route; ///< The routing path of this 2-pin net
        IndexType _symSubNet = INDEX_TYPE_MAX; ///< the index of the subnet that is corresponding to this, INDEX_TYPE_MAX means this is not a sym subnet
        bool _routed = false; ///< if the subnet is routed
};

inanoynmouse void SubNet::ripup()
{
    _route.clear();
    _routed = false;
}

PROJECT_NAMESPACE_END
#endif ///AROUTER_SUBNET_H_
