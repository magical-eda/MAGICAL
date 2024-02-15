#include "alignGrid.h"
#include "constraintGraphGeneration.h"
#include  <boost/heap/fibonacci_heap.hpp>

PROJECT_NAMESPACE_BEGIN

template<typename T>
T floorDif(T n, T stepSize)
{
    throw std::invalid_argument("Unsupport argument types. Need to be integer");
}
template<>
IntType floorDif(IntType n, IntType stepSize)
{
    return n % stepSize;
}


template<typename T>
T ceilDif(T n, T stepSize)
{
    throw std::invalid_argument("Unsupport argument types. Need to be integer");
}
template<>
IntType ceilDif(IntType n, IntType stepSize)
{
    return stepSize - n % stepSize;
}

void GridAligner::align(LocType stepSize)
{
    INF("Align to grid %d \n", stepSize);
    _stepSize = stepSize;
#ifdef MULTI_SYM_GROUP
    Assert(0);
#else
    //naiveAlign();
    bettherThanNaiveAlign();
#endif
    //adjustOffset(XY<LocType>(0, 0));
    INF("Ideaplace: finished alignment\n");
    for (IndexType cellIdx =0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &cell = _db.cell(cellIdx);
        INF("IDEAPLACE::%s cell %d %s center %d %d\n ", __FUNCTION__, cellIdx, cell.name().c_str(), cell.xCenter(), cell.yCenter());
    }
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto &cell = _db.cell(cellIdx);
        LocType xLo = cell.xLo();
        LocType yLo = cell.yLo();
        if (xLo % _stepSize != 0)
        {
            ERR("Ideaplace: align to grid failed xLo %d \n", xLo);
            Assert(0);
        }
        if(yLo % _stepSize != 0)
        {
            ERR("Ideaplace: align to grid failed yLo %d  \n");
            Assert(0);
        }
    }
    if (!_db.checkSym())
    {
        WRN("Ideaplace: the final results is not completely symmetric\n");
    }
}

LocType GridAligner::findCurrentSymAxis()
{
    LocType symAxis = 0;
    bool foundSym = false;
    for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
    {
        const auto &symGrp = _db.symGroup(symAxis);
        for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
        {
            const auto &symPair = symGrp.symPair(symPairIdx);
            symAxis = (_db.cell(symPair.firstCell()).xLo() + _db.cell(symPair.secondCell()).xHi()) / 2;
            foundSym = true;
            goto theEnd;
        }
        for (IndexType ssIdx = 0; ssIdx < symGrp.numSelfSyms(); ++ssIdx)
        {
            const auto &ssCell = _db.cell(symGrp.selfSym(ssIdx));
            symAxis = (ssCell.xLo() + ssCell.xHi()) / 2;
            foundSym = true;
            goto theEnd;
        }
    }
    if (!foundSym)
    {
        LocType xmin =LOC_TYPE_MAX;
        LocType xmax = LOC_TYPE_MIN;
        for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
        {
            const auto &cell = _db.cell(cellIdx);
            xmin = std::min(cell.xLo(), xmin);
            xmax = std::max(cell.xHi(), xmax);
        }
        LocType xCenter = (xmax + xmin) / 2;
        xCenter = xCenter + ceilDif(xCenter , _stepSize) + _stepSize / 2; // Ensure its on the correct grid
        return xCenter;
    }
theEnd:
    return symAxis;
}

void GridAligner::adjustSymPair(const SymPair &symPair, LocType symAxis)
{
    IndexType leftCellIdx, rightCellIdx;
    if (_db.cell(symPair.firstCell()).xLoc() < _db.cell(symPair.secondCell()).xLoc())
    {
        leftCellIdx = symPair.firstCell();
        rightCellIdx = symPair.secondCell();
    }
    else
    {
        leftCellIdx = symPair.secondCell();
        rightCellIdx = symPair.firstCell();
    }
    _db.cell(leftCellIdx).setXLoc(_db.cell(leftCellIdx).xLoc() - floorDif(_db.cell(leftCellIdx).xLo(), _stepSize));
    LocType rightXLo = 2 * symAxis - _db.cell(leftCellIdx).xHi();
    _db.cell(rightCellIdx).setXLoc(_db.cell(rightCellIdx).xLoc() + rightXLo - _db.cell(rightCellIdx).xLo());
    assert(_db.cell(leftCellIdx).xLo() % _stepSize == 0);
    assert(_db.cell(rightCellIdx).xLo() % _stepSize == 0);
}

void GridAligner::adjustSelfSym(IndexType cellIdx, LocType symAxis)
{
    auto &cell = _db.cell(cellIdx);
    LocType center = cell.xCenter(); 
    cell.setXLoc(cell.xLoc() -  center + symAxis);
    assert(cell.xLo() % _stepSize == 0);
    Assert(cell.xCenter() == symAxis);
}

