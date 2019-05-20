/**
 * @file   Math.h
 * @brief  mathematical utilities such as abs 
 * @author Yibo Lin
 * @date   Dec 2014
 */

#ifndef _LIMBO_MATH_MATH
#define _LIMBO_MATH_MATH

#include <iterator>
#include <limits>

/// namespace for Limbo
namespace limbo 
{

/// @brief generalized api can handle both integer and floating points 
/// @return absolute value of a number 
template <typename T>
inline T abs(T const& t)
{
	return (t > 0)? t : -t;
}
/// @brief get summation of an array 
/// @param first begin iterator 
/// @param last end iterator 
/// @return sum value of an array
/// @tparam Iterator iterator type 
template <typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type sum(Iterator first, Iterator last)
{
	typename std::iterator_traits<Iterator>::value_type v = 0;
	for (; first != last; ++first)
		v += *first;
	return v;
}
/// @brief get average of an array 
/// @param first begin iterator 
/// @param last end iterator 
/// @return average value of an array
/// @tparam Iterator iterator type 
template <typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type average(Iterator first, Iterator last)
{
	typename std::iterator_traits<Iterator>::value_type v = 0;
	size_t cnt = 0;
	for (; first != last; ++first)
	{
		v += *first;
		cnt += 1;
	}
	return v/cnt;
}
/// @brief get max of an array 
/// @param first begin iterator 
/// @param last end iterator 
/// @return max value of an array
/// @tparam Iterator iterator type 
template <typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type max(Iterator first, Iterator last)
{
	typename std::iterator_traits<Iterator>::value_type v = *first;
	for (; first != last; ++first)
	{
		if (v < *first)
			v = *first;
	}
	return v;
}
/// @brief get min of an array 
/// @param first begin iterator 
/// @param last end iterator 
/// @return min value of an array
/// @tparam Iterator iterator type 
template <typename Iterator>
inline typename std::iterator_traits<Iterator>::value_type min(Iterator first, Iterator last)
{
	typename std::iterator_traits<Iterator>::value_type v = *first;
	for (; first != last; ++first)
	{
		if (v > *first)
			v = *first;
	}
	return v;
}

/// @name generic functions to get lowest value of numbers, i.e., min for integers, -max for floating point numbers 
///@{
/// @brief generic function to get lowest value of numbers 
/// @tparam T number type 
template <typename T>
inline T lowest(); 

/// @brief specialization for integer types 
template <>
inline char lowest<char>()
{
    return std::numeric_limits<char>::min();
}
/// @brief specialization for integer types 
template <>
inline unsigned char lowest<unsigned char>()
{
    return std::numeric_limits<unsigned char>::min();
}
/// @brief specialization for integer types 
template <>
inline short lowest<short>()
{
    return std::numeric_limits<short>::min();
}
/// @brief specialization for integer types 
template <>
inline unsigned short lowest<unsigned short>()
{
    return std::numeric_limits<unsigned short>::min();
}
/// @brief specialization for integer types 
template <>
inline int lowest<int>()
{
    return std::numeric_limits<int>::min();
}
/// @brief specialization for integer types 
template <>
inline unsigned int lowest<unsigned int>()
{
    return std::numeric_limits<unsigned int>::min();
}
/// @brief specialization for integer types 
template <>
inline long lowest<long>()
{
    return std::numeric_limits<long>::min();
}
/// @brief specialization for integer types 
template <>
inline unsigned long lowest<unsigned long>()
{
    return std::numeric_limits<unsigned long>::min();
}
/// @brief specialization for integer types 
template <>
inline long long lowest<long long>()
{
    return std::numeric_limits<long long>::min();
}
/// @brief specialization for integer types 
template <>
inline unsigned long long lowest<unsigned long long>()
{
    return std::numeric_limits<unsigned long long>::min();
}
/// @brief specialization for floating point types 
template <>
inline float lowest<float>()
{
    return -std::numeric_limits<float>::max();
}
/// @brief specialization for floating point types 
template <>
inline double lowest<double>()
{
    return -std::numeric_limits<double>::max();
}
/// @brief specialization for floating point types 
template <>
inline long double lowest<long double>()
{
    return -std::numeric_limits<long double>::max();
}
///@}

} // namespace limbo 

#endif 
