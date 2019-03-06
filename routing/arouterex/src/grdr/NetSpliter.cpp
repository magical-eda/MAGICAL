#include "NetSpliter.h"
#include "util/solver/MSTSolver.h"

PROJECT_NAMESPACE_BEGIN

bool NetSpliter::solveEndToEnd()
{
    _splitedPairs.clear();
    if (_pinIdxArray.size() == 0)
    {
        WRN("%s: Pin size is 0!  \n", __PRETTY_FUNCTION__);
        return false;
    }
    for (IndexType first = 0; first < _pinIdxArray.size() - 1; ++first)
    {
        _splitedPairs.emplace_back(std::make_pair(_pinIdxArray.at(first), _pinIdxArray.at(first + 1)));
    }
    return true;
}

bool NetSpliter::solveMST()
{
    _splitedPairs.clear();
    if (_pinIdxArray.size() == 0)
    {
        WRN("%s: Pin size is 0! Could because that the pins are trimmed for overlapping \n", __PRETTY_FUNCTION__);
        return false;
    }
    /// Use KRUSKAL algorithm from boost
    ::klib::MSTSolver mst = ::klib::MSTSolver(::klib::MSTSolverMethodType::BOOST_KRUSKAL);
    // Add vertices to the mst solver
    for (IndexType pinIdx = 0; pinIdx < _pinIdxArray.size(); ++pinIdx)
    {
        mst.addVertex();
    }
    // Add edges for all the vertices
    for (IndexType out = 0; out < _pinIdxArray.size(); ++out)
    {
        for (IndexType in = 0; in < _pinIdxArray.size(); ++in)
        {
            auto &pinIn = _pinsPtr->at(in);
            auto &pinOut = _pinsPtr->at(out);
            LocType manDist = ::klib::manhattanDistance<LocType>(pinIn, pinOut);
            mst.addEdge(out, in, manDist);
        }
    }
    // solve for MST
    mst.solve();
    // Retrive the MST results
    for (auto &pair : mst.resultVertexPairs())
    {
        _splitedPairs.emplace_back(std::make_pair(_pinIdxArray.at(pair.first), _pinIdxArray.at(pair.second)));
    }
    /// Re-order the pairs so that the 2-pins are ordered to be end to end
    reorder();
    return true;
}

bool NetSpliter::solveFlute()
{
    AssertMsg(false, "%s: function has not been implemented yet :( \n", __PRETTY_FUNCTION__);
    return false;
}

void NetSpliter::reorder()
{
    // This process is actually pretty expensive in complexity. But the number of nets and number of pins might not be big in this project, so I get lazy to optimize for it :p
    
    // record each pin has been assigned to a pair
    std::map<IndexType, bool> assigned;
    for (IndexType pinIdx : _pinIdxArray)
    {
        assigned[pinIdx] = false;
    }
    // make the first pin the start point of 
    assigned[_pinIdxArray.at(0)] = true;

    // Copy the results
    std::vector<std::pair<IndexType, IndexType>> rawResults;
    // move the _splitedPairs into rawResults 
    // Using move make more sense when moving more expensive, instead of pair uint. The reason I am using move instead of simple copy is because std::move looks cool
    rawResults.insert(rawResults.end(), std::make_move_iterator(_splitedPairs.begin()),
                                        std::make_move_iterator(_splitedPairs.end()));
    // clear the _splitedPairs
    _splitedPairs.clear();
    
    // O(n^2) to reorder the pairs
    bool finished = true;
    do
    {
        finished = true;
        for (IndexType pairIdx = 0; pairIdx < rawResults.size(); ++pairIdx)
        {
            auto &pair = rawResults.at(pairIdx);
            if (assigned[pair.first] && assigned[pair.second])
            {
                continue;
            }
            if (assigned[pair.first] && !assigned[pair.second])
            {
                _splitedPairs.emplace_back(std::make_pair(pair.first, pair.second));
                assigned[pair.second] = true;
                continue;
            }
            if (!assigned[pair.first] && assigned[pair.second])
            {
                _splitedPairs.emplace_back(std::make_pair(pair.second, pair.first));
                assigned[pair.first] = true;
                continue;
            }
            // Neither the two pins in the pair has been assigned
            finished = false;
        }
    } while (!finished);
}

PROJECT_NAMESPACE_END
