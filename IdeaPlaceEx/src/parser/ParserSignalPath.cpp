#include "ParserSignalPath.h"

PROJECT_NAMESPACE_BEGIN


bool ParserSignalPath::read(const std::string &filename)
{
    std::ifstream inf(filename.c_str());
    if (!inf.is_open()) 
    {
        ERR("ParserPin::%s: cannot open file: %s \n", __FUNCTION__ , filename.c_str());
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
        if (words.size() <= 1)
        {
            continue;
        }
        auto & sig = _db.signalPath(_db.allocateSignalPath());
        for (IndexType idx =0; idx < words.size() - 1; idx +=2)
        {
            auto & cellName = words.at(idx);
            auto & pinName = words.at(idx + 1);
            IndexType pinIdx = INDEX_TYPE_MAX;
            for (IndexType idx = 0; idx < _db.numCells(); ++idx)
            {
                const auto & cell = _db.cell(idx);
                if (cell.name() == cellName)
                {
                    for (IndexType jdx = 0; jdx < cell.numPinIdx(); ++jdx)
                    {
                        const auto &pin = _db.pin(cell.pinIdx(jdx));
                        if (pin.name() == pinName)
                        {
                            pinIdx = cell.pinIdx(jdx);
                            break;
                        }
                    }
                    break;
                }
            }
            AssertMsg(pinIdx != INDEX_TYPE_MAX, "Unknown pin! cellname: %s, pinname %s \n", cellName.c_str(), pinName.c_str());
            sig.addPinIdx(pinIdx);
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
