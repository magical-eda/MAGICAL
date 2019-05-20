#include "DatabaseGR.h"

PROJECT_NAMESPACE_BEGIN

bool DatabaseGR::initISPD08()
{
    /// init pins in the grid
    this->initPinInGrid();
    /// init bounding box for each nets
    for (IndexType netIdx = 0; netIdx < this->nets().size(); ++netIdx)
    {
        if (! this->initNetBBox(netIdx)) { return false; }
    }
    this->edgeMap().resize(this->nets().size());
    return true;
}

void DatabaseGR::initPinInGrid()
{
    /// init the physical pins into the grid
    for (IndexType pinIdx = 0; pinIdx < this->phyPinArray().size(); ++pinIdx)
    {
        const auto &pin = this->phyPinArray().at(pinIdx);
        // Because it is needed to add pins to both 3D and 2D grids
        // Here using the getter of the vector instead of wrapoed gridCell()
        
        // 3D vector
        this->gridMap().gridCells().at(XYZ<IndexType>(pin.gridIdx(), pin.layer())).dropPin(pinIdx);
        // 2D vector
        this->gridMap().gridCells2D().at(pin.gridIdx()).dropPin(pinIdx);
    }
}

bool DatabaseGR::initNetBBox(IndexType netIdx)
{
    auto &net = this->nets().at(netIdx);
    net.gridBBox() = Box<IndexType>(INDEX_TYPE_MAX, 0, INDEX_TYPE_MAX, 0); ///< a impossible box
    for (IndexType pinIdx : net.physPinArray())
    {
        net.gridBBox().join(this->pin(pinIdx).gridIdx());
    }
    return true;
}

void DatabaseGR::addSubNet(IndexType netIdx, IndexType pinIdxFrom, IndexType pinIdxTo)
{
    IndexType subNetIdx = this->subNets().size();
    this->subNets().emplace_back(SubNet(netIdx, pinIdxFrom, pinIdxTo));
    this->nets().at(netIdx).subNets().emplace_back(subNetIdx);
}


PROJECT_NAMESPACE_END
