/*************************************************************************
    > File Name: GdsTxtParser.h
    > Author: Yibo Lin
    > Mail: yibolin@utexas.edu
    > Created Time: Wed 17 Sep 2014 03:28:37 PM CDT
 ************************************************************************/

#ifndef _GDSTXTPARSER_H
#define _GDSTXTPARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <limbo/parsers/gdsii/ascii/spirit/ErrorHandler.h>
//#include "Gadget.h"
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ostringstream;
using boost::int8_t;
using boost::int32_t;
using boost::int64_t;
using boost::uint8_t;
using boost::array;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix =  boost::phoenix;

#ifdef CASE_INSENSITIVE
	#define GDS_NO_CASE(X) no_case[X]
#else 
	#define GDS_NO_CASE(X) X
#endif 

// read ascii gds file 
// user can pass an object which contains the following member functions as callbacks 
// add_gds_lib(GdsTxtParser::GdsLib const&)
struct GdsTxtParser
{
	struct GdsItem 
	{
		virtual void print(ostringstream& ss) const {};
		friend std::ostream& operator<<(std::ostream& os, GdsItem const& rhs)
		{
			std::ostringstream ss;
			rhs.print(ss);
			os << ss.str();
			return os;
		}
		friend ostringstream& operator<<(ostringstream& ss, GdsItem const& rhs)
		{
			rhs.print(ss);
			return ss;
		}
	};
	struct GdsBoundary 
	{
		int32_t layer;
		int32_t datatype;
		vector<array<int32_t, 2> > vPoint;
		void reset()
		{
			layer = -1;
			datatype = -1;
			vPoint.clear();
		}
	};
	struct GdsText 
	{
		int32_t layer;
		int32_t texttype;
		int32_t presentation;
		int32_t strans;
		double mag;
		array<int32_t, 2> position;
		string content;
		void reset()
		{
			layer = texttype = presentation = strans = 0;
			mag = 0;
			position.fill(0);
			content = "";
		}
	};
	struct GdsCell 
	{
		string cell_name;
		vector<GdsBoundary> vBoundary;
		vector<GdsText> vText;
		void reset()
		{
			cell_name = "";
			vBoundary.clear();
			vText.clear();
		}
	};
	struct GdsLib 
	{
		string lib_name;
		array<double, 2> unit;
		vector<GdsCell> vCell;
		void reset()
		{
			lib_name = "";
			unit.fill(0);
			vCell.clear();
		}
	};
	/// dummy class to keep the same api with gds stream parser 
	struct GdsDataBase
	{
		virtual void add_gds_lib(GdsLib const&) = 0;
	};
	// grammar 
	template <typename Iterator, typename Skipper, typename DataBaseType>
	struct GdsTxtGrammar : qi::grammar<Iterator, Skipper>
	{
		qi::rule<Iterator, Skipper> expression;
		qi::rule<Iterator, Skipper> block_header;
		qi::rule<Iterator, Skipper> block_lib;
		qi::rule<Iterator, Skipper> block_str;
		qi::rule<Iterator, Skipper> block_boundary;
		qi::rule<Iterator, Skipper> block_text;
		qi::rule<Iterator, string(), Skipper> text;
		qi::rule<Iterator, string(), Skipper> text_nc; // no constraints
		qi::rule<Iterator, string(), Skipper> text_quote; // text quoted by ""

		GdsLib m_lib;

		DataBaseType& m_db;

