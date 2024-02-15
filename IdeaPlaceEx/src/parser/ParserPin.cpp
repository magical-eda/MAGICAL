#include "ParserPin.h"
#include <iostream>

PROJECT_NAMESPACE_BEGIN

RealType stripShape(std::string word)
{
    auto pos = word.rfind(')');
    while (pos != std::string::npos)
    {
        word = word.substr(0, pos);
        pos = word.rfind(')');
    }
    pos = word.find('(');
    while (pos != std::string::npos)
    {
        word = word.substr(pos+1);
        pos = word.find('(');
    }
    return ::atof(word.c_str());
}

bool ParserPin::read(const std::string &filename)
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
    IndexType cellIdx = INDEX_TYPE_MAX; 
    IndexType numShapes = INDEX_TYPE_MAX;
    IndexType pinIdx = INDEX_TYPE_MAX;
    while (std::getline(inf, line))
    {
        // Split the line into words
        std::vector<std::string> words;
        std::istringstream iss(line);
        for (std::string str; iss >> str; )
        {
            words.emplace_back(str);
        }
        if (words.size() == 1)
        {
            // Create a new cell
            cellIdx = _db.allocateCell();
            _db.cell(cellIdx).setName(words.at(0));
        }
        else if (words.size() == 2)
        {
            Assert(cellIdx !=  INDEX_TYPE_MAX);
            pinIdx = _db.allocatePin();
            _db.pin(pinIdx).setName(words.at(0));
            _db.cell(cellIdx).addPin(pinIdx); // Add pin to the cell
            numShapes = ::atoi(words.at(1).c_str()); // The second one denotes how many shapes there is in the pin
        }
        else if (words.size() > 2)
        {
            Assert(cellIdx != INDEX_TYPE_MAX);
            Assert(pinIdx != INDEX_TYPE_MAX);
            auto & pin = _db.pin(pinIdx);
            // Shapes are always rectagnles
            for (IndexType idx = 0; idx < numShapes; ++idx)
            {
                //IndexType gdsLayer = ::atoi(words.at(idx * 5).c_str());
                RealType real = stripShape(words.at(idx *5 + 1)) * _db.tech().dbu();
                LocType xLo = ::klib::autoRound<LocType>(real);
                real = stripShape(words.at(idx *5 + 2)) * _db.tech().dbu();
                LocType yLo = ::klib::autoRound<LocType>(real);
                real = stripShape(words.at(idx *5 + 3)) * _db.tech().dbu();
                LocType xHi = ::klib::autoRound<LocType>(real);
                real = stripShape(words.at(idx *5 + 4)) * _db.tech().dbu();
                LocType yHi = ::klib::autoRound<LocType>(real);
                pin.shape().unionBox(Box<LocType>(xLo, yLo, xHi, yHi));
            }
        }
        else
        {
            // 0? empty line?
            Assert(false);
            return false;
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
