#ifndef __PARSER_GDS_H__
#define __PARSER_GDS_H__

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>
#include "global/global.h"
#include "db/Layout.h"
#include "db/TechDB.h"
#include "util/Polygon2Rect.h"
#include <algorithm>
#include <string>
#include <vector>
#include <numeric>
#include <iostream>

PROJECT_NAMESPACE_BEGIN

using GdsParser::GdsDB::GdsCell;
using GdsParser::GdsDB::GdsPolygon;
using GdsParser::GdsDB::GdsRectangle;
using GdsParser::GdsDB::GdsCellReference;

class Parser
{
    public:
        explicit Parser(const std::string & fileName, Layout & layer, TechDB & techDB) : _layer(layer), _techDB(techDB)
        {
            read(fileName);
        }
        bool read(const std::string & filename);   
    private:
        GdsParser::GdsDB::GdsDB _db;
        Layout & _layer;
        TechDB & _techDB;
};

namespace ParseLayoutAction
{
    /// @brief default action
    template<typename ObjectType>
    inline void extractLayout(Layout & layer, TechDB & techDB, ::GdsParser::GdsRecords::EnumType type, ObjectType *object)
    {
    }
    /// @brief process gds rectangle
    template<>
    inline void extractLayout(Layout & layer, TechDB& techDB, ::GdsParser::GdsRecords::EnumType type, GdsRectangle *object)
    {
        std::cout << "Rectangles not supported yet";
    }

    /// @brief process gds polygon
    template<>
    inline void extractLayout(Layout & layer, TechDB& techDB, ::GdsParser::GdsRecords::EnumType type, GdsPolygon *object)
    {
        /// Polygon shapes will be processed into rectangles
        IndexType layer_id(object->layer()), datatype(object->datatype()); 
        IndexType rect_id;
        std::vector<Box<LocType>> rects;
        std::vector<XY<LocType>> pts;
        layer_id = techDB.pdkLayerToDb(layer_id);
        for (auto pt : *object)
        {
            pts.emplace_back(pt.x(), pt.y());
        }
        ::klib::convertPolygon2Rects<LocType>(pts, rects);
        for (auto rect : rects)
        {
            rect_id = layer.insertRect(layer_id, rect);
            if(datatype != 0)
                layer.setRectDatatype(layer_id, rect_id, datatype);
        }
        // LocType x_min = std::numeric_limits<LocType>::max();
        // LocType x_max = std::numeric_limits<LocType>::min();
        // LocType y_min = std::numeric_limits<LocType>::max();
        // LocType y_max = std::numeric_limits<LocType>::min();
        // for (auto pt : *object)
        // {
        //     if(pt.x() < x_min) x_min = pt.x();
        //     if(pt.x() > x_max) x_max = pt.x();
        //     if(pt.y() < y_min) y_min = pt.y();
        //     if(pt.y() > y_max) y_max = pt.y();
        // }
        // rect_id = layer.insertRect(layer_id, x_min, y_min, x_max, y_max);
        // if(datatype != 0) 
        //     layer.setRectDatatype(layer_id, rect_id, datatype);
    }
    /// @brief process path
    template<>
    inline void extractLayout(Layout & layer, TechDB & techDB, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object)
    {
        auto polygon = object->toPolygon();
        extractLayout(layer, techDB, type, &polygon);
    }

}

/// @brief aution function object to process the the 
struct ParseLayout
{
    /// @param first: the map between the gds layer indices to the router layers. Only care about the mapping within this map
    /// @param second: a reference to a vector of polygons, saved the read polygons in this vector
    ParseLayout(Layout& layer, TechDB & techDB) : _layer(layer), _techDB(techDB) {}
    template<typename ObjectType>
    void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
    {
        ParseLayoutAction::extractLayout(_layer, _techDB, type, object);
    }
    std::string message() const
    {
        return "ExtractLayout";
    }
    Layout & _layer;
    TechDB & _techDB;
};



PROJECT_NAMESPACE_END

#endif