struct CellIdxNode
{
    CellIdxNode(IndexType cellIdx_, std::vector<char> *fixed_, std::vector<char> *hasSym_,
            std::vector<LocType> *dis2SymAxis_) 
        : cellIdx(cellIdx_), fixed(fixed_), hasSym(hasSym_), dis2SymAxis(dis2SymAxis_) {}
    IndexType cellIdx;
    std::vector<char>  *fixed;
    std::vector<char>  *hasSym;
    std::vector<LocType> *dis2SymAxis;
    bool operator<(const CellIdxNode &rhs) const
    {
        if (hasSym->at(cellIdx) == hasSym->at(rhs.cellIdx))
        {
            if (fixed->at(cellIdx) == fixed->at(rhs.cellIdx))
            {
                if (dis2SymAxis->at(cellIdx) == dis2SymAxis->at(rhs.cellIdx))
                {
                    return cellIdx > rhs.cellIdx;
                }
                else
                {
                    return dis2SymAxis->at(cellIdx) > dis2SymAxis->at(rhs.cellIdx);
                }
            }
            else
            {
                return !fixed->at(cellIdx);
            }
        }
        else
        {
            return !hasSym->at(cellIdx);
        }

    }
};
struct compareCellIdxNode
{
    bool operator()(const CellIdxNode &cell1, const CellIdxNode &cell2) const 
    {
        return cell1 < cell2;
    }
};

