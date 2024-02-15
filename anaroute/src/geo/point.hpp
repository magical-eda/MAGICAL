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
  void flipX(const T x);
  void flipY(const T y);

  // static functions
  static T      Mdistance(const Point& p1, const Point& p2);
  static bool   bOnSegment(const Point& p, const Point& q, const Point& r); // checks if point q lies on line segment 'pr' 
  static Int_t  orientation(const Point& p, const Point& q, const Point& r); // 0: p,q,r colinear; 1: clockwise; -1:counterclockwise

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

template<typename T>
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

template<typename T>
void Point<T>::rotate180(const T x, const T y) {
  _d[0] = x + (x - _d[0]);
  _d[1] = y + (y - _d[1]);
}

template<typename T>
void Point<T>::flipX(const T x) {
  _d[0] = 2 * x - _d[0];
}

template<typename T>
void Point<T>::flipY(const T y) {
  _d[1] = 2* y - _d[1];
}

template<typename T>
T Point<T>::Mdistance(const Point<T>& p1, const Point<T>& p2) {
  return (p1._d[0] > p2._d[0] ? p1._d[0] - p2._d[0] : p2._d[0] - p1._d[0]) +
         (p1._d[1] > p2._d[1] ? p1._d[1] - p2._d[1] : p2._d[1] - p1._d[1]);
}

template<typename T>
Int_t Point<T>::orientation(const Point& p, const Point& q, const Point& r) {
	T val = (q._d[1] - p._d[1]) * (r._d[0] - q._d[0]) - (q._d[0] - p._d[0]) * (r._d[1] - q._d[1]); 
	if (val == 0)
    return 0;  // colinear 
	return (val > 0)? 1: -1; // clock or counterclock wise 
}

template<typename T>
bool Point<T>::bOnSegment(const Point& p, const Point& q, const Point& r) {
  if (q._d[0] <= std::max(p._d[0], r._d[0])
      and q._d[0] >= std::min(p._d[0], r._d[0])
      and q._d[1] <= std::max(p._d[1], r._d[1])
      and q._d[1] >= std::min(p._d[1], r._d[1])) 
    return true; 

  return false;
}


PROJECT_NAMESPACE_END

#if 0
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
#endif

// boost polygon
#include <boost/polygon/point_traits.hpp>
#include <boost/polygon/interval_traits.hpp>
#include <boost/polygon/polygon.hpp>
namespace boost { namespace polygon {
    template<typename CoordType>
    struct geometry_concept<PROJECT_NAMESPACE::Point<CoordType>> { typedef point_concept type;};
    // Point Concept
    template<typename CoordType>
    struct point_traits<PROJECT_NAMESPACE::Point<CoordType>>
    {
        typedef CoordType coordinate_type;
        static inline coordinate_type get(const PROJECT_NAMESPACE::Point<CoordType> &point, orientation_2d orient)
        {
            if (orient== HORIZONTAL)
            {
                return point.x();
            }
            else
            {
                return point.y();
            }
        }
    };
    template<typename CoordType>
    struct point_mutable_traits<PROJECT_NAMESPACE::Point<CoordType>>
    {
        typedef CoordType coordinate_type;
        static void inline set(PROJECT_NAMESPACE::Point<CoordType> &point, orientation_2d orient, CoordType value)
        {
            if (orient == HORIZONTAL)
            {
                point.setX(value);
            }
            else
            {
                point.setY(value);
            }
        }
        static inline PROJECT_NAMESPACE::Point<CoordType> construct(CoordType x_value, CoordType y_value)
        {
            return PROJECT_NAMESPACE::Point<CoordType>(x_value, y_value);
        }
    };
    // Interval Concept
    // The semantic of an interval is that it has a low and high coordinate and there is an invariant that low is less than or equal to high.  This invariant is enforced by the generic library functions that operate on intervals, and is not expected of the data type itself or the concept mapping of that data type to the interval concept through its traits.  In this way a std::pair<int, int>, boost::tuple<int, int> or boost::array<int, 2> could all be made models of interval by simply providing indirect access to their elements through traits.
    // In other words, the invariant checking in the Interval class is redundant
    template<typename CoordType>
    struct interval_traits<PROJECT_NAMESPACE::Point<CoordType>>
    {
        typedef CoordType coordinate_type;
        static inline coordinate_type get(const PROJECT_NAMESPACE::Point<CoordType> &interval, direction_1d dir)
        {
            if (dir == LOW)
            {
                return interval.x();
            }
            else
            {
                return interval.y();
            }
        }
    };
    template<typename CoordType>
    struct interval_mutable_traits<PROJECT_NAMESPACE::Point<CoordType>>
    {
        typedef CoordType coordinate_type;
        static inline void set(PROJECT_NAMESPACE::Point<CoordType> &interval, direction_1d dir, CoordType value)
        {
            if (dir == LOW)
            {
                interval.setX(value);
            }
            else
            {
                interval.setY(value);
            }
        }
        static inline PROJECT_NAMESPACE::Point<CoordType> construct(CoordType low_value, CoordType high_value)
        {
            return PROJECT_NAMESPACE::Point<CoordType>(low_value, high_value);
        }
    };
}};

// Boost geometry traits
#include <boost/geometry/geometries/concepts/point_concept.hpp>
namespace boost { namespace geometry { namespace traits {
  
  template<typename CoordType>
  struct tag<PROJECT_NAMESPACE::Point<CoordType>>
  {
    typedef point_tag type;
  };

  template<typename CoordType>
  struct coordinate_type<PROJECT_NAMESPACE::Point<CoordType>>
  {
    typedef CoordType type;
  };

  template<typename CoordType>
  struct coordinate_system<PROJECT_NAMESPACE::Point<CoordType>>
  {
    typedef boost::geometry::cs::cartesian type;
  };

  template<typename CoordType>
  struct dimension<PROJECT_NAMESPACE::Point<CoordType>>
  : boost::mpl::int_<2>
  {};

  template<typename CoordType>
  struct access<PROJECT_NAMESPACE::Point<CoordType>, 0>
  {
    static inline CoordType get (const PROJECT_NAMESPACE::Point<CoordType> &p)
    {
      return p.x();
    }
    
    static inline void set(PROJECT_NAMESPACE::Point<CoordType> &p, CoordType const & value)
    {
      p.setX(value);
    }
  };

  template<typename CoordType>
  struct access<PROJECT_NAMESPACE::Point<CoordType>, 1>
  {
    static inline CoordType get (const PROJECT_NAMESPACE::Point<CoordType> &p)
    {
      return p.y();
    }
    
    static inline void set(PROJECT_NAMESPACE::Point<CoordType> &p, CoordType const & value)
    {
      p.setY(value);
    }
  };

}}}

#endif /// _GEO_POINT_HPP_
