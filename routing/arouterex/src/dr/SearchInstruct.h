/**
 * @file SearchInstruct.h
 * @brief Searching instruction. Embeded the instruction of how to search and query the space to avoid violations
 * @author Keren Zhu
 * @date 11/09/2018
 */

#ifndef AROUTER_SEARCH_INSTRUCT_H_
#define AROUTER_SEARCH_INSTRUCT_H_

#include "global/global.h"
#include "util/Vector2D.h"
#include "db/drDB/DatabaseDR.h"
#include "db/macroDB/MacroDatabase.h"
#include "db/grDB/DatabaseGR.h"
#include "db/techDB/TechDatabase.h"

PROJECT_NAMESPACE_BEGIN

/// Forward Declaration for the Databse
class Database;

/// @class AROUTER::SpacingTable
/// @brief the spacing table for the different parallel run
class SpacingTable
{
    public:
        explicit SpacingTable() = default;
        /*------------------------------*/ 
        /* Table indices query          */
        /*------------------------------*/ 
    private:
        /// @brief get the index of x (width)
        /// @param the actaul width
        /// @return the x index in spacing table
        IndexType widthIdx(LocType width) const;
        /// @brief get the index of y (paraLen)
        /// @param the actaul paraLen
        /// @return the y index in spacing table
        IndexType paraLenIdx(LocType paraLen) const;
    public:
        /// @brief get the spacing requirement for the given width and parallel run length
        /// @param width: the wider width of the two shapes are considered. paraLen: their spacing 
        /// @return the required minimum spacing between the two shapes
        LocType spacing(LocType width, LocType paraLen) const { return _spacing.at(this->widthIdx(width), this->paraLenIdx(paraLen));}
        /// @brief the largest spacing requirement
        /// @return the largest spacing need to considered
        LocType largestSpacing() const { return _spacing.at(_width.size() - 1, _parallelRunLength.size() - 1); }
        /// @brief get the largest spacing with certain paralel run length
        LocType paraLenLargestSpacing(LocType paraLen) const { return _spacing.at(_width.size() - 1, this->paraLenIdx(paraLen)); }

        std::vector<LocType> & parallelRunLengthVec() { return _parallelRunLength; }
        std::vector<LocType> & widthVec() { return _width; }
        Vector2D<LocType> & spacingVec2D() { return _spacing; }
    private:
        std::vector<LocType> _parallelRunLength; ///< The thresholds of the parallel run
        std::vector<LocType> _width; ///< The thresholds of the widths
        Vector2D<LocType> _spacing; ///< The spacing requirement in the table. x: width, y: parallel run length
};

inline IndexType SpacingTable::widthIdx(LocType width) const 
{
    /// Assume the most query is in the low width, use linear search
    for (IndexType idx = 0; idx < _width.size() -1; ++ idx)
    {
        if (width < _width.at(idx + 1))
        {
            return idx;
        }
    }
    return _width.size() - 1;
}

inline IndexType SpacingTable::paraLenIdx(LocType paraLen) const 
{
    /// Maybe should use binary search? but the size of the table should be small, won't hurt to use linear search?
    for (IndexType idx = 0; idx < _parallelRunLength.size() - 1; ++idx)
    {
        if (paraLen < _parallelRunLength.at(idx + 1))
        {
            return idx;
        }
    }
    return _parallelRunLength.size() - 1;
}