void GridAligner::bettherThanNaiveAlign()
{
    // Assume one symmetric axis and it should be at the center line of a grid
    auto symAxis = findCurrentSymAxis();
    auto symDif = floorDif(static_cast<IntType>(symAxis - 0.5*_stepSize), _stepSize);
    symAxis = symAxis - symDif;
    INF("Alignment symaxis %d \n", symAxis);
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        auto &cell = _db.cell(cellIdx);
        if (cell.cellBBox().xLen() % _stepSize != 0)
        {
            ERR("Cell width %d is not multiples of the grid size %d ! \n", cell.cellBBox().xLen(), _stepSize);
        }
        if (cell.cellBBox().yLen() % _stepSize != 0)
        {
            ERR("Cell height %d is not multiples of the grid size %d ! \n", cell.cellBBox().yLen(), _stepSize);
        }

        cell.setXLoc(cell.xLoc() - symDif);
        cell.setYLoc(cell.yLoc() -  floorDif(cell.yLo(), _stepSize));
        if (cell.xLo() < symDif)
        {
            cell.setXLoc(cell.xLoc() - floorDif(cell.xLo(), _stepSize));
        }
        else
        {
            cell.setXLoc(cell.xLoc() + ceilDif(cell.xLo(), _stepSize));
        }
    }
    // Sweep line
    Constraints hc;
    Constraints vc;
    SweeplineConstraintGraphGenerator sweepline(_db, hc, vc);
    sweepline.solve();
    for (IndexType symGrpIdx = 0; symGrpIdx < _db.numSymGroups(); ++symGrpIdx)
    {
        const auto &symGrp = _db.symGroup(symGrpIdx);
        for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
        {
            const auto &symPair = symGrp.symPair(symPairIdx);
            adjustSymPair(symPair, symAxis);
        }
        for (IndexType ssIdx = 0; ssIdx < symGrp.numSelfSyms(); ++ssIdx)
        {
            IndexType selfSymNetIdx = symGrp.selfSym(ssIdx);
            adjustSelfSym(selfSymNetIdx, symAxis);
        }
    }

    // Legalize 
    std::vector<char> xDecided(_db.numCells(), false);
    std::vector<char> inHeap(_db.numCells(), true);
    std::vector<char> hasSym(_db.numCells(), false);
    std::vector<LocType> dis2SymAxis(_db.numCells());
    // fixed all syms
    using HeapType = boost::heap::fibonacci_heap<CellIdxNode, boost::heap::compare<compareCellIdxNode>>;
    using HandleType = HeapType::handle_type;
    HeapType cellNodeHeap;
    std::vector<HandleType> heapHandles;
    // Fix all the cells with symmetric constraints
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        if (_db.cell(cellIdx).hasSym())
        {
            xDecided.at(cellIdx) = true;
            hasSym.at(cellIdx) = true;
        }
        LocType center = (_db.cell(cellIdx).xLo() + _db.cell(cellIdx).xHi()) / 2;
        dis2SymAxis.at(cellIdx) = std::abs(center - symAxis);
    }
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        heapHandles.emplace_back(cellNodeHeap.push(CellIdxNode(cellIdx, &xDecided, &hasSym, &dis2SymAxis)));
    }
    // Decompose the constraints into look-up table
    std::vector<std::vector<IndexType>> edgeLUT;
    std::vector<std::vector<IndexType>> edgeLUTReverse;
    edgeLUT.resize(_db.numCells());
    edgeLUTReverse.resize(_db.numCells());
    for (const auto &edge : hc.edges())
    {
        if (edge.source() < _db.numCells() && edge.target() < _db.numCells())
        {
            edgeLUT.at(edge.source()).emplace_back(edge.target());
            edgeLUTReverse.at(edge.target()).emplace_back(edge.source());
        }
    }
    IntType count = 0;
    IntType trialLimit = _db.numCells() * 10;
    while (!cellNodeHeap.empty())
    {
        bool checkSym = count   < trialLimit;
        ++count;
        const auto &top = cellNodeHeap.top();
        IndexType cellIdx = top.cellIdx;
        cellNodeHeap.pop();
        inHeap.at(cellIdx) = false;
        xDecided.at(cellIdx) = true;
        for (IndexType target : edgeLUT.at(cellIdx))
        {
            auto spacing = _db.cell(target).xLo() - _db.cell(cellIdx).xHi();
            if (spacing >= 0)
            {
                continue;
            }
            _db.cell(target).setXLoc(_db.cell(target).xLoc() - spacing);
            xDecided.at(target) = true;
            if (inHeap.at(target))
            {
                cellNodeHeap.update(heapHandles.at(target), CellIdxNode(target, &xDecided, &hasSym, &dis2SymAxis));
            }
            else
            {
                inHeap.at(target) = true;
                heapHandles.at(target) = cellNodeHeap.push(CellIdxNode(target, &xDecided, &hasSym, &dis2SymAxis));
            }
            if (_db.cell(target).hasSymPair() && checkSym)
            {
                IndexType symNetIdx = _db.cell(target).symNetIdx();
                _db.cell(symNetIdx).setXLoc(_db.cell(symNetIdx).xLoc() + spacing);
                xDecided.at(symNetIdx) = true;
                if (inHeap.at(symNetIdx))
                {
                    cellNodeHeap.update(heapHandles.at(symNetIdx), CellIdxNode(symNetIdx, &xDecided, &hasSym, &dis2SymAxis));
                }
                else
                {
                    inHeap.at(symNetIdx) = true;
                    heapHandles.at(symNetIdx) = cellNodeHeap.push(CellIdxNode(symNetIdx, &xDecided, &hasSym, &dis2SymAxis));
                }
            }
            if (_db.cell(target).isSelfSym() && checkSym)
            {
                auto &cell = _db.cell(target);
                LocType center = cell.xCenter(); 
                cell.setXLoc(cell.xLoc() -  center + symAxis);
            }
        }
        for (IndexType source : edgeLUTReverse.at(cellIdx))
        {
            auto spacing = _db.cell(cellIdx).xLo() - _db.cell(source).xHi();
            if (spacing >= 0)
            {
                continue;
            }
            _db.cell(source).setXLoc(_db.cell(source).xLoc() + spacing);
            xDecided.at(source) = true;
            if (inHeap.at(source))
            {
                cellNodeHeap.update(heapHandles.at(source), CellIdxNode(source, &xDecided, &hasSym, &dis2SymAxis));
            }
            else
            {
                inHeap.at(source) = true;
                heapHandles.at(source) = cellNodeHeap.push(CellIdxNode(source, &xDecided, &hasSym, &dis2SymAxis));
            }
            if (_db.cell(source).hasSymPair() && checkSym)
            {
                IndexType symNetIdx = _db.cell(source).symNetIdx();
                _db.cell(symNetIdx).setXLoc(_db.cell(symNetIdx).xLoc() - spacing);
                xDecided.at(symNetIdx) = true;
                if (inHeap.at(symNetIdx))
                {
                    cellNodeHeap.update(heapHandles.at(symNetIdx), CellIdxNode(symNetIdx, &xDecided, &hasSym, &dis2SymAxis));
                }
                else
                {
                    inHeap.at(symNetIdx) = true;
                    heapHandles.at(symNetIdx) = cellNodeHeap.push(CellIdxNode(symNetIdx, &xDecided, &hasSym, &dis2SymAxis));
                }
            }
            if (_db.cell(source).isSelfSym() && checkSym)
            {
                auto &cell = _db.cell(source);
                LocType center = cell.xCenter(); 
                cell.setXLoc(cell.xLoc() -  center + symAxis);
            }
        }
    }
}

void GridAligner::naiveAlign()
{
    auto symAxis = findCurrentSymAxis();
    // Assume one symmetric axis
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        auto &cell = _db.cell(cellIdx);
        if (cell.xLo() <= symAxis)
        {
            cell.setXLoc(cell.xLoc() - floorDif(cell.xLo(), _stepSize));
        }
        else
        {
            cell.setXLoc(cell.xLoc() - floorDif(cell.xLo(), _stepSize));
        }
        cell.setYLoc(cell.yLoc() -  floorDif(cell.yLo(), _stepSize));
    }
}

void GridAligner::adjustOffset(const XY<LocType> &offset)
{
    LocType xLo = LOC_TYPE_MAX;
    LocType yLo = LOC_TYPE_MAX;
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        const auto & cell = _db.cell(cellIdx);
        xLo = std::min(xLo, cell.xLo());
        yLo = std::min(yLo, cell.yLo());
    }
    // Compute the dif
    xLo = offset.x() - xLo;
    yLo = offset.y() - yLo;
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        auto & cell = _db.cell(cellIdx);
        cell.setXLoc(cell.xLoc() + xLo);
        cell.setYLoc(cell.yLoc() + yLo);
    }
}

PROJECT_NAMESPACE_END
