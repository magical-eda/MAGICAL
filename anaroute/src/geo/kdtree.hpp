/**
 * @file   kdtree.hpp
 * @brief  Geometric Data Structure: K-Dimensional Tree
 * @author Hao Chen
 * @date   09/02/2019
 *
 **/

#ifndef _GEO_KDTREE_HPP_
#define _GEO_KDTREE_HPP_

#include "src/global/global.hpp"
#include "include/nanoflann.hpp"
#include "point.hpp"
#include "point3d.hpp"

PROJECT_NAMESPACE_START
namespace kdtree {
  template <typename T>
  class PointCloud {
  public:
    PointCloud() {}
    // nanoflann overload
    int kdtree_get_point_count() const { return _vPts.size(); }
    Point<T> point(const size_t idx) { 
      assert(idx >= 0 && idx < _vPts.size());    
      return _vPts[idx];
    }
    T kdtree_get_pt(const size_t idx, size_t dim) const {
      assert(idx >= 0 && idx < _vPts.size());
      switch (dim) {
        case 0: return _vPts[idx].x();
        case 1: return _vPts[idx].y();
      }
      __builtin_unreachable();
    }
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }

    // utils
    void clear() { _vPts.clear(); }
    void add_Point(const T x, const T y)  { _vPts.emplace_back(x, y); }
    void reserve(size_t n)                { _vPts.reserve(n);         }

  private:
    Vector_t<Point<T>>  _vPts;
  };
  
  template <typename T>
  class PointCloud3d {
  public:
    PointCloud3d() {}
    // nanoflann overload  
    int kdtree_get_point_count() const { return _vPts.size(); }
    Point3d<T> point(const size_t idx) { 
      assert(idx >= 0 && idx < _vPts.size());    
      return _vPts[idx];
    }
    T kdtree_get_pt(const size_t idx, size_t dim) const {
      assert(idx >= 0 && idx < _vPts.size());
      switch (dim) {
        case 0: return _vPts[idx].x();
        case 1: return _vPts[idx].y();
        case 2: return _vPts[idx].z();
      }
      __builtin_unreachable();
    }
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
    
    // utils
    void clear() { _vPts.clear(); }
    void add_Point(const T x, const T y, const T z)   { _vPts.emplace_back(x, y, z); }
    void reserve(size_t n)                            { _vPts.reserve(n);            }

  private:
    Vector_t<Point3d<T>>  _vPts;
  };

  // k2d types
  template<typename T>
  using k2d      = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L1_Adaptor<T, PointCloud<T> >, PointCloud<T>, 2>;
  template<typename T>
  using k2d_L2   = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<T, PointCloud<T> >, PointCloud<T>, 2>;
  // template<typename T>
  // using k2d_dynamic     = nanoflann::KDTreeSingleIndexDynamicAdaptor<nanoflann::L1_Adaptor<T, PointCloud<T> >, PointCloud<T>, 2>;
  // template<typename T>
  // using k2d_L2_dynamic  = nanoflann::KDTreeSingleIndexDynamicAdaptor<nanoflann::L2_Simple_Adaptor<T, PointCloud<T> >, PointCloud<T>, 2>;
  
  // k3d types
  template<typename T>
  using k3d      = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L1_Adaptor<T, PointCloud3d<T> >, PointCloud3d<T>, 3>;
  template<typename T>
  using k3d_L2   = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<T, PointCloud3d<T> >, PointCloud3d<T>, 3>;
  // template<typename T>
  // using k3d_dynamic     = nanoflann::KDTreeSingleIndexDynamicAdaptor<nanoflann::L1_Adaptor<T, PointCloud3d<T> >, PointCloud3d<T>, 3>;
  // template<typename T>
  // using k3d_L2_dynamic  = nanoflann::KDTreeSingleIndexDynamicAdaptor<nanoflann::L2_Simple_Adaptor<T, PointCloud3d<T> >, PointCloud3d<T>, 3>;

}

template<typename T, template<typename U> class KD = kdtree::k2d>
class K2dTree {
public:
  K2dTree() : _kdtree(2, _pc) {}
  ~K2dTree() {}

  // utils
  void clear()                       { _pc.clear();                 }
  void insert(const Point<T>& p)     { _pc.add_Point(p.x(), p.y()); }
  void insert(const T x, const T y)  { _pc.add_Point(x, y);         }
  void buildIndex()                  { _kdtree.buildIndex();        }
  
  // search
  void nearestSearch(const Point<T>& query_pt, Point<T>& ret, T& dist) {
    size_t ret_index;
    nanoflann::KNNResultSet<T> resultSet(1);
    resultSet.init(&ret_index, &dist);
    T qt[2] = {query_pt.x(), query_pt.y()};
    _kdtree.findNeighbors(resultSet, qt, nanoflann::SearchParams(10));
    ret = _pc.point(ret_index);
  }
  void knnSearch(const Point<T>& query_pt, const size_t n_search, Vector_t<Point<T> >& vRet_pts, Vector_t<T>& vDists) {
    vRet_pts.resize(n_search);
    vDists.resize(n_search, 0);
    Vector_t<size_t> vRet_indexes(n_search);
    nanoflann::KNNResultSet<T> resultSet(n_search);
    resultSet.init(&vRet_indexes[0], &vDists[0]);
    T qt[2] = {query_pt.x(), query_pt.y()};
    _kdtree.findNeighbors(resultSet, qt, nanoflann::SearchParams(10));
    for (int i = 0; i < (int)vRet_indexes.size(); ++i)
      vRet_pts[i] = _pc.point(vRet_indexes[i]);
  }

private:
  kdtree::PointCloud<T> _pc;
  KD<T> _kdtree;
};

template<typename T, template<typename U> class KD = kdtree::k3d>
class K3dTree {
public:
  K3dTree() : _kdtree(3, _pc) {}
  ~K3dTree() {}

  // utils
  void clear()                                  { _pc.clear();                        }
  void insert(const Point3d<T>& p)              { _pc.add_Point(p.x(), p.y(), p.z()); }
  void insert(const T x, const T y, const T z)  { _pc.add_Point(x, y, z);             }
  void buildIndex()                             { _kdtree.buildIndex();               }
  
  // search
  void nearestSearch(const Point3d<T>& query_pt, Point3d<T>& ret, T& dist) {
    size_t ret_index;
    nanoflann::KNNResultSet<T> resultSet(1);
    resultSet.init(&ret_index, &dist);
    T qt[3] = {query_pt.x(), query_pt.y(), query_pt.z()};
    _kdtree.findNeighbors(resultSet, qt, nanoflann::SearchParams(10));
    ret = _pc.point(ret_index);
  }
  void knnSearch(const Point3d<T>& query_pt, const size_t n_search, Vector_t<Point3d<T> >& vRet_pts, Vector_t<T>& vDists) {
    vRet_pts.resize(n_search);
    vDists.resize(n_search, 0);
    Vector_t<size_t> vRet_indexes(n_search);
    nanoflann::KNNResultSet<T> resultSet(n_search);
    resultSet.init(&vRet_indexes[0], &vDists[0]);
    T qt[3] = {query_pt.x(), query_pt.y(), query_pt.z()};
    _kdtree.findNeighbors(resultSet, qt, nanoflann::SearchParams(10));
    for (int i = 0; i < (int)vRet_indexes.size(); ++i)
      vRet_pts[i] = _pc.point(vRet_indexes[i]);
  }

private:
  kdtree::PointCloud3d<T> _pc;
  KD<T> _kdtree;
};


PROJECT_NAMESPACE_END

#endif /// _GEO_KDTREE_HPP_
