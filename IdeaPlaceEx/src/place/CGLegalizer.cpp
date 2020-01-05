#include "CGLegalizer.h"

PROJECT_NAMESPACE_BEGIN

bool CGLegalizer::legalize()
{
    this->generateConstraints();
    _wStar = lpLegalization(true);
    if (_wStar < 0)
    {
        return false;
    }
    _hStar = lpLegalization(false);
    if (_hStar < 0)
    {
        return false;
    }

    LocType xMin = LOC_TYPE_MAX;
    LocType xMax = LOC_TYPE_MIN;
    LocType yMin = LOC_TYPE_MAX;
    LocType yMax = LOC_TYPE_MIN;
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        auto cellBox = _db.cell(cellIdx).cellBBoxOff();
        xMin = std::min(xMin, cellBox.xLo());
        xMax = std::max(xMax, cellBox.xHi());
        yMin = std::min(yMin, cellBox.yLo());
        yMax = std::max(yMax, cellBox.yHi());
    }
    _wStar = std::max(0.0, static_cast<RealType>(xMax - xMin)) + 500;
    _hStar = std::max(0.0, static_cast<RealType>(yMax - yMin)) + 500;
    this->generateConstraints();
    if (!lpDetailedPlacement())
    {
        INF("CG Legalizer: detailed placement fine tunning failed. Directly output legalization output. \n");
        return true;
    }

    this->generateConstraints();
    _wStar = lpLegalization(true);
    if (_wStar < 0)
    {
        return false;
    }
    _hStar = lpLegalization(false);
    if (_hStar < 0)
    {
        return false;
    }

    xMin = LOC_TYPE_MAX;
    xMax = LOC_TYPE_MIN;
    yMin = LOC_TYPE_MAX;
    yMax = LOC_TYPE_MIN;
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        auto cellBox = _db.cell(cellIdx).cellBBoxOff();
        xMin = std::min(xMin, cellBox.xLo());
        xMax = std::max(xMax, cellBox.xHi());
        yMin = std::min(yMin, cellBox.yLo());
        yMax = std::max(yMax, cellBox.yHi());
    }
    _wStar = std::max(0.0, static_cast<RealType>(xMax - xMin)) + 500;
    _hStar = std::max(0.0, static_cast<RealType>(yMax - yMin)) + 500;
    if (!lpDetailedPlacement())
    {
        INF("CG Legalizer: detailed placement fine tunning failed. Directly output legalization output. \n");
        return true;
    }



    INF("CG Legalizer: legalization finished\n");
    return true;
}

/// @brief Find which direction is the least displacement direction to make the two boxes disjoint
/// @return 1 if moving box2 left
/// @return 2 if moving box2 right
/// @return 3 if moving box2 down
/// @return 4 if moving box2 up
IntType minOverlappingDirection(const Box<LocType> &box1, const Box<LocType> &box2)
{
    bool box1IsLower = true;
    bool box1IsLefter = true;
    if (box1.xLo() < box2.xLo())
    {
        box1IsLefter = true;
    }
    if (box1.yLo() < box2.yLo())
    {
        box1IsLower = true;
    }

    //Assert(box1.overlap(box2));
    LocType dispL = box2.xHi() - box1.xLo();
    LocType dispR = box1.xHi() - box2.xLo();
    LocType dispB = box2.yHi() - box1.yLo();
    LocType dispT = box1.yHi() - box2.yLo();
    
    LocType minDisp = std::min(std::min(dispL, dispR), std::min(dispB, dispT));
    
    if (minDisp == dispL) {  return 1; }
    if (minDisp == dispR) {  return 2; }
    if (minDisp == dispB) {  return 3; }
    if (minDisp == dispT) {  return 4; }
    
    if (box1IsLower == true && box1IsLefter == true)
    {
        if (dispR < dispT)
        {
            return 2;
        }
        else
        {
            return 4;
        }
    }
    else if (box1IsLower == false && box1IsLefter == true)
    {
        if (dispR < dispB)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }
    else if (box1IsLower == false && box1IsLefter == false)
    {
        if (dispL < dispB)
        {
            return 1;
        }
        else
        {
            return 3;
        }
    }
    else
    {
        // true false
        if (dispL < dispT)
        {
            return 1;
        }
        else
        {
            return 4;
        }
    }

    /*
    LocType minDisp = std::min(std::min(dispL, dispR), std::min(dispB, dispT));
    
    if (minDisp == dispL) {  return 1; }
    if (minDisp == dispR) {  return 2; }
    if (minDisp == dispB) {  return 3; }
    if (minDisp == dispT) {  return 4; }
    */
    return 0;
}