		GdsTxtGrammar(DataBaseType& db, ErrorHandler<Iterator>& error_handler) : GdsTxtGrammar::base_type(expression), m_db(db)
		{
			using qi::int_;
			using qi::uint_;
			using qi::double_;
			using qi::char_;
			using qi::_1;
			using qi::_2;
			using qi::_3;
			using qi::_4;
			using qi::_5;
			using qi::_a;
			using qi::lexeme;
			using qi::no_case;
			using spirit::repeat;
			using spirit::as_string;
			using spirit::lit;
			using spirit::inf;

			expression = 
				block_header 
				> *block_lib 
				;

			block_header = 
				lexeme[GDS_NO_CASE("HEADER")] > text_nc 
				;
			block_lib = 
				(lexeme[GDS_NO_CASE("BGNLIB")] > *(text_nc-lexeme[GDS_NO_CASE("LIBNAME")]))
				>> (lexeme[GDS_NO_CASE("LIBNAME")] >> text_quote)													[boost::phoenix::bind(&GdsTxtGrammar::libname_cbk, this, _1)]
				>> (lexeme[GDS_NO_CASE("UNITS")] > double_ >> double_)												[boost::phoenix::bind(&GdsTxtGrammar::units_cbk, this, _1, _2)]
				>> *block_str
				> lexeme[GDS_NO_CASE("ENDLIB")]																		[boost::phoenix::bind(&GdsTxtGrammar::library_cbk, this)]
				;
			block_str = 
				(lexeme[GDS_NO_CASE("BGNSTR")] > *(text_nc-lexeme[GDS_NO_CASE("STRNAME")]))							[boost::phoenix::bind(&GdsTxtGrammar::str_cbk, this)]
				>> (lexeme[GDS_NO_CASE("STRNAME")] >> text_quote)													[boost::phoenix::bind(&GdsTxtGrammar::strname_cbk, this, _1)]
				>> *(
						block_boundary 
						| block_text
					)
				> lexeme[GDS_NO_CASE("ENDSTR")]
				;
			block_boundary = 
				lexeme[GDS_NO_CASE("BOUNDARY")]																		[boost::phoenix::bind(&GdsTxtGrammar::boundary_begin_cbk, this)]
				>> 
				*(
					(lexeme[GDS_NO_CASE("LAYER")] > int_)															[boost::phoenix::bind(&GdsTxtGrammar::boundary_layer_cbk, this, _1)]
					| (lexeme[GDS_NO_CASE("DATATYPE")] > int_)														[boost::phoenix::bind(&GdsTxtGrammar::boundary_datatype_cbk, this, _1)]
					| (lexeme[GDS_NO_CASE("XY")] > repeat(5, inf)[int_])											[boost::phoenix::bind(&GdsTxtGrammar::boundary_xy_cbk, this, _1)]
				)
				> lexeme[GDS_NO_CASE("ENDEL")]
				;
			block_text = 
				lexeme[GDS_NO_CASE("TEXT")]																			[boost::phoenix::bind(&GdsTxtGrammar::text_begin_cbk, this)]
				>> 
				*(
					(lexeme[GDS_NO_CASE("LAYER")] > int_)															[boost::phoenix::bind(&GdsTxtGrammar::text_layer_cbk, this, _1)]
					| (lexeme[GDS_NO_CASE("TEXTTYPE")] > int_)														[boost::phoenix::bind(&GdsTxtGrammar::text_texttype_cbk, this, _1)]
					| (lexeme[GDS_NO_CASE("PRESENTATION")] > int_)													[boost::phoenix::bind(&GdsTxtGrammar::text_presentation_cbk, this, _1)]
					| (lexeme[GDS_NO_CASE("STRANS")] > int_)														[boost::phoenix::bind(&GdsTxtGrammar::text_strans_cbk, this, _1)]
					| (lexeme[GDS_NO_CASE("MAG")] > double_)														[boost::phoenix::bind(&GdsTxtGrammar::text_mag_cbk, this, _1)]
					| (lexeme[GDS_NO_CASE("XY")] > int_ >> int_)													[boost::phoenix::bind(&GdsTxtGrammar::text_xy_cbk, this, _1, _2)]
					| (lexeme[GDS_NO_CASE("STRING")] >> text_quote)													[boost::phoenix::bind(&GdsTxtGrammar::text_string_cbk, this, _1)]
				)
				> lexeme[GDS_NO_CASE("ENDEL")]
				;
			text = lexeme[(char_("a-zA-Z_") >> *char_("a-zA-Z_0-9-"))];
			text_nc = lexeme[+char_("a-zA-Z_0-9.-")]; // text no constraints
			text_quote = lexeme[char_("\"") >> *(char_ - char_("\"")) >> char_("\"")];

#if 0
			qi::on_error<qi::fail>(expression, 
					phoenix::ref(std::cout)
					<< phoenix::val("Error! Expecting ")
					<< _4
					<< " here: '"
					<< phoenix::construct<string>(_3, _2)
					<< phoenix::val("'\n")
					);
#endif 
        ///////////////////////////////////////////////////////////////////////
        // Error handling: on error in expr, call error_handler.
			qi::on_error<qi::fail>(expression,
					boost::phoenix::function<ErrorHandler<Iterator> >(error_handler)(
						"Error! Expecting ", _4, _3));
		}
		void library_cbk()
		{
			// required callback for DataBaseType
			m_db.add_gds_lib(m_lib);
			m_lib.reset();
		}
		void libname_cbk(string const& s1)
		{
			// skip ""
			m_lib.lib_name = s1.substr(1, s1.size()-2);
		}
		void units_cbk(double const& d1, double const& d2)
		{
			m_lib.unit[0] = d1; 
			m_lib.unit[1] = d2;
		}
		void str_cbk()
		{
			m_lib.vCell.push_back(GdsCell());
		}
		void strname_cbk(string const& s1)
		{
			GdsCell& cell = m_lib.vCell.back();
			// skip ""
			cell.cell_name = s1.substr(1, s1.size()-2);
		}
		void boundary_begin_cbk()
		{
			m_lib.vCell.back().vBoundary.push_back(GdsBoundary());
		}
		void boundary_layer_cbk(int32_t const& d1)
		{
			GdsBoundary& boundary = m_lib.vCell.back().vBoundary.back();
			boundary.layer = d1;
		}
		void boundary_datatype_cbk(int32_t const& d1)
		{
			GdsBoundary& boundary = m_lib.vCell.back().vBoundary.back();
			boundary.datatype = d1;
		}
		void boundary_xy_cbk(vector<int32_t> const& v1)
		{
			assert((v1.size()%2) == 0);
			GdsBoundary& boundary = m_lib.vCell.back().vBoundary.back();
			boundary.vPoint.resize(v1.size()>>1);
			for (int32_t i = 0; i < v1.size(); i += 2)
			{
				boundary.vPoint[i>>1][0] = v1[i];
				boundary.vPoint[i>>1][1] = v1[i+1];
			}
			assert(boundary.vPoint.front() == boundary.vPoint.back());
		}
		void text_begin_cbk()
		{
			m_lib.vCell.back().vText.push_back(GdsText());
		}
		void text_layer_cbk(int32_t const& d1)
		{
			GdsText& t = m_lib.vCell.back().vText.back();
			t.layer = d1;
		}
		void text_texttype_cbk(int32_t const& d1)
		{
			GdsText& t = m_lib.vCell.back().vText.back();
			t.texttype = d1;
		}
		void text_presentation_cbk(int32_t const& d1)
		{
			GdsText& t = m_lib.vCell.back().vText.back();
			t.presentation = d1;
		}
		void text_strans_cbk(int32_t const& d1)
		{
			GdsText& t = m_lib.vCell.back().vText.back();
			t.strans = d1;
		}
		void text_mag_cbk(double const& d1)
		{
			GdsText& t = m_lib.vCell.back().vText.back();
			t.mag = d1;
		}
		void text_xy_cbk(int32_t const& d1, int32_t const& d2)
		{
			GdsText& t = m_lib.vCell.back().vText.back();
			t.position[0] = d1; t.position[1] = d2;
		}
		void text_string_cbk(string const& s1)
		{
			GdsText& t = m_lib.vCell.back().vText.back();
			// skip ""
			t.content = s1.substr(1, s1.size()-2);
		}
	};
	template <typename Iterator>
	struct SkipperGrammar : qi::grammar<Iterator>
	{
		qi::rule<Iterator> skip;

