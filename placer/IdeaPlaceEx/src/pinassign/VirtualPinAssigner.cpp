#include "VirtualPinAssigner.h"
#include <lemon/list_graph.h>
#include <lemon/network_simplex.h>

PROJECT_NAMESPACE_BEGIN

void VirtualPinAssigner::solveFromDB()
{
    reconfigureVirtualPinLocationFromDB();
    pinAssignmentFromDB();
}

void VirtualPinAssigner::reconfigureVirtualPinLocationFromDB()
{
    Box<LocType> boundary(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN);
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &cell = _db.cell(cellIdx);
        boundary.unionBox(cell.cellBBoxOff());
    }
    reconfigureVirtualPinLocations(boundary);
}


void VirtualPinAssigner::pinAssignmentFromDB()
{
    auto cellLocQueryFunc = [&] (IndexType cellIdx)
    {
        const auto &cell = _db.cell(cellIdx);
        return cell.loc();
    };
    pinAssignment(cellLocQueryFunc);
}

void VirtualPinAssigner::reconfigureVirtualPinLocations(const Box<LocType> &cellsBBox)
{
    _boundary = cellsBBox;
    _boundary.enlargeBy(_virtualBoundaryExtension);
    // generate the virtual pin locations
    _virtualPins.clear();
    for (LocType x = _boundary.xLo() + _virtualPinInterval;  x < _boundary.xHi() - _virtualPinInterval; x += _virtualPinInterval)
    {
        _virtualPins.emplace_back(XY<LocType>(x, _boundary.yLo()));
        _virtualPins.emplace_back(XY<LocType>(x, _boundary.yHi()));
    }
    for (LocType y = _boundary.yLo() + _virtualPinInterval;  y < _boundary.yHi() - _virtualPinInterval; y += _virtualPinInterval)
    {
        _virtualPins.emplace_back(XY<LocType>(_boundary.xLo(), y));
        _virtualPins.emplace_back(XY<LocType>(_boundary.xHi(), y));
    }
}
void VirtualPinAssigner::pinAssignment(std::function<XY<LocType>(IndexType)> cellLocQueryFunc)
{
    std::vector<IndexType> ioNets; // The net indices that are IOs 
    ioNets.reserve(_db.numNets());
    for (IndexType netIdx = 0; netIdx < _db.numNets(); ++netIdx)
    {
        if (_db.net(netIdx).isIo())
        {
            ioNets.emplace_back(netIdx);
        }
    }

    // Build the network
    IndexType numNets = ioNets.size();
    IndexType numSites = _virtualPins.size();
    if (numSites <= numNets)
    {
        return;
    }

    // Calculate the current HPWLs without virtual pin
    std::vector<Box<LocType>> curNetBBox;
    curNetBBox.resize(numNets);
    for (IndexType netIdx : ioNets)
    {
        IndexType pinIdx = _db.net(netIdx).pinIdx(0);
        XY<LocType> pinOff = _db.pin(pinIdx).midLoc();
        XY<LocType> cellLoc = cellLocQueryFunc(_db.pin(pinIdx).cellIdx());
        curNetBBox.at(netIdx) = Box<LocType>(cellLoc + pinOff, cellLoc + pinOff); // Init into an area=0 box
        // The rest pins
        for (IndexType idx = 1; idx < _db.net(netIdx).numPinIdx(); ++idx)
        {
            pinIdx = _db.net(netIdx).pinIdx(idx);
            pinOff = _db.pin(pinIdx).midLoc();
            cellLoc = cellLocQueryFunc(_db.pin(pinIdx).cellIdx());
            curNetBBox.at(netIdx).join(cellLoc + pinOff);
        }
    }

    // Use lemon min cost max flow with network simplex algorithm
    lemon::ListDigraph graph;
    std::vector<lemon::ListDigraph::Node> lNodes, rNodes;
    std::vector<lemon::ListDigraph::Arc> lArcs, rArcs, mArcs;
    std::vector<std::pair<IndexType, IndexType>> mArcPairs;
    // Reserve the needed space (optional)
    lNodes.reserve(numNets); rNodes.reserve(numSites);
    lArcs.reserve(numNets); rArcs.reserve(numSites); mArcs.reserve(numNets * numSites);
    mArcPairs.reserve(numNets * numSites);

    lemon::ListDigraph::ArcMap<IntType> capLo(graph); // capacity low
    lemon::ListDigraph::ArcMap<IntType> capHi(graph); // capacity high
    lemon::ListDigraph::ArcMap<IntType> costMap(graph); // Cost map

    // Calculate the added HPWL if adding the virtual pin
    auto pinCostFunc = [&](const XY<LocType> &virtualPinLoc, IndexType ioNetIdx)
    {
        auto difX = std::max(virtualPinLoc.x() - curNetBBox.at(ioNetIdx).xHi(), curNetBBox.at(ioNetIdx).xLo() - virtualPinLoc.x());
        difX = std::max(difX, 0);
        auto difY = std::max(virtualPinLoc.y() - curNetBBox.at(ioNetIdx).yHi(), curNetBBox.at(ioNetIdx).yLo() - virtualPinLoc.y());
        difY = std::max(difY, 0);
        return difX + difY;
    };

    // Adding the nodes and arcs
    // Source and target nodes
    auto s = graph.addNode();
    auto t = graph.addNode();

    // Add arcs source->left
    for (IndexType i = 0; i < numNets; ++i)
    {
        lNodes.emplace_back(graph.addNode());
        lArcs.emplace_back(graph.addArc(s, lNodes.back()));
        capLo[lArcs.back()] = 0;
        capHi[lArcs.back()] = 1;
        costMap[lArcs.back()] = 0;
    }
    // Add arcs right->target
    for (IndexType i = 0; i < numSites; ++i)
    {
        rNodes.emplace_back(graph.addNode());
        rArcs.emplace_back(graph.addArc(rNodes.back(), t));
        capLo[rArcs.back()] = 0;
        capHi[rArcs.back()] = 1;
        costMap[rArcs.back()] = 0;
    }
    // Mid arcs
    for (IndexType l = 0; l < numNets; ++l)
    {
        for (IndexType r = 0; r < numSites; ++r)
        {
            mArcs.emplace_back(graph.addArc(lNodes[l], rNodes[r]));
            mArcPairs.emplace_back(l, r);
            capLo[mArcs.back()] = 0;
            capHi[mArcs.back()] = 1;
            costMap[mArcs.back()] = pinCostFunc(_virtualPins[r], l);
        }
    }

    // Solve min cost max flow sing network simplex algorithm
    lemon::NetworkSimplex<lemon::ListDigraph, IntType> networkSimplex(graph);
    networkSimplex.stSupply(s, t, numNets);
    networkSimplex.lowerMap(capLo).upperMap(capHi).costMap(costMap);
    networkSimplex.run();
    
    // Collect the solution and export into the database
    for (IndexType i = 0; i < mArcs.size(); ++i)
    {
        if (networkSimplex.flow(mArcs[i]))
        {
            const auto &pair = mArcPairs[i];
            _db.net(ioNets.at(pair.first)).setVirtualPin(_virtualPins.at(pair.second));
        }
    }

}

PROJECT_NAMESPACE_END
