#include "WriteGdsLayoutDb.h"

PROJECT_NAMESPACE_BEGIN

bool WriteGdsLayoutDb::debugWriteNet(IndexType netIdx, const std::string &filename )
{
    _togther = false;
    if (!readPlacementLayout())
    {
        return false;
    }
    if (_togther)
    {
        _interCell = ::klib::createSharedPtr<::GdsParser::GdsDB::GdsCell>(_gdsDb.cells().front());
    }
    else
    {
        // Erase the original gds and add a new cell "ROUTE"
        // The routing shapes will be stored in it
        _gdsDb.cells().clear();
        _gdsDb.addCell("ROUTE");
        _interCell = ::klib::createSharedPtr<::GdsParser::GdsDB::GdsCell>(_gdsDb.cells().front());
    }

    //_interCell = ::GdsParser::GdsDB::GdsCell();
    //_interCell.setName("ROUTE");

    // Add the routing to the cell
    if (!writeNetLayout(netIdx))
    {
        return false;
    }
    // write intra cell
    if (!writeIntraCell(netIdx))
    {
        return false;
    }

    // Write to gds
    GdsParser::GdsDB::GdsWriter gw (_gdsDb);
    gw(filename.c_str());
    INF("Write the gds to %s \n", filename.c_str());

    return true;
}

bool WriteGdsLayoutDb::debugWriteNetArray(const std::vector<IndexType> nets, const std::string &filename)
{
    _togther = false;
    if (!readPlacementLayout())
    {
        return false;
    }
    if (_togther)
    {
        _interCell = ::klib::createSharedPtr<::GdsParser::GdsDB::GdsCell>(_gdsDb.cells().front());
    }
    else
    {
        _gdsDb.cells().clear();
        _gdsDb.addCell("ROUTE");
        _interCell = ::klib::createSharedPtr<::GdsParser::GdsDB::GdsCell>(_gdsDb.cells().front());
    }

    //_interCell = ::GdsParser::GdsDB::GdsCell();
    //_interCell.setName("ROUTE");

    // Add the routing to the cell
    for (auto netIdx : nets)
    {
        if (!writeNetLayout(netIdx))
        {
            return false;
        }
        // write intra cell
        if (!writeIntraCell(netIdx))
        {
            return false;
        }
    }

    // Write to gds
    GdsParser::GdsDB::GdsWriter gw (_gdsDb);
    gw(filename.c_str());
    INF("Write the gds to %s \n", filename.c_str());

    return true;
}
bool WriteGdsLayoutDb::writeGds(bool together)
{
    WRN("%s: using hard-coded pin label layer  \n", __FUNCTION__);
    WRN("%s: using hard-coded layer datatype \n", __FUNCTION__);
    _togther = together;
    // Read in the placement layout
    if (!readPlacementLayout())
    {
        return false;
    }
    if (_togther)
    {
        // Find the top cell of the gds library
        auto topName = ::klib::topCell(_gdsDb);
        // Add the routing shapes to the top cell
        _interCell = ::klib::createSharedPtr<::GdsParser::GdsDB::GdsCell>(*_gdsDb.getCell(topName));
    }
    else
    {
        // Erase the original gds and add a new cell "ROUTE"
        // The routing shapes will be stored in it
        _gdsDb.cells().clear();
        _gdsDb.addCell("ROUTE");
        _interCell = ::klib::createSharedPtr<::GdsParser::GdsDB::GdsCell>(_gdsDb.cells().front());
    }

    //_interCell = ::GdsParser::GdsDB::GdsCell();
    //_interCell.setName("ROUTE");

    // Add the routing to the cell
    if (!addRoute())
    {
        return false;
    }

    // add cell to the gds db
    //_gdsDb.addCell(_interCell);

    // Write to gds
    GdsParser::GdsDB::GdsWriter gw (_gdsDb);
    gw(_db.progArgs().outputFile().c_str());
    INF("Write the gds to %s \n", _db.progArgs().outputFile().c_str());

    return true;
}

bool WriteGdsLayoutDb::readPlacementLayout()
{
    // Flaten the gds by the last cell
    ::GdsParser::GdsDB::GdsReader reader(_gdsDb);
    if (!reader(_db.progArgs().placementLayoutFileName()))
    {
        ERR("Write GDS: cannot open placement layout %s! \n", _db.progArgs().placementLayoutFileName().c_str());
        return false;
    }
    // Set the gds unit based on precision
    _scale = (1e-6 / _gdsDb.precision()) / _db.techDB().units().databaseNumber;

    return true;
}

bool WriteGdsLayoutDb::addRoute()
{
    for (IndexType netIdx = 0; netIdx < _db.drDB().numNets(); ++netIdx)
    {
        if (!writeNetLayout(netIdx))
        {
            return false;
        }
        // write intra cell
        if (!writeIntraCell(netIdx))
        {
            return false;
        }
    }
    return true;
}

