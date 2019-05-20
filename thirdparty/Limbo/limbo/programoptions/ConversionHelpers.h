/**
 * @file   ConversionHelpers.h
 * @brief  helper function objects for various data types 
 *
 * Utilize template specialization to handle generic operations on different data types 
 *
 * @author Yibo Lin
 * @date   Jul 2015
 */

#ifndef _LIMBO_PROGRAMOPTIONS_CONVERSIONHELPERS_H
#define _LIMBO_PROGRAMOPTIONS_CONVERSIONHELPERS_H

#include <iostream>
#include <vector>
#include <set>
#include <limbo/string/String.h>

/// @brief namespace for Limbo
namespace limbo 
{ 
/// @brief namespace for Limbo.ProgramOptions
namespace programoptions 
{

/// template function objects to parse, print and assign values 
/// template specialization is needed if default operators are not defined 

/**
 * @class limbo::programoptions::parse_helper
 * @brief metafunction for parsing a char-based string to a target data type 
 * @tparam T target data type 
 */
template <typename T>
struct parse_helper 
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(T& target, const char* value) const
    {
        target = value;
        return true;
    }
};

/**
 * @class limbo::programoptions::print_helper
 * @brief metafunction for printing a target data type 
 * @tparam T target data type 
 */
template <typename T>
struct print_helper
{
    /// @brief printing target 
    /// @param os output stream
    /// @param target target data 
    inline void operator()(std::ostream& os, T const& target) const
    {
        os << target;
    }
};

/**
 * @class limbo::programoptions::assign_helper
 * @brief metafunction for assign a source data type to a target data type 
 * @tparam T target data type 
 */
template <typename T>
struct assign_helper
{
    /// @brief assign source to target  
    /// @param target target data 
    /// @param source source data
    inline void operator()(T& target, T const& source) const
    {
        target = source;
    }
};

/**
 * @class limbo::programoptions::boolean_helper
 * @brief metafunction for boolean operation 
 * @tparam T target data type 
 */
template <typename T>
struct boolean_helper
{
    /// @brief generic boolean operation 
    /// @return false for types other than boolean, but return target value for boolean 
    inline bool operator()(T const&) const 
    {
        return false;
    }
};

/////////////////// specialization for parse_helper /////////////////
/**
 * @brief template specialization for boolean type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<bool>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(bool& target, const char* value) const
    {
        if (limbo::iequals(value, "true") || limbo::iequals(value, "1"))
            target = true;
        else if (limbo::iequals(value, "false") || limbo::iequals(value, "0"))
            target = false;
        else return false;
        return true;
    }
};
/**
 * @brief template specialization for char type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<char>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(char& target, const char* value) const
    {
        target = *value;
        return true;
    }
};
/**
 * @brief template specialization for unsigned char type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<unsigned char>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(unsigned char& target, const char* value) const
    {
        target = *value;
        return true;
    }
};
/**
 * @brief template specialization for int type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<int>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(int& target, const char* value) const
    {
        target = atoi(value);
        return true;
    }
};
/**
 * @brief template specialization for unsigned int type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<unsigned int>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(unsigned int& target, const char* value) const
    {
        target = atoi(value);
        return true;
    }
};
/**
 * @brief template specialization for long type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<long>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(long& target, const char* value) const
    {
        target = atol(value);
        return true;
    }
};
/**
 * @brief template specialization for unsigned long type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<unsigned long>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(unsigned long& target, const char* value) const
    {
        target = atol(value);
        return true;
    }
};
/**
 * @brief template specialization for long long type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<long long>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(long long& target, const char* value) const
    {
        target = atol(value);
        return true;
    }
};
/**
 * @brief template specialization for unsigned long long type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<unsigned long long>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(unsigned long long& target, const char* value) const
    {
        target = atol(value);
        return true;
    }
};
/**
 * @brief template specialization for float type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<float>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(float& target, const char* value) const
    {
        target = atof(value);
        return true;
    }
};
/**
 * @brief template specialization for double type to struct @ref limbo::programoptions::parse_helper
 */
template <>
struct parse_helper<double>
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(double& target, const char* value) const
    {
        target = atof(value);
        return true;
    }
};
/**
 * @brief template specialization for std::vector<T> type to struct @ref limbo::programoptions::parse_helper
 * @tparam T data type 
 */
template <typename T>
struct parse_helper<std::vector<T> >
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(std::vector<T>& target, const char* value) const
    {
        T v;
        parse_helper<T>()(v, value);
        target.push_back(v);
        return true;
    }
};
/**
 * @brief template specialization for std::set<T> type to struct @ref limbo::programoptions::parse_helper
 * @tparam T data type 
 */
template <typename T>
struct parse_helper<std::set<T> >
{
    /// @brief parse a char-based string to target 
    /// @param target target data 
    /// @param value char-based string 
    inline bool operator()(std::set<T>& target, const char* value) const
    {
        T v;
        parse_helper<T>()(v, value);
        target.insert(v);
        return true;
    }
};

/////////////////// specialization for print_helper /////////////////
/**
 * @brief template specialization for boolean type to struct @ref limbo::programoptions::print_helper
 */
template <>
struct print_helper<bool>
{
    /// @brief printing target 
    /// @param os output stream
    /// @param target target data 
    inline void operator()(std::ostream& os, bool const& target) const 
    {
        os << ((target)? "true" : "false");
    }
};
/**
 * @brief template specialization for std::vector<T> type to struct @ref limbo::programoptions::print_helper
 * @tparam T data type 
 */
template <typename T>
struct print_helper<std::vector<T> >
{
    /// @brief printing target 
    /// @param os output stream
    /// @param target target data 
    inline void operator()(std::ostream& os, std::vector<T> const& target) const
    {
        const char* prefix = "";
        for (typename std::vector<T>::const_iterator it = target.begin(); it != target.end(); ++it)
        {
            os << prefix;
            print_helper<T>()(os, *it);
            prefix = ",";
        }
    }
};
/**
 * @brief template specialization for std::set<T> type to struct @ref limbo::programoptions::print_helper
 * @tparam T data type 
 */
template <typename T>
struct print_helper<std::set<T> >
{
    /// @brief printing target 
    /// @param os output stream
    /// @param target target data 
    inline void operator()(std::ostream& os, std::set<T> const& target) const
    {
        const char* prefix = "";
        for (typename std::set<T>::const_iterator it = target.begin(); it != target.end(); ++it)
        {
            os << prefix;
            print_helper<T>()(os, *it);
            prefix = ",";
        }
    }
};

/////////////////// specialization for assign_helper /////////////////

/////////////////// specialization for boolean_helper /////////////////
/**
 * @brief template specialization for boolean type to struct @ref limbo::programoptions::boolean_helper
 */
template <>
struct boolean_helper<bool>
{
    /// @brief generic boolean operation 
    /// @return false for types other than boolean, but return target value for boolean 
    inline bool operator()(bool const& target) const 
    {
        return target;
    }
};

} // programoptions
} // namespace limbo

#endif
