/*************************************************************************
    > File Name: EbeamParser.h
    > Author: Yibo Lin
    > Mail: yibolin@utexas.edu
    > Created Time: Mon 15 Sep 2014 01:56:38 PM CDT
 ************************************************************************/

#ifndef _EBEAMPARSER_H
#define _EBEAMPARSER_H

//#ifdef EBEAM_AWARE

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
#include <limbo/parsers/ebeam/spirit/ErrorHandler.h>
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
using boost::iequals;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix =  boost::phoenix;

#ifdef CASE_INSENSITIVE
	#define EBEAM_NO_CASE(X) no_case[X]
#else 
	#define EBEAM_NO_CASE(X) X
#endif 

struct EbeamParser
{
	// abstract class for DataBaseType 
	// just want to generalize the flex/bison parser with boost spirit parser 
	class EbeamDataBase
	{};

	struct Item 
	{
		virtual void print(ostringstream&) const {};
		friend std::ostream& operator<<(std::ostream& os, Item const& rhs)
		{
			std::ostringstream ss;
			rhs.print(ss);
			os << ss.str();
			return os;
		}
		friend ostringstream& operator<<(ostringstream& ss, Item const& rhs)
		{
			rhs.print(ss);
			return ss;
		}
	};
	struct EbeamBoundary : public Item
	{
		double offset;
		double width;
		double step;
		vector<int32_t> vLayerId;
		vector<string> vLayer;
		EbeamBoundary() {reset();}
		void reset()
		{
			offset = 0;
			width = 0;
			step = 0;
			vLayerId.clear();
			vLayer.clear();
		}
		virtual void print(ostringstream& ss) const
		{
			ss << "//////// EbeamBoundary ////////" << endl
				<< "offset = " << offset << endl 
				<< "width = " << width << endl 
				<< "step = " << step << endl
				<< "vLayerId = ";
			BOOST_FOREACH(int32_t const& v, vLayerId)
			{
				ss << v << endl;
			}
			ss << "vLayer = ";
			BOOST_FOREACH(string const& v, vLayer)
			{
				ss << v << endl;
			}
		}
	};
	struct ConfSite : public Item
	{
		string confsite_name;
		int32_t layer_id;
		string layer;
		vector<int32_t> vSiteCnt;
		ConfSite() {reset();}
		void reset()
		{
			confsite_name = "";
			layer_id = -1;
			layer = "";
			vSiteCnt.clear();
		}
	};
	struct Macro : public Item 
	{
		string macro_name;
		vector<ConfSite> vConfSite;
		void reset()
		{
			macro_name = "";
			vConfSite.clear();
		}
	};

	// grammar 
	template <typename Iterator, typename Skipper, typename DataBaseType>
	struct EbeamGrammar : qi::grammar<Iterator, Skipper>
	{
		qi::rule<Iterator, Skipper> expression;
		qi::rule<Iterator, Skipper> block_units;
		qi::rule<Iterator, Skipper> block_ebeam;
		qi::rule<Iterator, Skipper> block_macro;
		qi::rule<Iterator, string(), Skipper> text;
		qi::rule<Iterator, string(), Skipper> text_nc; // no constraints

		EbeamBoundary m_ebeam_boundary;
		Macro m_macro;

		DataBaseType& m_db;

		EbeamGrammar(DataBaseType& db, ErrorHandler<Iterator>& error_handler) : EbeamGrammar::base_type(expression), m_db(db)
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

