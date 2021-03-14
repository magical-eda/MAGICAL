// ================================================================================
// Basic and commonly used function lib
// by Keren Zhu
// Data: 04/28/2018
// ================================================================================

#pragma once

#include "Assert.hpp"
#include "MsgPrinter.hpp"
#include "global/namespace.h"
#include "global/type.h"
#include "util/point.hpp"
#include <algorithm>
#include <cctype>
#include <cmath> // ceilf(), floorf()
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

namespace klib {
using Uint = PROJECT_NAMESPACE::IndexType;
using Int = PROJECT_NAMESPACE::IntType;
using Real = PROJECT_NAMESPACE::RealType;
using MsgPrinter = PROJECT_NAMESPACE::MsgPrinter;

// Below is from Google Test
//
// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Authors: wan@google.com (Zhanyong Wan), eefacm@gmail.com (Sean Mcafee)
//
// The Google C++ Testing Framework (Google Test)
//
// This header file declares functions and macros used internally by
// Google Test.  They are subject to change without notice.

// This template class serves as a compile-time function from size to
// type.  It maps a size in bytes to a primitive type with that
// size. e.g.
//
//   TypeWithSize<4>::UInt
//
// is typedef-ed to be unsigned int (unsigned integer made up of 4
// bytes).
//
// Such functionality should belong to STL, but I cannot find it
// there.
//
// Google Test uses this class in the implementation of floating-point
// comparison.
//
// For now it only handles UInt (unsigned int) as that's all Google Test
// needs.  Other types can be easily added in the future if need
// arises.
template <std::size_t size> class TypeWithSize {
public:
  // This prevents the user from using TypeWithSize<N> with incorrect
  // values of N.
  typedef void UInt;
};

// The specialization for size 4.
template <> class TypeWithSize<4> {
public:
  // unsigned int has size 4 in both gcc and MSVC.
  //
  // As base/basictypes.h doesn't compile on Windows, we cannot use
  // uint32, uint64, and etc here.
  typedef int Int;
  typedef unsigned int UInt;
};

// The specialization for size 8.
template <> class TypeWithSize<8> {
public:
#if _MSC_VER
  typedef __int64 Int;
  typedef unsigned __int64 UInt;
#else
  typedef long long Int;           // NOLINT
  typedef unsigned long long UInt; // NOLINT
#endif // _MSC_VER
};

// This template class represents an IEEE floating-point number
// (either single-precision or double-precision, depending on the
// template parameters).
//
// The purpose of this class is to do more sophisticated number
// comparison.  (Due to round-off error, etc, it's very unlikely that
// two floating-points will be equal exactly.  Hence a naive
// comparison by the == operation often doesn't work.)
//
// Format of IEEE floating-point:
//
//   The most-significant bit being the leftmost, an IEEE
//   floating-point looks like
//
//     sign_bit exponent_bits fraction_bits
//
//   Here, sign_bit is a single bit that designates the sign of the
//   number.
//
//   For float, there are 8 exponent bits and 23 fraction bits.
//
//   For double, there are 11 exponent bits and 52 fraction bits.
//
//   More details can be found at
//   http://en.wikipedia.org/wiki/IEEE_floating-point_standard.
//
// Template parameter:
//
//   RawType: the raw floating-point type (either float or double)
template <typename RawType> class FloatingPoint {
public:
  // Defines the unsigned integer type that has the same size as the
  // floating point number.
  typedef typename TypeWithSize<sizeof(RawType)>::UInt Bits;

  // Constants.

  // # of bits in a number.
  static const std::size_t kBitCount = 8 * sizeof(RawType);

  // # of fraction bits in a number.
  static const std::size_t kFractionBitCount =
      std::numeric_limits<RawType>::digits - 1;

  // # of exponent bits in a number.
  static const std::size_t kExponentBitCount =
      kBitCount - 1 - kFractionBitCount;

  // The mask for the sign bit.
  static const Bits kSignBitMask = static_cast<Bits>(1) << (kBitCount - 1);

  // The mask for the fraction bits.
  static const Bits kFractionBitMask = ~static_cast<Bits>(0) >>
                                       (kExponentBitCount + 1);

  // The mask for the exponent bits.
  static const Bits kExponentBitMask = ~(kSignBitMask | kFractionBitMask);

  // How many ULP's (Units in the Last Place) we want to tolerate when
  // comparing two numbers.  The larger the value, the more error we
  // allow.  A 0 value means that two numbers must be exactly the same
  // to be considered equal.
  //
  // The maximum error of a single floating-point operation is 0.5
  // units in the last place.  On Intel CPU's, all floating-point
  // calculations are done with 80-bit precision, while double has 64
  // bits.  Therefore, 4 should be enough for ordinary use.
  //
  // See the following article for more details on ULP:
  // http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
  static const std::size_t kMaxUlps = 4;

  // Constructs a FloatingPoint from a raw floating-point number.
  //
  // On an Intel CPU, passing a non-normalized NAN (Not a Number)
  // around may change its bits, although the new value is guaranteed
  // to be also a NAN.  Therefore, don't expect this constructor to
  // preserve the bits in x when x is a NAN.
  explicit FloatingPoint(const RawType &x) { u_.value_ = x; }

  // Static methods

  // Reinterprets a bit pattern as a floating-point number.
  //
  // This function is needed to test the AlmostEquals() method.
  static RawType ReinterpretBits(const Bits bits) {
    FloatingPoint fp(0);
    fp.u_.bits_ = bits;
    return fp.u_.value_;
  }

  // Returns the floating-point number that represent positive infinity.
  static RawType Infinity() { return ReinterpretBits(kExponentBitMask); }

  // Returns the maximum representable finite floating-point number.
  static RawType Max();

  // Non-static methods

  // Returns the bits that represents this number.
  const Bits &bits() const { return u_.bits_; }

  // Returns the exponent bits of this number.
  Bits exponent_bits() const { return kExponentBitMask & u_.bits_; }

  // Returns the fraction bits of this number.
  Bits fraction_bits() const { return kFractionBitMask & u_.bits_; }

  // Returns the sign bit of this number.
  Bits sign_bit() const { return kSignBitMask & u_.bits_; }

  // Returns true iff this is NAN (not a number).
  bool is_nan() const {
    // It's a NAN if the exponent bits are all ones and the fraction
    // bits are not entirely zeros.
    return (exponent_bits() == kExponentBitMask) && (fraction_bits() != 0);
  }

  // Returns true iff this number is at most kMaxUlps ULP's away from
  // rhs.  In particular, this function:
  //
  //   - returns false if either number is (or both are) NAN.
  //   - treats really large numbers as almost equal to infinity.
  //   - thinks +0.0 and -0.0 are 0 DLP's apart.
  bool AlmostEquals(const FloatingPoint &rhs) const {
    // The IEEE standard says that any comparison operation involving
    // a NAN must return false.
    if (is_nan() || rhs.is_nan())
      return false;

    return DistanceBetweenSignAndMagnitudeNumbers(u_.bits_, rhs.u_.bits_) <=
           kMaxUlps;
  }

private:
  // The data type used to store the actual floating-point number.
  union FloatingPointUnion {
    RawType value_; // The raw floating-point number.
    Bits bits_;     // The bits that represent the number.
  };

  // Converts an integer from the sign-and-magnitude representation to
  // the biased representation.  More precisely, let N be 2 to the
  // power of (kBitCount - 1), an integer x is represented by the
  // unsigned number x + N.
  //
  // For instance,
  //
  //   -N + 1 (the most negative number representable using
  //          sign-and-magnitude) is represented by 1;
  //   0      is represented by N; and
  //   N - 1  (the biggest number representable using
  //          sign-and-magnitude) is represented by 2N - 1.
  //
  // Read http://en.wikipedia.org/wiki/Signed_number_representations
  // for more details on signed number representations.
  static Bits SignAndMagnitudeToBiased(const Bits &sam) {
    if (kSignBitMask & sam) {
      // sam represents a negative number.
      return ~sam + 1;
    } else {
      // sam represents a positive number.
      return kSignBitMask | sam;
    }
  }

  // Given two numbers in the sign-and-magnitude representation,
  // returns the distance between them as an unsigned number.
  static Bits DistanceBetweenSignAndMagnitudeNumbers(const Bits &sam1,
                                                     const Bits &sam2) {
    const Bits biased1 = SignAndMagnitudeToBiased(sam1);
    const Bits biased2 = SignAndMagnitudeToBiased(sam2);
    return (biased1 >= biased2) ? (biased1 - biased2) : (biased2 - biased1);
  }

  FloatingPointUnion u_;
};

// We cannot use std::numeric_limits<T>::max() as it clashes with the max()
// macro defined by <windows.h>.
template <> inline float FloatingPoint<float>::Max() { return FLT_MAX; }
template <> inline double FloatingPoint<double>::Max() { return DBL_MAX; }

template <typename T> bool AlmostEquals(T first, T second) {
  FloatingPoint<T> firstAsFloatingPoint(first);
  FloatingPoint<T> secondAsFloatingPoint(second);

  return firstAsFloatingPoint.AlmostEquals(secondAsFloatingPoint);
}

// ================================================================================
// STL extension
// ================================================================================
/// @breif Erase an element in the vector by swapping with the last element and
/// resize
/// @brief Ordering is not kept
template <typename T>
inline bool eraseElementInVec(std::vector<T> &vec, Uint idx) {
  Uint vectorSize = vec.size();
  if (idx >= vectorSize) {
    MsgPrinter::err("eraseElementInVec error: idx out of bound \n");
    Assert(false);
    return false;
  }
  // idx < vectorSize
  vec[idx] = std::move(vec.back());
  vec.resize(vectorSize - 1);
  return true;
}

/// @breif Erase an element in the vector by swapping with the last element and
/// resize
/// @brief Ordering is not kept
template <typename T>
inline bool eraseElementInVec(std::vector<T> &vec, Int idx) {
  if (idx < 0) {
    MsgPrinter::err("eraseElementInVec error: idx out of bound \n");
    Assert(false);
    return false;
  }
  return eraseElementInVec(vec, static_cast<Uint>(idx));
}

/// @brief Swap t1 and t2 if t1 > t2. Ie. put t1 and t2 in order
template <typename T> inline void putTwoInOrder(T &t1, T &t2) {
  if (t1 > t2) {
    std::swap(t1, t2);
  }
}

template <typename T>
inline void putTwoInOrder(T &t1, T &t2,
                          std::function<bool(T &, T &)> const compare) {
  if (compare(t1, t2)) {
    std::swap(t1, t2);
  }
}

/// Calculate two usigned int difference
inline Uint idxDif(Uint i1, Uint i2) { return i1 > i2 ? i1 - i2 : i2 - i1; }

template <typename T> inline T absDif(T t1, T t2) {
  return t1 > t2 ? t1 - t2 : t2 - t1;
}

inline bool idxAddOverflow(Uint idx1, Uint idx2) {
  Uint sum = idx1 + idx2;
  return sum < idx1 || sum < idx2;
}

/// @brief move v1 elements into v2, and erase v1
/// @param two reference to two vectors containing the same type of elements
template <typename T>
inline void moveVector(std::vector<T> &vFrom, std::vector<T> &vTo) {
  vTo.insert(vTo.end(), std::make_move_iterator(vFrom.begin()),
             std::make_move_iterator(vFrom.end()));
  vFrom.clear();
}

/// @brief Stuct for null deleter
namespace createSharedPtrDetails {
struct null_deleter {
  void operator()(void const *) const {}
};
} // namespace createSharedPtrDetails

/// @brief create a shared_ptr to a existing target
template <typename T> inline std::shared_ptr<T> createSharedPtr(T &t) {
  return std::shared_ptr<T>(&t, createSharedPtrDetails::null_deleter());
}

// ================================================================================
// Numerical functions
// ================================================================================

/// @breif Automatically decide whether to round a float/double. depending on
/// the returning type.
template <typename T> inline T autoRound(Real rValue) {
  return static_cast<T>(std::round(rValue));
}

template <> inline float autoRound(Real rValue) { return rValue; }

template <> inline double autoRound(Real rValue) { return rValue; }

template <typename T>
inline T manhattanDistance(const PROJECT_NAMESPACE::Point<T> &t1,
                           const PROJECT_NAMESPACE::Point<T> &t2) {
  return absDif(t1.x(), t2.x()) + absDif(t1.y(), t2.y());
}
// ================================================================================
// Parser string
// ================================================================================
/// @brief trim left space of a string
inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(),
          std::find_if(s.begin(), s.end(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

/// @brief trim right space of a string
inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       std::not1(std::ptr_fun<int, int>(std::isspace)))
              .base(),
          s.end());
  return s;
}

