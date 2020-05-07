/**
 * @file   type.hpp
 * @brief  Types used across the project
 * @author Hao Chen
 * @date   09/01/2019
 *
 **/

#ifndef ANAROUTE_TYPE_HPP_
#define ANAROUTE_TYPE_HPP_

#include <cstdint>
#include <limits>
#include <climits>
#include <string>
#include "namespace.hpp"

PROJECT_NAMESPACE_START

using Int_t     = std::int32_t;
using UInt_t    = std::uint32_t;
using Float_t   = double;
using Byte_t    = std::uint8_t;
using String_t  = std::string;
template<typename T, typename U>
using Pair_t    = std::pair<T, U>;
template<typename T>
using Queue_t   = std::queue<T>;
template<typename T>
using Vector_t  = std::vector<T>;
template<typename T>
using List_t    = std::list<T>;
template<typename T>
using Set_t     = std::set<T>;
template<typename T, typename U>
using Map_t     = std::map<T, U>;
template<typename T>
using USet_t    = std::unordered_set<T>;
template<typename T, typename U>
using UMap_t    = std::unordered_map<T, U>;

constexpr Int_t   MAX_INT   = std::numeric_limits<Int_t>::max()       / 3;
constexpr UInt_t  MAX_UINT  = std::numeric_limits<UInt_t>::max()      / 3;
constexpr Float_t MAX_FLOAT = std::numeric_limits<Float_t>::max()     / 3;
constexpr Int_t   MIN_INT   = std::numeric_limits<Int_t>::lowest()    / 3;
constexpr UInt_t  MIN_UINT  = std::numeric_limits<UInt_t>::lowest()   / 3;
constexpr Float_t MIN_FLOAT = std::numeric_limits<Float_t>::lowest()  / 3;

constexpr Float_t EPSILON   = 1e-8;

constexpr Float_t TIME_SCALE = 1000000.00;
constexpr Float_t MEM_SCALE  = 1024.0;


PROJECT_NAMESPACE_END

#endif /// ANAROUTE_TYPE_HPP_