		SkipperGrammar() : SkipperGrammar::base_type(skip)
		{
			using qi::char_;
			using qi::eol;
			using spirit::lit;

			skip = ascii::space 
				| ":"
				| ","
				| ("#" >> *(char_ - eol) >> eol )
				;
		}
	};
	template <typename DataBaseType>
	static bool read(DataBaseType& db, const string& gdsTxtFile)
	{
		std::ifstream in;

		in.open(gdsTxtFile.c_str(), std::ios::in);
		if (!in.is_open())
		{
			cout << "Unable to open input file " << gdsTxtFile << endl;
			return false;
		}

		in.unsetf(std::ios::skipws);      // No white space skipping!
		string str =  std::string(std::istreambuf_iterator<char>(in.rdbuf()),
						   std::istreambuf_iterator<char>());
		string::const_iterator iter = str.begin();
		string::const_iterator end = str.end();

		typedef SkipperGrammar<string::const_iterator> skip_type;
		skip_type skip;
		typedef ErrorHandler<string::const_iterator> error_handler_type;
		error_handler_type error_handler (iter, end);
		GdsTxtGrammar<string::const_iterator, skip_type, DataBaseType> gtg(db, error_handler);

		//TitleWriter().print("parsing gdsTxt file", 50, "#");
		//bool r = qi::phrase_parse(iter, end, lg, boost::spirit::ascii::space);
		bool r = qi::phrase_parse(iter, end, gtg, skip);
		//TitleWriter().print((r && iter == end)? "parsing succeed" : "parsing failed", 50, "#");

		return r && iter == end;
	}
};

#endif 
