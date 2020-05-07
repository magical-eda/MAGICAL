#include "ParserGds.h"
#include "util/Polygon2Rect.h"

PROJECT_NAMESPACE_BEGIN

bool ParserCellGds::parseCellGds(const std::string &filename, IndexType cellIdx)
{
    // Flaten the gds by the last cell
    ::GdsParser::GdsDB::GdsDB unflatenDb;
    ::GdsParser::GdsDB::GdsReader reader(unflatenDb);
    if (!reader(filename))
    {
        ERR("Placement Gds Parser: cannot open file %s! \n", filename.c_str());
    }
    // Set the gds unit based on precision
    _gdsDBU = ::klib::autoRound<IntType>(1e-6 / unflatenDb.precision());


    // Flaten the gds
    std::string topCellName = ::klib::topCell(unflatenDb);
    auto flatCell = unflatenDb.extractCell(topCellName);
    ::GdsParser::GdsDB::GdsDB _gdsDb;
    _gdsDb.addCell(flatCell);


    // Process the shapes in the gds
    for (const auto &object : flatCell.objects())
    {
        ::GdsParser::GdsDB::GdsObjectHelpers()(object.first, object.second, ExtractShapeLayerAction(_db.tech().layerIdxMap(), _polygons));
    }

    // Scale the shapes to the placer database unit
    this->scaleDesign();

    // Find the cell index if not given
    if (cellIdx == INDEX_TYPE_MAX)
    {
        // Check the cell name with the top cell name
        for (IndexType idx = 0; idx < _db.numCells(); ++idx)
        {
            if (topCellName == _db.cell(idx).name())
            {
                cellIdx = idx;
                break;
            }
        }
        AssertMsg(cellIdx != INDEX_TYPE_MAX, "ParserCellGds::%s cannot locate the cell %s in database \n", __FUNCTION__, topCellName.c_str());
    }

    // Write the read shapes into the database
    this->dumpToDb(cellIdx);
    return true;
}


void ParserCellGds::scaleDesign()
{
    auto dbDBU = _db.tech().dbu();
    RealType scale = std::round(static_cast<RealType>(dbDBU) / static_cast<RealType>(_gdsDBU));
    for (auto & poly : _polygons)
    {
        poly.scale(scale, scale);
    }
}

void ParserCellGds::dumpToDb(IndexType cellIdx)
{
    auto &cell = _db.cell(cellIdx);
    for (const auto &poly : _polygons)
    {
        IndexType placerLayer = poly.layer;
        std::vector<Box<LocType>> shapes;
        ::klib::convertPolygon2Rects<LocType>(poly.pts, shapes);
        for (const auto &shape : shapes)
        {
            cell.unionBBox(placerLayer, shape);
        }
    }
    cell.calculateCellBBox();
}

PROJECT_NAMESPACE_END
