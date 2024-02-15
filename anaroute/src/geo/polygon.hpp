// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.
// Copyright (c) 2014-2018 Adam Wulkiewicz, Lodz, Poland.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


/**
 * @file polygon.hpp
 * @brief GEO: polygon class
 * @author Keren Zhu
 * @date 02/14/2020
 */

#ifndef _GEO_POLYGON_HPP_
#define _GEO_POLYGON_HPP_

#define _GEO_POLYGON_USING_POLYGON90
//#define _GEO_POLYGON_USING_POLYGON

#include "src/global/namespace.hpp"
#include "point.hpp"

PROJECT_NAMESPACE_START
template<typename CoordType>
class Ring : public std::vector<Point<CoordType>, std::allocator<Point<CoordType>>>
{
  typedef std::vector<Point<CoordType>, std::allocator<Point<CoordType>>> baseType;
  public:
    inline Ring() : baseType() {}

    template<typename Iterator>
    inline Ring(Iterator begin, Iterator end)
    : baseType(begin, end)
    {}

    inline Ring(std::initializer_list<Point<CoordType>> l)
      : baseType(l.begin(), l.end())
    {}
};
PROJECT_NAMESPACE_END

// Boost geometry ring concept
#include <boost/geometry/core/closure.hpp>
#include <boost/geometry/core/point_order.hpp>
#include <boost/geometry/geometries/concepts/ring_concept.hpp>

namespace boost { namespace geometry { namespace traits {

  template<typename CoordType>
  struct tag<PROJECT_NAMESPACE::Ring<CoordType>>
  {
    typedef ring_tag type;
  };

  template<typename CoordType>
  struct point_order<PROJECT_NAMESPACE::Ring<CoordType>>
  {
    static const order_selector value = clockwise;
  };
  
  template<typename CoordType>
  struct closure<PROJECT_NAMESPACE::Ring<CoordType>>
  {
    static const closure_selector value = open;
  };
}}}; //boost::geometry::traits

PROJECT_NAMESPACE_START

template<typename CoordType>
class Polygon
{
  public:
    typedef Point<CoordType> PointType;
    typedef Ring<CoordType> RingType;
    typedef std::vector<RingType> InnnerContainerType; 

    const RingType & outer() const { return _outer; }
    RingType & outer() { return _outer; }
    const std::vector<RingType> & inners() const { return _inners; }
    std::vector<RingType> & inners() { return _inners; }

    Polygon()
      : _outer(), _inners() {}

    Polygon(std::initializer_list<RingType> l)
      : _outer(l.size() > 0 ? *l.begin() : RingType())
      , _inners(l.size() > 0? l.begin() + 1: l.begin(), l.end())
      {}

    void clear()
    {
      _outer.clear();
      _inners.clear();
    }
  protected:
    RingType _outer; ///< The outer polygon/ring
    std::vector<RingType> _inners; ///< The inner rings/holes
};

PROJECT_NAMESPACE_END


// boost::geometry traits
#include <boost/geometry/geometries/polygon.hpp>

namespace boost { namespace geometry { namespace traits {

  template<typename CoordType>
  struct tag<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef polygon_tag type;
  };

  template<typename CoordType>
  struct ring_const_type<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::RingType const& type;
  };

  template<typename CoordType>
  struct ring_mutable_type<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::RingType& type;
  };

  template<typename CoordType>
  struct interior_const_type<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::InnnerContainerType const& type;
  };

  template<typename CoordType>
  struct interior_mutable_type<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::InnnerContainerType& type;
  };

  template<typename CoordType>
  struct exterior_ring<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef PROJECT_NAMESPACE::Polygon<CoordType> polygon_type;
    static inline typename polygon_type::RingType& get(polygon_type& p)
    {
      return p.outer();
    }
    
    static inline typename polygon_type::RingType const& get(const polygon_type &p)
    {
      return p.outer();
    }
  };

  template<typename CoordType>
  struct interior_rings<PROJECT_NAMESPACE::Polygon<CoordType>>
  {

    typedef PROJECT_NAMESPACE::Polygon<CoordType> polygon_type;
    static inline typename polygon_type::InnnerContainerType& get(polygon_type& p)
    {
      return p.inners();
    }
    
    static inline typename polygon_type::InnnerContainerType const& get(const polygon_type &p)
    {
      return p.inners();
    }
  };

}}} //boost::geometry::traits

// boost::polygon::traits
#include <boost/polygon/polygon.hpp>
#include <boost/polygon/detail/iterator_points_to_compact.hpp>
#include <boost/polygon/detail/iterator_compact_to_points.hpp>
namespace boost { namespace polygon {
#if defined(_GEO_POLYGON_USING_POLYGON90)
  // Here we register the polygon as boost:polygon polygon90 concept 
  // One of the key difference in data between 90 and polygon is that polygon90 is using compact representation
  // For example, if compact coordinates are x1, y1, e 2, y2,
  // its points in polygon is (x1, y1) (x2, y1) (x2 y2).
  // In other words, also we can register Polygon as Polygon90, 
  // its efficiency would be lower than using native compact data structure.
  //
  // However, boost::geometry does not have a direct alternative for polygon90
  template<typename CoordType>
  struct geometry_concept<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef polygon_90_concept type;
  };

