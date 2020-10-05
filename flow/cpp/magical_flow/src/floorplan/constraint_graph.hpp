/**
 * @file constraint_graph.hpp
 * @brief The sweepline and constraint graph implementation
 * @author Keren Zhu
 * @date 10/05/2020
 */

#pragma once

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

namespace constraint_graph
{
    /// @brief Events: the time stamps for lower and higher edges in sweep line. 
    /// Basically, when finding horizontal edges. a sweep line is visited all cell in vertical direction. 
    /// The events determine when to add or removes the devices as the sweep line cross the cell edge.
    class Event
    {
        public:
            explicit Event(IndexType cellIdx, LocType loc, bool isLow)
                : _cellIdx(cellIdx), _loc(loc), _isLow(isLow)
            {}
            /// @brief get the cell index this event belonging to
            IndexType cellIdx() const { return _cellIdx; }
            /// @brief get the coordinate of this event
            LocType loc() const { return _loc; }
            /// @brief get whether this event is low
            /// @return true: low. false: high
            bool isLow() const { return _isLow; }
            /// @brief comparing operator. The events are sorted in increasing loc order. 
            /// When two coordinates are the same, the high events must before the low events. 
            /// NOTE: what if there are multiple same coodinates?
            bool operator<(const Event &rhs) const
            {
                if (_loc == rhs.loc())
                {
                    if (_isLow == rhs.isLow())
                    {
                        return _cellIdx < rhs.cellIdx();
                    }
                    return !_isLow; // if lhs is low, lhs > rhs. if lhs is high (!_isLow), lhs < rhs.
                }
                return _loc < rhs.loc();
            }
        private:
            IndexType _cellIdx = INDEX_TYPE_MAX; ///< The index of cell this event belonging to
            LocType _loc = LOC_TYPE_MIN; ///< The coordinate of the event. If finding horzional edges, the loc is the lower/higher y edge.
            bool _isLow = true; ///< true: low edge. false: high edge
    };

    /// @brief a simple data structure to contain the information of the xLo/yLo
    class CellCoord
    {
        public:
            explicit CellCoord(IndexType cellIdx, LocType loc)
                : _cellIdx(cellIdx), _loc(loc)
            {}
            IndexType cellIdx() const { return _cellIdx; }
            LocType loc() const { return _loc; }
            /// @brief comparison operator. \lambda relation in the original paper x_i > x_j or (xi = xj and i > j). Here take itsopposite
            bool operator<(const CellCoord &rhs) const
            {
                if (_loc == rhs.loc())
                {
                    return _cellIdx < rhs.cellIdx();
                }
                return _loc < rhs.loc();
            }
        private:
            IndexType _cellIdx; ///< The cell index
            LocType _loc; ///< xLo or yLo
    };

    /// @brief a directed edge representing a constraint
    class ConstraintEdge
    {
        public:
            explicit ConstraintEdge(IndexType source, IndexType target):
                _source(source), _target(target)
        {}
            /// @brief Get the index of source vertex
            /// @return the index of source vertex
            IndexType source() const { return _source; }
            /// @brief Get the index of target vertex
            /// @return the index of target vertex
            IndexType target() const { return _target; }
            /// @brief Get the weight of this edge
            /// @return the weight of this edge
            IntType weight() const { return 1; }
            /// @brief to string for debuging
            std::string toStr() const 
            {
                std::stringstream ss;
                ss << "source "<< _source <<" target "<< _target ;
                return ss.str();
            }
            bool operator<(const ConstraintEdge &rhs) const
            {
                if (_source == rhs.source())
                {
                    return _target < rhs.target();
                }
                return _source < rhs.source();
            }
            bool operator==(const ConstraintEdge &rhs) const 
            {
                return _source == rhs.source() && _target == rhs.target();
            }
        private:
            IndexType _source;  ///< The index of source vertex
            IndexType _target; ///< The index of target vertex
            //IntType _weight;  ///< The weight of this edge
    };


    /// @brief Just a wrappers for ConstraintEdge
    class Constraints
    {

        public:
            /// @brief default constructor
            explicit Constraints() = default;
            /// @breif clear the constraint edges
            void clear() { _edges.clear(); }
            /// @brief get the constraint edges
            /// @return the constraint edges
            const std::set<ConstraintEdge> & edges() const { return _edges; }
            /// @brief get the constraint edges
            /// @return the constraint edges
            std::set<ConstraintEdge> & edges() { return _edges; }
            /// @brief add a constraint edge
            /// @param the source cell index
            /// @param the target cell index
            /// @param the weight of the edge
            void addConstraintEdge(IndexType sourceIdx, IndexType targetIdx)
            {
                _edges.insert(ConstraintEdge(sourceIdx, targetIdx));
            }
            bool hasEdgeNoDirection(IndexType sourceIdx, IndexType targetIdx)
            {
                auto it = _edges.find(ConstraintEdge(sourceIdx, targetIdx));
                if (it != _edges.end())
                {
                    return true;
                }
                it = _edges.find(ConstraintEdge(targetIdx, sourceIdx));
                if (it != _edges.end())
                {
                    return true;
                }
                return false;
            }
            void removeConstraintEdge(IndexType sourceIdx, IndexType targetIdx)
            {
                auto it = _edges.find(ConstraintEdge(sourceIdx, targetIdx));
                if (it != _edges.end())
                {
                    _edges.erase(it);
                }
                it = _edges.find(ConstraintEdge(targetIdx, sourceIdx));
                if (it != _edges.end())
                {
                    _edges.erase(it);
                }

            }
            
