#include "DatabaseGR.h"

PROJECT_NAMESPACE_BEGIN

bool DatabaseGR::initISPD08()
{
    /// init pins in the grid
    this->initPinInGrid();
    /// init bounding box for each nets
    for (IndexType netIdx = 0; netIdx < this->nets().size(); ++netIdx)
    {
        if (! this->initNetBBox(netIdx)) { return false; }
    }
    this->edgeMap().resize(this->nets().size());
    return true;
}

void DatabaseGR::initPinInGrid()
{
    /// init the physical pins into the grid
    for (IndexType pinIdx = 0; pinIdx < this->phyPinArray().size(); ++pinIdx)
    {
        const auto &pin = this->phyPinArray().at(pinIdx);
        // Because it is needed to add pins to both 3D and 2D grids
        // Here using the getter of the vector instead of wrapoed gridCell()
        
        // 3D vector
        this->gridMap().gridCells().at(XYZ<IndexType>(pin.gridIdx(), pin.layer())).dropPin(pinIdx);
        // 2D vector
        this->gridMap().gridCells2D().at(pin.gridIdx()).dropPin(pinIdx);
    }
}

bool DatabaseGR::initNetBBox(IndexType netIdx)
{
    auto &net = this->nets().at(netIdx);
    net.gridBBox() = Box<IndexType>(INDEX_TYPE_MAX, 0, INDEX_TYPE_MAX, 0); ///< a impossible box
    for (IndexType pinIdx : net.physPinArray())
    {
        net.gridBBox().join(this->pin(pinIdx).gridIdx());
    }
    return true;
}

bool DatabaseGR::initSubNetBBox(IndexType subNetIdx)
{
    auto &subNet = this->subNets().at(subNetIdx);
    subNet.gridBBox() = Box<IndexType>(INDEX_TYPE_MAX, 0, INDEX_TYPE_MAX, 0); ///< a impossible box
    subNet.gridBBox().join(this->pin(subNet.sourcePinIdx()).gridIdx());
    subNet.gridBBox().join(this->pin(subNet.targetPinIdx()).gridIdx());
    return true;
}

void DatabaseGR::addSubNet(IndexType netIdx, IndexType pinIdxFrom, IndexType pinIdxTo)
{
    IndexType subNetIdx = this->subNets().size();
    this->subNets().emplace_back(SubNet(netIdx, pinIdxFrom, pinIdxTo));
    this->nets().at(netIdx).subNets().emplace_back(subNetIdx);
    initSubNetBBox(subNetIdx);
}

std::vector<XYZ<IndexType>> DatabaseGR::retrieveNetPath(IndexType netIdx, const XYZ<IndexType> &from, const XYZ<IndexType> &to)
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
        if (current == to)
        {
            break;
        }
        nodeQueue.pop();
        // if isn't the target, search the nearby
        // Because it is not accessing the real grids, it's okay to have the out of bound indices
        auto top = current.top();
        if (netHasEdge(netIdx, current, top) && checked.find(top) == checked.end())
        {
            nodeQueue.push(top);
            checked.insert(std::make_pair(top, current));
        }
        auto bottom = current.bottom();
        if (netHasEdge(netIdx, current, bottom) && checked.find(bottom) == checked.end())
        {
            nodeQueue.push(bottom);
            checked.insert(std::make_pair(bottom, current));
        }
        auto north = current.north();
        if (netHasEdge(netIdx, current, north) && checked.find(north) == checked.end())
        {
            nodeQueue.push(north);
            checked.insert(std::make_pair(north, current));
        }
        auto south = current.south();
        if (netHasEdge(netIdx, current, south) && checked.find(south) == checked.end())
        {
            nodeQueue.push(south);
            checked.insert(std::make_pair(south, current));
        }
        auto east = current.east();
        if (netHasEdge(netIdx, current, east) && checked.find(east) == checked.end())
        {
            nodeQueue.push(east);
            checked.insert(std::make_pair(east, current));
        }
        auto west = current.west();
        if (netHasEdge(netIdx, current, west) && checked.find(west) == checked.end())
        {
            nodeQueue.push(west);
            checked.insert(std::make_pair(west, current));
        }
    }

    // Assert if the searching results doesn't go to the target
    Assert(nodeQueue.front() == to);

    std::vector<XYZ<IndexType>> results;
    XYZ<IndexType> current = to;
    while(current != from)
    {
        results.emplace_back(current);
        current =checked[current];
    }
    //results.emplace_back(from);
    return results;
}

PROJECT_NAMESPACE_END