void CGLegalizer::generateConstraints()
{
    _hCG.clear();
    _vCG.clear();
    _hConstraints.clear();
    _vConstraints.clear();
    // Init the irredundant constraint edges
    
    // Sort the edges of cells
    std::vector<BoxEdge> hBoxEdges, vBoxEdges;
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &cell = _db.cell(cellIdx);
        hBoxEdges.emplace_back(BoxEdge(cell.yLo(), cellIdx, false));
        hBoxEdges.emplace_back(BoxEdge(cell.yHi(), cellIdx, true));
        vBoxEdges.emplace_back(BoxEdge(cell.xLo(), cellIdx, false));
        vBoxEdges.emplace_back(BoxEdge(cell.xHi(), cellIdx, true));
    }
    std::sort(hBoxEdges.begin(), hBoxEdges.end());
    std::sort(vBoxEdges.begin(), vBoxEdges.end());


    // Add edges
    // Also calculate overlapAny. overlapAny[cellIdx] = true if the cell is overlapping with any other
    std::vector<bool> overlapAny(_db.numCells(), false);
    for (IndexType idx1 = 0; idx1 < _db.numCells(); ++idx1)
    {
        const auto &cell1 = _db.cell(idx1);
        auto cellBox1 = _db.cell(idx1).cellBBoxOff();
        for (IndexType idx2 = idx1 + 1; idx2 < _db.numCells(); ++idx2)
        {
            const auto &cell2 = _db.cell(idx2);
            auto cellBox2 = _db.cell(idx2).cellBBoxOff();
            // overlapAny part
            if (cellBox1.overlap(cellBox2))
            {
                AT(overlapAny, idx1) = true;
                AT(overlapAny, idx2) = true;
            }
            else
            {
                // FIXME: figure out whether we should add continue here
                continue;
            }
            // Add edge part
            // First detect whether the pair are sym pair or are both self-symmetric
            bool symPairFlag = false;
            bool selfSymFlag = false;
            for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
            {
                const auto &symGrp = _db.symGroup(symGrpIdx);
                for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
                {
                    const auto &symPair = symGrp.symPair(symPairIdx);
                    if ((idx1 == symPair.firstCell() && idx2 == symPair.secondCell())
                            || (idx2 == symPair.firstCell() && idx1 == symPair.secondCell()))
                    {
                        symPairFlag = true;
                        break;
                    }
                }
                bool found1 = false;
                bool found2 = false;
                for (IndexType selfSymIdx = 0; selfSymIdx < symGrp.numSelfSyms(); ++selfSymIdx)
                {
                    if (symGrp.selfSym(selfSymIdx) == idx1)
                    {
                        found1 = true;
                    }
                    if (symGrp.selfSym(selfSymIdx) == idx2)
                    {
                        found2 = true;
                    }
                }
                if (found1 && found2)
                {
                    selfSymFlag = true;
                    break;
                }
            }
            // If sym pair add them in the following way
            if (symPairFlag)
            {
                // Only add horizontal edges
                // Assume all horizontally symmetric
                if (cell1.xLo() <  cell2.xLo())
                {
                    _hConstraints.addConstraintEdge(idx1, idx2, - cell1.cellBBox().xLen());
                }
                else
                {
                    _hConstraints.addConstraintEdge(idx2, idx1, - cell2.cellBBox().xLen());
                }
                // Do not proceed into other types of constraint edges
                continue;
            }
            // Both are self symmetric cells
            if (selfSymFlag)
            {
                // Only add vertical edges
                // Add edges based on the displacement distance of moving one over another
                //LocType dispB = cell2.yHi() - cell1.yLoc();
                //LocType dispT = cell1.yHi() - cell2.yLoc();
                if (cell1.yLoc() < cell2.yLoc())
                {
                    _vConstraints.addConstraintEdge(idx1, idx2, - cell1.cellBBox().yLen());
                }
                else
                {
                    _vConstraints.addConstraintEdge(idx2, idx1, - cell2.cellBBox().yLen());
                }
            }
            IntType dispDirection = minOverlappingDirection(cellBox1, cellBox2);
            if (dispDirection == 1)
            {
                _hConstraints.addConstraintEdge(idx2, idx1, -cellBox2.xLen());
            }
            else if (dispDirection == 2)
            {
                _hConstraints.addConstraintEdge(idx1, idx2, -cellBox1.xLen());
            }
            else if (dispDirection == 3)
            {
                _vConstraints.addConstraintEdge(idx2, idx1, -cellBox2.yLen());
            }
            else if (dispDirection == 4)
            {
                _vConstraints.addConstraintEdge(idx1, idx2, -cellBox1.yLen());
            }
        }
    }
    
    // Execute the saved BoxEdge. 
    std::vector<IndexType> vOrders, hOrders;
    // "the right hand side candidates". The direction relation between vertices
    // The two additional vertices are virtural source and target
    std::vector<IntType> hCand(_db.numCells() + 2, -1), vCand(_db.numCells() + 2, -1);

    for (const auto &ev : hBoxEdges)
    {
        if (ev.isTop)
        {
            initIrredundantEdgesDelete(true, hOrders, hCand, ev.cellIdx, overlapAny);
        }
        else
        {
            initIrredundantEdgesInsert(true, hOrders, hCand, ev.cellIdx);
        }
    }
    for (const auto &ev : vBoxEdges)
    {
        
        if (ev.isTop)
        {
            initIrredundantEdgesDelete(false, vOrders, vCand, ev.cellIdx, overlapAny);
        }
        else
        {
            initIrredundantEdgesInsert(false, vOrders, vCand, ev.cellIdx);
        }
    }
