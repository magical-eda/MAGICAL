#include "GlobalRouter.h"
#include "grdr/NetSpliter.h"
#include "gr/GlobalRoutingAStarSequential.h"

PROJECT_NAMESPACE_BEGIN

bool GlobalRouter::solve()
{
    // init grDB
    if (!init()) {  ERR("Global routing initialization fails\n"); return false; }

    // AStar
    GlobalRoutingAStarSequential algo(_db);
    splitSubNets();
    if (!algo.runKernel()) { ERR("Global routing fails\n"); return false; }

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
        {
            _pinShapeMode = false;
            bool init = _db.grDB().initISPD08();
            for (auto &symNet : _db.symNetArray())
            {
                if (!initOneSymNet(symNet)) { init = false; }
            }
            for (auto &selfSym : _db.selfSymNetArray())
            {
                if (!initOneSelfSymNet(selfSym)) { init = false; }
            }
            return init;
        }
        case DesignBenchmarkType::ISPD08PIN:
        {
            _pinShapeMode = true;
            bool init = this->initTerminalMode();
            if (!init) { ERR("Init terminal failed \n"); return false; }
            for (auto &symNet : _db.symNetArray())
            {
                if (!initOneSymNet(symNet)) { ERR("Initialize sym net fails \n"); }
            }
            for (auto &selfSym : _db.selfSymNetArray())
            {
                if (!initOneSelfSymNet(selfSym)) { init = false; ERR("Initialize self sym net fails \n"); return false; }
            }
            // Block grid cells with blockages and pins
            blockGrid();
            return init;
        }
    }
    return false;
}

bool GlobalRouter::initTerminalMode()
{
    /// init the terminals into the grid
    for (IndexType termIdx = 0; termIdx < _db.numTerminals(); ++termIdx)
    {
        const auto &term = _db.terminal(termIdx);
        // Because it is needed to add pins to both 3D and 2D grids
        XY<IndexType> idxLo = _db.grDB().gridIdx(term.primarySearchPoint().loc());
        XY<IndexType> idxHi = idxLo;
        for (const auto &otherPt : term.otherSearchPoints())
        {
            Assert(otherPt.layer() == term.primarySearchPoint().layer());
            auto idx = _db.grDB().gridIdx(otherPt.loc());
            if (idx.x() < idxLo.x()) { idxLo.setX(idx.x()); }
            if (idx.y() < idxLo.y()) { idxLo.setY(idx.y()); }
            if (idx.x() > idxHi.x()) { idxHi.setX(idx.x()); }
            if (idx.y() > idxHi.y()) { idxHi.setY(idx.y()); }
        }

        for (IndexType xIdx = idxLo.x(); xIdx <=  idxHi.x(); ++xIdx)
        {
            for (IndexType yIdx = idxLo.y(); yIdx <=  idxHi.y(); ++yIdx)
            {
                // 3D vector
                _db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, term.primarySearchPoint().layer())).dropTerminal(termIdx);
                _db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, term.primarySearchPoint().layer())).setBlocked(false);
                // 2D vector
            }
        }
      
    }
    /// init bounding box for each nets
    for (IndexType netIdx = 0; netIdx < _db.grDB().nets().size(); ++netIdx)
    {
        if (! _db.grDB().initNetBBox(netIdx)) { return false; }
    }
    _db.grDB().edgeMap().resize(_db.grDB().nets().size());
    return true;
}

