#include "DetailedRouter.h"
#include "DetailedNCR.h"
#include "DetailedPreProcessing.h"
#include "grdr/NetSpliter.h"

PROJECT_NAMESPACE_BEGIN

DetailedRouter::DetailedRouter(Database &db) : _db(db)
{
    _db.drDB().init();
    
    // Preprocessing the data
    //DetailedPreProcessing prepro(db);
    //prepro.processPins();
    //prepro.addPinToGlobalResult();
}

bool DetailedRouter::solveGridlessAstarNCR()
{
    this->splitNetsMST();
    DetailedNCR ncr(_db);
    bool solved = ncr.runKernel();

    return solved;
}

bool DetailedRouter::splitNetsMST()
{
    NetSpliter split;
    // Prepare the locations of the terminals
    std::vector<XY<LocType>> locs;
    for (IndexType termIdx  = 0; termIdx < _db.numTerminals(); ++termIdx)
    {
        locs.emplace_back(_db.terminal(termIdx).primarySearchPoint().loc());
    }
    split.setPins(locs);
    for (IndexType netIdx = 0; netIdx < _db.drDB().numNets(); ++netIdx)
    {
        split.setNetPins(_db.drDB().net(netIdx).terminalArray());
        if (!split.solveMST())
        {
            WRN("%s: split fails for net %u %s \n", __FUNCTION__, netIdx, _db.grDB().nets().at(netIdx).name().c_str());
        }
        _db.drDB().nets().at(netIdx).branches() = split.results();
    }
    // SelfSym nets
    for (IndexType selfSymIdx = 0; selfSymIdx < _db.numSelfSymNets(); ++selfSymIdx)
    {
        auto &selfSym = _db.selfSymNet(selfSymIdx);
        std::vector<IndexType> sideTerm;
        // Middle terminal is always at the begining
        sideTerm.emplace_back(selfSym.midTermIdx());
        // Add half of the terminal to the vector
        for (IndexType pairIdx = 0; pairIdx < selfSym.terminalPairs().size(); ++ pairIdx)
        {
            // Do not add the middle terminal again
            if (selfSym.terminalPairs().at(pairIdx).first != selfSym.midTermIdx())
            {
                sideTerm.emplace_back(selfSym.terminalPairs().at(pairIdx).first);
            }
        }
        // Solve the MST
        split.setNetPins(sideTerm);
        split.solveMST();
        // Save the result
        selfSym.branches() = split.results();
    }
    return true;
}


PROJECT_NAMESPACE_END
