#include "ParserGuide.h"

PROJECT_NAMESPACE_BEGIN

bool ParserGuide::parse(const std::string &configFile)
{
    // Init net name map
    this->initNetNameMap();
    std::ifstream inf(configFile.c_str());
    if (!inf.is_open()) 
    {
        ERR("Routing guide parser:%s: cannot open file: %s \n", __FUNCTION__ , configFile.c_str());
        Assert(false);
        return false;
    }
    std::string lineStr;
    while(std::getline(inf, lineStr))
    {
        std::istringstream iss(lineStr);
        std::vector<std::string> split;
        std::string token;
        while(std::getline(iss, token, ' '))
        {
            split.emplace_back(token);
        }
        if (split.size() != 2)
        {
            ERR("Routing guide parser: wrong syntax \n");
            Assert(false);
            return false;
        }
        if (_netNameMap.find(split.at(0)) == _netNameMap.end())
        {
            ERR("Parse routing guide: net %s is not found! \n", split.at(0).c_str());
            continue;
        }
        IndexType netIdx = _netNameMap.at(split.at(0));
        if (!parseGuideFile(::klib::parseFile2Path(configFile) +"/" + split.at(1), netIdx))
        {
            ERR("Routing guide parser failed \n");
            return false;
        }
    }
    _db.routeGuide().initAverageProb(_db);
    return true;
}

bool ParserGuide::parseGuideFile(const std::string & guideFile, IndexType netIdx)
{
    std::ifstream inf(guideFile.c_str());
    if (!inf.is_open()) 
    {
        ERR("Routing guide parser:%s: cannot open file: %s \n", __FUNCTION__ , guideFile.c_str());
        Assert(false);
        return false;
    }
    std::string lineStr;
    // <# of x> <# of y> <range>
    std::getline(inf, lineStr);
    std::istringstream iss(lineStr);
    std::vector<std::string> split;
    std::string token;
    while(std::getline(iss, token, ' '))
    {
        split.emplace_back(token);
    }
    if (split.size() != 3)
    {
        ERR("Routing guide parser: wrong syntax \n");
        Assert(false);
        return false;
    }
    IndexType numX = std::stoi(split.at(0));
    IndexType numY = std::stoi(split.at(1));
    IntType range = std::stoi(split.at(2));

    // If the routeguide is not intialized, initialize it 
    if (!_db.routeGuide().isInit())
    {
        initRouteGuide(numX, numY, range);
    }

    // Set the net has guide
    _db.routeGuide().setNetHasGuide(netIdx, true);

    // Read the probability
    for (IndexType x = 0; x < numX; ++x)
    {
        for (IndexType y = 0; y < numY; ++y)
        {
            std::getline(inf, lineStr);
            _db.routeGuide().setGuide(std::stoi(lineStr), netIdx, x, y);
        }
    }
    // Read the probability
    for (IndexType y = 0; y < numY; ++y)
    {
        for (IndexType x = 0; x < numX; ++x)
        {
            std::cout<< _db.routeGuide().probability(netIdx, x, y)<<" ";
        }
        std::cout<< " end\n";
    }

#ifdef DEBUG_INPUT
    _db.routeGuide().drawRouteGuide(netIdx, "./debug/guide.png");
#endif


    return true;
}

void ParserGuide::initNetNameMap()
{
    for (IndexType netIdx = 0; netIdx < _db.grDB().numNets(); ++netIdx)
    {
        _netNameMap[_db.grDB().net(netIdx).name()] = netIdx;
    }
}

void ParserGuide::initRouteGuide(IndexType numX, IndexType numY, IntType range)
{
    _db.routeGuide() = RouteGuide(numX, numY, range, _db.grDB().numNets());
}
PROJECT_NAMESPACE_END
