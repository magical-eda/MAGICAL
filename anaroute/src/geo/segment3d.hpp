/**
 * @file   segment3d.hpp
 * @brief  Geometric Data Structure: 3D segment type
 * @author Hao Chen
 * @date   09/05/2019
 *
 **/

#ifndef _GEO_SEGMENT3D_HPP_
#define _GEO_SEGMENT3D_HPP_

#include "src/global/global.hpp"
#include "point3d.hpp"

PROJECT_NAMESPACE_START

template<class T>
class Segment3d {
public:
  Segment3d(T x1 = 0, T y1 = 0, T z1 = 0, T x2 = 0, T y2 = 0, T z2 = 0)
    : _p1(std::min(x1, x2), std::min(y1, y2), std::min(z1, z2)), _p2(std::max(x1, x2), std::max(y1, y2), std::max(z1, z2)) {}
  Segment3d(const Point3d<T>& p1, const Point3d<T>& p2)
    : _p1{std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()), std::min(p1.z(), p2.z())},
      _p2{std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()), std::max(p1.z(), p2.z())} {}
  ~Segment3d() {}

  // Basic setting functions

  // Basic access functions
  Point3d<T>&       p1()             { return _p1; }
  Point3d<T>&       p2()             { return _p2; }
  const Point3d<T>& p1()       const { return _p1; }
  const Point3d<T>& p2()       const { return _p2; }
  T                 xl()       const { return _p1.x(); }
  T                 xh()       const { return _p2.x(); }
  T                 yl()       const { return _p1.y(); }
  T                 yh()       const { return _p2.y(); }
  T                 zl()       const { return _p1.z(); }
  T                 zh()       const { return _p2.z(); }
  T                 length()   const { return Point3d<T>::Mdistance(_p1, _p2); }

  // operators
  friend std::ostream&  operator <<  (std::ostream& os, const Segment3d& s)       { os << s._p1 << ' ' << s._p2; return os; }
  bool                  operator ==  (const Segment3d& s)                   const { return _p1 == s._p1 && _p2 == s._p2; }
  bool                  operator !=  (const Segment3d& s)                   const { return !(*this == s); }
  bool                  operator <   (const Segment3d& s)                   const { if (_p1 != s._p1) return _p1 < s._p1; else return _p2 < s._p2; }
  void                  operator =   (const Segment3d& s)                         { _p1 = s._p1; _p2 = s._p2; }
private:
  Point3d<T> _p1, _p2;
};

PROJECT_NAMESPACE_END

#endif /// _GEO_SEGMENT3D_HPP_