#ifdef DEBUG_LEGALIZE
    DBG("Before reload: Print the generated constraints... \n\n");
    for (const auto &edge : _hConstraints.edges())
    {
        DBG("horizontal i %s \n",  edge.toStr().c_str());
    }
    for (const auto &edge : _vConstraints.edges())
    {
        DBG("vertical i  %s \n",  edge.toStr().c_str());
    }
#endif
    // Construct the boost constraint graphs
    constructConstraintGraphs();
    // Minimize the DAG
    dagTransitiveReduction(_hCG);
    dagTransitiveReduction(_vCG);
    // Get necessary edges
    getNecessaryEdges();
    // Minimize the DAG
    dagTransitiveReduction(_hCG);
    dagTransitiveReduction(_vCG);

    // FIXME: remove vertical edges between symmetric pairs and add the corresponding horizontal edges
    for (IndexType symGroupIdx = 0; symGroupIdx < _db.numSymGroups(); ++symGroupIdx)
    {
        const auto &symGroup = _db.symGroup(symGroupIdx);
        for (IndexType symPairIdx = 0; symPairIdx < symGroup.numSymPairs(); ++symPairIdx)
        {
            const auto & symPair = symGroup.symPair(symPairIdx);
            IndexType cellIdx1 = symPair.firstCell();
            IndexType cellIdx2 = symPair.secondCell();
            boost::remove_edge(boost::vertex(cellIdx1, _vCG.boostGraph()), boost::vertex(cellIdx2, _vCG.boostGraph()), 
                    _vCG.boostGraph());
            boost::remove_edge(boost::vertex(cellIdx2, _vCG.boostGraph()), boost::vertex(cellIdx1, _vCG.boostGraph()), 
                    _vCG.boostGraph());
            auto hEdge1 = boost::edge(boost::vertex(cellIdx1, _hCG.boostGraph()),
                    boost::vertex(cellIdx2, _hCG.boostGraph()), _hCG.boostGraph());
            auto hEdge2 = boost::edge(boost::vertex(cellIdx2, _hCG.boostGraph()),
                    boost::vertex(cellIdx1, _hCG.boostGraph()), _hCG.boostGraph());
            bool hasHorEdge = hEdge1.second || hEdge2.second;
            if (!hasHorEdge)
            {
                if (_db.cell(cellIdx1).xLo() <= _db.cell(cellIdx2).xLo())
                {
                    boost::add_edge(boost::vertex(cellIdx1, _hCG.boostGraph()),
                            boost::vertex(cellIdx2, _hCG.boostGraph()),
                            -_db.cell(cellIdx1).cellBBox().xLen(), _hCG.boostGraph());
                }
                else
                {
                    boost::add_edge(boost::vertex(cellIdx2, _hCG.boostGraph()),
                            boost::vertex(cellIdx1, _hCG.boostGraph()),
                            -_db.cell(cellIdx2).cellBBox().xLen(), _hCG.boostGraph());
                }
            }
        }
    } // for (IndexType symPairIdx = 0; symPairIdx < _db.numSymGroups(); ++symPairIdx)

    // reload the constraints from the boost graph and prepare for the LP solving
    readloadConstraints();
    for (const auto &edge : _hConstraints.edges())
    {
        if (edge.source() >= _db.numCells() || edge.target() >= _db.numCells())
        {
            continue;
        }
        bool hasDuplicate = _vConstraints.hasEdgeNoDirection(edge.source(), edge.target());
        if (hasDuplicate)
        {
            WRN("CG Legalizer: has duplicate edges \n");
            IndexType i = edge.source();
            IndexType j = edge.target();
            auto cellBox1 = _db.cell(i).cellBBoxOff();
            auto cellBox2 = _db.cell(j).cellBBoxOff();
            LocType spaceH = std::max(cellBox1.xLo() - cellBox2.xHi(), cellBox2.xLo() - cellBox1.xHi());
            LocType spaceV = std::max(cellBox1.yLo() - cellBox2.yHi(), cellBox2.yLo() - cellBox1.yHi());
            _hConstraints.removeConstraintEdge(i, j);
            _vConstraints.removeConstraintEdge(i, j);
            if (spaceH >= spaceV)
            {
                // Add edge to horizontal constraint graph
                if (cellBox1.xLo() < cellBox2.xLo())
                {
                    _hConstraints.addConstraintEdge(i, j, 0);
                }
                else
                {
                    _hConstraints.addConstraintEdge(j, i, 0);
                }
            }
            else
            {
                // Add edge to vertical constraint graph
                if (cellBox1.yLo() < cellBox2.yLo())
                {
                    _vConstraints.addConstraintEdge(i, j, 0);
                }
                else
                {
                    _vConstraints.addConstraintEdge(j, i, 0);
                }
            }
        }
    }
