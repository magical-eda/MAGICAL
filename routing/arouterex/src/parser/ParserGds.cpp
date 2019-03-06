#include "ParserGds.h"
#include "util/Polygon2Rect.h"

PROJECT_NAMESPACE_BEGIN


bool ParserGds::readPlacementLayout(const std::string &fileName)
{
    // Flaten the gds by the last cell
    ::GdsParser::GdsDB::GdsDB unflatenDb;
    ::GdsParser::GdsDB::GdsReader reader(unflatenDb);
    if (!reader(fileName))
    {
        ERR("Placement Gds Parser: cannot open file %s! \n", fileName.c_str());
    }
    // Set the gds unit based on precision
    _gdsDBU = ::klib::round2Int(1e-6 / unflatenDb.precision());

    // Flaten the gds
    std::string topCellName = topCell(unflatenDb);
    auto flatCell = unflatenDb.extractCell(topCellName);
    ::GdsParser::GdsDB::GdsDB _gdsDb;
    _gdsDb.addCell(flatCell);

    // Build the mapping between the gds layer and the router layers
    buildLayerMap();

    // Process the shapes in the gds
    for (const auto &object : flatCell.objects())
    {
        ::GdsParser::GdsDB::GdsObjectHelpers()(object.first, object.second, ExtractShapeLayerAction(_maskId2LayerMap, _polygons));
    }

    // Read in poblk
    for (const auto &object : flatCell.objects())
    {
        ::GdsParser::GdsDB::GdsObjectHelpers()(object.first, object.second, ExtractPoBlkAction(_maskId2LayerMap, _poblks));
    }


    // Scale the design
    scaleDesign(); 
    return true;
}

void ParserGds::buildLayerMap()
{
    for (IndexType layerIdx = 0; layerIdx < _db.techDB().layerTypeVec().size(); ++layerIdx)
    {
        if (_db.techDB().isRouterLayer(layerIdx))
        {
            _maskId2LayerMap.emplace_back(std::make_pair(_db.techDB().layerIdx2MaskID(layerIdx), layerIdx));
        }
    }
}

void ParserGds::scaleDesign()
{
    auto routerDBU = _db.techDB().units().databaseNumber;
    RealType scale = std::round(static_cast<RealType>(routerDBU) / static_cast<RealType>(_gdsDBU));
    for (auto & poly : _polygons)
    {
        poly.scale(scale, scale);
    }
    for (auto & poblk : _poblks)
    {
        poblk.scale(scale, scale);
    }
}

bool ParserGds::saveShapesAsBlockages()
{
    // Get poblks
    std::vector<Box<LocType>> poblk;
    for (auto & po : _poblks)
    {
        ::klib::convertPolygon2Rects<LocType>(po.pts, poblk);
    }
    std::vector<Box<LocType>> rects;
    // Convert the polygons to rectangles and save to db
    for (auto &poly : _polygons)
    {
        ::klib::convertPolygon2Rects<LocType>(poly.pts, rects);
        for (const auto &rect : rects)
        {
            if (poly.layer == INDEX_TYPE_MAX)
            {
                // Dummy blockages
                for (const auto &po : poblk)
                {
                    if (po.cover(rect))
                    {
                        _db.addBlockage(rect, poly.layer);
                        continue;
                    }
                }
                continue;
            }
            _db.addBlockage(rect, poly.layer);
        }
        rects.clear();
    }
    return true;
}

std::string ParserGds::topCell(::GdsParser::GdsDB::GdsDB _gdsDb)
{
    // Whether each cell is found as the subcell of the other
    std::map<std::string, bool> nameFound;
    // Iterate all the cells and record their names
    // Add reversely
    for (IntType idx = _gdsDb.cells().size() - 1; idx >= 0; idx--)
    {
        nameFound[_gdsDb.cells().at(idx).name()] = false;
    }
    for (auto &cell : _gdsDb.cells())
    {
        for (auto obj : cell.objects())
        {
            std::string name = "";
            ::GdsParser::GdsDB::GdsObjectHelpers()(obj.first, obj.second, GetSRefNameActionParser(name));
            if (name != "")
            {
                nameFound[name] = true;
            }
        }
    }
    // Return the name that was not included in the other cells
    for (auto pair : nameFound)
    {
        if (pair.first != "" &&  !pair.second)
        {
            return pair.first;
        }
    }

    AssertMsg(0 ,"Reading Gds::%s: all cells are referenced in other cells! \n", __FUNCTION__);
    return "";
}
PROJECT_NAMESPACE_END
