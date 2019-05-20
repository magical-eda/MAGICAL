/*************************************************************************
    > File Name: DefParser.h
    > Author: Yibo Lin
    > Mail: yibolin@utexas.edu
    > Created Time: Fri 12 Sep 2014 05:37:36 PM CDT
 ************************************************************************/

#ifndef _DEFPARSER_H
#define _DEFPARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <boost/cstdint.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <limbo/parsers/def/spirit/ErrorHandler.h>
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

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix =  boost::phoenix;

#ifdef CASE_INSENSITIVE
	#define DEF_NO_CASE(X) no_case[X]
#else 
	#define DEF_NO_CASE(X) X
#endif 

struct DefParser
{
	// abstract class for DataBaseType 
	// just want to generalize the flex/bison parser with boost spirit parser 
	class DefDataBase
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
	struct Row : public Item
	{
		string row_name;
		string macro_name;
		int32_t origin[2]; // x, y
		string orient;
		int32_t repeat[2]; // do x by y  
		int32_t step[2]; // x, y
		void reset()
		{
			row_name = macro_name = orient = "";
			origin[0] = origin[1]  = -1;
			repeat[0] = repeat[1] = -1;
			step[0] = step[1] = -1;
		}
		virtual void print(ostringstream& ss) const
		{
			ss << "//////// Row ////////" << endl
				<< "row_name = " << row_name << endl 
				<< "macro_name = " << macro_name << endl 
				<< "origin = " << origin[0] << " " << origin[1] << endl 
				<< "orient = " << orient << endl 
				<< "repeat = " << repeat[0] << " " << repeat[1] << endl 
				<< "step = " << step[0] << " " << step[1] << endl; 
		}
	};
	struct Component : public Item
	{
		string comp_name;
		string macro_name;
		string status;
		int32_t origin[2];
		string orient;
		void reset()
		{
			comp_name = macro_name = status = orient = "";
			origin[0] = origin[1] = -1;
		}
		virtual void print(ostringstream& ss) const
		{
			ss << "//////// Component ////////" << endl
				<< "comp_name = " << comp_name << endl 
				<< "macro_name = " << macro_name << endl 
				<< "status = " << status << endl 
				<< "origin = " << origin[0] << " " << origin[1] << endl 
				<< "orient = " << orient << endl; 
		}
	};
	struct Pin : public Item
	{
		string pin_name;
		string net_name;
		string direct;
		string status; 
		int32_t origin[2];
		string orient; 
		string layer_name;
		int32_t bbox[4];
		void reset()
		{
			pin_name = net_name = direct = status = orient = layer_name = "";
			origin[0] = origin[1] = -1;
			bbox[0] = bbox[1] = bbox[2] = bbox[3] = -1;
		}
		virtual void print(ostringstream& ss) const
		{
			ss << "//////// Pin ////////" << endl
				<< "pin_name = " << pin_name << endl 
				<< "net_name = " << net_name << endl 
				<< "direct = " << direct << endl 
				<< "status = " << status << endl 
				<< "origin = " << origin[0] << " " << origin[1] << endl 
				<< "orient = " << orient << endl
				<< "layer_name = " << layer_name << endl 
				<< "bbox = " << bbox[0] << " " << bbox[1] << " " << bbox[2] << " " << bbox[3] << endl;
		}
	};
	struct Net : public Item
	{
		string net_name;
		vector< std::pair<string, string> > vNetPin;
		void reset()
		{
			net_name = "";
			vNetPin.clear();
		}
		virtual void print(ostringstream& ss) const
		{
			ss << "//////// Net ////////" << endl
				<< "net_name = " << net_name << endl; 
			for (uint32_t i = 0; i < vNetPin.size(); ++i)
				ss << "(" << vNetPin[i].first << ", " << vNetPin[i].second << ") ";
			ss << endl;
		}
	};
	// grammar 
	template <typename Iterator, typename Skipper, typename DataBaseType>
	struct DefGrammar : qi::grammar<Iterator, Skipper>
	{
		qi::rule<Iterator, Skipper> expression;
		qi::rule<Iterator, Skipper> block_other;
		qi::rule<Iterator, Skipper> block_row;
		qi::rule<Iterator, Skipper> block_comp;
		qi::rule<Iterator, Skipper> block_pin;
		qi::rule<Iterator, Skipper> block_net;
		qi::rule<Iterator, string(), Skipper> text;
		qi::rule<Iterator, string(), Skipper> text_nc; // no constraints
		qi::rule<Iterator, string(), Skipper> orient;
		qi::rule<Iterator, string(), Skipper> status;
		qi::rule<Iterator, string(), Skipper> direct;