#ifdef DEBUG_LEGALIZE
    DBG("After reload. Print the generated constraints... \n\n");
    for (const auto &edge : _hConstraints.edges())
    {
        DBG("horizontal i %s \n",  edge.toStr().c_str());
    }
    for (const auto &edge : _vConstraints.edges())
    {
        DBG("vertical i  %s \n",  edge.toStr().c_str());
    }
#endif
}

void CGLegalizer::readloadConstraints()
{
    // Clear the constraint edges and reload from the boost graph
    _hConstraints.clear();
    _vConstraints.clear();
    auto idxMapH = boost::get(boost::vertex_index, _hCG.boostGraph());
    auto idxMapV = boost::get(boost::vertex_index, _vCG.boostGraph());
    auto weightMapH = boost::get(boost::edge_weight, _hCG.boostGraph());
    auto weightMapV = boost::get(boost::edge_weight, _vCG.boostGraph());
    ConstraintGraph::edge_iterator ei, eiEnd;
    // horizontal
    for (boost::tie(ei, eiEnd) = boost::edges(_hCG.boostGraph()); ei != eiEnd; ++ei)
    {
        IndexType sourceIdx = idxMapH[boost::source(*ei, _hCG.boostGraph())]; // source node of the edge
        IndexType targetIdx = idxMapH[boost::target(*ei, _hCG.boostGraph())]; // target node of the edge
        auto weight = boost::get(weightMapH, *ei);
        _hConstraints.addConstraintEdge(sourceIdx, targetIdx, weight);
    }
    // vertical
    for (boost::tie(ei, eiEnd) = boost::edges(_vCG.boostGraph()); ei != eiEnd; ++ei)
    {
        IndexType sourceIdx = idxMapV[boost::source(*ei, _vCG.boostGraph())]; // source node of the edge
        IndexType targetIdx = idxMapV[boost::target(*ei, _vCG.boostGraph())]; // target node of the edge
        auto weight = boost::get(weightMapV, *ei);
        _vConstraints.addConstraintEdge(sourceIdx, targetIdx, weight);
    }
}

void CGLegalizer::getNecessaryEdges()
{
    // DFS algorithm to get the other necessary edges
    // FIXME: no symmetry nodes
    IndexType numNodes = _db.numCells();

    Vector2D<IntType> dpTabH(numNodes, numNodes, 0);
    Vector2D<IntType> dpTabV(numNodes, numNodes, 0);
    std::vector<IntType> visitedH(numNodes, 0);
    std::vector<IntType> visitedV(numNodes, 0);
    IndexType sourceIdx = _db.numCells();
    ConstraintGraph::IndexMap idxMap = boost::get(boost::vertex_index, _hCG.boostGraph());

    // DFS to find the mutal constrainted cells
    dfsGraph(dpTabH, visitedH, sourceIdx, _hCG, idxMap);
    dfsGraph(dpTabV, visitedV, sourceIdx, _vCG, idxMap);

    for (IndexType i = 0; i < numNodes; ++i)
    {
        for (IndexType j = i+1; j < numNodes; ++j)
        {
            bool dp = dpTabH.at(i, j) == 1 || dpTabH.at(j, i) == 1
                || dpTabV.at(i, j) == 1 || dpTabV.at(j, i) == 1;
            if (!dp)
            {
                addEdgeGreedy(i, j);
            }
        }
    }
}