void GlobalRouter::blockGrid()
{
    for (IndexType idx = 0; idx < _db.numBlockages(); ++idx)
    {
        const auto & block = _db.blockage(idx);
        if (!DRUtil::allLayerIsRouteLayer(block.layer()))
        {
            continue;
        }
        auto metalLayer = DRUtil::allLayer2RouteLayer(block.layer());
        XY<IndexType> idxLo = _db.grDB().gridIdx(block.rect().ll());
        XY<IndexType> idxHi = _db.grDB().gridIdx(block.rect().ur());
        for (IndexType xIdx = idxLo.x(); xIdx <=  idxHi.x(); ++xIdx)
        {
            for (IndexType yIdx = idxLo.y(); yIdx <=  idxHi.y(); ++yIdx)
            {
                // 3D vector
                if (xIdx >= 0  && xIdx < _db.grDB().gridMap().numX())
                {
                    if (yIdx >= 0  && yIdx < _db.grDB().gridMap().numY())
                    {
                        if (metalLayer < HIGHEST_LAYER)
                        {
                            //_db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, metalLayer)).setBlocked(true);
                            _db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, metalLayer)).increPenalty();
                        }
                        // 2D vector
                    }
                }
            }
        }
    }
    // Dummy/pblk
    for (IndexType idx = 0; idx < _db.numBlockages(); ++idx)
    {
        const auto &block = _db.blockage(idx);
        if (block.layer()== INDEX_TYPE_MAX)
        {
            XY<IndexType> idxLo = _db.grDB().gridIdx(block.rect().ll());
            XY<IndexType> idxHi = _db.grDB().gridIdx(block.rect().ur());
            for (IndexType xIdx = idxLo.x(); xIdx <=  idxHi.x(); ++xIdx)
            {
                for (IndexType yIdx = idxLo.y(); yIdx <=  idxHi.y(); ++yIdx)
                {
                    for (IndexType metalLayer = 0; metalLayer < _db.grDB().gridMap().numLayers(); ++ metalLayer)
                    {
                        // 3D vector
                        //_db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, metalLayer)).setBlocked(true);
                        _db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, metalLayer)).increPenalty();
                        // 2D vector
                    }
                }
            }
        }
    }
    for (IndexType netIdx = 0; netIdx < _db.drDB().numNets(); ++netIdx)
    {
        const auto &net = _db.drDB().net(netIdx);
        for (IndexType termIdx = 0; termIdx < net.numTerminals(); ++ termIdx)
        {
            const auto &terminal = _db.terminal(net.terminal(termIdx));
            // Add the shapes belonging to the terminals
            for (IndexType shapeIdx = 0; shapeIdx < terminal.intraCellRoute().size(); ++shapeIdx)
            {
                const auto &boxLayer = terminal.intraShape(shapeIdx);
                if (!DRUtil::allLayerIsRouteLayer(boxLayer.layer()))
                {
                    continue;
                }
                auto metalLayer = DRUtil::allLayer2RouteLayer(boxLayer.layer());
                // This should be used as a connection points 
                XY<IndexType> idxLo = _db.grDB().gridIdx(boxLayer.box().ll());
                XY<IndexType> idxHi = _db.grDB().gridIdx(boxLayer.box().ur());
                for (IndexType xIdx = idxLo.x(); xIdx <=  idxHi.x(); ++xIdx)
                {
                    for (IndexType yIdx = idxLo.y(); yIdx <=  idxHi.y(); ++yIdx)
                    {
                        // 3D vector
                        _db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, metalLayer)).setBlocked(true);
                        // 2D vector
                    }
                }
            }
            // Add the remaining pin shapes related to this terminal
            for (IndexType pinIdx : terminal.pinArray())
            {
                const auto &pin = _db.drDB().pin(pinIdx);
                ShapeID pinShapeID = ShapeID();
                pinShapeID.configpin(pinIdx);
                for (IndexType pinShapeIdx = 0; pinShapeIdx <  pin.shapeRects().size(); ++pinShapeIdx)
                {
                    if (!pin.isIncludeTerm(pinShapeIdx))
                    {
                        // If it is not included in the terminal connection, add it as a blockage
                        const auto &pinShape  =  pin.shapeRects().at(pinShapeIdx);
                        XY<IndexType> idxLo = _db.grDB().gridIdx(pinShape.ll());
                        XY<IndexType> idxHi = _db.grDB().gridIdx(pinShape.ur());
                        for (IndexType xIdx = idxLo.x(); xIdx <=  idxHi.x(); ++xIdx)
                        {
                            for (IndexType yIdx = idxLo.y(); yIdx <=  idxHi.y(); ++yIdx)
                            {
                                // 3D vector
                                _db.grDB().gridCell(XYZ<IndexType>(xIdx, yIdx, pin.layer())).setBlocked(true);
                                // 2D vector
                            }
                        }
                    }
                }
            }
        }
    }
}

