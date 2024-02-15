#include "signalPathMgr.h"

PROJECT_NAMESPACE_BEGIN

SigPathMgr::SigPathMgr(Database &db)
    : _db(db)
{
    this->decomposeSignalPaths();
}

void SigPathMgr::decomposeSignalPaths()
{
    for (IndexType pathIdx = 0; pathIdx < _db.vSignalPaths().size(); ++pathIdx)
    {
        const auto &path = _db.vSignalPaths().at(pathIdx);
        _segByPath.emplace_back(std::vector<SigPathSeg>());
        _currentFlowRemainingTwoPinSegs.emplace_back(std::vector<SigPathTwoPinSeg>());
        // since the automatic generated current flow could include both sides of differential pair, we need to split the path is seen such scenario
        for (IntType idx = 0; idx < static_cast<IntType>(path.vPinIdxArray().size()) - 3; ++idx)
        {
            const IndexType pinIdx1 = path.vPinIdxArray().at(idx);   const auto &pin1 = _db.pin(pinIdx1); const IndexType cellIdx1 = pin1.cellIdx();
            const IndexType pinIdx2 = path.vPinIdxArray().at(idx+1); const auto &pin2 = _db.pin(pinIdx2); const IndexType cellIdx2 = pin2.cellIdx();
            const IndexType pinIdx3 = path.vPinIdxArray().at(idx+2); const auto &pin3 = _db.pin(pinIdx3); const IndexType cellIdx3 = pin3.cellIdx();
            const IndexType pinIdx4 = path.vPinIdxArray().at(idx+3); const auto &pin4 = _db.pin(pinIdx4); const IndexType cellIdx4 = pin4.cellIdx();

            bool pin23Same = cellIdx2 == cellIdx3;
            bool pin12Diff = cellIdx1 != cellIdx2;
            bool pin34Diff = cellIdx3 != cellIdx4;
            bool pin14Diff = cellIdx1 != cellIdx4;
            if (pin23Same and pin12Diff and pin34Diff and pin14Diff)
            {
                // valid segment
                _segs.emplace_back(SigPathSeg(pinIdx1, pinIdx2, pinIdx3, pinIdx4));
                _segByPath.back().emplace_back(SigPathSeg(pinIdx1, pinIdx2, pinIdx3, pinIdx4));
                DBG("SigPathMgr: add cell %d %s  pin %d %s-> cell %d %s pin %d %s -> cell %d %s pin %d %s -> cell %d %s -> pin %d  %s\n",
                        cellIdx1, _db.cell(cellIdx1).name().c_str(), pinIdx1, _db.pin(pinIdx1).name().c_str(),
                        cellIdx2, _db.cell(cellIdx2).name().c_str(), pinIdx2, _db.pin(pinIdx2).name().c_str(),
                        cellIdx3, _db.cell(cellIdx3).name().c_str(), pinIdx3, _db.pin(pinIdx3).name().c_str(),
                        cellIdx4, _db.cell(cellIdx4).name().c_str(), pinIdx4, _db.pin(pinIdx4).name().c_str());
            }
        }
        // Process the corner case that a current flow path has only two pin. In this case it should implicitly mean the path ending in ground pin
        if (path.vPinIdxArray().size() == 2 and path.isPower())
        {
            const IndexType pinIdx1 = path.vPinIdxArray().at(0);   const auto &pin1 = _db.pin(pinIdx1); const IndexType cellIdx1 = pin1.cellIdx();
            const IndexType pinIdx2 = path.vPinIdxArray().at(1); const auto &pin2 = _db.pin(pinIdx2); const IndexType cellIdx2 = pin2.cellIdx();
            if (cellIdx1 != cellIdx2)
            {
                DBG("SigPathMgr: add cell %d %s  pin %d %s-> cell %d %s pin %d %s \n",
                        cellIdx1, _db.cell(cellIdx1).name().c_str(), pinIdx1, _db.pin(pinIdx1).name().c_str(),
                        cellIdx2, _db.cell(cellIdx2).name().c_str(), pinIdx2, _db.pin(pinIdx2).name().c_str());
                _currentFlowRemainingTwoPinSegs.back().emplace_back(SigPathTwoPinSeg(cellIdx1, cellIdx2));
            }
        }
    }
}

PROJECT_NAMESPACE_END
