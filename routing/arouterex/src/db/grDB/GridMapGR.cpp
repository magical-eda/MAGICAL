#include "GridMapGR.h"

PROJECT_NAMESPACE_BEGIN

GridMapGR::GridMapGR(IndexType numX, IndexType numY, IndexType numLayers)
{
    _gridCellMap.resize(numX, numY, numLayers);
    _viaEdgeMap.resize(numX, numY, numLayers - 1);
    _horizontalEdgeMap.resize(numX - 1, numY, numLayers);
    _verticalEdgeMap.resize(numX, numY - 1, numLayers);
}

void GridMapGR::init2D()
{
    /// Resize the 2D vectors with the parameters of 3D vectors
    IndexType numX = this->numX();
    IndexType numY = this->numY();
    IndexType numZ = this->numLayers();
    _gridCellMap2D.resize(numX, numY);
    _hEdgeMap2D.resize(numX - 1, numY);
    _vEdgeMap2D.resize(numX, numY - 1);

    // Init the horizontal edge Cap
    for (IndexType x = 0; x < numX - 1; ++x)
    {
        for (IndexType y = 0; y < numY; ++y)
        {
            IntType totalCap = 0;
            for (IndexType z = 0; z <numZ; ++ z)
            {
                totalCap += _horizontalEdgeMap.at(x, y, z).totalCap();
            }
            _hEdgeMap2D.at(x, y).setTotalCap(totalCap);
        }
    }
    // Vertical
    for (IndexType x = 0; x < numX; ++x)
    {
        for (IndexType y = 0; y < numY - 1; ++y)
        {
            IntType totalCap = 0;
            for (IndexType z = 0; z <numZ; ++ z)
            {
                totalCap += _verticalEdgeMap.at(x, y, z).totalCap();
            }
            _vEdgeMap2D.at(x, y).setTotalCap(totalCap);
        }
    }
}

const GridEdgeGR & GridMapGR::gridEdge(const XYZ<IndexType> &node1, const XYZ<IndexType> &node2) const
{
    if (!_mode2D)
    {
        Assert(klib::xyzAdjacent(node1, node2));
        if (node1.z() != node2.z())
        {
            // Via edge
            if (node1.z() < node2.z()) { return  _viaEdgeMap.at(node1); }
            else { return _viaEdgeMap.at(node2); }
        }
        else if (node1.x() != node2.x())
        {
            // horizontal edge
            if (node1.x() < node2.x()) { return _horizontalEdgeMap.at(node1); }
            else { return _horizontalEdgeMap.at(node2); }
        }
        // vertical edge
        if (node1.y() < node2.y()) { return _verticalEdgeMap.at(node1); }
        else { return _verticalEdgeMap.at(node2); }
    }
    else
    {
        Assert(klib::xyzAdjacent(node1, node2));
        if (node1.x() != node2.x())
        {
            // horizontal edge
            if (node1.x() < node2.x()) { return _hEdgeMap2D.at(node1.xy()); }
            else { return _hEdgeMap2D.at(node2.xy()); }
        }
        // vertical edge
        if (node1.y() < node2.y()) { return _vEdgeMap2D.at(node1.xy()); }
        else { return _vEdgeMap2D.at(node2.xy()); }

    }
}

GridEdgeGR & GridMapGR::gridEdge(const XYZ<IndexType> &node1, const XYZ<IndexType> &node2)
{
    if (!_mode2D)
    {
        Assert(klib::xyzAdjacent(node1, node2));
        if (node1.z() != node2.z())
        {
            // Via edge
            if (node1.z() < node2.z()) { return  _viaEdgeMap.at(node1); }
            else { return _viaEdgeMap.at(node2); }
        }
        else if (node1.x() != node2.x())
        {
            // horizontal edge
            if (node1.x() < node2.x()) { return _horizontalEdgeMap.at(node1); }
            else { return _horizontalEdgeMap.at(node2); }
        }
        // vertical edge
        if (node1.y() < node2.y()) { return _verticalEdgeMap.at(node1); }
        else { return _verticalEdgeMap.at(node2); }
    }
    else
    {
        Assert(klib::xyzAdjacent(node1, node2));
        if (node1.x() != node2.x())
        {
            // horizontal edge
            if (node1.x() < node2.x()) { return _hEdgeMap2D.at(node1.xy()); }
            else { return _hEdgeMap2D.at(node2.xy()); }
        }
        // vertical edge
        if (node1.y() < node2.y()) { return _vEdgeMap2D.at(node1.xy()); }
        else { return _vEdgeMap2D.at(node2.xy()); }

    }
}

const GridEdgeGR & GridMapGR::gridEdge(const XY<IndexType> &node1, const XY<IndexType> &node2) const
{
    Assert(_mode2D);
    if (node1.x() != node2.x())
    {
        // horizontal edge
        if (node1.x() < node2.x()) { return _hEdgeMap2D.at(node1); }
        else { return _hEdgeMap2D.at(node2); }
    }
    // vertical edge
    if (node1.y() < node2.y()) { return _vEdgeMap2D.at(node1); }
    else { return _vEdgeMap2D.at(node2); }
}

GridEdgeGR & GridMapGR::gridEdge(const XY<IndexType> &node1, const XY<IndexType> &node2)
{
    Assert(_mode2D);
    if (node1.x() != node2.x())
    {
        // horizontal edge
        if (node1.x() < node2.x()) { return _hEdgeMap2D.at(node1); }
        else { return _hEdgeMap2D.at(node2); }
    }
    // vertical edge
    if (node1.y() < node2.y()) { return _vEdgeMap2D.at(node1); }
    else { return _vEdgeMap2D.at(node2); }
}

void GridMapGR::initHCap(IndexType layerIdx, IntType totalCap)
{
    for (IndexType x = 0; x < _horizontalEdgeMap.xSize(); ++x)
    {
        for (IndexType y = 0; y <_horizontalEdgeMap.ySize(); ++ y)
        {
            _horizontalEdgeMap.at(x, y, layerIdx).setTotalCap(totalCap);
        }
    }
}

void GridMapGR::initVCap(IndexType layerIdx, IntType totalCap)
{
    for (IndexType x = 0; x < _verticalEdgeMap.xSize(); ++x)
    {
        for (IndexType y = 0; y <_verticalEdgeMap.ySize(); ++ y)
        {
            _verticalEdgeMap.at(x, y, layerIdx).setTotalCap(totalCap);
        }
    }
}


void GridMapGR::initViaCap(IndexType layerIdx, IntType totalCap)
{
    for (IndexType x = 0; x < _viaEdgeMap.xSize(); ++x)
    {
        for (IndexType y = 0; y <_viaEdgeMap.ySize(); ++ y)
        {
            _viaEdgeMap.at(x, y, layerIdx).setTotalCap(totalCap);
        }
    }
}
PROJECT_NAMESPACE_END
