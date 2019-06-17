/**
 * @file type.h
 * @brief Define the types used in the project
 * @author Keren Zhu
 * @date 06/14/2019
 */


#ifndef MAGICAL_FLOW_TYPE_H_
#define MAGICAL_FLOW_TYPE_H_

#include <cstdint>
#include <string>
#include <sstream>
#include "namespace.h"

PROJECT_NAMESPACE_BEGIN
// Built-in type aliases
using IndexType  = std::uint32_t;
using IntType    = std::int32_t;
using RealType   = double;
using Byte       = std::uint8_t;
using LocType    = std::int32_t; // Location/design unit // Location/design unit
 // Location/design unit
// Built-in type constants
constexpr IndexType INDEX_TYPE_MAX  = UINT32_MAX;
constexpr IntType INT_TYPE_MAX      = INT32_MAX;
constexpr IntType INT_TYPE_MIN      = INT32_MIN;
constexpr RealType REAL_TYPE_MAX    = 1e100;
constexpr RealType REAL_TYPE_MIN    = -1e100;
constexpr RealType REAL_TYPE_TOL    = 1e-6;
constexpr LocType LOC_TYPE_MAX      = INT32_MAX;
constexpr LocType LOC_TYPE_MIN      = INT32_MIN;

// Type aliases


// Enums

PROJECT_NAMESPACE_END

#endif // MAGICAL_FLOW_TYPE_H_