/// @class AROUTER::SearchInstruct
/// @brief the instruction for instructing the detailed router
class SearchInstruct
{
    public:
        /// @brief constructor
        /// @param database of the software. The class should not contain a copy to the database, but it need the information from the database to build the variables of the class
        explicit SearchInstruct() = default;
        /// @brief init the SearchInstructor
        /// @param the database for the router
        bool init(Database &db);
        /*------------------------------*/ 
        /* Search instuction            */
        /*------------------------------*/ 
        /// @brief aggresive min spacing checking for line extension
        /// @param wire: specification of wire. layer: the idx in routable layers
        /// @return The range should be used in ShapeMap::segmentNearest(const ManSegment<LocType> &segment, IndexType layer, LocType range)
        LocType wireParallelNearestAggrRange(NetNodeWire wire, IndexType layer) const;
        /*---- Cut ----*/
        /// @brief give the conservative cut spacing checking range
        /// @param lowerMetalLayer: the index of the routable layer of the metal layer that below the CUT
        /// @return the conversative via spacing requirement
        LocType cutConservativeSpacing(IndexType lowerMetalLayer) const { return _conservativeCutSpacings.at(DRUtil::routeLayer2CutLayer(lowerMetalLayer)); }
        /*------------------------------*/ 
        /* Design rule querying         */
        /*------------------------------*/ 
        /*---- Metal ----*/
        /// @brief get the spacing requirement with given width and parallel run length
        /// @param width: the wider width of the two shapes are considered. paraLen: their spacing. layer: the index of the layer in routable layers
        /// @return the required minimum spacing between the two shapes
        LocType metalSpacing(LocType width, LocType paraLen, IndexType layer) const { return _metalSpacingTables.at(layer).spacing(width, paraLen); }
        /// @brief get the largest spacing requirement for a layer
        /// @param layer: the index in the routable layers
        /// @return the spacing requirement in unit of database
        LocType metalLargestSpacing(IndexType layer) const { return _metalSpacingTables.at(layer).largestSpacing();}
        /// @brief get the largest spacing with a given parallel run length
        LocType metalParaLenLargestSpacing(LocType paraLen, IndexType layer) const { return _metalSpacingTables.at(layer).paraLenLargestSpacing(paraLen); }
        /// @brief get the minimum metal spacing
        /// @param layer: the index in the routable layers
        /// @return the minimum spacing allowed on this layer
        LocType metalMinSpacing(IndexType layer) const { return _metalMinSpacings.at(layer); }
        /// @brief get the minimum metal area
        /// @param layer: the index in the routable layers
        /// @return the minimum shape area on the layer
        LocType metalMinArea(IndexType layer) const { return _metalMinAreas.at(layer); }
        /*---- Cut ----*/
        /// @brief get the standard cut spacing requirement
        /// @param lowerMetalLayer: the index in the routable layer of the bottom metal layer of the via
        /// @return the standard cut spacing requirement
        LocType cutSpacing(IndexType lowerMetalLayer) const { return _cutSpacings.at(DRUtil::routeLayer2CutLayer(lowerMetalLayer)); }
        /// @brief get the cut spacing requirement if two CUT shapes are from the same net
        /// @param lowerMetalLayer: the index in the routable layer of the bottom metal layer of the via
        /// @return the cut spacing requirement if two cut shapes are from the same net
        LocType cutSameNetSpacing(IndexType lowerMetalLayer) const { return _cutSameNetSapcings.at(DRUtil::routeLayer2CutLayer(lowerMetalLayer)); }
        /// @brief get the cut spacing requirement if two CUT shapes are parallel overlapped
        /// @param lowerMetalLayer: the index of the routable layer of the bottom metal layer of the via
        /// @return the cut spacing requirement if two CUT shapes are parallel overlapped
        LocType cutParallelOverlapSpacings(IndexType lowerMetalLayer) const { return _cutParallelOverlapSpacings.at(DRUtil::routeLayer2CutLayer(lowerMetalLayer)); }
        /*------------------------------*/ 
        /* Getter                       */
        /*------------------------------*/ 
        /// @brief get the min metal width array. Masterslice also counts
        /// @param the index of the layer in the routable layers
        /// @return the min metal width for routable layer
        LocType metalMinWidth(IndexType routeLayer) const { return _metalMinWidth.at(routeLayer); }
        /// @brief get the routing direction for each routable layer
        /// @param the index of the routable layer
        /// @return the routing direction of that layer
        RouteDirType routeDirection(IndexType routeLayer) const { return _routingDir.at(routeLayer); }
        /// @brief number of routable layers
        /// @return number of routable layers
        IndexType numRouteLayers() const { return _routingDir.size(); }
        /* -----------------------------*/
        /* Misc.                        */
        /// @brief align with the grid
        LocType alignGrid(LocType loc)
        {
            return ::klib::round2Int(loc / static_cast<RealType>(_gridAlign)) * _gridAlign; 
        }
        /// @brief align with the grid
        XY<LocType> alignGrid(XY<LocType> loc)
        {
            loc.setX(alignGrid(loc.x()));
            loc.setY(alignGrid(loc.y()));
            return loc;
        }
    private:
        /*------------------------------*/ 
        /* Init and processing          */
        /*------------------------------*/ 
        /// @brief compute the conservative cut spacing requriements
        void initConvervativeCutSpacings();
        /// @brief read in a cut layer
        void readCutLayer(const LefLayerCut &cut, Database &db);
        /// @brief read in a masterslice layer
        void readMasterSliceLayer(const LefLayerMasterslice &master, Database &db);
        /// @brief read in a metal layer
        void readMetalLayer(const LefLayerRouting &metal, Database &db);
    private:
        //std::shared_ptr<ShapeConstructor> _shapeConPtr; ///< Pointer to a shape constuctor
        std::vector<LocType> _metalMinSpacings; ///< The minimum spacing of each routable layers
        std::vector<SpacingTable> _metalSpacingTables; ///< Spacing tables of each routable layers
        std::vector<LocType> _metalMinAreas; ///< The minimum area of the metal shape for each routable layers
        std::vector<LocType> _cutSpacings; ///< The required spacing of each CUT layers
        std::vector<LocType> _cutSameNetSapcings; ///< The required sapcings if the two CUT shapes belonging to the same net
        std::vector<LocType> _cutParallelOverlapSpacings; ///< The required spacing between two CUT shapes with parallel overLap
        std::vector<LocType> _conservativeCutSpacings; ///< The largest cut spacing requirements
        std::vector<LocType> _metalMinWidth; ///< The minimum width of the routable layers
        std::vector<RouteDirType> _routingDir; ///< Routing direction
        LocType _gridAlign = 10; ///< The grid alignment in dbu
};

inline LocType SearchInstruct::wireParallelNearestAggrRange(NetNodeWire wire, IndexType layer) const
{
    /// Use the minimum spacing with the given wdith and no parallel run 
    return this->metalSpacing(wire.halfWidth() * 2, 0, layer);
}


/*------------------------------*/ 
/* Init and processing          */
/*------------------------------*/ 

inline void SearchInstruct::initConvervativeCutSpacings()
{
    for (IndexType layer = 0; layer < _cutSpacings.size(); ++ layer)
    {
        _conservativeCutSpacings.emplace_back(std::max(std::max(_cutSpacings.at(layer), _cutSameNetSapcings.at(layer)), _cutParallelOverlapSpacings.at(layer)));
    }
}

PROJECT_NAMESPACE_END


#endif ///AROUTER_SEARCH_INSTRUCT_H_