bool GlobalRouter::splitSubNets()
{
    // Split symmetry nets
    bool pass = true;
//#pragma omp parallel for schedule(runtime)
    for (IndexType symNetIdx = 0; symNetIdx < _db.numSymNets(); ++ symNetIdx)
    //for (auto &symNet : _db.symNetArray())
    {
        NetSpliter netSpliter;
        // point the netSpliter to the pins
        netSpliter.setTerms(_db.terminalArray());
        auto &symNet = _db.symNet(symNetIdx);
        if (!symNet.valid())
        {
            continue;
        }
        // Spliting the net1 using MST
        IndexType netIdx1 = symNet.firstNetIdx();
        auto &net1 = _db.grDB().nets().at(netIdx1);
        netSpliter.setNetPins(net1.terminalArray());
        if (!netSpliter.solveMST(_db)) { pass = false; }
        for (auto pair : netSpliter.results())
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
    if (!pass) { return false; }
    // split nets 
//#pragma omp parallel for schedule(runtime)
    for (IndexType netIdx = 0; netIdx < _db.grDB().nets().size(); ++netIdx)
    {
        NetSpliter netSpliter;
        // point the netSpliter to the pins
        netSpliter.setTerms(_db.terminalArray());
        auto &net = _db.grDB().nets().at(netIdx);
        // Symmetry nets have been splited
        if (net.isSym()) { continue; }
        netSpliter.setNetPins(net.terminalArray());
        if (!netSpliter.solveMST(_db)) { pass = false; }
        for (const auto &pair : netSpliter.results())
        {
            _db.grDB().addSubNet(netIdx, pair.first, pair.second);
        }
        netSpliter.clear();
    }
    if (!pass) { return false; }
    for (IndexType subNetIdx = 0; subNetIdx <  _db.grDB().subNets().size(); ++subNetIdx)
    {
        initSubNetBBox(subNetIdx);
    }
    return true;
}

void GlobalRouter::initSubNetBBox(IndexType subNetIdx)
{
    auto &subNet = _db.grDB().subNets().at(subNetIdx);
    subNet.gridBBox() = Box<IndexType>(INDEX_TYPE_MAX, 0, INDEX_TYPE_MAX, 0); ///< a impossible box
    const auto &sourceTerm = _db.terminal(subNet.sourceTermIdx());
    const auto &targetTerm = _db.terminal(subNet.targetTermIdx());
    subNet.gridBBox().join(_db.grDB().gridIdx(sourceTerm.primarySearchPoint().loc()));
    subNet.gridBBox().join(_db.grDB().gridIdx(targetTerm.primarySearchPoint().loc()));
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
    return true;

}

bool GlobalRouter::initOneSelfSymNet(SelfSymNet &selfSym)
{
    if (selfSym.symType() == SymType::HORIZONTAL)
    {
        LocType axis = selfSym.symAxis();
        IndexType symAxisGC = (axis - _db.grDB().dieLL().x()) / _db.grDB().gridMap().gridWidth();
        selfSym.setSymAxisGC(symAxisGC);
        return true;
    }
    if (selfSym.symType() == SymType::VERTICAL)
    {
        LocType axis = selfSym.symAxis();
        IndexType symAxisGC = (axis - _db.grDB().dieLL().y()) / _db.grDB().gridMap().gridHeight(); 
        selfSym.setSymAxisGC(symAxisGC);
        return true;
    }
    return true;
}

/*------------------------------*/ 
/* Output print draw etc.       */
/*------------------------------*/ 
void GlobalRouter::writeGridCellUsage() const
{
    // Very inefficient algorithm to generate the usage. But who cares?
    // Use a 2D vector to record the usage of each grid
    Vector2D<IntType> gridMapUse;
    gridMapUse.resize(_db.grDB().gridMap().numX(), _db.grDB().gridMap().numY());
    
    // Add each net's usage to the gridMapUse
    for (IndexType netIdx = 0; netIdx < _db.grDB().nets().size(); ++netIdx)
    {
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
