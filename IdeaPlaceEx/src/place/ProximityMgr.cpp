#include "ProximityMgr.h"

PROJECT_NAMESPACE_BEGIN

void ProximityMgr::applyProximityWithDummyNets()
{
    for (const auto &proxmtGrp : _db.proximityGrps())
    {
        this->addDummyNetForProximityGroup(proxmtGrp);
    }
}

void ProximityMgr::addDummyNetForProximityGroup(const ProximityGroup &pg)
{
    IndexType netIdx = _db.allocateNet();
    _dummyNets.emplace_back(netIdx);
    auto &net = _db.net(netIdx);
    net.setIsIo(false);
    net.markAsDummyNet();
    net.setWeight(DEFAULT_DUMMY_NET_WEIGHT);
    for (IndexType cellIdx : pg.cells())
    {
        IndexType pinIdx = _db.allocatePin();
        net.addPin(pinIdx);
        auto &pin = _db.pin(pinIdx);
        pin.setCellIdx(cellIdx);
        _dummyPins.emplace_back(pinIdx);
        auto &cell = _db.cell(cellIdx);
        cell.addPin(pinIdx);
        _dummyCells.emplace_back(PinInCellIdx(cellIdx, cell.numPinIdx() - 1));
        // Congigure the dummy pin
        pin.markAsDummyPin();
        pin.shape() = Box<LocType>(0, 0, 0, 0);
    }
}

void ProximityMgr::restore()
{
    // Sort the indices in descending order and erase the vector
    std::sort(_dummyPins.begin(), _dummyPins.end(), std::greater<IndexType>());
    std::sort(_dummyNets.begin(), _dummyNets.end(), std::greater<IndexType>());
    auto comparePinInCellFunc = [&](const PinInCellIdx &lhs, const PinInCellIdx &rhs)
    {
        if (lhs.cellIdx == rhs.cellIdx) { return lhs.pinIdx > rhs.pinIdx; }
        else { return lhs.cellIdx < rhs.cellIdx; }
    };
    std::sort(_dummyCells.begin(), _dummyCells.end(), comparePinInCellFunc);
    // The logic in the below are similar
    // 1. check the indices are in arithemeic progression
    // 2. clear the vector
    if (_dummyPins.size() > 0)
    {
        IndexType last = _dummyPins.front();
        Assert(last == _db.numPins() - 1);
        for (IndexType idx = 1; idx < _dummyPins.size(); ++idx)
        {
            Assert(last -1 == _dummyPins[idx]);
            last = _dummyPins[idx];
        }
        _db.vPinArray().erase(_db.vPinArray().begin() + _dummyPins.back(), _db.vPinArray().end());
    }
    if (_dummyNets.size() > 0)
    {
        IndexType last = _dummyNets.front();
        Assert(last == _db.numNets() - 1);
        for (IndexType idx = 1; idx < _dummyNets.size(); ++idx)
        {
            Assert(last -1 == _dummyNets[idx]);
            last = _dummyNets[idx];
        }
        _db.nets().erase(_db.nets().begin() + _dummyNets.back(), _db.nets().end());
    }
    // Dummy pins in cells need to be clear cell-wise
    auto clearCellFunc = [&] (IndexType idx)
    {
        IndexType cellIdx = _dummyCells.at(idx).cellIdx;
        IndexType last = _dummyCells.at(idx).pinIdx;
        Assert(last = _db.cell(cellIdx).pins().size() - 1);
        for(idx += 1; idx < _dummyCells.size(); ++idx)
        {
            if (_dummyCells.at(idx).cellIdx != cellIdx) { idx -=1; break; }
            Assert(last + 1 == _dummyCells.at(idx).pinIdx);
            last = _dummyCells[idx].pinIdx;
        }
        _db.cell(cellIdx).pins().erase(_db.cell(cellIdx).pins().begin() + last, _db.cell(cellIdx).pins().end());
        return idx;
    };
    for (IndexType idx = 0; idx < _dummyCells.size(); ++idx)
    {
        idx = clearCellFunc(idx);
    }
}

PROJECT_NAMESPACE_END
