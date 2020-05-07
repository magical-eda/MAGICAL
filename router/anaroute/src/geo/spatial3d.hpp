/**
 * @file   spatial3d.hpp
 * @brief  Geometric Data Structure: 3d spatial type (R-Tree Kernel)
 * @author Hao Chen
 * @date   09/06/2019
 *
 **/

#ifndef _GEO_SPATIAL3d_HPP_
#define _GEO_SPATIAL3d_HPP_

#include <vector>
#include <boost/geometry.hpp>
#include <boost/function_output_iterator.hpp>

#include "src/global/global.hpp"
#include "point3d.hpp"

PROJECT_NAMESPACE_START

namespace spatial3d {
  namespace bg = boost::geometry;
  namespace bgi = boost::geometry::index;

  template<typename T>
  using b_box = bg::model::box<Point3d<T> >;

  template<typename T, typename Value>
  using b_value = Pair_t<b_box<T>, Value>;

  template<typename T>
  struct B_box_equal {
    bool operator() (const b_box<T>& b0, const b_box<T>& b1) const {
      return b0.min_corner() == b1.min_corner() &&
             b0.max_corner() == b1.max_corner();
    }
  };

  template<typename T, typename Value>
  struct B_value_equal {
    bool operator() (const b_value<T, Value>& b0, const b_value<T, Value>& b1) const {
      return b0.first.min_corner() == b1.first.min_corner() &&
             b0.first.max_corner() == b1.first.max_corner() &&
             b0.second == b1.second;
    }
  };

  template<typename T>
  using Rtree = bgi::rtree<b_box<T>, bgi::rstar<16>, bgi::indexable<b_box<T> >, B_box_equal<T> >;

  template<typename T, typename Value>
  using RtreeMap = bgi::rtree<b_value<T, Value>, bgi::rstar<16>, bgi::indexable<b_value<T, Value> >, B_value_equal<T, Value> >;

  template<typename T>
  struct SearchCallback { // return b_box or b_value
    SearchCallback(Vector_t<T>& ret)
      : _ret(ret) {}
    void operator () (const T& t) { _ret.push_back(t); }
    Vector_t<T>& _ret;
  };
  
  template<typename BBox>
  struct SearchCallback_first { // return b_value.first
    SearchCallback_first(Vector_t<BBox>& ret)
      : _ret(ret) {}
    template<typename BValue>
    void operator () (const BValue& bv) { _ret.push_back(bv.first); }
    Vector_t<BBox>& _ret;
  };
  
  template<typename Value>
  struct SearchCallback_second { // return b_value.second
    SearchCallback_second(Vector_t<Value>& ret)
      : _ret(ret) {}
    template<typename BValue>
    void operator () (const BValue& bv) { _ret.push_back(bv.second); }
    Vector_t<Value>& _ret;
  };

  // Query options
  enum class QueryType {
    contains,
    covered_by,
    covers,
    disjoint,
    intersects,
    overlaps,
    within
  };
}

template<typename T>
class Spatial3d {
  spatial3d::Rtree<T> _rtree;
  using const_iterator = typename spatial3d::Rtree<T>::const_iterator;

public:
  // constructors
  Spatial3d() {}
  Spatial3d(const spatial3d::Rtree<T>& t) : _rtree(t) {}
  Spatial3d(const Spatial3d& sp) : _rtree(sp._rtree) {}
  template<typename Container>
  Spatial3d(const Container &container) : _rtree(container) {} // use packing algorithm
  template<typename Container_Iterator>
  Spatial3d(Container_Iterator begin, Container_Iterator end) : _rtree(begin, end) {} // use packing algorithm
  ~Spatial3d() {}

  // iterator
  inline const_iterator begin() const { return _rtree.begin(); }
  inline const_iterator end()   const { return _rtree.end();   }
  
  // operators
  void operator = (const Spatial3d& sp) { _rtree = sp._rtree; }
  
  // get
  bool    empty() const { return _rtree.empty(); }
  size_t  size()  const { return _rtree.size();  }
  
  // set
  void    clear()                                                               { _rtree.clear(); }
  
