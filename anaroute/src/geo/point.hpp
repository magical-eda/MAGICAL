/**
 * @file   point.hpp
 * @brief  Geometric Data Structure: 2D point type
 * @author Hao Chen
 * @date   09/02/2019
 *
 **/

#ifndef _GEO_POINT_HPP_
#define _GEO_POINT_HPP_

#include <boost/geometry.hpp>
#include <boost/functional/hash.hpp>
#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

template<typename T>
class Point {
public:
  Point(T x = 0, T y = 0) : _d{x, y} {}
  Point(const Point& p) { memcpy(_d, p.d(), 2 * sizeof(T)); }
  ~Point() {}

  // Basic setting functions
  void  set(const T v, const int i)       { _d[i] = v;              }
  void  setX(const T x)                   { _d[0] = x;              }
  void  setY(const T y)                   { _d[1] = y;              }
  void  setXY(const T x, const T y)       { _d[0] = x; _d[1] = y;   }
  void  shift(const T v, const int i)     { _d[i] += v;             }
  void  shiftX(const T x)                 { _d[0] += x;             }
  void  shiftY(const T y)                 { _d[1] += y;             }
  void  shiftXY(const T x, const T y)     { _d[0] += x; _d[1] += y;    }
  void  scale(const T s)                  { _d[0] *= s; _d[1] *= s;    }
  void  scale(const T sx, const T sy)     { _d[0] *= sx; _d[1] *= sy;  }

  // Basic access functions
  T        x()              const { return _d[0]; }
  T        y()              const { return _d[1]; }
  T*       d()                    { return _d;    }
  T const *d()              const { return _d;    }
  T        val(const int i) const { assert(i >= 0 && i < 2); return _d[i]; }
  T&       val(const int i)       { assert(i >= 0 && i < 2); return _d[i]; }

  // util functions
  void rotate90(const T x, const T y, const bool bClockWise); // rotate 90 degree with respect to (x, y)
  void rotate180(const T x, const T y);                        // rotate 180 degree with respect to (x, y)

  // static functions
  static T    Mdistance(const Point& p1, const Point& p2);

  // operators
  friend std::ostream&  operator <<  (std::ostream& os, const Point& p)        { os << '(' << p._d[0] << ' ' << p._d[1] << ')'; return os; }
  bool                  operator ==  (const Point& p)              const       { return _d[0] == p._d[0] && _d[1] == p._d[1]; }
  bool                  operator !=  (const Point& p)              const       { return !(*this == p); }
  bool                  operator <   (const Point& p)              const       { if (_d[0] != p._d[0]) return _d[0] < p._d[0]; else return _d[1] < p._d[1]; }
  bool                  operator >   (const Point& p)              const       { if (_d[0] != p._d[0]) return _d[0] > p._d[0]; else return _d[1] > p._d[1]; }
  void                  operator =   (const Point& p)                          { setXY(p._d[0], p._d[1]); }
  Point                 operator +   (const Point& p)              const       { return Point(_d[0] + p._d[0], _d[1] + p._d[1]); }
  Point                 operator -   (const Point& p)              const       { return Point(_d[0] - p._d[0], _d[1] - p._d[1]); }
  long long             operator ^   (const Point& p)              const       { return (long long) _d[0] * p._d[1] - (long long) p._d[0] * _d[1];   }
  bool                  operator <=  (const Point& p)              const       { return _d[0] <= p._d[0] && _d[1] <= p._d[1]; }
  bool                  operator >=  (const Point& p)              const       { return _d[0] >= p._d[0] && _d[1] >= p._d[1]; }

  // comparators
  struct CompareX { bool operator() (const Point& p1, const Point& p2) { return p1._d[0] != p2._d[0] ? p1._d[0] < p2._d[0] : p1._d[1] < p2._d[1]; } };
  struct CompareY { bool operator() (const Point& p1, const Point& p2) { return p1._d[1] != p2._d[1] ? p1._d[1] < p2._d[1] : p1._d[0] < p2._d[0]; } };

  // hasher
  struct hasher {
    std::size_t operator() (const Point<T> &p) const {
      return boost::hash_range(p.d(), p.d()+2);
    }
  };

private:
  T _d[2];
};

template<class T>
void Point<T>::rotate90(const T x, const T y, const bool bClockWise) {
  T lx = _d[0] - x;
  T ly = _d[1] - y;
  if (bClockWise) {
    _d[0] = ly + x;
    _d[1] = -lx + y;
  }
  else {
    _d[0] = -ly + x;
    _d[1] = lx + y;
  }
}

template<class T>
void Point<T>::rotate180(const T x, const T y) {
  _d[0] = x + (x - _d[0]);
  _d[1] = y + (y - _d[1]);
}

template<class T>
T Point<T>::Mdistance(const Point<T>& p1, const Point<T>& p2) {
  return (p1._d[0] > p2._d[0] ? p1._d[0] - p2._d[0] : p2._d[0] - p1._d[0]) +
         (p1._d[1] > p2._d[1] ? p1._d[1] - p2._d[1] : p2._d[1] - p1._d[1]);
}


PROJECT_NAMESPACE_END

namespace boost { namespace geometry { namespace traits {
  // int32_t
  using Int_t = PROJECT_NAMESPACE::Int_t;
  template<> struct tag<PROJECT_NAMESPACE::Point<Int_t> > { typedef point_tag type; };
  template<> struct coordinate_type<PROJECT_NAMESPACE::Point<Int_t> > { typedef Int_t type; };
  template<> struct coordinate_system<PROJECT_NAMESPACE::Point<Int_t> > { typedef cs::cartesian type; };
  template<> struct dimension<PROJECT_NAMESPACE::Point<Int_t> > : boost::mpl::int_<2> {};
  template<> struct access<PROJECT_NAMESPACE::Point<Int_t>, 0> {
    static Int_t get(PROJECT_NAMESPACE::Point<Int_t>  const& p) { return p.x(); }
    static void set(PROJECT_NAMESPACE::Point<Int_t> & p, Int_t const& value) { p.setX(value); }
  };
  template<> struct access<PROJECT_NAMESPACE::Point<Int_t>, 1> {
    static int get(PROJECT_NAMESPACE::Point<Int_t> const& p) { return p.y(); }
    static void set(PROJECT_NAMESPACE::Point<Int_t> & p, Int_t const& value) { p.setY(value); }
  };
}}}

#endif /// _GEO_POINT_HPP_
