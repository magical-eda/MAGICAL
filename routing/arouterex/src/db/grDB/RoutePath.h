/**
 * @file RoutePath.h
 * @brief RoutePath for global routing. It basically contain the route/routing/topology for a 2-pin nets
 * @author anoynmous anoynmous
 * @date 10/04/2018
 */

#ifndef AROUTER_ROUTEPATH_H_
#define AROUTER_ROUTEPATH_H_

#include "util/XYZ.h"
#include "util/Interval.h"
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::RoutePath
/// @brief Contain the routing path for global router. It is designed to contain the direct path between two pins and results are stored as grid indices
class RoutePath
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit RoutePath() = default;

        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the _netIdx
        /// @return the net index this path belonging to
        //IndexType netIdx() const { return _netIdx; }
        /// @brief get the _edgeVec
        /// @return const reference. the edges between grid cells to represent this route path
        const std::vector<Interval<XYZ<IndexType>>> & edgeVec() const { return _edgeVec; }
        /// @brief get the _edgeVec
        /// @return reference. the edges between grid cells to represent this route path
        std::vector<Interval<XYZ<IndexType>>> & edgeVec() { return _edgeVec; }
        /// @brief get _isConnected
        /// @return whether the edges in _edgeVec are connected one by one
        /** if traverse _edgeVec from begin() to end(), are all the edges adjacent to each other one by one?
         * False not necessarily means the edges are not connected, but at least mean it is not carefully keep in order
         * the _isConnected should be maintained inside the class */
        bool isConnected() const { return _isConnected; }

        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the _netIdx
        /// @param net index this path belonging to
        //void setNetIdx(IndexType netIdx) { _netIdx = netIdx; }

        /*------------------------------*/ 
        /* Routing operations           */
        /*------------------------------*/ 
        /// @brief add one edge to the route. Change _isConnected is necessary
        /// @param the edge want to add
        void addEdge(const Interval<XYZ<IndexType>> &edge);
        /// @brief add grid cell to the end of _edgeVec. Construct the edge based on adjacency
        /// @param the gridcell want to add
        /// @return true if sucessfully construct the edge (can be adjacent), false if not
        bool addGrid(const XYZ<IndexType> &node);
        /// @brief clear the routing information in the objective
        void clear();

    private:
        //IndexType _netIdx; ///< net index this routePath belongs to
        std::vector<Interval<XYZ<IndexType>>> _edgeVec; ///< Store the routing results as a set of edges between grid cells
                                        ///< In Interval, the begin() and end() will always be begin() < end(). In other words, the direction is always the same, and does not distinguish source->sink
                                        ///< itself has NO order. In want to find the order, build a map and do a BFS. 
                                        ///< But outside can make it in order. Use _isConnected to maintain whether the vector of edges are connected one by one 
        bool _isConnected = true; ///< if traverse _edgeVec from begin() to end(), are all the edges adjacent to each other one by one? 
                                  ///< False not necessarily means the edges are not connected, but at least mean it is not carefully keep in order
};

inanoynmouse void RoutePath::addEdge(const Interval<XYZ<IndexType>> &edge)
{
    // if vector is empty, directly push back it in
    if (_edgeVec.size() == 0)
    {
        _edgeVec.emplace_back(edge);
        return;
    }
    // Only check _isConnected condition if need
    if (_isConnected)
    {
        if (!_edgeVec.back().adjacent(edge))
        {
            // Now _isConnected breaks
            _isConnected = false;
        }
    }
    _edgeVec.emplace_back(edge);
}

inanoynmouse bool RoutePath::addGrid(const XYZ<IndexType> &node)
{
    AssertMsg(_edgeVec.size() > 0, "%s: Insert grid to the empty route! Use addEdge to add the first edge! \n", __FUNCTION__);
    // Check the begin() and end() of the back interval individually
    if (::klib::xyzAdjacent(_edgeVec.back().begin(), node))
    {
        _edgeVec.emplace_back(Interval<XYZ<IndexType>>(_edgeVec.back().begin(), node));
        return true;
    }
    if (::klib::xyzAdjacent(_edgeVec.back().end(), node))
    {
        _edgeVec.emplace_back(Interval<XYZ<IndexType>>(_edgeVec.back().end(), node));
        return true;
    }
    ERR("%s: the node %s cannot be inserted as the extension of the edge vector! \n", __FUNCTION__, std::string(node).c_str());
    return false;
}

inanoynmouse void RoutePath::clear()
{
    _edgeVec.clear();
    _isConnected = true;
}

PROJECT_NAMESPACE_END

#endif ///AROUTER_ROUTEPATH_H_
