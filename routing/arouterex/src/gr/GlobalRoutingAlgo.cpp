#include "GlobalRoutingAlgo.h"

PROJECT_NAMESPACE_BEGIN

void GlobalRoutingAlgo::retrieveNetPath(IndexType netIdx,  const XYZ<IndexType> &from, IndexType termIdx, std::vector<XYZ<IndexType>> &results)
{
    std::queue<XYZ<IndexType>> nodeQueue;
    // The set of nodes checked, value is the parent
    std::unordered_map<XYZ<IndexType>, XYZ<IndexType>> checked;
    nodeQueue.push(from);
    // Make the first one specially
    checked.insert(std::make_pair(from, from));
    while(!nodeQueue.empty())
    {
        auto current = nodeQueue.front();
        // if the current is the "to"
        if (_db.grDB().gridCell(current).existTerm(termIdx))
        {
            break;
        }
        nodeQueue.pop();
        // if isn't the target, search the nearby
        // Because it is not accessing the real grids, it's okay to have the out of bound indices
        auto top = current.top();
        if (_db.grDB().netHasEdge(netIdx, current, top) && checked.find(top) == checked.end())
        {
            nodeQueue.push(top);
            checked.insert(std::make_pair(top, current));
        }
        auto bottom = current.bottom();
        if (_db.grDB().netHasEdge(netIdx, current, bottom) && checked.find(bottom) == checked.end())
        {
            nodeQueue.push(bottom);
            checked.insert(std::make_pair(bottom, current));
        }
        auto north = current.north();
        if (_db.grDB().netHasEdge(netIdx, current, north) && checked.find(north) == checked.end())
        {
            nodeQueue.push(north);
            checked.insert(std::make_pair(north, current));
        }
        auto south = current.south();
        if (_db.grDB().netHasEdge(netIdx, current, south) && checked.find(south) == checked.end())
        {
            nodeQueue.push(south);
            checked.insert(std::make_pair(south, current));
        }
        auto east = current.east();
        if (_db.grDB().netHasEdge(netIdx, current, east) && checked.find(east) == checked.end())
        {
            nodeQueue.push(east);
            checked.insert(std::make_pair(east, current));
        }
        auto west = current.west();
        if (_db.grDB().netHasEdge(netIdx, current, west) && checked.find(west) == checked.end())
        {
            nodeQueue.push(west);
            checked.insert(std::make_pair(west, current));
        }
    }

    // Assert if the searching results doesn't go to the target
    Assert(_db.grDB().gridCell(nodeQueue.front()).existTerm(termIdx));

    XYZ<IndexType> current = nodeQueue.front();
    while(current != from)
    {
        results.emplace_back(current);
        current =checked[current];
    }
}
PROJECT_NAMESPACE_END