			expression = //qi::eps > 
				*(
					block_units
					| block_ebeam
					| block_macro
				) 
				> lexeme[EBEAM_NO_CASE("END LIBRARY")]
				;
			block_units = 
				lexeme[EBEAM_NO_CASE("UNITS")] 
				>> lexeme[EBEAM_NO_CASE("DATABASE MICRONS")] >> int_										[boost::phoenix::bind(&EbeamGrammar::units_cbk, this, _1)]
					> ";"
				>> lexeme[EBEAM_NO_CASE("END UNITS")]
				;
			block_ebeam = 
				lexeme[EBEAM_NO_CASE("EBEAMBOUNDARY")]
				>> *(
						(lexeme[EBEAM_NO_CASE("OFFSET")] >> double_ > ";")									[boost::phoenix::bind(&EbeamGrammar::ebeam_cbk_offset, this, _1)]
						| (lexeme[EBEAM_NO_CASE("WIDTH")] >> double_ > ";")									[boost::phoenix::bind(&EbeamGrammar::ebeam_cbk_width, this, _1)]
						| (lexeme[EBEAM_NO_CASE("STEP")] >> double_ > ";")									[boost::phoenix::bind(&EbeamGrammar::ebeam_cbk_step, this, _1)]
						| (lexeme[EBEAM_NO_CASE("LAYERID")] >> repeat(0, inf)[int_] > ";")					[boost::phoenix::bind(&EbeamGrammar::ebeam_cbk_layerid, this, _1)]
						| (lexeme[EBEAM_NO_CASE("LAYER")] >> repeat(0, inf)[text] > ";")					[boost::phoenix::bind(&EbeamGrammar::ebeam_cbk_layer, this, _1)]
					)
				>> lexeme[EBEAM_NO_CASE("END EBEAMBOUNDARY")]												[boost::phoenix::bind(&EbeamGrammar::ebeam_cbk, this)]
				;
			block_macro = 
				lexeme[EBEAM_NO_CASE("MACRO")] >> text														[boost::phoenix::bind(&EbeamGrammar::macro_cbk_name, this, _1)]
				>> *(
						(lexeme[EBEAM_NO_CASE("CONFLICTSITE")] >> text)										[boost::phoenix::bind(&EbeamGrammar::macro_cbk_confsite_name, this, _1)]
						>> 
						*(
							(lexeme[EBEAM_NO_CASE("LAYERID")] >> int_ > ";")								[boost::phoenix::bind(&EbeamGrammar::macro_cbk_confsite_layerid, this, _1)]
							| (lexeme[EBEAM_NO_CASE("LAYER")] >> text > ";")								[boost::phoenix::bind(&EbeamGrammar::macro_cbk_confsite_layer, this, _1)]
						
							| (lexeme[EBEAM_NO_CASE("SITE")] >> repeat(0, inf)[int_] > ";")					[boost::phoenix::bind(&EbeamGrammar::macro_cbk_confsite_site, this, _1)]
	
						)
						
						>> lexeme[EBEAM_NO_CASE("END")] >> text												[boost::phoenix::bind(&EbeamGrammar::macro_cbk_confsite, this, _1)]
					)
				>> lexeme[EBEAM_NO_CASE("END")] >> text														[boost::phoenix::bind(&EbeamGrammar::macro_cbk, this, _1)]
				;
			text = lexeme[(char_("a-zA-Z") >> *char_("a-zA-Z_0-9-"))];
			text_nc = lexeme[+char_("a-zA-Z_0-9.-")]; // text no constraints
			
			expression.name("Ebeam expression");
			block_units.name("Ebeam block_units");
			block_macro.name("Ebeam block_macro");
			block_ebeam.name("Ebeam block_ebeam");
			text.name("Ebeam text");
			text_nc.name("Ebeam text_nc");

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

		void units_cbk(int32_t const& d1)
		{
			// call db
			m_db.set_ebeam_unit(d1);
		}
		void ebeam_cbk()
		{
			// call db
			m_db.set_ebeam_boundary(m_ebeam_boundary);
			m_ebeam_boundary.reset();
		}
		void ebeam_cbk_offset(double const& d1)
		{
			m_ebeam_boundary.offset = d1;
		}
		void ebeam_cbk_width(double const& d1)
		{
			m_ebeam_boundary.width = d1;
		}
		void ebeam_cbk_step(double const& d1)
		{
			m_ebeam_boundary.step = d1;
		}
		void ebeam_cbk_layerid(vector<int32_t> const& v1)
		{
			m_ebeam_boundary.vLayerId = v1;
		}
		void ebeam_cbk_layer(vector<string> const& v1)
		{
			m_ebeam_boundary.vLayer = v1;
		}
		// callbacks for macro 
		void macro_cbk(string const& s1)
		{
			assert(m_macro.macro_name == s1);
			// call db 
			m_db.add_ebeam_macro(m_macro);
			// reset 
			m_macro.reset();
		}
		void macro_cbk_name(string const& s1)
		{
			m_macro.macro_name = s1;
		}
		void macro_cbk_confsite_name(string const& s1)
		{
			m_macro.vConfSite.push_back(ConfSite());
			m_macro.vConfSite.back().confsite_name = s1;
		}
		void macro_cbk_confsite_layerid(int32_t const& d1)
		{
			m_macro.vConfSite.back().layer_id = d1;
		}
		void macro_cbk_confsite_layer(string const& s1)
		{
			m_macro.vConfSite.back().layer = s1;
		}
		void macro_cbk_confsite_site(vector<int32_t> const& v1)
		{
			m_macro.vConfSite.back().vSiteCnt = v1;
		}
		void macro_cbk_confsite(string const& s1)
		{
			assert(m_macro.vConfSite.back().confsite_name == s1);
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
				| ("#" >> *(char_ - eol) >> eol )
				;
		}
	};
	template <typename DataBaseType>
	static bool read(DataBaseType& db, const string& ebeamFile)
	{
		std::ifstream in;

		in.open(ebeamFile.c_str(), std::ios::in);
		if (!in.is_open())
		{
			cout << "Unable to open input file " << ebeamFile << endl;
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
		EbeamGrammar<string::const_iterator, skip_type, DataBaseType> eg(db, error_handler);

		bool r = qi::phrase_parse(iter, end, eg, skip);

		return r && iter == end;
	}
};

//#endif // end of EBEAM_AWARE

#endif 
