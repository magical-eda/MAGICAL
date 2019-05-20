/**
 * @file   LpDataBase.h
 * @brief  Database for Lp parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef LPPARSER_DATABASE_H
#define LPPARSER_DATABASE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <limits>
#include <limbo/math/Math.h>

/// namespace for LpParser
namespace LpParser {

/// @nowarn
using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long int64_t;
/// @endnowarn

/// @brief integer array 
typedef std::vector<int64_t> IntegerArray; 
/// @brief string array 
typedef std::vector<std::string> StringArray; 

#if 0
/// @brief bison does not support vector very well, 
/// so here create a dummy class for integer array. 
class IntegerArray : public vector<int64_t>
{
	public: 
        /// @nowarn 
		typedef vector<int64_t> base_type;
		using base_type::size_type;
		using base_type::value_type;
		using base_type::allocator_type;
        /// @endnowarn

        /// @brief constructor 
        /// @param alloc memory allocator 
		IntegerArray(const allocator_type& alloc = allocator_type())
			: base_type(alloc) {}
        /// @brief constructor 
        /// @param n number of values 
        /// @param val data value
        /// @param alloc memory allocator 
		IntegerArray(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
			: base_type(n, val, alloc) {}
};

/// @brief bison does not support vector very well, 
/// so here create a dummy class for string array. 
class StringArray : public vector<string>
{
	public: 
        /// @nowarn 
		typedef vector<string> base_type;
		using base_type::size_type;
		using base_type::value_type;
		using base_type::allocator_type;
        /// @endnowarn

        /// @brief constructor 
        /// @param alloc memory allocator 
		StringArray(const allocator_type& alloc = allocator_type())
			: base_type(alloc) {}
        /// @brief constructor 
        /// @param n number of values 
        /// @param val data value
        /// @param alloc memory allocator 
		StringArray(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
			: base_type(n, val, alloc) {}
};
#endif

/// @brief a term denotes coefficient times a variable 
struct Term 
{
	double coef; ///< coefficient 
	string var; ///< variable 

    /// @brief constructor 
    /// @param c coefficient 
    /// @param v variable 
	Term(double c, string const& v) : coef(c), var(v) {}
};

/// @brief array of terms 
typedef std::vector<Term> TermArray; 

// temporary data structures to hold parsed data 

// forward declaration
/// @class LpParser::LpDataBase
/// @brief Base class for lp database. 
/// Only pure virtual functions are defined.  
/// User needs to inheritate this class and derive a custom database type with all callback functions defined.  
class LpDataBase
{
	public:
        /// @brief add variable that \a l <= \a vname <= \a r. 
        /// @param vname variable name 
        /// @param l lower bound 
        /// @param r upper bound 
		virtual void add_variable(string const& vname, 
				double l = limbo::lowest<double>(), 
				double r = std::numeric_limits<double>::max()) = 0;
        /// @brief add constraint that \a terms \a compare \a constant. 
        /// @param cname constraint name 
        /// @param terms array of terms in left hand side 
        /// @param compare operator '<', '>', '='
        /// @param constant constant in the right hand side 
        virtual void add_constraint(string const& cname, TermArray const& terms, char compare, double constant) = 0; 
        /// @brief add object terms 
        /// @param minimize true denotes minimizing object, false denotes maximizing object 
        /// @param terms array of terms 
		virtual void add_objective(bool minimize, TermArray const& terms) = 0;
        /// @brief set integer variables 
        /// @param vname integer variables  
        /// @param binary denotes whether they are binary variables 
        virtual void set_integer(string const& vname, bool binary) = 0; 
};

} // namespace DefParser

#endif
