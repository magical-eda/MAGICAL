#include "GlobalRouter.h"
#include "grdr/NetSpliter.h"
#include "gr/GlobalRoutingAStarSequential.h"

PROJECT_NAMESPACE_BEGIN

bool GlobalRouter::solve()
{
    // init grDB
    if (!init()) { return false; }

    // AStar
    GlobalRoutingAStarSequential algo(_db);
    splitSubNets();
    if (!algo.runKernel()) { ERR("Global routing fails\n"); }

#ifdef DEBUG_GR_
    writeGridCellUsage();
#endif

    return true;
}

bool GlobalRouter::skipSolve()
{
    WRN("%s: global routing is skipped \n", __FUNCTION__);
    // init grDB
    if (!init()) { return false; }

    // AStar
    GlobalRoutingAStarSequential algo(_db);
    splitSubNets();
    return true;
}

bool GlobalRouter::init()
{
    switch(_db.progArgs().designType())
    {
        case DesignBenchmarkType::ISPD08:
        case DesignBenchmarkType::ISPD08PIN:
        {
            bool init = _db.grDB().initISPD08();
            for (auto &symNet : _db.symNetArray())
            {
                if (!initOneSymNet(symNet)) { init = false; }
            }
            return init;
        }
    }
    return false;
}

bool GlobalRouter::splitSubNets()
{
    NetSpliter netSpliter;
    std::vector<XY<LocType>> pinLocs;
    for (const auto pin : _db.grDB().phyPinArray())
    {
        pinLocs.emplace_back(pin.loc());
    }
    // point the netSpliter to the pins
    netSpliter.setPins(pinLocs);
    // Split symmetry nets
    for (auto &symNet : _db.symNetArray())
    {
        // Spliting the net1 using MST
        IndexType netIdx1 = symNet.firstNetIdx();
        auto &net1 = _db.grDB().nets().at(netIdx1);
        netSpliter.setNetPins(net1.physPinArray());
        if (!netSpliter.solveMST()) { return false; }
        for (const auto &pair : netSpliter.results())
        {
            _db.grDB().addSubNet(netIdx1, pair.first, pair.second);
            // matching in net2
            IndexType pin1 = INDEX_TYPE_MAX;
            IndexType pin2 = INDEX_TYPE_MAX;
            for (auto &pinPair : symNet.terminalPairs())
            {
                if (pinPair.first == pair.first) { pin1 = pinPair.second; }
                if (pinPair.first == pair.second) { pin2 = pinPair.second; }
            }
            Assert(pin1 != INDEX_TYPE_MAX);
            Assert(pin2 != INDEX_TYPE_MAX);
            _db.grDB().addSubNet(symNet.secondNetIdx(), pin1, pin2);
        }
        netSpliter.clear();
    }
    // split nets 
    for (IndexType netIdx = 0; netIdx < _db.grDB().nets().size(); ++netIdx)
    {
        auto &net = _db.grDB().nets().at(netIdx);
        // Symmetry nets have been splited
        if (net.isSym()) { continue; }
        netSpliter.setNetPins(net.physPinArray());
        if (!netSpliter.solveMST()) { return false; }
        for (const auto &pair : netSpliter.results())
        {
            _db.grDB().addSubNet(netIdx, pair.first, pair.second);
        }
        netSpliter.clear();
    }
    return true;
}

bool GlobalRouter::initOneSymNet(SymNet &symNet)
{
    if (symNet.symType() == SymType::HORIZONTAL)
    {
        LocType axis = symNet.symAxis();
        IndexType symAxisGC = (axis - _db.grDB().dieLL().x()) / _db.grDB().gridMap().gridWidth();
        symNet.setSymAxisGC(symAxisGC);
        // terminates if horizontal symmetry is found
        return true;
    }
    if (symNet.symType() == SymType::VERTICAL)
    {
        LocType axis = symNet.symAxis();
        IndexType symAxisGC = (axis - _db.grDB().dieLL().y()) / _db.grDB().gridMap().gridHeight(); 
        symNet.setSymAxisGC(symAxisGC);
        return true;
    }
    return false;

}

/*------------------------------*/ 
/* Output print draw etc.       */
/*------------------------------*/ 
inanoynmouse void GlobalRouter::writeGridCellUsage() const
{
    // Very inefficient algorithm to generate the usage. But who cares?
    // Use a 2D vector to record the usage of each grid
    Vector2D<IntType> gridMapUse;
    gridMapUse.resize(_db.grDB().gridMap().numX(), _db.grDB().gridMap().numY());
    
    // Add each net's usage to the gridMapUse
    for (IndexType netIdx = 0; netIdx < _db.grDB().nets().size(); ++netIdx)
    {
        //if (!_db.grDB().nets().at(netIdx).isSym()) { continue; }
        std::set<XY<IndexType>> nodeSet; // The set of the nodes being used in this net
        for (auto &edgeMap : _db.grDB().edgeMap().at(netIdx))
        {
            nodeSet.insert(edgeMap.first.begin().xy());
            nodeSet.insert(edgeMap.first.end().xy());
        }
        for (auto &gridNode : nodeSet)
        {
            gridMapUse.at(gridNode)++;
        }
    }
    // Output to a file
    std::string outFileTable = "./output/grResult.txt";
    std::ofstream out(outFileTable.c_str());
    for (IndexType x = 0; x < gridMapUse.xSize(); ++x)
    {
        for (IndexType y = 0; y < gridMapUse.ySize(); ++y)
        {
            out<< x << " "<< y << " " << gridMapUse.at(x, y)<< std::endl;
        }
    }
    out.close();
}
PROJECT_NAMESPACE_END
