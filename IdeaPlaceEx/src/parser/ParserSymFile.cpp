#include "ParserSymFile.h"

PROJECT_NAMESPACE_BEGIN

IndexType ParserSymFile::cellIdx(const std::string &cellName)
{
    IndexType findIdx = INDEX_TYPE_MAX;
    for (IndexType idx = 0; idx < _db.numCells(); ++idx)
    {
        if (_db.cell(idx).name() == cellName)
        {
            findIdx = idx;
            break;
        }
    }
    return findIdx;
}

bool ParserSymFile::read(const std::string &filename)
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

    IntType idx = -1;
    while (std::getline(inf, line))
    {
        // Split the line into words
        std::vector<std::string> words;
        std::istringstream iss(line);
        for (std::string str; iss >> str; )
        {
            words.emplace_back(str);
        }
        if (idx == -1 && words.size() != 0)
        {
            idx = _db.allocateSymGrp();
        }
        if (words.size() == 0)
        {
            idx = -1;
        }
        if (words.size() == 2)
        {
            std::string cellName1 = words.at(0);
            std::string cellName2 = words.at(1);
            // Low efficency implementation
            IndexType cellIdx1 = cellIdx(cellName1);
            IndexType cellIdx2 = cellIdx(cellName2);
            if (cellIdx1 == INDEX_TYPE_MAX)
            {
                ERR("ParserSymFile: cannot find cell %s \n", cellName1.c_str());
                Assert(false);
                return false;
            }
            if (cellIdx2 == INDEX_TYPE_MAX)
            {
                ERR("ParserSymFile: cannot find cell %s \n", cellName2.c_str());
                Assert(false);
                return false;
            }
            _db.symGroup(idx).addSymPair(cellIdx1, cellIdx2);
            _db.cell(cellIdx1).setSymNetIdx(cellIdx2);
            _db.cell(cellIdx2).setSymNetIdx(cellIdx1);
        }
        if (words.size() == 1)
        {
            std::string cellName = words.at(0);
            // Low efficency implementation
            IndexType cellId = cellIdx(cellName);
            if (cellId == INDEX_TYPE_MAX)
            {
                ERR("ParserSymFile: cannot find cell %s \n", cellName.c_str());
                Assert(false);
                return false;
            }
            _db.symGroup(idx).addSelfSym(cellId);
            _db.cell(cellId).setSelfSym(true);
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
