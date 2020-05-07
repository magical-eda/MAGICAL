#include "Database.h"

PROJECT_NAMESPACE_BEGIN

bool Database::initCells()
{
    for (IndexType cellIdx = 0; cellIdx < this->numCells(); ++cellIdx)
    {
        if (!this->initCell(cellIdx))
        {
            return false;
        }
    }
    return true;
}


bool Database::initCell(IndexType cellIdx)
{
    IndexType numLayers = this->tech().numLayers();
    AT(_cellArray, cellIdx).allocateLayers(numLayers);
    return true;
}

LocType Database::hpwl() const
{
    LocType hpwl = 0;
    for (const auto & net : _netArray)
    {
        if (net.numPinIdx() <= 1)
        {
            continue;
        }
        LocType xMax = LOC_TYPE_MIN;
        LocType xMin = LOC_TYPE_MAX;
        LocType yMax = LOC_TYPE_MIN;
        LocType yMin = LOC_TYPE_MAX;
        for (IndexType pinIdx : net.pinIdxArray())
        {
            const auto &pin = AT(_pinArray, pinIdx);
            auto pinLoc = pin.midLoc();
            IndexType cellIdx = pin.cellIdx();
            const auto &cell = AT(_cellArray, cellIdx);
            auto cellLoc = cell.loc();
            auto pinFinalLoc = pinLoc + cellLoc;
            xMax = std::max(xMax, pinFinalLoc.x());
            xMin = std::min(xMin, pinFinalLoc.x());
            yMax = std::max(yMax, pinFinalLoc.y());
            yMin = std::min(yMin, pinFinalLoc.y());
        }
        hpwl = ((xMax - xMin) + (yMax - yMin)) * net.weight();
    }
    return hpwl;
}

LocType Database::hpwlWithVitualPins() const 
{
    LocType hpwl = 0;
    for (const auto & net : _netArray)
    {
        if (net.numPinIdx() <= 1)
        {
            continue;
        }
        LocType xMax = LOC_TYPE_MIN;
        LocType xMin = LOC_TYPE_MAX;
        LocType yMax = LOC_TYPE_MIN;
        LocType yMin = LOC_TYPE_MAX;
        for (IndexType pinIdx : net.pinIdxArray())
        {
            const auto &pin = AT(_pinArray, pinIdx);
            auto pinLoc = pin.midLoc();
            IndexType cellIdx = pin.cellIdx();
            const auto &cell = AT(_cellArray, cellIdx);
            auto cellLoc = cell.loc();
            auto pinFinalLoc = pinLoc + cellLoc;
            xMax = std::max(xMax, pinFinalLoc.x());
            xMin = std::min(xMin, pinFinalLoc.x());
            yMax = std::max(yMax, pinFinalLoc.y());
            yMin = std::min(yMin, pinFinalLoc.y());
        }
        if (net.isIo())
        {
            const auto &virPin = net.virtualPinLoc();
            xMax = std::max(xMax, virPin.x());
            xMin = std::min(xMin, virPin.x());
            yMax = std::max(yMax, virPin.y());
            yMin = std::min(yMin, virPin.y());
        }
        hpwl = ((xMax - xMin) + (yMax - yMin)) * net.weight();
    }
    return hpwl;
}

bool Database::checkSym()
{
#ifndef MULTI_SYM_GROUP
    LocType axis = LOC_TYPE_MIN;
    for (const auto &symGrp : _symGroups)
    {
        if (symGrp.numSymPairs() == 0)
        {
            break;
        }
        LocType symAxis;
        for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
        {
            const auto &symPair = symGrp.symPair(symPairIdx);
            symAxis = (cell(symPair.firstCell()).xCenter() + cell(symPair.secondCell()).xCenter()) / 2;
            break;
        }
        if (axis != LOC_TYPE_MIN)
        {
            if (axis != symAxis)
            {
                ERR("Check Symmetry: different sym axises! \n");
                return false;
            }
        }
        else
        {
            axis = symAxis;
        }
        for (IndexType symPairIdx = 0; symPairIdx < symGrp.numSymPairs(); ++symPairIdx)
        {
            const auto &symPair = symGrp.symPair(symPairIdx);
            if (symAxis != (cell(symPair.firstCell()).xCenter() + cell(symPair.secondCell()).xCenter()) / 2)
            {
                ERR("Check Symmetry: symPair failed. axis %d, cell %d %d \n", symAxis, cell(symPair.firstCell()).xCenter(),
                        cell(symPair.secondCell()).xCenter());
                return false;
            }
            if ((cell(symPair.firstCell()).yLoc() != cell(symPair.secondCell()).yLoc()))
            {
                ERR("Check Symmetry: symPair failed. different y, cell %d %d \n", symAxis, cell(symPair.firstCell()).yLoc(),
                        cell(symPair.secondCell()).yLoc());
                return false;
            }
        }
        for (IndexType ssIdx = 0; ssIdx < symGrp.numSelfSyms(); ++ssIdx)
        {
            if (cell(symGrp.selfSym(ssIdx)).xCenter() != symAxis)
            {
                ERR("Check Symmetry: selfSym failed. axis %d, cell idx %d at %d \n", symAxis, symGrp.selfSym(ssIdx), cell(symGrp.selfSym(ssIdx)).xCenter());
                return false;
            }
        }
    }
#endif
    return true;
}

/*------------------------------*/ 
/* Debug functions              */
/*------------------------------*/ 
#ifdef DEBUG_DRAW
PROJECT_NAMESPACE_END
#include "writer/gdsii/WriteGds.h"
PROJECT_NAMESPACE_BEGIN
void Database::drawCellBlocks(const std::string &filename)
{
    auto wg = std::make_shared<WriteGds>(filename);
    if (!wg->initWriter())
    {
        return;
    }
    if (!wg->createLib("TOP", 2000, 1e-6/2000)) // Hardcoded numbers
    {
        return;
    }
    if (!wg->writeCellBgn("DEBUG"))
    {
        return;
    }
    // Write all the cells
    for (IndexType cellIdx = 0; cellIdx < this->numCells(); ++cellIdx)
    {
        const auto &cell = this->cell(cellIdx);
        Box<LocType> cellBox = cell.cellBBox();
        cellBox.enlargeBy(0);
        XY<LocType> cellLoc = XY<LocType>(cell.xLoc(), cell.yLoc());
        cellBox.offsetBy(cellLoc);
        wg->writeRectangle(cellBox, cellIdx, 0);
        // Also wrute pins
        for (IndexType pinIdxInCell = 0; pinIdxInCell < cell.numPinIdx(); ++pinIdxInCell)
        {
            const auto &pin = this->pin(cell.pinIdx(pinIdxInCell));
            Box<LocType> pinBox = pin.shape();
            pinBox.offsetBy(cellLoc);
            wg->writeRectangle(pinBox, 100 + cellIdx, 0);
        }
    }
    // net virtual pins
    for (IndexType netIdx = 0; netIdx < numNets(); ++netIdx)
    {
        const auto &net = _netArray.at(netIdx);
        if (net.isValidVirtualPin())
        {
            Box<LocType> box = Box<LocType>(net.virtualPinLoc(), net.virtualPinLoc());
            box.enlargeBy(100);
            wg->writeRectangle(box, 500 + netIdx, 0);
        }
    }
    // END
    wg->writeCellEnd();
    wg->endLib();
}

#endif //DEBUG_DRAW
PROJECT_NAMESPACE_END
