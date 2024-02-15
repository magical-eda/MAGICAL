#include "ParserConnection.h"

PROJECT_NAMESPACE_BEGIN

bool ParserConnection::read(const std::string &filename)
{
    std::ifstream inf(filename.c_str());
    if (!inf.is_open()) 
    {
        ERR("ParserConnection::%s: cannot open file: %s \n", __FUNCTION__ , filename.c_str());
        Assert(false);
        return false;
    }
    // Read in the file
    std::string line;
    while (std::getline(inf, line))
    {
        // Split the line into words
        std::vector<std::string> words;
        std::istringstream iss(line);
        for (std::string str; iss >> str; )
        {
            words.emplace_back(str);
        }
        if (words.size() == 0)
        {
            ERR("ParserConnection::%s: unexpected syntax %s \n", __FUNCTION__, line.c_str());
            Assert(false);
            return false;
        }
        IndexType netIdx = _db.allocateNet();
        _db.net(netIdx).setName(words.at(0));
        std::vector<std::string> cells; 
        std::vector<std::string> pins;
        for (IndexType idx = 0; idx < (words.size() - 1)/2; ++idx)
        {
            std::string cellName = words.at(1 + idx * 2);
            std::string pinName = words.at(1 + idx * 2 + 1);
            if (pinName == "B" || pinName == "BULK")
            {
                WRN("ParserConnection::%s: ignore the bulk connection \n", __FUNCTION__);
                continue;
            }
            cells.emplace_back(cellName);
            pins.emplace_back(pinName);
        }
        // Find the corresponding pins in the database
        for (IndexType cellsIdx = 0; cellsIdx < cells.size(); ++cellsIdx)
        {
            const auto &cellName = cells.at(cellsIdx);
            const auto &pinName = pins.at(cellsIdx);
            IndexType cellIdx = INDEX_TYPE_MAX;
            for (IndexType idx = 0; idx < _db.numCells(); ++idx)
            {
                if (_db.cell(idx).name() == cellName)
                {
                    cellIdx = idx;
                    break;
                }
            }
            if (cellIdx == INDEX_TYPE_MAX)
            {
                // Did not find the cell
                ERR("ParserConnection::%s: cannot find the cell %s \n", __FUNCTION__, cellName.c_str());
                Assert(false);
                return false;
            }
            const auto &cell = _db.cell(cellIdx);
            IndexType pinIdx = INDEX_TYPE_MAX;
            for (IndexType idx = 0; idx < cell.numPinIdx(); ++idx)
            {
                const auto & name = _db.pin(cell.pinIdx(idx)).name();
                if (pinName == name)
                {
                    pinIdx = cell.pinIdx(idx);
                    break;
                }
            }
            if (pinIdx == INDEX_TYPE_MAX)
            {
                // Did not find the pin
                continue;
                // ERR("ParserConnection::%s: cannot find the pin %s in cell %s \n",
                //         __FUNCTION__, cellName.c_str(), pinName.c_str());
                // Assert(false);
                // return false;
            }
            // Add the pin to the net in the database
            _db.net(netIdx).addPin(pinIdx);
            _db.pin(pinIdx).addNetIdx(netIdx);
            _db.pin(pinIdx).setCellIdx(cellIdx);
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