void CGLegalizer::addEdgeGreedy(IndexType i, IndexType j)
{
    auto cellBox1 = _db.cell(i).cellBBoxOff();
    auto cellBox2 = _db.cell(j).cellBBoxOff();
    LocType spaceH = std::max(cellBox1.xLo() - cellBox2.xHi(), cellBox2.xLo() - cellBox1.xHi());
    LocType spaceV = std::max(cellBox1.yLo() - cellBox2.yHi(), cellBox2.yLo() - cellBox1.yHi());
    if (spaceH >= spaceV)
    {
        // Add edge to horizontal constraint graph
        if (cellBox1.xLo() < cellBox2.xLo())
        {
            boost::add_edge(boost::vertex(i, _hCG.boostGraph()), boost::vertex(j, _hCG.boostGraph()), 
                    -cellBox1.xLen(), _hCG.boostGraph());
        }
        else
        {
            boost::add_edge(boost::vertex(j, _hCG.boostGraph()), boost::vertex(i, _hCG.boostGraph()), 
                    -cellBox2.xLen(), _hCG.boostGraph());
        }
    }
    else
    {
        // Add edge to vertical constraint graph
        if (cellBox1.yLo() < cellBox2.yLo())
        {
            boost::add_edge(boost::vertex(i, _vCG.boostGraph()), boost::vertex(j, _vCG.boostGraph()), 
                    -cellBox1.yLen(), _vCG.boostGraph());
        }
        else
        {
            boost::add_edge(boost::vertex(j, _vCG.boostGraph()), boost::vertex(i, _vCG.boostGraph()), 
                    -cellBox2.yLen(), _vCG.boostGraph());
        }
    }
}

void CGLegalizer::dfsGraph(Vector2D<IntType>& dpTab, std::vector<IntType> &visited, IndexType nodeIdx, 
        ConstraintGraph &cg, ConstraintGraph::IndexMap &idxMap)
{
    IndexType sourceIdx  = _db.numCells();
    IndexType targetIdx = sourceIdx + 1;
    if (nodeIdx < sourceIdx)
    {
        AT(visited, nodeIdx) = 1;
        dpTab.at(nodeIdx, nodeIdx) = 1;
    }
    auto neighbors = boost::adjacent_vertices(boost::vertex(nodeIdx, cg.boostGraph()), cg.boostGraph());
    for (; neighbors.first != neighbors.second; ++neighbors.first)
    {
        IndexType neiNode = idxMap[*neighbors.first];
        if (neiNode == targetIdx)
        {
            continue;
        }
        if (AT(visited, neiNode) == 0)
        {
            dfsGraph(dpTab, visited, neiNode, cg, idxMap);
        }
        if (nodeIdx == sourceIdx)
        {
            continue;
        }
        bool allOnes = true;
        for (IndexType idx = 0; idx < dpTab.ySize(); ++ idx)
        {
            auto dp = dpTab.at(nodeIdx, idx);
            if (dp == 0)
            {
                allOnes = false;
                break;
            }
        }
        if (!allOnes)
        {
            for(IndexType idx = 0; idx < dpTab.ySize(); ++ idx)
            {
                if (dpTab.at(nodeIdx, idx) == 1 || dpTab.at(neiNode, idx) == 1)
                {
                    dpTab.at(nodeIdx, idx) = 1;
                }
                else
                {
                    dpTab.at(nodeIdx, idx) = 0;
                }
            }
        }
    } // for (; neighbors.first != neighbors.second; ++neighbors.first)
}

void CGLegalizer::dagTransitiveReduction(ConstraintGraph &cg)
{
    // DFS-based transitive reduction N(N+M)
    IndexType numNodes = boost::num_vertices(cg.boostGraph());
    // Build edge matrix
    Vector2D<IntType> edgeMat(numNodes, numNodes, 0), reachable(numNodes, numNodes, 0);
    auto edges = boost::edges(cg.boostGraph());
    for (auto it = edges.first; it != edges.second; ++it)
    {
        IntType sourceNode = boost::source(*it, _hCG.boostGraph());
        IntType targetNode = boost::target(*it, _hCG.boostGraph());
        edgeMat.at(sourceNode, targetNode) = 1;
    }
    // DFS remove edge
    std::vector<bool> visited(numNodes, false);
    ConstraintGraph::IndexMap idxMap = boost::get(boost::vertex_index, cg.boostGraph());
    dfsRemoveTransitiveEdge(cg, edgeMat, _db.numCells(), visited, reachable, idxMap);
}

