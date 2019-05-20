/**
 * @file   ErrorHandler.h
 * @brief  error handler for Boost.Spirit parser 
 *
 * Please refer to
 * $BOOST_DIR/libs/spirit/example/qi/compiler_tutorial/mini_c/error_handler.hpp. 
 * Print line number and line content if error is met.  
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _ERRORHANDLER_H
#define _ERRORHANDLER_H

#include <iostream>
#include <string>
#include <vector>

/// @brief error handler 
/// @tparam Iterator iterator of text 
template <typename Iterator>
struct ErrorHandler
{
    /// @brief probably used as some kind of type traits 
	template <typename, typename, typename>
	struct result 
    { 
        /// @nowarn 
        typedef void type; 
        /// @endnowarn
    };

    /// @brief constructor 
    /// @param first, last begin and end iterators to text 
	ErrorHandler(Iterator first, Iterator last)
	  : first(first), last(last) {}

    /// @brief API to invoke the error handler 
    /// @tparam Message message 
    /// @tparam What what error it is 
    /// @param message message to print 
    /// @param what what error to print 
    /// @param err_pos position of error occurred 
	template <typename Message, typename What>
	void operator()(
		Message const& message,
		What const& what,
		Iterator err_pos) const
	{
		int line;
		Iterator line_start = get_pos(err_pos, line);
		if (err_pos != last)
		{
			std::cout << message << what << " line " << line << ':' << std::endl;
			std::cout << get_line(line_start) << std::endl;
			for (; line_start != err_pos; ++line_start)
				std::cout << ' ';
			std::cout << '^' << std::endl;
		}
		else
		{
			std::cout << "Unexpected end of file. ";
			std::cout << message << what << " line " << line << std::endl;
		}
	}

    /// @brief get position of error 
    /// @param err_pos iterator of error position 
    /// @param line line number 
    /// @return iterator to the starting line of the error 
	Iterator get_pos(Iterator err_pos, int& line) const
	{
		line = 1;
		Iterator i = first;
		Iterator line_start = first;
		while (i != err_pos)
		{
			bool eol = false;
			if (i != err_pos && *i == '\r') // CR
			{
				eol = true;
				line_start = ++i;
			}
			if (i != err_pos && *i == '\n') // LF
			{
				eol = true;
				line_start = ++i;
			}
			if (eol)
				++line;
			else
				++i;
		}
		return line_start;
	}

    /// @brief get line from iterator 
    /// @param err_pos iterator to error position 
    /// @return line string 
	std::string get_line(Iterator err_pos) const
	{
		Iterator i = err_pos;
		// position i to the next EOL
		while (i != last && (*i != '\r' && *i != '\n'))
			++i;
		return std::string(err_pos, i);
	}

	Iterator first; ///< begin iterator 
	Iterator last; ///< end iterator 
	std::vector<Iterator> iters; ///< not sure what it is used and why it is here 
};

#endif 
