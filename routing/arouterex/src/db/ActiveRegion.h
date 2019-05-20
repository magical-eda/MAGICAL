/**
 * @file ActiveRegion.h
 * @brief Information for active regions
 * @author Keren Zhu
 * @date 03/09/2019
 */

#ifndef AROUTER_ACTIVE_REGION_H_
#define AROUTER_ACTIVE_REGION_H_

#include "global/global.h"
#include "db/RTree.h"

PROJECT_NAMESPACE_BEGIN

/// @class PROJECT_NAMESPACE::ActiveRegion
/// @brief the interface for querying the information about the active region
class ActiveRegion
{
    public:
        /*------------------------------*/ 
        /* Initialization               */
        /*------------------------------*/ 
        /// @brief init the object with a vector of OD boxes
        /// @param a vector of OD boxes
        virtual bool init(const std::vector<Box<LocType>> &ods) = 0;
        /*------------------------------*/ 
        /* Query                        */
        /*------------------------------*/ 
        /// @brief whether the path overlapping with active region
        /// @param the location from and to
        /// @return if the path is overlapping with some active region
        virtual bool overlapPath(const XY<LocType> &from, const XY<LocType> &to) const = 0;
};

/// @class PROJECT_NAMESPACE::ActiveRegionRtree
/// @brief the R tree implementation of ActiveRegion
class ActiveRegionRtree : public ActiveRegion
{
    /*------------------------------*/ 
    /* Types                        */
    /*------------------------------*/ 
    public:
        typedef LocType coordinate_type;
        typedef gtl::coordinate_traits<coordinate_type>::manhattan_area_type area_type;
        typedef gtl::coordinate_traits<coordinate_type>::coordinate_difference coordinate_difference;
        typedef rectangle_data<coordinate_type> base_type;
        typedef point_data<coordinate_type> point_type;
        typedef bg::model::segment<point_type> path_type; /// Use geometic model for the segment query
		typedef rectangle_data<coordinate_type> rectangle_query_type;
        typedef Rectangle<coordinate_type> rectangle_type;

        /// rtree
        typedef bgi::rtree<rectangle_type, bgi::linear<16, 4> > rtree_type;
    public:
        /*------------------------------*/ 
        /* Initialization               */
        /*------------------------------*/ 
        /// @brief init the object with a vector of OD boxes
        /// @param a vector of OD boxes
        bool init(const std::vector<Box<LocType>> &ods) override;
        /*------------------------------*/ 
        /* Query                        */
        /*------------------------------*/ 
        /// @brief whether the path overlapping with active region
        /// @param the location from and to
        /// @return if the path is overlapping with some active region
        bool overlapPath(const XY<LocType> &from, const XY<LocType> &to) const override;
        /*------------------------------*/ 
        /* Debug                        */
        /*------------------------------*/ 
        /// @brief draw all shapes into a gds
        /// @param the filename
        /// @return if successful
        bool writeGds(const std::string &filename);
    protected:
        rtree_type _tree;
};

inline bool ActiveRegionRtree::init(const std::vector<Box<LocType>> &ods)
{
    for (const auto & od : ods)
    {
        rectangle_type rect(od.xLo(), od.yLo(), od.xHi(), od.yHi());
        _tree.insert(rect);
    }
    return true;
}

inline bool ActiveRegionRtree::overlapPath(const XY<LocType> &from, const XY<LocType> &to) const 
{
    return false;
    LocType xLo = std::min(from.x(), to.x()) -70; LocType yLo = std::min(from.y(), to.y()) -70;
    LocType xHi = std::max(from.x(), to.x()) +70; LocType yHi = std::max(from.y(), to.y()) +70;
    rectangle_query_type searchRange(xLo, yLo, xHi, yHi);
    /// Find all the intersect shapes
    std::list<rectangle_type> candidate;
    _tree.query(bgi::intersects(searchRange), std::back_inserter(candidate));
    return !candidate.empty();
}

PROJECT_NAMESPACE_END

#include "writer/gdsii/WriteGds.h"
PROJECT_NAMESPACE_BEGIN
inline bool ActiveRegionRtree::writeGds(const std::string &filename)
{
    auto _wg = std::make_shared<WriteGds>(filename);
    //_wg = std::shared_ptr<WriteGds>(new WriteGds(outputFile));
    if (!_wg->initWriter())
    {
        return false;
    }
    if (!_wg->createLib("TOP", 2000, 1e-6/2000))
    {
        return false;
    }
    if (!_wg->writeCellBgn("RTREE"))
    {
        return false;
    }
    /// Iterate through the shapes in the r trees and write them into gds
    for (auto iter = _tree.begin(); iter != _tree.end(); ++iter)
    {
        IntType xl = gtl::xl(*iter); IntType yl = gtl::yl(*iter);
        IntType xh = gtl::xh(*iter); IntType yh = gtl::yh(*iter);
        Box<IntType> shape = Box<IntType> (xl, yl, xh, yh);
        _wg->writeRectangle(shape, 1, 0);
    }
    
    if (!_wg->writeCellEnd())
    {
        return false;
    }
    if (!_wg->endLib())
    {
        return false;
    }
    DBG("%s: debug rtree shapes saved in %s \n", __FUNCTION__, filename.c_str());
    return true;
}
PROJECT_NAMESPACE_END
#endif //AROUTER_ACTIVE_REGION_H_