  void    insert(const spatial3d::b_box<T>& box)                                { _rtree.insert(box); }
  template<typename Container>
  void    insert(const Container &container)                                    { _rtree.insert(container); } // use packing algorithm
  template<typename Container_Iterator>
  void    insert(Container_Iterator begin, Container_Iterator end)              { _rtree.insert(begin, end); } // use packing algorithm
  void    insert(const Point3d<T>& min_corner, const Point3d<T>& max_corner)  { _rtree.insert({min_corner, max_corner}); }
  
  bool    erase(const spatial3d::b_box<T>& box)                                 { return _rtree.remove(box); }
  template<typename Container>
  size_t  erase(const Container &container)                                     { return _rtree.remove(container); }
  template<typename Container_Iterator>
  size_t  erase(Container_Iterator begin, Container_Iterator end)               { return _rtree.remove(begin, end); }
  bool    erase(const Point3d<T>& min_corner, const Point3d<T>& max_corner)   { return _rtree.remove({min_corner, max_corner}); }
  
  // query
  void    query(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<spatial3d::b_box<T> >& ret, spatial::QueryType qt = spatial::QueryType::intersects) const;

};

template<typename T, typename Value>
class SpatialMap3d {

  spatial3d::RtreeMap<T, Value> _rtreeMap;
  using const_iterator = typename spatial3d::RtreeMap<T, Value>::const_iterator;

public:
  // constructors
  SpatialMap3d() {}
  SpatialMap3d(const spatial3d::RtreeMap<T, Value>& t) : _rtreeMap(t) {}
  SpatialMap3d(const SpatialMap3d& sp) : _rtreeMap(sp._rtreeMap) {}
  template<typename Container>
  SpatialMap3d(const Container &container) : _rtreeMap(container) {} // use packing algorithm
  template<typename Container_Iterator>
  SpatialMap3d(Container_Iterator begin, Container_Iterator end) : _rtreeMap(begin, end) {} // use packing algorithm
  ~SpatialMap3d() {}

  // iterator
  inline const_iterator begin() const { return _rtreeMap.begin(); }
  inline const_iterator end()   const { return _rtreeMap.end();   }
  
  // operators
  void operator = (const SpatialMap3d& sp) { _rtreeMap = sp._rtreeMap; }
  
  // get
  bool    empty() const { return _rtreeMap.empty(); }
  size_t  size()  const { return _rtreeMap.size();  }
  
  // set
  void    clear()                                                                                 { _rtreeMap.clear(); }
  
  void    insert(const spatial3d::b_value<T, Value>& bval)                                        { _rtreeMap.insert(bval); }
  template<typename Container>
  void    insert(const Container &container)                                                      { _rtreeMap.insert(container); } // use packing algorithm
  template<typename Container_Iterator>
  void    insert(Container_Iterator begin, Container_Iterator end)                                { _rtreeMap.insert(begin, end); } // use packing algorithm
  void    insert(const Point3d<T>& min_corner, const Point3d<T>& max_corner, const Value& val)  { _rtreeMap.insert({{min_corner, max_corner}, val}); }
  
  bool    erase(const spatial3d::b_value<T, Value>& bval)                                         { return _rtreeMap.remove(bval); }
  template<typename Container>
  size_t  erase(const Container &container)                                                       { return _rtreeMap.remove(container); }
  template<typename Container_Iterator>
  size_t  erase(Container_Iterator begin, Container_Iterator end)                                 { return _rtreeMap.remove(begin, end); }
  bool    erase(const Point3d<T>& min_corner, const Point3d<T>& max_corner, const Value& val)   { return _rtreeMap.remove({{min_corner, max_corner}, val}); }
  
  // query
  void    query(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<Value>& ret, spatial::QueryType qt = spatial::QueryType::intersects) const;
  void    queryBox(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<spatial3d::b_box<T> >& ret, spatial::QueryType qt = spatial::QueryType::intersects) const;
  void    queryBoth(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<spatial3d::b_value<T, Value> >& ret, spatial::QueryType qt = spatial::QueryType::intersects) const;
};