		Row m_row;
		Component m_comp;
		Pin m_pin;
		Net m_net;

		DataBaseType& m_db;

		DefGrammar(DataBaseType& db, ErrorHandler<Iterator>& error_handler) : DefGrammar::base_type(expression), m_db(db)
		{
			using qi::int_;
			using qi::uint_;
			using qi::double_;
			using qi::char_;
			using qi::_1; using qi::_2; using qi::_3; using qi::_4; using qi::_5;
			using qi::_a;
			using qi::lexeme;
			using qi::no_case;
			using spirit::repeat;
			using spirit::as_string;
			using spirit::lit;

			expression = //qi::eps > 
				*(
					block_other
					| block_row
					| block_comp
					| block_pin
					| block_net
				) 
				> lexeme[DEF_NO_CASE("END DESIGN")]
				;
			block_other = 
				(lexeme[DEF_NO_CASE("VERSION")] >> text_nc > ";" )											[boost::phoenix::bind(&DefGrammar::version_cbk, this, _1)]
				| (lexeme[DEF_NO_CASE("BUSBITCHARS")] >> char_("\"") >> +(char_-char_("\"")) >> lit("\"") > ";")
				| (lexeme[DEF_NO_CASE("DIVIDERCHAR")] >> char_("\"") >> (char_-char_("\"")) >> lit("\"") > ";")
				| (lexeme[DEF_NO_CASE("DESIGN")] >> text > ";")											[boost::phoenix::bind(&DefGrammar::design_cbk, this, _1)]
				| (lexeme[DEF_NO_CASE("UNITS DISTANCE MICRONS")] >> int_ > ";")								[boost::phoenix::bind(&DefGrammar::unit_cbk, this, _1)]
				| (lexeme[DEF_NO_CASE("DIEAREA")] >> lit("(") >> int_ >> int_ >> lit(")") 
						>> lit("(") >> int_ >> int_ >> lit(")") > ";")									[boost::phoenix::bind(&DefGrammar::diearea_cbk, this, _1, _2, _3, _4)]
				;
			block_row = 
				(lexeme[DEF_NO_CASE("ROW")] >> text >> text) 									[boost::phoenix::bind(&DefGrammar::row_cbk_name, this, _1, _2)]
				>> (int_ >> int_ 
				>> orient)																		[boost::phoenix::bind(&DefGrammar::row_cbk_position, this, _1, _2, _3)]
				>> (lexeme[DEF_NO_CASE("DO")] >> int_ 
				>> lexeme[DEF_NO_CASE("BY")] >> int_ 
				>> lexeme[DEF_NO_CASE("STEP")] >> int_ >> int_)									[boost::phoenix::bind(&DefGrammar::row_cbk_repeat, this, _1, _2, _3, _4)]
				> lexeme[";"]																	[boost::phoenix::bind(&DefGrammar::row_cbk, this)]
				;
			block_comp = lexeme[DEF_NO_CASE("COMPONENTS")] >> uint_								[boost::phoenix::bind(&DefGrammar::component_cbk_size, this, _1)]
				> ";" >> 
				*(
					
					 (lit("-") >> text >> text)													[boost::phoenix::bind(&DefGrammar::component_cbk_name, this, _1, _2)]
					>> 
					-(
						lit("+") >> status														[boost::phoenix::bind(&DefGrammar::component_cbk_status, this, _1)]
						>> 
						-(
							(lit("(") >> int_ >> int_ >> lit(")") >> orient)					[boost::phoenix::bind(&DefGrammar::component_cbk_position, this, _1, _2, _3)]
						)												
					)
					> lexeme[";"]																[boost::phoenix::bind(&DefGrammar::component_cbk, this)]
				)
				>> lexeme[DEF_NO_CASE("END COMPONENTS")]
				;
			block_pin = lexeme[DEF_NO_CASE("PINS")] >> uint_									[boost::phoenix::bind(&DefGrammar::pin_cbk_size, this, _1)]
				>> ";" >> 
				*(
					lit("-") >> text															[boost::phoenix::bind(&DefGrammar::pin_cbk_name, this, _1)]
					>> 
					*(
						lit("+") >> 
						(
						(lexeme[DEF_NO_CASE("NET")] >> text)									[boost::phoenix::bind(&DefGrammar::pin_cbk_net, this, _1)]
						| (lexeme[DEF_NO_CASE("DIRECTION")] >> direct)							[boost::phoenix::bind(&DefGrammar::pin_cbk_direct, this, _1)]
						| (
							status																[boost::phoenix::bind(&DefGrammar::pin_cbk_status, this, _1)]
							>> 
							-(
								(lit("(") >> int_ >> int_ >> lit(")") >> orient)				[boost::phoenix::bind(&DefGrammar::pin_cbk_position, this, _1, _2, _3)]
								)
							)			
						| (lexeme[DEF_NO_CASE("LAYER")] >> text 
							>> lit("(") >> int_ >> int_ >> lit(")") 
							>> lit("(") >> int_ >> int_ >> lit(")"))							[boost::phoenix::bind(&DefGrammar::pin_cbk_layer, this, _1, _2, _3, _4, _5)]
						)
					)
					> lexeme[";"]																[boost::phoenix::bind(&DefGrammar::pin_cbk, this)]
				)
				>> lexeme[DEF_NO_CASE("END PINS")]
				;
			block_net = lexeme[DEF_NO_CASE("NETS")] >> uint_									[boost::phoenix::bind(&DefGrammar::net_cbk_size, this, _1)]
				>> ";" >> 
				*(
					lit("-") >> text															[boost::phoenix::bind(&DefGrammar::net_cbk_name, this, _1)]
					>> 
					+(
						(lit("(") >> text >> text >> lit(")"))									[boost::phoenix::bind(&DefGrammar::net_cbk_pin, this, _1, _2)]
					)										
					> lexeme[";"]																[boost::phoenix::bind(&DefGrammar::net_cbk, this)]
				)
				>> lexeme[DEF_NO_CASE("END NETS")]
				;

			text = lexeme[(char_("a-zA-Z_") >> *char_("a-zA-Z_0-9-"))];
			text_nc = lexeme[+char_("a-zA-Z_0-9.-")]; // text no constraints
			orient = lexeme[as_string[DEF_NO_CASE("N")] | as_string[DEF_NO_CASE("S")] | as_string[DEF_NO_CASE("E")] | as_string[DEF_NO_CASE("W")]
				| as_string[DEF_NO_CASE("FN")] | as_string[DEF_NO_CASE("FS")] | as_string[DEF_NO_CASE("FE")] | as_string[DEF_NO_CASE("FW")]];
			status = lexeme[as_string[DEF_NO_CASE("PLACED")] | as_string[DEF_NO_CASE("FIXED")] | as_string[DEF_NO_CASE("UNPLACED")]];
			direct = lexeme[as_string[DEF_NO_CASE("INPUT")] | as_string[DEF_NO_CASE("OUTPUT")] | as_string[DEF_NO_CASE("INOUT")]];
			
			expression.name("DEF expression");
			block_other.name("DEF block_other");
			block_row.name("DEF block_row");
			block_comp.name("DEF block_comp");
			block_pin.name("DEF block_pin");
			block_net.name("DEF block_net");
			text.name("DEF text");
			text_nc.name("DEF text_nc");
			orient.name("DEF orient");
			status.name("DEF status");
			direct.name("DEF direct");

			///////////////////////////////////////////////////////////////////////
#if (BOOST_VERSION/100)%1000 == 55
			// following Error handler only works in boost 1.55.0 
			// and there will be compilation error for 1.56.0 and 1.57.0
			// Error handling: on error in expr, call error_handler.
			qi::on_error<qi::fail>(expression,
					boost::phoenix::function<ErrorHandler<Iterator> >(error_handler)(
						"Error! Expecting ", _4, _3));
#else 
			qi::on_error<qi::fail>(expression, 
					phoenix::ref(std::cout)
					<< phoenix::val("Error! Expecting ")
					<< _4
					<< " here: '"
					<< phoenix::construct<string>(_3, _2)
					<< phoenix::val("'\n")
					);
#endif 
		}
		
