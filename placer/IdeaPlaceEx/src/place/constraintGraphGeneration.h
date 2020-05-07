/**
 * @file constraintGraphGeneration.h
 * @brief The reimplement of the sweep line and transitive reduction algorithm on generating the constraint graph for legalization
 * @author Keren Zhu
 * @date 01/26/2020
 */

#ifndef IDEAPLACE_CONSTRAINT_GRAPH_GENERATION_H_
#define IDEAPLACE_CONSTRAINT_GRAPH_GENERATION_H_

#include "CGLegalizer.h"

PROJECT_NAMESPACE_BEGIN

/// @brief Events: the time stamps for lower and higher edges in sweep line. Basically, when finding horizontal edges. a sweep line is visited all cell in vertical direction. The events determine when to add or removes the devices as the sweep line cross the cell edge.
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
        /// @brief comparing operator. The events are sorted in increasing loc order. When two coordinates are the same, the high events must before the low events. NOTE: what if there are multiple same coodinates?
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

/// @class Sweep line for generating constraints
class SweeplineConstraintGraphGenerator
{
    public:
        explicit SweeplineConstraintGraphGenerator(Database &db, Constraints &hC, Constraints &vC)
            : _db(db), _hC(hC), _vC(vC)
        {}
        /// @brief solve the sweep line
        void solve();
    private:
        /// @brief generate the events.
        /// @param first: the referece to a vector of events. The vector will be cleared first.
        /// @param second: true: generating horizontal edges. false: generating vertical edges
        void generateEvents(std::vector<Event> &events, bool isHor);
        /// @brief original TCAD-87 algorithm
        void originalSweepLine();
        /// @brief record all the cell coordinates
        /// @param first: A reference to the vector of cell coordinates. The vector will be cleared in the beginning
        /// @param second: true: if generating horizontal edges. false if vertical
        void recordCellCoords(std::vector<CellCoord> &cellCords, bool isHor);
        /// @brief generate the constraint edges with TCAD-1987 compact ver. 1
        /// @param first: the constraints to save results in
        /// @param second: the sorted events
        /// @param third: the recorded cell coordinates
        //void originalConstraintGeneration(Constraints &cs, std::vector<Event> &events, std::vector<CellCoord> &cellCoords);

    private:
        Database &_db; ///< The placement database
        Constraints &_hC; ///< The horizontal edges
        Constraints &_vC; ///< The vertical edges
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_CONSTRAINT_GRAPH_GENERATION_H_
