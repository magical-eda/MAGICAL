/**
 * @file ParserGds.h
 * @brief Parser for cell gds files
 * @author Keren Zhu
 * @date 10/15/2019
 */

#ifndef IDEAPLACE_PARSER_GDS_H_
#define IDEAPLACE_PARSER_GDS_H_

#include "db/Database.h"
#include "util/GdsHelper.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::ParserCellGds
/// @brief Parser for gds file. Each time reading in a gds for a cell
class ParserCellGds
{
    public:

    /// @briVVef struct for polygon with layer
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
            void scale(RealType scaleX, RealType scaleY)
            {
                for (IndexType ptIdx = 0; ptIdx < pts.size(); ++ptIdx)
                {
                    RealType x = pts.at(ptIdx).x();
                    RealType y = pts.at(ptIdx).y();
                    LocType newX = ::klib::autoRound<LocType>(x * scaleX);
                    LocType newY = ::klib::autoRound<LocType>(y * scaleY);

                    pts.at(ptIdx).setX(newX);
                    pts.at(ptIdx).setY(newY);
                }
            }

            ///Members
            std::vector<XY<LocType>> pts; ///< The points of the polygon
            IndexType layer = INDEX_TYPE_MAX; ///< The layer of the polygon
    };

    public:
        /// @brief constructor
        /// @param the reference to the placement database
        explicit ParserCellGds(Database &db) : _db(db) {}
        /// @brief parsing in a gds file
        /// @param first: the gds file name
        /// @param second: the cell index. If given INDEX_TYPE_MAX or ungiven, search the cell by name
        /// @return if the parsing is successful
        bool parseCellGds(const std::string &filename, IndexType cellIdx = INDEX_TYPE_MAX);
        /// @brief scale the design based on the router precision
        void scaleDesign();
        /// @brief dump the read gds shapes to the database
        /// @param the cell index
        void dumpToDb(IndexType cellIdx);
    private:
        Database &_db; ///< The database for the placement engine
        IntType _gdsDBU; ///< Based on the gds, x database units per um
        std::vector<PolygonLayer> _polygons; ///< The polygons read from the gds
};

namespace ParserCellGdsDetails
{
    /// @brief parsing all the gds files
    /// @param first: the placement database
    /// @param second: a vector of gds file names
    /// @return if successful
    inline bool parseAllGdsFiles(Database &db, const std::vector<std::string> &gdsFiles)
    {
        for (const auto &gds : gdsFiles)
        {
            if (!ParserCellGds(db).parseCellGds(gds))
            {
                ERR("%s: parse %s failed! \n", __FUNCTION__, gds.c_str());
                return false;
            }
        }
        return true;
    }
}


/// @brief detailed struct for the functions of processing gds shapes
namespace ExtractShapeLayerActionDetails
{
    namespace gtl = boost::polygon;
    /// @brief convert from gds layer to router layer
    /// @return INDEX_TYPE_MAX if not found, otherwise, the router layer
    inline IndexType layerIdx(IntType gdsLayer, std::unordered_map<IndexType, IndexType> &map)
    {
        /// Search in the map
        const auto findIter = map.find(static_cast<IndexType>(gdsLayer));
        if (findIter != map.end())
        {
            return findIter->second;
        }
        return INDEX_TYPE_MAX;
    }
    /// @brief default action
    template<typename ObjectType>
    inline void extractShape(std::unordered_map<IndexType, IndexType> &map, std::vector<ParserCellGds::PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ObjectType *object)
    {
    }


    /// @brief process gds rectangle
    template<>
    inline void extractShape(std::unordered_map<IndexType, IndexType> &map, std::vector<ParserCellGds::PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsRectangle *object)
    {
        DBG("Rect \n");
        AssertMsg(0, "%s: Rectangle type not support yet \n", __FUNCTION__);
        //DBG("Gds rectangle layer %d, (%d, %d ), (%d, %d) \n", object->layer(), gtl::xl(object), gtl::yl(object), gtl::xh(object), gtl::yh(object));
    }

    /// @brief process gds polygon
    template<>
    inline void extractShape(std::unordered_map<IndexType, IndexType> &map, std::vector<ParserCellGds::PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPolygon *object)
    {
        /// Convert the gds layer to the router layer
        IndexType routerLayer = layerIdx(object->layer(), map);
        if (routerLayer != INDEX_TYPE_MAX)
        {
            polygons.emplace_back(ParserCellGds::PolygonLayer());
            polygons.back().layer = routerLayer;
            /// Add points
            for (auto pt : *object)
            {
                polygons.back().pts.emplace_back(XY<LocType>(pt.x(), pt.y()));
            }
        }
    }

    /// @brief process path
    template<>
    inline void extractShape(std::unordered_map<IndexType, IndexType> &map, std::vector<ParserCellGds::PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object)
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
    ExtractShapeLayerAction(std::unordered_map<IndexType, IndexType> &map, std::vector<ParserCellGds::PolygonLayer> &polygons) : _layerIdxMap(map), _polygons(polygons) {}
    template<typename ObjectType>
    void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
    {
        ExtractShapeLayerActionDetails::extractShape(_layerIdxMap, _polygons, type, object);
    }

    /// @return a message of action for debug
    std::string message() const 
    {
        return "ExtractShapeLayerAction";
    }

    std::unordered_map<IndexType, IndexType> & _layerIdxMap; ///< A map from gds layer to IDEAPLACE layer. _layerIdxMap[
    std::vector<ParserCellGds::PolygonLayer> &_polygons; ///< The polygons read from the gds
};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PARSER_GDS_H_
