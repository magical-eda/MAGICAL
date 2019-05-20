#include "GridCellGR.h"

PROJECT_NAMESPACE_BEGIN

GridCellGR::GridCellGR(XY<IndexType> gridIdx, IndexType numLayers)
    : _gridIdx(gridIdx)
{
    _netIdxSetArray.resize(numLayers);
    _pinIdxSetArray.resize(numLayers);
    _blockedArray.resize(numLayers);
    _hisCostArray.resize(numLayers);
}

bool GridCellGR::dropBlockage(IndexType layerIdx)
{
    if (!_blockedArray.at(layerIdx))
    {
        _blockedArray.at(layerIdx) = true;
        return true;
    }
    else 
    {
        return false;
    }
}

bool GridCellGR::eraseBlockage(IndexType layerIdx)
{
    if (_blockedArray.at(layerIdx))
    {
        _blockedArray.at(layerIdx) = false;
        return true;
    }
    else
    {
        return false;
    }
}

PROJECT_NAMESPACE_END
