#include "StopWatch.hpp"

namespace klib
{
    std::vector<std::uint64_t> StopWatchMgr::_us = std::vector<std::uint64_t>(1, 0);
    std::unordered_map<std::string, std::uint32_t> StopWatchMgr::_nameToIdxMap;
    StopWatch StopWatchMgr::_watch = StopWatch(0); 

    std::unique_ptr<StopWatch> StopWatchMgr::createNewStopWatch(std::string &&name) 
    {
        auto idx = _us.size();
        _us.emplace_back(0);
        _nameToIdxMap[std::move(name)] = idx;
        return std::make_unique<StopWatch>(StopWatch(idx));
    }
    void StopWatchMgr::quickStart()
    {
        _watch.clear();
        _watch.start();
    }
    std::uint64_t StopWatchMgr::quickEnd()
    {
        _watch.stop();
        return _watch.record();
    }
}

