#include "IdeaPlaceEx.h"
/* Parsers */
#include "parser/ProgArgs.h"
#include "parser/ParserTechSimple.h"
#include "parser/ParserPin.h"
#include "parser/ParserConnection.h"
#include "parser/ParserNetwgt.h"
#include "parser/ParserGds.h"
#include "parser/ParserSymFile.h"
#include "parser/ParserSymNet.h"
#include "parser/ParserSignalPath.h"
/* Placement */
#include "pinassign/VirtualPinAssigner.h"
#include "place/ProximityMgr.h"
/* Post-Processing */
#include "place/alignGrid.h"
#include <omp.h>

PROJECT_NAMESPACE_BEGIN


void IdeaPlaceEx::readTechSimpleFile(const std::string &techsimple)
{
    ParserTechSimple(_db).read(techsimple);
}

void IdeaPlaceEx::readPinFile(const std::string &pinFile)
{
    WRN("Ideaplace: Using file parser %s. This is designed for internal debugging only and may not be kept maintained \n", __FUNCTION__);
    ParserPin(_db).read(pinFile);
}

void IdeaPlaceEx::readConnectionFile(const std::string &connectionFile)
{
    WRN("Ideaplace: Using file parser %s. This is designed for internal debugging only and may not be kept maintained \n", __FUNCTION__);
    ParserConnection(_db).read(connectionFile);

    // Init cells before read in the gds
    if(!_db.initCells())
    {
        ERR("IdeaPlaceEx::%s initializing the cells failed! \n", __FUNCTION__);
        Assert(false);
    }
}

void IdeaPlaceEx::readNetWgtFile(const std::string &netWgtFile)
{
    WRN("Ideaplace: Using file parser %s. This is designed for internal debugging only and may not be kept maintained \n", __FUNCTION__);
    ParserNetwgt(_db).read(netWgtFile);
}

void IdeaPlaceEx::readSymFile(const std::string &symFile)
{
    WRN("Ideaplace: Using file parser %s. This is designed for internal debugging only and may not be kept maintained \n", __FUNCTION__);
    ParserSymFile(_db).read(symFile);
}
void IdeaPlaceEx::readGdsLayout(const std::string &gdsFile, IndexType cellIdx)
{
    WRN("Ideaplace: Using file parser %s. This is designed for internal debugging only and may not be kept maintained \n", __FUNCTION__);
    ParserCellGds(_db).parseCellGds(gdsFile, cellIdx);
}
void IdeaPlaceEx::readSymNetFile(const std::string &symnetFile)
{
    WRN("Ideaplace: Using file parser %s. This is designed for internal debugging only and may not be kept maintained \n", __FUNCTION__);
    ParserSymNet(_db).read(symnetFile);
}
void IdeaPlaceEx::readSigpathFile(const std::string &sigpathFile)
{
    WRN("Ideaplace: Using file parser %s. This is designed for internal debugging only and may not be kept maintained \n", __FUNCTION__);
    ParserSignalPath(_db).read(sigpathFile);
}

bool IdeaPlaceEx::parseFileBased(int argc, char **argv)
{
    ProgArgs _args = ProgArgsDetails::parseProgArgsCMD(argc, argv);

    // Start message printer timer
    MsgPrinter::startTimer();

    if (!_args.techsimpleFileIsSet())
    {
        ERR("IdeaPlaceEx::%s no techsimple file is given! \n", __FUNCTION__);
        Assert(false);
        return false;
    }
    ParserTechSimple(_db).read(_args.techsimpleFile());
    if (!_args.pinFileIsSet())
    {
        ERR("IdeaPlaceEx::%s no pin file is given! \n", __FUNCTION__);
        Assert(false);
        return false;
    }
    ParserPin(_db).read(_args.pinFile());
    if (!_args.connectionFileIsSet())
    {
        ERR("IdeaPlaceEx::%s no connection is given! \n", __FUNCTION__);
        Assert(false);
        return false;
    }
    ParserConnection(_db).read(_args.connectionFile());
    if (_args.netwgtFileIsSet())
    {
        // If .netwgt file is set, read it
        INF("IdeaPlaceEx::%s Read in the .netwgt ... \n", __FUNCTION__);
        ParserNetwgt(_db).read(_args.netwgtFile());
    }
    else
    {
        // If .netwgt is not set, skip
        INF("IdeaPlaceEx::%s no .netwgt file, skip... \n", __FUNCTION__);
    }

    if (_args.symFileIsSet())
    {
        INF("IdeaPlaceEx:%s Read in .sym ... \n", __FUNCTION__);
        ParserSymFile(_db).read(_args.symFile());
    }
    if (_args.symnetFileIsSet())
    {
        INF("IdeaPlaceEx:%s Read in .symnet ... \n", __FUNCTION__);
        readSymNetFile(_args.symnetFile());
    }

    if (_args.sigpathFileIsSet())
    {
        INF("IdeaPlaceEx:%s Read in .sigpath ... \n", __FUNCTION__);
        readSigpathFile(_args.sigpathFile());
    }


    // Init cells before read in the gds
    if(!_db.initCells())
    {
        ERR("IdeaPlaceEx::%s initializing the cells failed! \n", __FUNCTION__);
        Assert(false);
        return false;
    }
    
    // Parsing the gds files...
    ParserCellGdsDetails::parseAllGdsFiles(_db, _args.gdsFiles());
    return true;
}