bool WriteGdsLayoutDb::writeNetLayout(IndexType netIdx)
{
    auto &net = _db.drDB().net(netIdx);
    auto netName = _db.grDB().nets().at(netIdx).name();
    bool writeLabel = true;
    if (_db.progArgs().isIopinFileSet())
    {
        writeLabel = _db.isIOport(netName);
    }
    for (IndexType nodeIdx = 0; nodeIdx < net.numNodes(); ++nodeIdx)
    {
        auto &node = net.node(nodeIdx);
        if (node.hasVia())
        {
            //if (node.layer() >= 6 )
            if (1)
            {
                if (!this->writeShape(ShapeID(netIdx, nodeIdx), false))
                {
                    ERR("Write VIA fails! \n");
                    return false;
                }
            }
            else
            {
                if (!this->writeShape(ShapeID(netIdx, nodeIdx), writeLabel))
                {
                    ERR("Write VIA fails! \n");
                    return false;
                }
                writeLabel = false;
            }
        }
        else if (!node.isFake())
        {
            if (node.hasParent())
            {
                if (node.layer() >= 7)
                {
                    if (!this->writeShape(ShapeID(netIdx, nodeIdx), false))
                    {
                        ERR("Write wire fails! \n");
                        return false;
                    }
                }
                else
                {
                    if (!this->writeShape(ShapeID(netIdx, nodeIdx), writeLabel))
                    {
                        ERR("Write wire fails! \n");
                        return false;
                    }
                    writeLabel = false;
                }
            }
        }
    }

    // If there is no routing tree, write the label at the pin
    if (writeLabel)
    {
        IndexType termIdx = net.terminal(0);
        const auto &term = _db.terminal(termIdx);
        if (_db.isIOport(_db.grDB().net(term.netIdx()).name()))
        {
            if (!this->addText2Cell(term.primarySearchPoint().loc(), _db.techDB().accessLayerMaskId(term.primarySearchPoint().layer()), 20, netName))
            {
                ERR("Write label fails \n");
                return false;
            }
        }
    }
    return true;
}

bool WriteGdsLayoutDb::writeIntraCell(IndexType netIdx)
{
    const auto &net = _db.drDB().net(netIdx);
    for (const auto termIdx : net.terminalArray())
    {
        const auto &term = _db.terminal(termIdx);
        for (IndexType shapeIdx = 0; shapeIdx <  term.intraCellRoute().size(); ++shapeIdx)
        {
            if (!term.generated(shapeIdx))
            {
                continue;
            }
            const auto &shape = term.intraShape(shapeIdx);
            if (!addBox2Cell(shape.box(), _db.techDB().layerIdx2MaskID(shape.layer())))
            {
                return false;
            }
        }
    }
    return true;
}

bool WriteGdsLayoutDb::writeShape(ShapeID shapeID, bool writeLabel)
{
    _shapeComp.computeShape(shapeID);

    // For wrting labels
    XY<LocType> pt;
    IntType layer = 0;
    IntType size = 0;
    const auto &shapeVec = _shapeComp.result();
    for (const auto &shape : shapeVec)
    {
        // Update the pt and layer
        pt = shape.first.center();
        layer = _db.techDB().layerIdx2MaskID(shape.second);
        size = std::min(shape.first.xLen(), shape.first.yLen());
        IntType datatype = 0;
        if (shape.second > DRUtil::routeLayer2AllLayer(6))
        {
            datatype = 40;
        }
        if (!addBox2Cell(shape.first, _db.techDB().layerIdx2MaskID(shape.second), datatype))
        {
            return false;
        }
    }
    // Write the label
    if (writeLabel)
    {
        Assert(shapeID.isRouteShape());
        const auto &net = _db.grDB().nets().at(shapeID.net());
        std::string label = net.name();
        if (_db.isIOport(label))
        {
            if (!addText2Cell(pt, layer, size, label))
            {
                return false;
            }
        }
    }
    return true;
}



bool WriteGdsLayoutDb::addBox2Cell(const Box<LocType> &rect, IntType layer, IntType datatype)
{
    std::vector<point_type> pts;
    pts.emplace_back(converAndScalePt(XY<LocType>(rect.xLo(), rect.yLo())));
    pts.emplace_back(converAndScalePt(XY<LocType>(rect.xLo(), rect.yHi())));
    pts.emplace_back(converAndScalePt(XY<LocType>(rect.xHi(), rect.yHi())));
    pts.emplace_back(converAndScalePt(XY<LocType>(rect.xHi(), rect.yLo())));
    pts.emplace_back(converAndScalePt(XY<LocType>(rect.xLo(), rect.yLo())));

    // Add to the cells
    _interCell->addPolygon(layer, datatype, pts);

    
    return true;
}

bool WriteGdsLayoutDb::addText2Cell(XY<LocType> pt, IntType metalLayer, IntType size, const std::string &str)
{
    // trick: set datatype to int max so that the gds writer will force to skip
    _interCell->addText(100 + metalLayer, std::numeric_limits<int>::max(), 0, str, converAndScalePt(pt), std::numeric_limits<int>::max(), 5, 0, 0.2, 0);
    return true;
}

PROJECT_NAMESPACE_END