bool CGLegalizer::dfsRemoveTransitiveEdge(ConstraintGraph &cg, Vector2D<IntType> &edgeMat, IndexType node, 
        std::vector<bool> &visited, Vector2D<IntType> &reachable, ConstraintGraph::IndexMap &idxMap)
{
    bool hasTransitiveEdge = false;
    visited[node] = true;
    auto neighbors = boost::adjacent_vertices(boost::vertex(node, cg.boostGraph()), cg.boostGraph());
    IndexType numNodes = boost::num_vertices(cg.boostGraph());
    for (; neighbors.first != neighbors.second; ++neighbors.first)
    {
        IndexType neighborNode = idxMap[*neighbors.first];
        // FIXME: reimplement this function to avoid dynamically deleting the edges
        if (neighborNode > numNodes)
        {
            assert(0);
            ERR("CG legalizer: edge not found in transitive edge reduction \n");
            break;
        }
        if (!visited[neighborNode])
        {
            if (dfsRemoveTransitiveEdge(cg, edgeMat, neighborNode, visited, reachable, idxMap))
            {
                hasTransitiveEdge = true;
            }
        }
        for (IndexType i = 0; i < numNodes; ++i)
        {
            if (reachable.at(node, i) == 0 && reachable.at(neighborNode, i) == 1)
            {
                // If has edge, remove it
                if (edgeMat.at(node, i) == 1)
                {
                    edgeMat.at(node, i) = 0;
                    boost::remove_edge(boost::vertex(node, cg.boostGraph()), 
                            boost::vertex(i, cg.boostGraph()), 
                            cg.boostGraph());
                    hasTransitiveEdge = true;
                }
                reachable.at(node, i) = 1;
            }
        }
        reachable.at(node, neighborNode) = 1;
    }
    return hasTransitiveEdge;
}

void CGLegalizer::constructConstraintGraphs()
{
    IndexType numHNodes = _db.numCells() + 2;
    IndexType numVNodes = _db.numCells() + 2;
    _hCG.allocateVertices(numHNodes);
    for (const auto &edge : _hConstraints.edges())
    {
        _hCG.addEdge(edge.source(), edge.target(), edge.weight());
    }
    _vCG.allocateVertices(numVNodes);
    for (const auto &edge : _vConstraints.edges())
    {
        _vCG.addEdge(edge.source(), edge.target(), edge.weight());
    }
}

bool horizontalOverlapping(const Box<LocType> &lhs, const Box<LocType> &rhs)
{
    if (lhs.xHi() <= rhs.xLo() || rhs.xHi() <= lhs.xLo())
    {
        return false;
    }
    return true;
}


bool verticalOverlapping(const Box<LocType> &lhs, const Box<LocType> &rhs)
{
    if (lhs.yHi() <= rhs.yLo() || rhs.yHi() <= lhs.yLo())
    {
        return false;
    }
    return true;
}

void CGLegalizer::initIrredundantEdgesInsert(bool isHor , std::vector<IndexType> &orders, 
        std::vector<IntType> &cand, IndexType cellIdx)
{
    orders.emplace_back(cellIdx);
    if (isHor)
    {
        auto compLeft = [&] (const IndexType lhs, const IndexType rhs)
        {
            return _db.cell(lhs).xLo() < _db.cell(rhs).xLo();
        };
        std::sort(orders.begin(), orders.end(), compLeft);
    }
    else
    {
        auto compBottom = [&] (const IndexType lhs, const IndexType rhs)
        {
            return _db.cell(lhs).yLoc() < _db.cell(rhs).yLoc();
        };
        std::sort(orders.begin(), orders.end(), compBottom);
    }
    auto it = std::find(orders.begin(), orders.end(), cellIdx);
    // First check the outgoing node
    if (it != orders.end() - 1)
    {
        // the cell has right neighbor
        const auto & cellBox1 = _db.cell(*it).cellBBoxOff();
        auto it2 = it + 1; 
        while (it2 != orders.end())
        {
            const auto &cellBox2 = _db.cell(*it2).cellBBoxOff();
            /// FIXME: I am not sure about whether the following checking should be separated?
            if (!horizontalOverlapping(cellBox1, cellBox2) || !verticalOverlapping(cellBox1, cellBox2))
            {
                break;
            }
            ++it2;
        }
        if (it2 == orders.end())
        {
            AT(cand, cellIdx) = cand.size() - 1; // Point to virtual target
        }
        else
        {
            IndexType cellIdx2 = *it2;
            AT(cand, cellIdx) = cellIdx2;
        }
    } 
    else // it != orders.end() - 1
    {
        AT(cand, cellIdx) = cand.size() - 1; // Point to virtual target
    }
    // Then check the ingoing node
    if (it != orders.begin())
    {
        // Has left neighbor
        const auto & cellBox1 = _db.cell(*it).cellBBoxOff();
        auto it2 = it - 1;
        while (it2 + 1 != orders.begin())
        {
            const auto &cellBox2 = _db.cell(*it2).cellBBoxOff();
            /// FIXME: I am not sure about whether the following checking should be separated?
            if (!horizontalOverlapping(cellBox1, cellBox2) || !verticalOverlapping(cellBox1, cellBox2))
            {
                break;
            }
            --it2;
        }
        if (it2 + 1 == orders.begin())
        {
            AT(cand, cand.size() - 1) = cellIdx; // Point virtual source to this cell
        }
        else
        {
            IndexType cellIdx2 = *it2;
            AT(cand, cellIdx2) = cellIdx; // Point it2 to it
        }
    }
    else // it != orders.begin()
    {
        AT(cand, cand.size() - 1) = cellIdx; // Point virtual source to this cell
    }
}

