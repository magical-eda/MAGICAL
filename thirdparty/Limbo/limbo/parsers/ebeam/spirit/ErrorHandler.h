/*************************************************************************
    > File Name: ErrorHandler.h
    > Author: Yibo Lin
    > Mail: yibolin@utexas.edu
    > Created Time: Thu 09 Oct 2014 08:19:40 PM CDT
 ************************************************************************/

#ifndef _ERRORHANDLER_H
#define _ERRORHANDLER_H

#include <iostream>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//  The error handler
//  Please refer to
//	$BOOST_DIR/libs/spirit/example/qi/compiler_tutorial/mini_c/error_handler.hpp
//	print line number and line content if error is met 
///////////////////////////////////////////////////////////////////////////////

template <typename Iterator>
struct ErrorHandler
{
	template <typename, typename, typename>
	struct result { typedef void type; };

	ErrorHandler(Iterator first, Iterator last)
	  : first(first), last(last) {}

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

	std::string get_line(Iterator err_pos) const
	{
		Iterator i = err_pos;
		// position i to the next EOL
		while (i != last && (*i != '\r' && *i != '\n'))
			++i;
		return std::string(err_pos, i);
	}

	Iterator first;
	Iterator last;
	std::vector<Iterator> iters;
};

#endif 
