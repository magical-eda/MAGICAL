#ifndef __BASICTYPESELECTION_H__
#define __BASICTYPESELECTION_H__

#include <cstdint>

// This file contains basic type selection utilities using meta-programming features of C++
// One common case that needs basic type selection is that when we have template function accepts two basic types.
// Considering the following function.
// 
// template <typename T1, typename T2>
// (RETURN_TYPE) add(T1 a, T2 b)
// {
//     return a + b;
// }
//
// What should be the RETURN_TYPE here? We cannot tell it should be T1 or T2 without knowing what are they
// For example, if T1 is int and T2 is double, then the return type should be a double (T2).
// However, if T1 is long, T2 is int, the return type should be long (T1).
//
// So we need a way to tell, between two types, which one has larger range, and we should return that type.
// For integer types with same range, like std::int8_t and std::uint8_t, we define the signed version has larger rank

namespace MetaProg
{

// Struct to get the rank of a given type T
template <typename T>
struct TypeRank;

// Template specialization to rank different types by their range from low to high
template <> struct TypeRank<std::uint8_t>  {static constexpr std::uint32_t value = 0; };
template <> struct TypeRank<std::int8_t>   {static constexpr std::uint32_t value = 1; };
template <> struct TypeRank<std::uint16_t> {static constexpr std::uint32_t value = 2; };
template <> struct TypeRank<std::int16_t>  {static constexpr std::uint32_t value = 3; };
template <> struct TypeRank<std::uint32_t> {static constexpr std::uint32_t value = 4; };
template <> struct TypeRank<std::int32_t>  {static constexpr std::uint32_t value = 5; };
template <> struct TypeRank<std::uint64_t> {static constexpr std::uint32_t value = 6; };
template <> struct TypeRank<std::int64_t>  {static constexpr std::uint32_t value = 7; };
template <> struct TypeRank<float>         {static constexpr std::uint32_t value = 8; };
template <> struct TypeRank<double>        {static constexpr std::uint32_t value = 9; };

// Struct to get the type of a given rank R
template <std::uint32_t Rank>
struct RankType;

// Template specialization to map different ranks back to the corresponding types
template <> struct RankType<0> { using type = std::uint8_t; };
template <> struct RankType<1> { using type = std::int8_t; };
template <> struct RankType<2> { using type = std::uint16_t; };
template <> struct RankType<3> { using type = std::int16_t; };
template <> struct RankType<4> { using type = std::uint32_t; };
template <> struct RankType<5> { using type = std::int32_t; };
template <> struct RankType<6> { using type = std::uint64_t; };
template <> struct RankType<7> { using type = std::int8_t; };
template <> struct RankType<8> { using type = float; };
template <> struct RankType<9> { using type = double; };

// Function that returns the max between two ranks
template <typename T>
constexpr T maxRank(const T &r1, const T &r2) { return (r1 > r2 ? r1 : r2); }

// Function to choose between two types
// Choose the one with higher rank
template <typename T1, typename T2>
struct selectBasicType
{
    static constexpr std::uint32_t r1 = TypeRank<T1>::value;
    static constexpr std::uint32_t r2 = TypeRank<T2>::value;
    static constexpr std::uint32_t max = maxRank(r1, r2);
    using type = typename RankType<max>::type;
};
        
} // End of namespace MetaProg


#endif // __BASICTYPESELECTION_H__