////////// Spatial3d Implementation /////////////
template<typename T>
void Spatial3d<T>::query(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<spatial3d::b_box<T> >& ret, spatial::QueryType qt) const {
  spatial3d::SearchCallback<spatial3d::b_box<T> > callback(ret);
  spatial3d::b_box<T> query_box(min_corner, max_corner);
  switch (qt) {
    case spatial::QueryType::contains :
      _rtree.query(spatial3d::bgi::contains(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covered_by :
      _rtree.query(spatial3d::bgi::covered_by(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covers :
      _rtree.query(spatial3d::bgi::covers(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::disjoint :
      _rtree.query(spatial3d::bgi::disjoint(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::intersects :
      _rtree.query(spatial3d::bgi::intersects(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::overlaps :
      _rtree.query(spatial3d::bgi::overlaps(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::within :
      _rtree.query(spatial3d::bgi::within(query_box), boost::make_function_output_iterator(callback)); break;
    default:
      assert(false);
  }
}

////////// SpatialMap3d Implementation /////////////
template<typename T, typename Value>
void SpatialMap3d<T, Value>::query(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<Value>& ret, spatial::QueryType qt) const {
  spatial3d::SearchCallback_second<Value> callback(ret);
  spatial3d::b_box<T> query_box(min_corner, max_corner);
  switch (qt) {
    case spatial::QueryType::contains :
      _rtreeMap.query(spatial3d::bgi::contains(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covered_by :
      _rtreeMap.query(spatial3d::bgi::covered_by(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covers :
      _rtreeMap.query(spatial3d::bgi::covers(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::disjoint :
      _rtreeMap.query(spatial3d::bgi::disjoint(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::intersects :
      _rtreeMap.query(spatial3d::bgi::intersects(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::overlaps :
      _rtreeMap.query(spatial3d::bgi::overlaps(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::within :
      _rtreeMap.query(spatial3d::bgi::within(query_box), boost::make_function_output_iterator(callback)); break;
    default:
      assert(false);
  }
}

template<typename T, typename Value>
void SpatialMap3d<T, Value>::queryBox(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<spatial3d::b_box<T> >& ret, spatial::QueryType qt) const {
  spatial3d::SearchCallback_first<spatial3d::b_box<T> > callback(ret);
  spatial3d::b_box<T> query_box(min_corner, max_corner);
  switch (qt) {
    case spatial::QueryType::contains :
      _rtreeMap.query(spatial3d::bgi::contains(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covered_by :
      _rtreeMap.query(spatial3d::bgi::covered_by(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covers :
      _rtreeMap.query(spatial3d::bgi::covers(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::disjoint :
      _rtreeMap.query(spatial3d::bgi::disjoint(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::intersects :
      _rtreeMap.query(spatial3d::bgi::intersects(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::overlaps :
      _rtreeMap.query(spatial3d::bgi::overlaps(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::within :
      _rtreeMap.query(spatial3d::bgi::within(query_box), boost::make_function_output_iterator(callback)); break;
    default:
      assert(false);
  }
}

template<typename T, typename Value>
void SpatialMap3d<T, Value>::queryBoth(const Point3d<T>& min_corner, const Point3d<T>& max_corner, Vector_t<spatial3d::b_value<T, Value> >& ret, spatial::QueryType qt) const {
  spatial3d::SearchCallback<spatial3d::b_value<T, Value> > callback(ret);
  spatial3d::b_box<T> query_box(min_corner, max_corner);
  switch (qt) {
    case spatial::QueryType::contains :
      _rtreeMap.query(spatial3d::bgi::contains(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covered_by :
      _rtreeMap.query(spatial3d::bgi::covered_by(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::covers :
      _rtreeMap.query(spatial3d::bgi::covers(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::disjoint :
      _rtreeMap.query(spatial3d::bgi::disjoint(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::intersects :
      _rtreeMap.query(spatial3d::bgi::intersects(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::overlaps :
      _rtreeMap.query(spatial3d::bgi::overlaps(query_box), boost::make_function_output_iterator(callback)); break;
    case spatial::QueryType::within :
      _rtreeMap.query(spatial3d::bgi::within(query_box), boost::make_function_output_iterator(callback)); break;
    default:
      assert(false);
  }
}


PROJECT_NAMESPACE_END

#endif /// _GEO_SPATIAL3d_HPP_
