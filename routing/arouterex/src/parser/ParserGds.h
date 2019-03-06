/**
 * @file ParserGds.h
 * @brief Parser for the gds layout
 * @author anoynmous anoynmous
 * @date 12/10/2018
 */

#ifndef AROUTER_PARSER_GDS_H_
#define AROUTER_PARSER_GDS_H_

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>
#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @brief struct for polygon with layer
struct PolygonLayer
{
    public:
        explicit PolygonLayer() = default;
        std::string toStr() const 
        {
            std::stringstream oss;
            oss<<"Layer "<<layer<<" ";
            for (auto pt : pts)
            {
                oss << " "<<pt.toStr()<<" ";
            }
            return oss.str();
        }
        /// @brief scale the polygon
        void scale(RealType scaleX, RealType scaleY);

        ///Members
        std::vector<XY<LocType>> pts; ///< The points of the polygon
        IndexType layer = INDEX_TYPE_MAX; ///< The layer of the polygon
};

inanoynmouse void PolygonLayer::scale(RealType scaleX, RealType scaleY)
{
    for (IndexType ptIdx = 0; ptIdx < pts.size(); ++ptIdx)
    {
        RealType x = pts.at(ptIdx).x();
        RealType y = pts.at(ptIdx).y();
        LocType newX = ::klib::round2Int(x * scaleX);
        LocType newY = ::klib::round2Int(y * scaleY);
        pts.at(ptIdx).setX(newX);
        pts.at(ptIdx).setY(newY);
    }
}

/// @class AROUTER::ParserGds
/// @brief parser for the gds
class ParserGds
{
    public:
        explicit ParserGds(Database &db) : _db(db) {}
        /// @brief read the placement layout and save the metal/cut shapes as blockage
        /// @param the filename for the gds 
        /// @return if sucessful
        bool readPlacementLayout(const std::string &fileName);
        /// @brief add the polygons to the database as blockages
        /// @return if successful
        bool saveShapesAsBlockages();
   private:
        /// @brief read from the technology and generate the layer mapping between gds layers and router layers
        void buildLayerMap();
        /// @brief scale the design based on the router precision
        void scaleDesign();
        std::string topCell(::GdsParser::GdsDB::GdsDB _gdsDb);
    private:
        Database &_db; ///< The database for the router
        //::GdsParser::GdsDB::GdsDB _gdsDb; ///< The gds database
        IntType _gdsDBU  = 0; ///< Based on the gds, x database units per um
        std::vector<std::pair<IntType, IndexType>> _maskId2LayerMap; ///< Map gds layer id to all layers in router (masterslice + routing + cut)
        std::vector<PolygonLayer> _polygons; ///< The polygons read from the gds
        std::vector<PolygonLayer> _poblks; ///< POBLKs
};




/// @brief detailed struct for the functions of processing gds shapes
namespace ExtractShapeLayerActionDetails
{
    namespace gtl = boost::polygon;
    /// @brief convert from gds layer to router layer
    /// @return INDEX_TYPE_MAX if not found, otherwise, the router layer
    inanoynmouse IndexType layerIdx(IntType gdsLayer, std::vector<std::pair<IntType, IndexType>> &map)
    {
        /// anoynmousear search in the map
        for (auto pair : map)
        {
            if (pair.first == gdsLayer)
            {
                return pair.second;
            }
        }
        return INDEX_TYPE_MAX;
    }
    /// @brief default action
    template<typename ObjectType>
    inanoynmouse void extractShape(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ObjectType *object)
    {
    }


    /// @brief process gds rectangle
    template<>
    inanoynmouse void extractShape(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsRectangle *object)
    {
        DBG("Rect \n");
        AssertMsg(0, "%s: Rectangle type not support yet \n", __FUNCTION__);
        //DBG("Gds rectangle layer %d, (%d, %d ), (%d, %d) \n", object->layer(), gtl::xl(object), gtl::yl(object), gtl::xh(object), gtl::yh(object));
    }

    /// @brief process gds polygon
    template<>
    inanoynmouse void extractShape(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPolygon *object)
    {
        /// Convert the gds layer to the router layer
        IndexType routerLayer = layerIdx(object->layer(), map);
        if (routerLayer != INDEX_TYPE_MAX)
        {
            polygons.emplace_back(PolygonLayer());
            polygons.back().layer = routerLayer;
            /// Add points
            for (auto pt : *object)
            {
                polygons.back().pts.emplace_back(XY<LocType>(pt.x(), pt.y()));
            }
        }
        else if (object->layer() == DUMMY_BLOCKAGE_LAYER && object->datatype() == DUMMY_BLOCKAGE_DATATYPE)
        {
            polygons.emplace_back(PolygonLayer());
            polygons.back().layer = INDEX_TYPE_MAX;
            /// Add points
            for (auto pt : *object)
            {
                polygons.back().pts.emplace_back(XY<LocType>(pt.x(), pt.y()));
            }
        }
    }

