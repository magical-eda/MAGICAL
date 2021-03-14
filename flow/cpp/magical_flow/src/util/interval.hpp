/**
 * @file interval.hpp
 * @brief A interval from begin() to end(). begin() is always less than end()
 * @author Keren Zhu
 * @date 05/08/2018
 */

#ifndef _GEO_INTERVAL_H_
#define _GEO_INTERVAL_H_

#include <algorithm> /// std::max std::min
//#include <boost/functional/hash_fwd.hpp>
#include "global/namespace.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>

PROJECT_NAMESPACE_BEGIN

template <typename T> class Interval {
public:
  explicit Interval() = default;
  explicit Interval(const T &begin, const T &end) {
    _begin = std::min(begin, end);
    _end = std::max(begin, end);
  }

  // Getters
  const T &begin() const { return _begin; }
  T &begin() { return _begin; }
  const T &end() const { return _end; }
  T &end() { return _end; }
  /// @brief get the length of the interval
  /// @return the length of the interval
  T length() const { return _end - _begin; }

  // setters
  void setBegin(const T &begin) {
    _begin = std::min(begin, _end);
    _end = std::max(begin, _end);
  }
  void setEnd(const T &end) {
    _begin = std::min(_begin, end);
    _end = std::max(_begin, end);
  }

  /*------------------------------*/
  /* Useful operations            */
  /*------------------------------*/
  /// @brief determine if two interval are adjacent
  /// @param another Interval<T> want to compare
  /// @return true if two intervals are adjacent to each other, false if not
  bool adjacent(const Interval<T> &other) const;
  /// @brief operator <
  /// @param another Interval
  bool operator<(const Interval<T> &rhs) const {
    return this->begin() == rhs.begin() ? this->end() < rhs.end()
                                        : this->begin() < rhs.end();
  }
  /// @brief operator ==
  /// @pararm another Interval
  bool operator==(const Interval<T> &rhs) const {
    return this->begin() == rhs.begin() && this->end() == rhs.end();
  }

private:
  T _begin;
  T _end;
};

template <typename T>
bool Interval<T>::adjacent(const Interval<T> &other) const {

  if (this->end() == other.begin()) {
    return true;
  }
  if (this->begin() == other.end()) {
    return true;
  }
  if (this->begin() == other.begin()) {
    return true;
  }
  if (this->end() == other.end()) {
    return true;
  }
  return false;
}

PROJECT_NAMESPACE_END

/// @brief hash function for PROJECT_NAMASPACE::Interval
namespace std {
template <typename T> struct hash<PROJECT_NAMESPACE::Interval<T>> {
  typedef PROJECT_NAMESPACE::Interval<T> argumentType;
  typedef std::size_t resultType;
  resultType operator()(argumentType const &s) const noexcept {
    resultType seed = 0;
    boost::hash_combine(seed, hash<T>(s.begin()));
    boost::hash_combine(seed, hash<T>(s.end()));
    return seed;
  }
};
} // namespace std

#include <boost/polygon/interval_traits.hpp>
#include <boost/polygon/polygon.hpp>

namespace boost {
namespace polygon {
template <typename CoordType>
struct geometry_concept<PROJECT_NAMESPACE::Interval<CoordType>> {
  typedef interval_concept type;
};
// The semantic of an interval is that it has a low and high coordinate and
// there is an invariant that low is less than or equal to high.  This invariant
// is enforced by the generic library functions that operate on intervals, and
// is not expected of the data type itself or the concept mapping of that data
// type to the interval concept through its traits.  In this way a
// std::pair<int, int>, boost::tuple<int, int> or boost::array<int, 2> could all
// be made models of interval by simply providing indirect access to their
// elements through traits. In other words, the invariant checking in the
// Interval class is redundant
template <typename CoordType>
struct interval_traits<PROJECT_NAMESPACE::Interval<CoordType>> {
  typedef CoordType coordinate_type;
  static inline coordinate_type
  get(const PROJECT_NAMESPACE::Interval<CoordType> &interval,
      direction_1d dir) {
    if (dir == LOW) {
      return interval.begin();
    } else {
      return interval.end();
    }
  }
};
template <typename CoordType>
struct interval_mutable_traits<PROJECT_NAMESPACE::Interval<CoordType>> {
  typedef CoordType coordinate_type;
  static inline void set(PROJECT_NAMESPACE::Interval<CoordType> &interval,
                         direction_1d dir, CoordType value) {
    if (dir == LOW) {
      interval.setBegin(value);
    } else {
      interval.setEnd(value);
    }
  }
  static inline PROJECT_NAMESPACE::Interval<CoordType>
  construct(CoordType low_value, CoordType high_value) {
    return PROJECT_NAMESPACE::Interval<CoordType>(low_value, high_value);
  }
};
} // namespace polygon
};     // namespace boost
#endif ///_GEO_INTERVAL_H_
