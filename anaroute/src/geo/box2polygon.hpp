/*
 * Copyright 2008 Intel Corporation
 *
 * Use, modification and distribution are subject to the Boost Software License,
 * Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt).
 * */

/**
 * @file   box2polygon.hpp
 * @brief  Geometric Utils: Convert boxes to polygon90 or disjoined boxes
 * @author Keren Zhu
 * @date   02/11/2020
 *
 **/

// Based on https://www.boost.org/doc/libs/1_70_0/libs/polygon/doc/gtl_property_merge_usage.htm

#ifndef _GEO_BOX_2_POLYGON_HPP_
#define _GEO_BOX_2_POLYGON_HPP_

#include <boost/polygon/polygon.hpp>
#include "src/global/global.hpp"
#include "polygon2box.hpp"
#include "polygon.hpp"


PROJECT_NAMESPACE_START
namespace geo
{
    template<typename CoordType>
    inline boost::polygon::polygon_90_set_data<CoordType> box2NativePolygon(const Vector_t<Box<CoordType>> &vBoxes)
    {
        typedef boost::polygon::property_merge_90<CoordType, Int_t> PropertyMergeType; // use Int_t as property_type -> we don't care basically
        typedef boost::polygon::polygon_90_set_data<CoordType>  PolygonSetType;
        typedef std::map<std::set<Int_t>, PolygonSetType> PropertyMergeResultType;
        PropertyMergeType pm;
        for (UInt_t idx = 0; idx < vBoxes.size(); ++idx)
        {
            pm.insert(vBoxes[idx], 0); // Use 0 as property -> do not distinguish shapes
        }
        PropertyMergeResultType result;
        pm.merge(result);
        for (auto & mapPair : result)
        {
            // Should only have one component
            return mapPair.second;
        }
        assert(false);
        return (*result.begin()).second;
    }
    template<typename CoordType>
    void box2Polygon(const Vector_t<Box<CoordType>> &vBoxes, Vector_t<Polygon<CoordType>> &polygonVec)
    {
      if (vBoxes.size() == 0)
      {
          return;
      }
      auto polygonSet = box2NativePolygon(vBoxes);
      polygonSet.get_polygons(polygonVec);
    }
    /// @brief convert the boxes into nonoverlapping boxes
    template<typename CoordType>
    inline void boxes2Boxes(const Vector_t<Box<CoordType>> &vBoxes, Vector_t<Box<CoordType>> &results)
    {
        if (vBoxes.size() == 0)
        {
            return;
        }
        auto polygon = box2NativePolygon(vBoxes);
        polygon.get_rectangles(results);
    }

    /// @brief convert the boxes into nonoverlapping boxes
    template<typename CoordType>
    inline void boxes2BoxesEmplace(Vector_t<Box<CoordType>> &vBoxes)
    {
        if (vBoxes.size() == 0)
        {
            return;
        }
        auto polygon = box2NativePolygon(vBoxes);
        vBoxes.clear();
        polygon.get_rectangles(vBoxes);
    }


    /// @brief do difference on two vector of boxes
    template<typename CoordType>
    inline void boxesDiffAssign(std::vector<Box<CoordType>> &lhs, const std::vector<Box<CoordType>> &rhs)
    {
        typedef boost::polygon::polygon_90_set_data<CoordType>  PolygonSetType;
        PolygonSetType lps, rps;
        for (const auto &rect : lhs)
        {
            lps.insert(rect);
        }
        for (const auto &rect : rhs)
        {
            rps.insert(rect);
        }
        boost::polygon::operators::operator-=(lps, rps);
        lhs.clear();
        lps.get_rectangles(lhs);
    }
    
    /// @brief do difference on two vector of boxes
    template<typename CoordType>
    inline void boxesAddAssign(std::vector<Box<CoordType>> &lhs, const std::vector<Box<CoordType>> &rhs)
    {
        typedef boost::polygon::polygon_90_set_data<CoordType>  PolygonSetType;
        PolygonSetType lps, rps;
        for (const auto &rect : lhs)
        {
            lps.insert(rect);
        }
        for (const auto &rect : rhs)
        {
            rps.insert(rect);
        }
        boost::polygon::operators::operator+=(lps, rps);
        lhs.clear();
        lps.get_rectangles(lhs);
    }
};

PROJECT_NAMESPACE_END

#endif //_GEO_BOX_2_POLYGON_HPP_