void CGLegalizer::initIrredundantEdgesDelete(bool isHor, std::vector<IndexType> &orders, std::vector<IntType> &cand, 
        IndexType cellIdx, std::vector<bool> &overlapAny)
{
    IntType targetIdx = static_cast<IntType>(cand.size()) - 1;
    IntType sourceIdx = static_cast<IntType>(cand.size()) - 2;
    auto it = std::find(orders.begin(), orders.end(), cellIdx);
    // First working on the right hand side
    if (it != orders.end() - 1)
    {
        // Has right neighbor
        const auto & cellBox1 = _db.cell(*it).cellBBoxOff();
        auto it2 = it + 1;
        while (it2 != orders.end())
        {
            const auto &cellBox2 = _db.cell(*it2).cellBBoxOff();
            while (!horizontalOverlapping(cellBox1, cellBox2) || !verticalOverlapping(cellBox1, cellBox2))
            {
                break;
            }
            ++it2;
        }
        if (it2 == orders.end())
        {
            // Point to the virtual target
            if (AT(cand, cellIdx) == targetIdx)
            {
                if (!isHor)
                {
                    IntType weight = - cellBox1.yLen();
                    _vConstraints.addConstraintEdge(cellIdx, targetIdx, weight);
                }
                else
                {
                    IntType weight = - cellBox1.xLen();
                    _hConstraints.addConstraintEdge(cellIdx, targetIdx, weight);
                }
            }
        }
        else // it2 == orders.end()
        {
            IndexType cellIdx2 = *it2;
            auto cellBox2 = _db.cell(*it2).cellBBoxOff();
            if (AT(cand, cellIdx) == static_cast<IntType>(cellIdx2) 
                    || AT(cand, cellIdx) == targetIdx
                    || (!horizontalOverlapping(cellBox1, cellBox2) && !verticalOverlapping(cellBox1, cellBox2))
                    )
            {
                if (isHor)
                {
                    IntType weight = - cellBox1.xLen();
                    _hConstraints.addConstraintEdge(cellIdx, cellIdx2, weight);
                }
                else
                {
                    IntType weight = - cellBox1.yLen();
                    _vConstraints.addConstraintEdge(cellIdx, cellIdx2, weight);
                }
            }
            else // AT(cand, cellIdx) ...
            {
                while (it2 != orders.end()
                        && AT(overlapAny, cellIdx2)
                        && ! (horizontalOverlapping(cellBox1, cellBox2) && verticalOverlapping(cellBox1, cellBox2)))
                {
                    if (isHor)
                    {
                        IntType weight = -cellBox1.xLen();
                        _hConstraints.addConstraintEdge(cellIdx, *it2, weight);
                    }
                    else
                    {
                        IntType weight = -cellBox2.yLen();
                        _vConstraints.addConstraintEdge(cellIdx, *it2, weight);
                    }
                    ++it2;
                    if (it2 == orders.end())
                    {
                        break;
                    }
                    cellBox2 = _db.cell(*it2).cellBBoxOff();
                }
            }
        }
    }
    else // it != orders.end() - 1
    {
        const auto & cellBox1 = _db.cell(cellIdx).cellBBoxOff();
        // Point to the virtual target
        if (AT(cand, cellIdx) == targetIdx)
        {
            if (isHor)
            {
                IntType weight = -cellBox1.xLen();
                _hConstraints.addConstraintEdge(cellIdx, targetIdx, weight);
            }
            else
            {
                IntType weight = -cellBox1.yLen();
                _vConstraints.addConstraintEdge(cellIdx, targetIdx, weight);
            }
        }
    }

    // Then check the left
    if (it != orders.begin())
    {
        const auto &cellBox1 = _db.cell(cellIdx).cellBBoxOff();
        // Has left neighbor
        auto it2 = it - 1;
        while (it2 + 1 != orders.begin())
        {
            const auto &cellBox2 = _db.cell(*it2).cellBBoxOff();
            if (!horizontalOverlapping(cellBox1, cellBox2) || !verticalOverlapping(cellBox1, cellBox2))
            {
                break;
            }
            --it2;
        }
        if (it2 + 1 == orders.begin())
        {
            // Virtual source point to it
            if (AT(cand, sourceIdx) == static_cast<IntType>(cellIdx) 
                    || AT(overlapAny, cellIdx))
            {
                // Add both h and v constraints
                // Weight = 0
                if (isHor)
                {
                    _hConstraints.addConstraintEdge(sourceIdx, cellIdx, 0);
                }
                else
                {
                    _vConstraints.addConstraintEdge(sourceIdx, cellIdx, 0);
                }
            }
        }
        else // it2 + 1 == orders.begin()
        {
            auto cellBox2 = _db.cell(*it2).cellBBoxOff();
            IndexType cellIdx2 = *it2;
            if (isHor)
            {
                IntType weight = -cellBox2.xLen();
                _hConstraints.addConstraintEdge(cellIdx2, cellIdx, weight);
            }
            else
            {
                IntType weight = -cellBox2.yLen();
                _vConstraints.addConstraintEdge(cellIdx2, cellIdx, weight);
            }
            // Ensure no overlap
            if (it2 != orders.begin())
            {
                --it2; // So that the --it2 in the while loop can be in the end
            }
            while (it2 != orders.begin())
            {
                auto cellBox2 = _db.cell(*it2).cellBBoxOff();
                if (!AT(overlapAny, *it2))
                {
                    --it2;
                    continue;
                }
                if (horizontalOverlapping(cellBox1, cellBox2) && verticalOverlapping(cellBox1, cellBox2))
                {
                    --it2;
                    continue;
                }
                if (isHor)
                {
                    IntType weight = -cellBox2.xLen();
                    _hConstraints.addConstraintEdge(cellIdx2, cellIdx, weight);
                }
                else
                {
                    IntType weight = -cellBox2.yLen();
                    _vConstraints.addConstraintEdge(cellIdx2, cellIdx, weight);
                }
                --it2;
            }
        }
    } // it != orders.begin()
    else
    {
        // Virtual source point to it
        if (AT(cand, sourceIdx) == static_cast<IntType>(cellIdx) || AT(overlapAny, cellIdx))
        {
            // Weight = 0
            if (isHor)
            {
                _hConstraints.addConstraintEdge(sourceIdx, cellIdx, 0); 
            }
            else
            {
                _vConstraints.addConstraintEdge(sourceIdx, cellIdx, 0);
            }
        }
    }
    orders.erase(it);
}