		void version_cbk(string const& s1)
		{
			m_db.set_def_version(s1);
		}
		void design_cbk(string const& s1)
		{
			m_db.set_def_design(s1);
		}
		void unit_cbk(int32_t const& d1)
		{
			m_db.set_def_unit(d1);
		}
		void diearea_cbk(int32_t const& d1, int32_t const& d2, int32_t const& d3, int32_t const& d4)
		{
			m_db.set_def_diearea(d1, d2, d3, d4);
		}
		void row_cbk_name(string const& s1, string const& s2)
		{
			m_row.row_name = s1; 
			m_row.macro_name = s2;
		}
		void row_cbk_position(int32_t d1, int32_t d2, string const& s3)
		{
			m_row.origin[0] = d1; 
			m_row.origin[1] = d2; 
			m_row.orient = s3;
		}
		void row_cbk_repeat(int32_t d1, int32_t d2, int32_t d3, int32_t d4)
		{
			m_row.repeat[0] = d1; m_row.repeat[1] = d2;
			m_row.step[0] = d3; m_row.step[1] = d4;
		}
		void row_cbk()
		{
			m_db.add_def_row(m_row);
			m_row.reset();
		}
		void component_cbk_size(uint32_t const& d1)
		{
			m_db.resize_def_component(d1);
		}
		void component_cbk_name(string const& s1, string const& s2)
		{
			m_comp.comp_name = s1;
			m_comp.macro_name = s2;
		}
		void component_cbk_status(string const& s1)
		{
			m_comp.status = s1;
		}
		void component_cbk_position(int32_t const& d1, int32_t const& d2, string const& s3)
		{
			m_comp.origin[0] = d1; m_comp.origin[1] = d2;
			m_comp.orient = s3;
		}
		void component_cbk() 
		{
			m_db.add_def_component(m_comp);
			m_comp.reset();
		}
		void pin_cbk_size(uint32_t d1)
		{
			m_db.resize_def_pin(d1);
		}
		void pin_cbk_name(string const& s1)
		{
			m_pin.pin_name = s1;
		}
		void pin_cbk_net(string const& s1)
		{
			m_pin.net_name = s1;
		}
		void pin_cbk_direct(string const& s1)
		{
			m_pin.direct = s1;
		}
		void pin_cbk_status(string const& s1)
		{
			m_pin.status = s1;
		}
		void pin_cbk_position(int32_t const& d1, int32_t const& d2, string const& s3)
		{
			m_pin.origin[0] = d1; m_pin.origin[1] = d2;
			m_pin.orient = s3;
		}
		void pin_cbk_layer(string const& s1, int32_t const& d1, int32_t const& d2, int32_t const& d3, int32_t const& d4)
		{
			m_pin.layer_name = s1;
			m_pin.bbox[0] = d1; m_pin.bbox[1] = d2;
			m_pin.bbox[2] = d3; m_pin.bbox[3] = d4;
		}
		void pin_cbk()
		{
			m_db.add_def_pin(m_pin);
			m_pin.reset();
		}
		void net_cbk_size(uint32_t d1)
		{
			m_db.resize_def_net(d1);
		}
		void net_cbk_name(string const& s1)
		{
			m_net.net_name = s1;
		}
		void net_cbk_pin(string const& s1, string const& s2)
		{
			m_net.vNetPin.push_back(std::make_pair(s1, s2));
		}
		void net_cbk()
		{
			m_db.add_def_net(m_net);
			m_net.reset();
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
	static bool read(DataBaseType& db, const string& defFile)
	{
		std::ifstream in;

		in.open(defFile.c_str(), std::ios::in);
		if (!in.is_open())
		{
			cout << "Unable to open input file " << defFile << endl;
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
		DefGrammar<string::const_iterator, skip_type, DataBaseType> dg(db, error_handler);

		//TitleWriter().print("parsing lef file", 50, "#");
		//bool r = qi::phrase_parse(iter, end, dg, boost::spirit::ascii::space);
		bool r = qi::phrase_parse(iter, end, dg, skip);
		//TitleWriter().print((r && iter == end)? "parsing succeed" : "parsing failed", 50, "#");

		return r && iter == end;
	}
};

#endif 