LocType IdeaPlaceEx::solve(LocType gridStep)
{
    auto stopWatch = WATCH_CREATE_NEW("IdeaPlaceEx");
    stopWatch->start();
    omp_set_num_threads(_db.parameters().numThreads());
    // Start message printer timer
    MsgPrinter::startTimer();
    // Solve cleaning up tasks for safe...
    for (IndexType cellIdx = 0; cellIdx < _db.numCells(); ++cellIdx)
    {
        _db.cell(cellIdx).calculateCellBBox();
#ifdef DEBUG_GR
        DBG("cell %d %s bbox %s \n", cellIdx, _db.cell(cellIdx).name().c_str(), _db.cell(cellIdx).cellBBox().toStr().c_str());
#endif
    }

    if (gridStep > 0)
    {
        _db.parameters().setGridStep(gridStep);
        _db.expandCellToGridSize(gridStep);
    }

    // Set proximity group
    ProximityMgr proximityMgr(_db);
    proximityMgr.applyProximityWithDummyNets();

    // Clean up the signal path
    _db.splitSignalPathsBySymPairs();

    INF("Ideaplace: Entering global placement...\n");

    NlpGPlacerFirstOrder<nlp::nlp_default_settings> placer(_db);
    placer.solve();
#ifdef DEBUG_GR
#ifdef DEBUG_DRAW
    _db.drawCellBlocks("./debug/after_gr.gds");
#endif //DEBUG_DRAW
#endif
    INF("Ideaplace: Entering legalization and detailed placement...\n");
    CGLegalizer legalizer(_db);
    bool legalizeResult = legalizer.legalize();
    INF("Ideaplace: Assigning IO pin...\n");
    VirtualPinAssigner pinAssigner(_db);
    pinAssigner.solveFromDB();
    INF("IdeaPlaceEx:: HPWL %d \n", _db.hpwl());
    INF("IdeaPlaceEx:: HPWL with virtual pin: %d \n",  _db.hpwlWithVitualPins());
    LocType symAxis(0);

    // Restore proxmity group
    proximityMgr.restore();

    // stats for sigpath current path
    LocType sigHpwl = 0;
    LocType crfOverflow = 0;
    auto findpinLoc = [&](IndexType pinIdx)
    {
        const auto &pin = _db.pin(pinIdx);
        const auto &cell = _db.cell(pin.cellIdx());
        return cell.loc() + pin.midLoc();
    };
    for (const auto &sigpath : _db.vSignalPaths())
    {
        if (sigpath.isPower())
        {
            for (IndexType i = 0; i < sigpath.vPinIdxArray().size() - 1; ++i)
            {
                IndexType pinIdx1 = sigpath.vPinIdxArray().at(i);
                IndexType pinIdx2 = sigpath.vPinIdxArray().at(i+1);
                crfOverflow += std::max((findpinLoc(pinIdx2) - findpinLoc(pinIdx1)).y(), 0);
            }
        }
        else
        {
            for (IndexType i = 0; i < sigpath.vPinIdxArray().size() - 1; ++i)
            {
                IndexType pinIdx1 = sigpath.vPinIdxArray().at(i);
                IndexType pinIdx2 = sigpath.vPinIdxArray().at(i+1);
                auto loc1 = findpinLoc(pinIdx1);
                auto loc2 = findpinLoc(pinIdx2);
                auto dif = ::klib::manhattanDistance(loc1, loc2);
                sigHpwl += dif;
            }
        }
    }
    INF("\n\n\nOVERFLOW: crf %d \n HPWL: path %d \n \n\n", crfOverflow, sigHpwl);

    _db.checkSym();

    if (gridStep > 0)
    {
        INF("Ideaplace: Aligning the placement to grid...\n");
        symAxis = alignToGrid(gridStep);
    }
    else
    {
        symAxis = alignToGrid(1);
    }

#ifdef DEBUG_GR
#ifdef DEBUG_DRAW
    _db.drawCellBlocks("./debug/after_evertt.gds");
#endif //DEBUG_DRAW
#endif

    stopWatch->stop();

    return symAxis;
}

bool IdeaPlaceEx::outputFileBased(int , char **)
{
    return true;
}

IndexType IdeaPlaceEx::cellIdxName(const std::string name)
{
    IndexType cellIdx = INDEX_TYPE_MAX;
    for (IndexType idx = 0; idx < _db.numCells(); ++idx)
    {
        if (_db.cell(idx).name() == name)
        {
            cellIdx = idx;
            break;
        }
    }
    return cellIdx;
}

LocType IdeaPlaceEx::alignToGrid(LocType gridStepSize)
{
    GridAligner align(_db);
    align.align(gridStepSize);
#ifdef DEBUG_GR
#ifdef DEBUG_DRAW
    _db.drawCellBlocks("./debug/after_alignment.gds");
#endif //DEBUG_DRAW
#endif
    VirtualPinAssigner pinAssigner(_db);
    pinAssigner.solveFromDB();
    return align.findCurrentSymAxis();
}


void IdeaPlaceEx::setNumThreads(IndexType numThreads)
{
    _db.parameters().setNumThreads(numThreads);
    omp_set_num_threads(numThreads);
}
PROJECT_NAMESPACE_END