  template<typename CoordType>
  struct polygon_90_traits<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef CoordType coordinate_type;
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::RingType::const_iterator pointer_iterator_type;
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::PointType PointType;
    typedef iterator_points_to_compact<pointer_iterator_type, PointType> compact_iterator_type;

    static inline compact_iterator_type begin_compact(const PROJECT_NAMESPACE::Polygon<CoordType> &t)
    {
      return compact_iterator_type(t.outer().begin(), t.outer().end());
    }
    static inline compact_iterator_type end_compact(const PROJECT_NAMESPACE::Polygon<CoordType> &t)
    {
      return compact_iterator_type(t.outer().end(), t.outer().end());
    }
    static inline unsigned int size(const PROJECT_NAMESPACE::Polygon<CoordType> &t)
    {
      return t.outer().size();
    }
    static inline winding_direction winding(const PROJECT_NAMESPACE::Polygon<CoordType> &t)
    {
      return boost::polygon::winding_direction::clockwise_winding; // Clockwise
    }
  };

  template<typename CoordType>
  struct polygon_90_mutable_traits<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::PointType PointType;
    template<typename iT>
    static inline PROJECT_NAMESPACE::Polygon<CoordType> & set_compact(PROJECT_NAMESPACE::Polygon<CoordType> &t,
        iT input_begin, iT input_end)
    {
      using iter_type =  iterator_compact_to_points<iT, PointType>;
      t.outer().clear();
      auto iter = iter_type(input_begin, input_end);
      auto end = iter_type(input_end, input_end);
      while (iter != end)
      {
        t.outer().emplace_back(PROJECT_NAMESPACE::Point<CoordType>());
        boost::polygon::assign(t.outer().back(), *iter);
        ++iter;
      }
      return t;
    }
  };

#elif defined(_GEO_POLYGON_USING_POLYGON)

  // Here register Polygon as boost::polygon polygon concept
  template<typename CoordType>
  struct geometry_concept<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef polygon_concept type;
  };

  template<typename CoordType>
  struct polygon_traits<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    typedef CoordType coordinate_type;
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::RingType::const_iterator iterator_type;
    typedef typename PROJECT_NAMESPACE::Polygon<CoordType>::PointType point_type;

    static inline iterator_type begin_points(const PROJECT_NAMESPACE::Polygon<CoordType> &p)
    {
      return p.outer().begin();
    }

    static inline iterator_type end_points(const PROJECT_NAMESPACE::Polygon<CoordType> &p)
    {
      return p.outer().end();
    }

    static inline unsigned int size(const PROJECT_NAMESPACE::Polygon<CoordType> &p)
    {
      return p.outer().size();
    }

    static inline winding_direction winding(const PROJECT_NAMESPACE::Polygon<CoordType> & p)
    {
      return boost::polygon::winding_direction::clockwise_winding; // clockwise
    }
  };

  template<typename CoordType>
  struct polygon_mutable_traits<PROJECT_NAMESPACE::Polygon<CoordType>>
  {
    template<typename iT>
    static inline PROJECT_NAMESPACE::Polygon<CoordType> & set_points(PROJECT_NAMESPACE::Polygon<CoordType> &t,
        iT input_begin, iT input_end)
    {
      t.outer().clear();
      while (input_begin != input_end)
      {
        t.outer().emplace_back(PROJECT_NAMESPACE::Point<CoordType>());
        boost::polygon::assign(t.outer().back(), *input_begin);
        ++input_begin;
      }
      return t;
    }
  };

#endif // _GEO_POLYGON_USING_POLYGON90

}}; //boost::polygon

PROJECT_NAMESPACE_START

template<typename CoordType>
using PolygonSet = std::vector<CoordType>;

PROJECT_NAMESPACE_END

namespace boost { namespace polygon {
  template<typename CoordType>
  struct geometry_concept<PROJECT_NAMESPACE::PolygonSet<CoordType>>
  {
    typedef polygon_90_set_concept type;
  };

  template<typename CoordType>
  struct polygon_set_traits<PROJECT_NAMESPACE::PolygonSet<CoordType>>
  {
    typedef CoordType coordinate_type;
    typedef typename PROJECT_NAMESPACE::PolygonSet<CoordType>::const_iterator iterator_type;
    typedef PROJECT_NAMESPACE::PolygonSet<CoordType> operator_arg_type;

    static inline iterator_type begin(const PROJECT_NAMESPACE::PolygonSet<CoordType> &t)
    {
      return view_as<polygon_90_concept>(t).begin();
    }

    static inline iterator_type end(const PROJECT_NAMESPACE::PolygonSet<CoordType> &t)
    {
      return view_as<polygon_90_concept>(t).end();
    }

    static inline bool clean(const PROJECT_NAMESPACE::PolygonSet<CoordType> &t) { return false; }
    static inline bool sorted(const PROJECT_NAMESPACE::PolygonSet<CoordType> &t) { return false; }
  };
#if 0
  template<typename CoordType>
  struct polygon_set_mutable_traits<PROJECT_NAMESPACE::PolygonSet<CoordType>>
  {
    template<typename input_iter_type>
    static inline set(PROJECT_NAMESPACE::PolygonSet<CoordType> &polygonSet, 
        input_iter_type begin, input_iter_type end)
    {
      polygonSet.clear();
      polygonSet.insert(begin, end);
    }
  };
#endif
}} //boost:;polygon

#endif //_GEO_POLYGON_HPP_ 