        private:
            std::set<ConstraintEdge> _edges; ///< The constraint edges
    };


} // namespace constraint_graph


/// @class Sweep line for generating constraints
class SweeplineConstraintGraphGenerator
{
    public:
        explicit SweeplineConstraintGraphGenerator(
                const std::function<LocType(IndexType)> &getLowerCoordFunc,
                const std::function<LocType(IndexType)> &getHigherCoordFunc)
            : _getLowerCoordFunc(getLowerCoordFunc), _getHigherCoordFunc(getHigherCoordFunc)
        {}
        /// @brief solve the sweep line
        void solve();
        void setExemptFunc(std::function<bool(IndexType, IndexType)> exexmptFunc)
        {
            _exemptFunc = exexmptFunc;
            _setExempted = true;
        }
        /// @brief get the constraint edges
        const std::set<constraint_graph::ConstraintEdge> & constrEdges() const { return _vC.edges(); }
    /// @brief the balance tree for containing the "D" in TCAD-1987. Using the std::set implementation
    class CellCoordTree
    {
        public:
            explicit CellCoordTree() = default;
            /// @brief insert a CellCoord
            void insert(const constraint_graph::CellCoord &cellCoord) { _tree.insert(cellCoord); }
            /// @brief erase a CellCoord
            void erase(const constraint_graph::CellCoord &cellCoord) { _tree.erase(cellCoord); }
            /// @brief find the right cell index in the tree
            /// @param a CellCoord, this object should already inside the tree
            /// @return the cell index. -1 if nil
            IntType right(const constraint_graph::CellCoord &cellCoord)
            {
                auto findIter = _tree.find(cellCoord);
                Assert(findIter != _tree.end());
                findIter++;
                if (findIter == _tree.end())
                {
                    return -1;
                }
                else
                {
                    return static_cast<IntType>(findIter->cellIdx());
                }
            }
            /// @brief find the left cell index in the tree
            /// @param a CellCoord, this object should already inside the tree
            /// @return the cell index. -1 if nil
            IntType left(const constraint_graph::CellCoord &cellCoord)
            {
                auto findIter = _tree.find(cellCoord);
                Assert(findIter != _tree.end());
                if (findIter == _tree.begin())
                {
                    return -1; // This cellCoord is the leftmost in the tree
                }
                else
                {
                    findIter --;
                    return static_cast<IntType>(findIter->cellIdx());
                }
            }
        private:
            std::set<constraint_graph::CellCoord> _tree; ///< std::set to act as the balanced tree
    };
    private:
        /// @brief generate the events.
        /// @param first: the referece to a vector of events. The vector will be cleared first.
        /// @param second: true: generating horizontal edges. false: generating vertical edges
        void generateEvents(std::vector<constraint_graph::Event> &events);
        /// @brief original TCAD-87 algorithm
        void originalSweepLine();
        /// @brief record all the cell coordinates
        /// @param first: A reference to the vector of cell coordinates. The vector will be cleared in the beginning
        /// @param second: true: if generating horizontal edges. false if vertical
        void recordCellCoords(std::vector<constraint_graph::CellCoord> &cellCords);
        /// @brief generate the constraint edges with TCAD-1987 compact ver. 1
        /// @param first: the constraints to save results in
        /// @param second: the sorted events
        /// @param third: the recorded cell coordinates
        //void originalConstraintGeneration(Constraints &cs, std::vector<Event> &events, std::vector<CellCoord> &cellCoords);
        void originalDelete(const constraint_graph::CellCoord &cellCoord, CellCoordTree &dTree, std::vector<IntType> &cand, constraint_graph::Constraints &cs);
        void originalConstraintGeneration(std::vector<constraint_graph::Event> &events, std::vector<constraint_graph::CellCoord> &cellCoords);
        bool isExempted(IndexType cell1, IndexType cell2) const
        {
            if (!_setExempted)
            {
                return false;
            }
            else
            {
                return _exemptFunc(cell1, cell2);
            }
        }

    private:
        constraint_graph::Constraints _vC; ///< The vertical edges
        std::function<bool(IndexType, IndexType)> _exemptFunc; ///< exempt pair of cells to be add constraints
        bool _setExempted = false;
        std::function<LocType(IndexType)> _getLowerCoordFunc; ///< A function to get the xLo/yLo coordinate of a cell
        std::function<LocType(IndexType)> _getHigherCoordFunc; ///< A function to get the xLo/yLo coordinate of a cell
        IntType _numCells = -1; ///< The number of the cells
};

PROJECT_NAMESPACE_END
