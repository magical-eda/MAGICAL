/**
 * @file WriteGdsLayoutDb.h
 * @brief Wirte routing results to gds the db version
 * @author Keren Zhu
 * @date 12/13/2018
 */

#ifndef AROUTER_WRITE_GDS_LAYOUT_DB_H_
#define AROUTER_WRITE_GDS_LAYOUT_DB_H_

#include "util/GdsHelper.h"
#include "db/Database.h"
#include "dr/ShapeConstructor.h"

PROJECT_NAMESPACE_BEGIN

class WriteGdsLayoutDb
{
    // Point type for the gds db
    typedef boost::polygon::point_data<IntType> point_type;
    public:
        explicit WriteGdsLayoutDb(Database &db) : _db(db), _shapeComp(db) {}
        /// @brief Write the layout
        bool writeGds(bool together);
        /// @brief debug: write a net
        bool debugWriteNet(IndexType netIdx, const std::string &filename);
        /// @brief debug: write several nets
        bool debugWriteNetArray(const std::vector<IndexType> nets, const std::string &filename);
    private:
        /// @brief read placement
        bool readPlacementLayout();
        /// @brief add routing shapes to the _interCell
        bool addRoute();
        /// @brief write net layout
        bool writeNetLayout(IndexType netIdx);
        /// @brief write intraCell connection
        bool writeIntraCell(IndexType netIdx);
        /// @brief write a shape for via or wire
        /// @param the shapeID of the via or wire
        /// @param whether to write the net label on the segment
        /// @return if successful
        bool writeShape(ShapeID shapeID, bool writeLabel);

        /// @brief add a rect to the _interCell
        bool addBox2Cell(const Box<LocType> &rect, IntType layer, IntType datatype = 0);

        /// @brief write the label on the rect
        bool addText2Cell(XY<LocType> pt, IntType metalLayer, IntType size, const std::string &str);

        point_type converAndScalePt(XY<LocType> pt)
        {
            RealType x = pt.x();
            RealType y = pt.y();
            IntType newX = ::klib::round2Int(x * _scale);
            IntType newY = ::klib::round2Int(y * _scale);
            return point_type(newX, newY);
        }


    private:
        Database &_db; /// Router database
        ShapeConstructor _shapeComp;
        ::GdsParser::GdsDB::GdsDB _gdsDb; ///< The gds database
        RealType _scale; /// How much need to scale from the router to gds
        std::shared_ptr<::GdsParser::GdsDB::GdsCell> _interCell = nullptr; ///< The interconnection cell
        bool _togther;
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_WRITE_GDS_LAYOUT_DB_H_