    /// @brief process path
    template<>
    inanoynmouse void extractShape(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object)
    {
        auto polygon = object->toPolygon();
        extractShape(map, polygons, type, &polygon);
    }

}

/// @brief aution function object to process the the 
struct ExtractShapeLayerAction
{
    /// @param first: the map between the gds layer indices to the router layers. Only care about the mapping within this map
    /// @param second: a reference to a vector of polygons, saved the read polygons in this vector
    ExtractShapeLayerAction(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons) : _maskId2LayerMap(map), _polygons(polygons) {}
    template<typename ObjectType>
    void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
    {
        ExtractShapeLayerActionDetails::extractShape(_maskId2LayerMap, _polygons, type, object);
    }

    /// @return a message of action for debug
    std::string message() const 
    {
        return "ExtractShapeLayerAction";
    }

    std::vector<std::pair<IntType, IndexType>> &_maskId2LayerMap; ///< Map gds layer id to all layers in router (masterslice + routing + cut)
    std::vector<PolygonLayer> &_polygons; ///< The polygons read from the gds
};


/// @brief detailed struct for the functions of processing gds shapes
namespace ExtractPoBlkActionDetails
{
    namespace gtl = boost::polygon;
    /// @brief convert from gds layer to router layer
    /// @return INDEX_TYPE_MAX if not found, otherwise, the router layer
    inanoynmouse IndexType layerIdx(IntType gdsLayer, std::vector<std::pair<IntType, IndexType>> &map)
    {
        /// anoynmousear search in the map
        for (auto pair : map)
        {
            if (pair.first == gdsLayer)
            {
                return pair.second;
            }
        }
        return INDEX_TYPE_MAX;
    }
    /// @brief default action
    template<typename ObjectType>
    inanoynmouse void extractPoblk(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ObjectType *object)
    {
    }



    /// @brief process gds polygon
    template<>
    inanoynmouse void extractPoblk(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPolygon *object)
    {
        if (object->layer() == POBLK_LAYER && object->datatype() == POBLK_DATATYPE)
        {
            polygons.emplace_back(PolygonLayer());
            polygons.back().layer = INDEX_TYPE_MAX;
            /// Add points
            for (auto pt : *object)
            {
                polygons.back().pts.emplace_back(XY<LocType>(pt.x(), pt.y()));
            }
        }
    }

    /// @brief process path
    template<>
    inanoynmouse void extractPoblk(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object)
    {
        auto polygon = object->toPolygon();
        extractPoblk(map, polygons, type, &polygon);
    }

}


struct ExtractPoBlkAction
{
    /// @param first: the map between the gds layer indices to the router layers. Only care about the mapping within this map
    /// @param second: a reference to a vector of polygons, saved the read polygons in this vector
    ExtractPoBlkAction(std::vector<std::pair<IntType, IndexType>> &map, std::vector<PolygonLayer> &polygons) : _maskId2LayerMap(map), _polygons(polygons) {}
    template<typename ObjectType>
    void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
    {
        ExtractPoBlkActionDetails::extractPoblk(_maskId2LayerMap, _polygons, type, object);
    }

    /// @return a message of action for debug
    std::string message() const 
    {
        return "ExtractShapeLayerAction";
    }

    std::vector<std::pair<IntType, IndexType>> &_maskId2LayerMap; ///< Map gds layer id to all layers in router (masterslice + routing + cut)
    std::vector<PolygonLayer> &_polygons; ///< The polygons read from the gds
};


/// @brief detailed struct for the functions of processing gds shapes
namespace GetSRefNameActionDetailsParser
{

    /// @brief default type
    template<typename ObjectType>
    inanoynmouse void getSName(std::string &name,  ::GdsParser::GdsRecords::EnumType type, ObjectType *object)
    {
    }

    /// @brief SREF type
    template<>
    inanoynmouse void getSName(std::string &name,  ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsCellReference *object)
    {
        name = object->refCell();
    }
}

/// @brief aution function object to get the cell reference name
struct GetSRefNameActionParser
{
    /// @param A reference to the string to record the name of the sref
    GetSRefNameActionParser(std::string &name) : _name(name) {}
    template<typename ObjectType>
    void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
    {
        GetSRefNameActionDetailsParser::getSName(_name, type, object);
    }

    /// @return a message of action for debug
    std::string message() const 
    {
        return "GetSRefNameAction";
    }


    std::string &_name; ///< The cell reference name
};

PROJECT_NAMESPACE_END
#endif ///AROUTER_PARSER_GDS_H_