/// @brief trim spaces from both ends
inline std::string &trim(std::string &s) { return ltrim(rtrim(s)); }

/// @brief given path in str_path, output the design name
/// @brief for example: with input "path1/path2/sparc_exu_alu", output
/// "sparc_exu_alu"
inline std::string parsePath2Name(std::string str_path) {
  size_t pos = str_path.rfind('/');
  if (pos == std::string::npos)
    return str_path;

  return str_path.substr(pos + 1);
}

/// @brief given path in str_path, output the design name
/// @brief for example: with input "path1/path2/sparc_exu_alu", output
/// "path1/path2"
inline std::string parseFile2Path(std::string str_path) {
  size_t pos = str_path.rfind('/');
  if (pos == std::string::npos)
    return str_path;

  return str_path.substr(0, pos);
}

/// @brief parse filename suffix
inline std::string parseSuffix(std::string str) {
  size_t pos = str.rfind('.');
  if (pos == std::string::npos)
    return str;
  return str.substr(pos + 1);
}

/// @brief trim filename suffix
inline std::string trimSuffix(std::string str) {
  size_t pos = str.rfind('.');
  return str.substr(0, pos);
}

// ================================================================================
// File-reading functions
// ================================================================================

/// @brief Check whether the file exists
inline bool isFileExist(const char *fileName) {
  std::ifstream infile(fileName);
  return infile.good();
}

/// @brief read from ifstream in, search target1 & target2,
/// @brief if found, return true, and the whole line is stored in "str_line"
/// @brief if reach EOF, return false
inline bool readSearchUntil(std::ifstream &in, std::string &str_line,
                            const std::string &target1,
                            const std::string &target2 = "") {
  bool skipTarget2 = target2.length() <= 0;
  while (!in.eof()) {
    getline(in, str_line);
    if (in.eof())
      return false;
    if (std::string::npos != str_line.find(target1))
      return true;
    if (skipTarget2)
      continue;
    if (std::string::npos != str_line.find(target2))
      return true;
  } // while
  return false;
}

} // namespace klib