RealType CGLegalizer::lpLegalization(bool isHor)
{
    RealType obj;
    if (isHor)
    {
        INF("CG legalizer: legalize horizontal LP...\n");
        auto solver = LpLegalizeSolver(_db, _hConstraints, isHor);
        bool optimal = solver.solve();
        if (optimal)
        {
            solver.exportSolution();
        }
        else
        {
            return -1;
        }
        obj = solver.evaluateObj();
        //_db.drawCellBlocks("./debug/after_legalization_hor.gds");
    }
    else
    {
        INF("CG legalizer: legalize vertical LP...\n");
        auto solver = LpLegalizeSolver(_db, _vConstraints, isHor);
        bool optimal = solver.solve();
        if (optimal)
        {
            solver.exportSolution();
        }
        else
        {
            return -1;
        }
        obj = solver.evaluateObj();
    }
#ifdef DEBUG_LEGALIZE
#ifdef DEBUG_DRAW
    _db.drawCellBlocks("./debug/after_legalization.gds");
#endif
#endif

    return obj;
}

bool CGLegalizer::lpDetailedPlacement()
{
    // Horizontal
    INF("CG legalizer: detailed placement horizontal LP...\n");
    auto horSolver = LpLegalizeSolver(_db, _hConstraints, true, 1, 0);
#ifdef DEBUG_LEGALIZE
    DBG("wstar for width %f \n", _wStar);
#endif
    horSolver.setWStar(_wStar);
    bool horpass = horSolver.solve();
    if (horpass)
    {
        horSolver.exportSolution();
    }
    else
    {
        return false;
    }
    //_db.drawCellBlocks("./debug/after_dp_hor.gds");
    // Vertical
    INF("CG legalizer: detailed placement vertical LP...\n");
    auto verSolver = LpLegalizeSolver(_db, _vConstraints, false, 1, 0);
    verSolver.setWStar(_hStar);
    bool verpass =verSolver.solve();
    if (verpass)
    {
        verSolver.exportSolution();
    }
    else
    {
        return false;
    }
    
#ifdef DEBUG_LEGALIZE
#ifdef DEBUG_DRAW
    _db.drawCellBlocks("./debug/after_dp.gds");
#endif
#endif
    return true;
}

PROJECT_NAMESPACE_END
