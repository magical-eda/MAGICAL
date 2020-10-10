#include "constraint_graph.hpp"

PROJECT_NAMESPACE_BEGIN


void SweeplineConstraintGraphGenerator::solve()
{
    originalSweepLine(); // TCAD-87
}


void originalInsert(const constraint_graph::CellCoord &cellCoord, SweeplineConstraintGraphGenerator::CellCoordTree &dTree, std::vector<IntType> &cand)
{
    dTree.insert(cellCoord);
    cand.at(cellCoord.cellIdx()) = dTree.left(cellCoord);
    cand.at(dTree.right(cellCoord)) = cellCoord.cellIdx(); // dTree.right should never be -1. Because R0 is always there
}

void SweeplineConstraintGraphGenerator::originalDelete(const constraint_graph::CellCoord &cellCoord, CellCoordTree &dTree, std::vector<IntType> &cand, constraint_graph::Constraints &cs)
{
    auto left = dTree.left(cellCoord);
    if (left != -1 && left == cand.at(cellCoord.cellIdx()))
    {
        IntType from = left;
        IntType to = cellCoord.cellIdx();
        if (!isExempted(from, to))
        {
            cs.addConstraintEdge(from, to);
        }
    }
    auto right = dTree.right(cellCoord);
    if (cand.at(right) == static_cast<IntType>(cellCoord.cellIdx()))
    {
        IntType from = cellCoord.cellIdx();
        IntType to = right;
        if (!isExempted(from, to))
        {
            cs.addConstraintEdge(from, to);
        }
    }
    dTree.erase(cellCoord);
}

/// @brief generate the constraint edges with TCAD-1987 compact ver. 1
/// @param first: the constraints to save results in
/// @param second: the sorted events
/// @param third: the recorded cell coordinates
void SweeplineConstraintGraphGenerator::originalConstraintGeneration(std::vector<constraint_graph::Event> &events, std::vector<constraint_graph::CellCoord> &cellCoords)
{
    SweeplineConstraintGraphGenerator::CellCoordTree dTree;
    IndexType numCells = cellCoords.size();
    std::vector<IntType> cand(numCells + 2, -1);
    // Insert R0
    dTree.insert(constraint_graph::CellCoord(numCells + 1, LOC_TYPE_MAX)); // R0 is behaving as the target node in the DAG. which in convention is numCells + 1.
    for (const auto &event : events)
    {
        if (event.isLow())
        {
            originalInsert(cellCoords.at(event.cellIdx()), dTree, cand);
        }
        else
        {
            originalDelete(cellCoords.at(event.cellIdx()), dTree, cand, _vC);
        }
    }
}

void SweeplineConstraintGraphGenerator::recordCellCoords(std::vector<constraint_graph::CellCoord> &cellCoords)
{
    cellCoords.clear();
    for (IntType cellIdx = 0; cellIdx < _numCells; ++cellIdx)
    {
        cellCoords.emplace_back(constraint_graph::CellCoord(cellIdx, _getLowerCoordFunc(cellIdx)));
    }
}

void SweeplineConstraintGraphGenerator::generateEvents(std::vector<constraint_graph::Event> &events)
{
    events.clear();
    for (IntType cellIdx = 0; cellIdx < _numCells; ++cellIdx)
    {
        events.emplace_back(constraint_graph::Event(cellIdx, _getOtherDirLowerCoordFunc(cellIdx), true));
        events.emplace_back(constraint_graph::Event(cellIdx, _getOtherDirHigherCoordFunc(cellIdx), false));
    }
    std::sort(events.begin(), events.end());
}

void addEdgesFromSource(constraint_graph::Constraints &cs, IndexType numCells)
{
    std::vector<char> hasIngoingEdges(numCells + 2, false);
    for (const auto &edge : cs.edges())
    {
        IndexType to = edge.target();
        hasIngoingEdges.at(to) = true;
    }
    IndexType sourceIdx = numCells;
    for (IndexType idx = 0; idx < numCells; ++idx)
    {
        if (!hasIngoingEdges.at(idx))
        {
            IntType from = sourceIdx;
            IntType to = idx;
            cs.addConstraintEdge(from, to);
        }
    }
}

void SweeplineConstraintGraphGenerator::originalSweepLine()
{
    std::vector<constraint_graph::Event> events;
    std::vector<constraint_graph::CellCoord> cellCoords;
    // Vertical
    generateEvents(events);
    recordCellCoords(cellCoords);
    originalConstraintGeneration(events, cellCoords);
    addEdgesFromSource(_vC, cellCoords.size());
}

PROJECT_NAMESPACE_END
