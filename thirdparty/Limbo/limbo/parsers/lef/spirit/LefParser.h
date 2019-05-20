/*************************************************************************
    > File Name: LefParser.h
    > Author: Yibo Lin
    > Mail: yibolin@utexas.edu
    > Created Time: Thu 12 Jun 2014 03:27:23 PM PDT
 ************************************************************************/

#ifndef _LEFPARSER_H
#define _LEFPARSER_H

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
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/version.hpp>
#include <limbo/parsers/lef/spirit/ErrorHandler.h>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ostringstream;
using boost::int8_t;
using boost::int32_t;
using boost::int64_t;
using boost::uint8_t;
using boost::uint32_t;
using boost::array;
using boost::iequals;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix =  boost::phoenix;

#ifdef CASE_INSENSITIVE
	#define LEF_NO_CASE(X) no_case[X]
#else 
	#define LEF_NO_CASE(X) X
#endif 

struct LefParser
{
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
	struct Pin : public Item
	{
		string name;
		string direct;
		string use;
		string shape;
		string layer_name;
		string port_shape; 
		vector<double> vBBox;
		void reset()
		{
			name = direct = use = shape = layer_name = port_shape = "";
			vBBox.reserve(4);
		}
		virtual void print(ostringstream& ss) const
		{
			ss << "//////// Pin ////////" << endl
				<< "name = " << name << endl 
				<< "direct = " << direct << endl 
				<< "use = " << use << endl 
				<< "shape = " << shape << endl 
				<< "layer_name = " << layer_name << endl
				<< "port_shape = " << endl 
				<< "bbox = ";
			BOOST_FOREACH(double const& v, vBBox)
			{
				ss << v << endl;
			}
		}
	};
	struct Macro  : public Item
	{
		string name;
		string class_name;
		string site_name;
		array<double, 2> origin;
		array<double, 2> size;
		vector<string> vSymmetry;
		vector<Pin> vPin;
		Macro()
		{
			origin.fill(0);
			size.fill(0);
		}
		void reset()
		{
			name = class_name = site_name = "";
			origin.fill(0);
			size.fill(0);
			vSymmetry.clear();
			vPin.clear();
		}
		virtual void print(ostringstream& ss) const
		{
			ss << "//////// Macro ////////" << endl
				<< "name = " << name << endl 
				<< "class_name = " << class_name << endl 
				<< "site_name = " << site_name << endl 
				<< "origin = " << origin[0] << " " << origin[1] << endl 
				<< "size = " << size[0] << " " << size[1] << endl;
			BOOST_FOREACH(Pin const& pin, vPin)
			{
				ss << pin;
			}
		}
	};
	struct Layer  : public Item
	{
		string name;
		string type;
		string direct;
		array<double, 2> pitch;
		array<double, 2> offset;
		double width;
		double spacing;
		double resistance;
		double capacitance;
		double height;
		double thickness;
		double edge_capacitance;
		Layer()
		{
			pitch.fill(0);
			offset.fill(0);
			width = spacing = resistance = capacitance = 0.0;
			height = thickness = edge_capacitance = 0.0;
		}
		void reset()
		{
			name = type = direct = "";
			pitch.fill(0); offset.fill(0);
			width = spacing = 0;
			resistance = capacitance = height = 0;
			thickness = edge_capacitance = 0;
		}
		void print(ostringstream& ss) const
		{
			ss << "//////// Layer ////////" << endl
				<< "name = " << name << endl 
				<< "type = " << type << endl 
				<< "direct = " << direct << endl 
				<< "pitch = " << pitch[0] << ", " << pitch[1] << endl 
				<< "offset = " << offset[0] << ", " << offset[1] << endl 
				<< "width = " << width << endl
				<< "resistance = " << resistance << endl 
				<< "capacitance = " << capacitance << endl 
				<< "height = " << height << endl 
				<< "thickness = " << thickness << endl
				<< "edge_capacitance = " << edge_capacitance << endl;
		}
	};
	struct Site  : public Item
	{
		string name;
		string class_name;
		vector<string> vSymmetry;
		array<double, 2> size;
		Site()
		{
			size.fill(0);
		}
		void reset()
		{
			name = class_name = "";
			size.fill(0);
			vSymmetry.clear();
		}
		void print(ostringstream& ss) const
		{
			ss << "//////// Site ////////" << endl
				<< "name = " << name << endl 
				<< "class_name = " << class_name << endl 
				<< "size = " << size[0] << " " << size[1] << endl;
		}
	};
	/// base abstract class and user needs to inherit this class 
	/// it contains required callbacks for LefParser 
	struct LefDataBase
	{
		virtual void set_lef_version(string const&) = 0;
		virtual void set_lef_unit(int32_t const&) = 0;
		virtual void set_lef_site(Site const&) = 0;
		virtual void add_lef_layer(Layer const&) = 0;
		virtual void add_lef_macro(Macro const&) = 0;
	};
	// grammar 
	template <typename Iterator, typename Skipper, typename DataBaseType>
	struct LefGrammar : qi::grammar<Iterator, Skipper>
	{
		qi::rule<Iterator, Skipper> expression;
		qi::rule<Iterator, Skipper> block_site;
		qi::rule<Iterator, Skipper> block_layer;
		qi::rule<Iterator, Skipper> block_units;
		qi::rule<Iterator, Skipper> block_macro;
		qi::rule<Iterator, Skipper> block_other;
		qi::rule<Iterator, Skipper> block_via_macro;
		qi::rule<Iterator, Skipper> block_viarule_macro;
		qi::rule<Iterator, Skipper> block_spacing;
		qi::rule<Iterator, string(), Skipper> text;
		qi::rule<Iterator, string(), Skipper> text_nc; // no constraints
		Pin m_pin;
		Macro m_macro;
		Layer m_layer;
		Site m_site;

		DataBaseType& m_db;

		LefGrammar(DataBaseType& db, ErrorHandler<Iterator>& error_handler) : LefGrammar::base_type(expression), m_db(db)
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
					block_other
					| block_units
					| block_site
					| block_layer
					| block_macro
					| block_via_macro
					| block_viarule_macro
					| block_spacing
				) 
				> lexeme[LEF_NO_CASE("END LIBRARY")]
				;
			block_other = 
				(lexeme[LEF_NO_CASE("VERSION")] >> text_nc > ";" )											[boost::phoenix::bind(&LefGrammar::version_cbk, this, _1)]
				| (lexeme[LEF_NO_CASE("NAMESCASESENSITIVE")] >> 
					(lexeme[as_string[LEF_NO_CASE("ON")] | as_string[LEF_NO_CASE("OFF")]]) > ";")
				| (lexeme[LEF_NO_CASE("BUSBITCHARS")] >> char_("\"") >> (+(char_-char_("\""))) >> lit("\"") > ";")
				| (lexeme[LEF_NO_CASE("DIVIDERCHAR")] >> char_("\"") >> (char_-char_("\"")) >> lit("\"") > ";")
				| (lexeme[LEF_NO_CASE("MANUFACTURINGGRID")] >> double_ > ";")
				;
			block_units = 
				lexeme[LEF_NO_CASE("UNITS")] 
				>> lexeme[LEF_NO_CASE("DATABASE MICRONS")] >> int_										[boost::phoenix::bind(&LefGrammar::units_cbk, this, _1)]
					> ";"
				>> lexeme[LEF_NO_CASE("END UNITS")]
				;
			block_site = 
				lexeme[LEF_NO_CASE("SITE")] >> text															[boost::phoenix::bind(&LefGrammar::site_cbk_name, this, _1)]
				>> *(
						(lexeme[LEF_NO_CASE("SIZE")] >> double_ >> lexeme[LEF_NO_CASE("BY")] >> double_ > ";") [boost::phoenix::bind(&LefGrammar::site_cbk_size, this, _1, _2)]
						| (lexeme[LEF_NO_CASE("CLASS")] >> text > ";")										[boost::phoenix::bind(&LefGrammar::site_cbk_class, this, _1)]
						| (lexeme[LEF_NO_CASE("SYMMETRY")] >> repeat(1, 2)[text] > ";")						[boost::phoenix::bind(&LefGrammar::site_cbk_symmetry, this, _1)]
					)
				>> lexeme[LEF_NO_CASE("END")] >> text														[boost::phoenix::bind(&LefGrammar::site_cbk, this, _1)]
				;
			block_layer = 
				lexeme[LEF_NO_CASE("LAYER")] >> text [boost::phoenix::bind(&LefGrammar::layer_cbk_name, this, _1)]
				>> *(
						(lexeme[LEF_NO_CASE("TYPE")] >> text > ";")											[boost::phoenix::bind(&LefGrammar::layer_cbk_type, this, _1)]
						| (lexeme[LEF_NO_CASE("DIRECTION")] >> text > ";")									[boost::phoenix::bind(&LefGrammar::layer_cbk_direction, this, _1)]
						| (lexeme[LEF_NO_CASE("PITCH")] >> repeat(1, 2)[double_] > ";")						[boost::phoenix::bind(&LefGrammar::layer_cbk_pitch, this, _1)]
						| (lexeme[LEF_NO_CASE("OFFSET")] >> repeat(1, 2)[double_] > ";")					[boost::phoenix::bind(&LefGrammar::layer_cbk_offset, this, _1)]
						| (lexeme[LEF_NO_CASE("WIDTH")] >> double_ > ";")									[boost::phoenix::bind(&LefGrammar::layer_cbk_width, this, _1)]
						| (lexeme[LEF_NO_CASE("SPACING")] >> double_ > ";")									[boost::phoenix::bind(&LefGrammar::layer_cbk_spacing, this, _1)]
						// spacing table is skipped 
						| (lexeme[LEF_NO_CASE("SPACINGTABLE")] >> *(text_nc-";") > ";")	
						| (lexeme[LEF_NO_CASE("RESISTANCE")] >> repeat(0, 1)[text] >> double_ > ";")		[boost::phoenix::bind(&LefGrammar::layer_cbk_resistance, this, _1, _2)]
						| (lexeme[LEF_NO_CASE("CAPACITANCE")] >> repeat(0, 1)[text] >> double_ > ";")		[boost::phoenix::bind(&LefGrammar::layer_cbk_capacitance, this, _1, _2)]
						| (lexeme[LEF_NO_CASE("HEIGHT")] >> double_ > ";")									[boost::phoenix::bind(&LefGrammar::layer_cbk_height, this, _1)]
						| (lexeme[LEF_NO_CASE("THICKNESS")] >> double_ > ";")								[boost::phoenix::bind(&LefGrammar::layer_cbk_thickness, this, _1)]
						| (lexeme[LEF_NO_CASE("EDGECAPACITANCE")] >> double_ > ";")							[boost::phoenix::bind(&LefGrammar::layer_cbk_edge_capacitance, this, _1)]
					)
				>> lexeme[LEF_NO_CASE("END")] > text														[boost::phoenix::bind(&LefGrammar::layer_cbk, this, _1)]
				;
			block_macro = 
				lexeme[LEF_NO_CASE("MACRO")] >> text														[boost::phoenix::bind(&LefGrammar::macro_cbk_name, this, _1)]
				>> *(
						(lexeme[LEF_NO_CASE("CLASS")] >> text > ";")										[boost::phoenix::bind(&LefGrammar::macro_cbk_class, this, _1)]
						| (lexeme[LEF_NO_CASE("ORIGIN")] >> double_ >> double_ > ";")						[boost::phoenix::bind(&LefGrammar::macro_cbk_origin, this, _1, _2)]
						| (lexeme[LEF_NO_CASE("SIZE")] >> double_ >> lexeme[LEF_NO_CASE("BY")] >> double_ > ";") [boost::phoenix::bind(&LefGrammar::macro_cbk_size, this, _1, _2)]
						| (lexeme[LEF_NO_CASE("SITE")] >> text > ";")										[boost::phoenix::bind(&LefGrammar::macro_cbk_site, this, _1)]
						| (lexeme[LEF_NO_CASE("SYMMETRY")] >> repeat(1, 2)[text] > ";")						[boost::phoenix::bind(&LefGrammar::macro_cbk_symmetry, this, _1)]
						| (lexeme[LEF_NO_CASE("FOREIGN")] >> text >> double_ >> double_ > ";")	
						| (
								(lexeme[LEF_NO_CASE("PIN")] >> text )										[boost::phoenix::bind(&LefGrammar::macro_cbk_pin_name, this, _1)]
								>> 
								(lexeme[LEF_NO_CASE("DIRECTION")] >> text > ";")							[boost::phoenix::bind(&LefGrammar::macro_cbk_pin_direct, this, _1)]
								| (lexeme[LEF_NO_CASE("USE")] >> text > ";")								[boost::phoenix::bind(&LefGrammar::macro_cbk_pin_use, this, _1)]
								| (lexeme[LEF_NO_CASE("SHAPE")] >> text > ";")								[boost::phoenix::bind(&LefGrammar::macro_cbk_pin_shape, this, _1)]
								| (lexeme[LEF_NO_CASE("ANTENNAPARTIALMETALAREA")] >> double_ >> lexeme[LEF_NO_CASE("LAYER")] >> text > ";")
								| (lexeme[LEF_NO_CASE("ANTENNAPARTIALMETALSIDEAREA")] >> double_ >> lexeme[LEF_NO_CASE("LAYER")] >> text > ";")
								| (lexeme[LEF_NO_CASE("ANTENNAMODEL")] >> text > ";")
								| (lexeme[LEF_NO_CASE("ANTENNAGATEAREA")] >> double_ > ";")
								| (lexeme[LEF_NO_CASE("ANTENNADIFFAREA")] >> double_ > ";")
								| (lexeme[LEF_NO_CASE("PORT")] 
									>> 
									+(
										(lexeme[LEF_NO_CASE("LAYER")] >> text > ";")						[boost::phoenix::bind(&LefGrammar::macro_cbk_pin_layer, this, _1)]
										| (text >> repeat(4, inf)[double_] > ";")								[boost::phoenix::bind(&LefGrammar::macro_cbk_pin_port_shape, this, _1, _2)]
									)
									>> 
									lexeme[LEF_NO_CASE("END")])
								>> lexeme[LEF_NO_CASE("END")] >> text										[boost::phoenix::bind(&LefGrammar::macro_cbk_pin, this, _1)]
							)
						| (
								// if you need callbacks for OBS 
								// add them yourself 
								lexeme[LEF_NO_CASE("OBS")]
								>> +(
									lexeme[LEF_NO_CASE("LAYER")] >> text >> ";"
									>> 
									+(
									 // add here if there are other shapes 
									 text >> repeat(4, inf)[double_] > ";"
									 )
									>> lexeme[LEF_NO_CASE("END")]
									)
							)
						)
				>> lexeme[LEF_NO_CASE("END")] >> text														[boost::phoenix::bind(&LefGrammar::macro_cbk, this, _1)]
				;
			// you can add custom callbacks here 
			block_via_macro = 
				lexeme[LEF_NO_CASE("VIA")] >> text >> text 
				>> 
				*(
					lexeme[LEF_NO_CASE("LAYER")] >> text >> ";"
					>> text >> repeat(4, inf)[double_] >> ";"
				)
				>> lexeme[LEF_NO_CASE("END")] >> text
				; 
			block_viarule_macro = 
				lexeme[LEF_NO_CASE("VIARULE")] >> text >> text 
				>> 
				*(
					lexeme[LEF_NO_CASE("LAYER")] >> text >> ";"
					>> 
					+(
					 (text >> repeat(2, inf)[double_] >> ";")
					 | (lexeme[LEF_NO_CASE("SPACING")] >> double_ >> lexeme[LEF_NO_CASE("BY")] >> double_ >> ";")
					)
				)
				>> lexeme[LEF_NO_CASE("END")] >> text
				; 
			block_spacing = 
				lexeme[LEF_NO_CASE("SPACING")]
				>> 
				*(
					lexeme[LEF_NO_CASE("SAMENET")] >> text >> text >> double_ >> -text >> ";"
				)
				>> lexeme[LEF_NO_CASE("END")] >> lexeme[LEF_NO_CASE("SPACING")]
				; 
			text = lexeme[(char_("a-zA-Z") >> *char_("a-zA-Z_0-9-"))];
			text_nc = lexeme[+char_("a-zA-Z_0-9.-")]; // text no constraints
			
			expression.name("LEF expression");
			block_site.name("LEF block_site");
			block_other.name("LEF block_other");
			block_units.name("LEF block_units");
			block_macro.name("LEF block_macro");
			block_layer.name("LEF block_layer");
			text.name("LEF text");
			text_nc.name("LEF text_nc");

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
			// call db 
			m_db.set_lef_version(s1);
		}
		void units_cbk(int32_t const& d1)
		{
			// call db
			m_db.set_lef_unit(d1);
		}
		// callbacks for site 
		void site_cbk(string const& s1)
		{
			assert(m_site.name == s1);
			// call db 
			m_db.set_lef_site(m_site);
			// reset 
			m_site.reset();
		}
		void site_cbk_name(string const& s1)
		{
			m_site.name = s1;
		}
		void site_cbk_size(double const& d1, double const& d2)
		{
			m_site.size[0] = d1;
			m_site.size[1] = d2;
		}
		void site_cbk_class(string const& s1)
		{
			m_site.class_name = s1;
		}
		void site_cbk_symmetry(vector<string> const& s1)
		{
			m_site.vSymmetry = s1;
		}
		// callbacks for layer 
		void layer_cbk(string const& s1)
		{
			assert(m_layer.name == s1);
			// call db 
			m_db.add_lef_layer(m_layer);
			// reset 
			m_layer.reset();
		}
		void layer_cbk_name(string const& s1)
		{
			m_layer.name = s1;
		}
		void layer_cbk_type(string const& s1)
		{
			m_layer.type = s1;
		}
		void layer_cbk_direction(string const& s1)
		{
			m_layer.direct = s1;
		}
		void layer_cbk_pitch(vector<double> const& v1)
		{
			if (v1.size() < 2)
				m_layer.pitch.fill(v1[0]);
			else 
			{
				for (uint32_t i = 0; i < v1.size(); ++i)
					m_layer.pitch[i] = v1[i];
			}
		}
		void layer_cbk_offset(vector<double> const& v1)
		{
			if (v1.size() < 2)
				m_layer.offset.fill(v1[0]);
			else 
			{
				for (uint32_t i = 0; i < v1.size(); ++i)
					m_layer.offset[i] = v1[i];
			}
		}
		void layer_cbk_width(double const& d1)
		{
			m_layer.width = d1;
		}
		void layer_cbk_spacing(double const& d1)
		{
			m_layer.spacing = d1;
		}
		void layer_cbk_resistance(vector<string> const& , double const& d2)
		{
			//assert(s1 == "RPERSQ");
			m_layer.resistance = d2;
		}
		void layer_cbk_capacitance(vector<string> const& , double const& d2)
		{
			//assert(s1 == "CPERSQDIST");
			m_layer.capacitance = d2;
		}
		void layer_cbk_height(double const& d1)
		{
			m_layer.height = d1;
		}
		void layer_cbk_thickness(double const& d1)
		{
			m_layer.thickness= d1;
		}
		void layer_cbk_edge_capacitance(double const& d1)
		{
			m_layer.edge_capacitance = d1;
		}
		// callbacks for macro 
		void macro_cbk(string const& s1)
		{
			assert(m_macro.name == s1);
			// call db 
			m_db.add_lef_macro(m_macro);
			// reset 
			m_macro.reset();
		}
		void macro_cbk_name(string const& s1)
		{
			m_macro.name = s1;
		}
		void macro_cbk_class(string const& s1)
		{
			m_macro.class_name = s1;
		}
		void macro_cbk_origin(double const& d1, double const& d2)
		{
			m_macro.origin[0] = d1;
			m_macro.origin[1] = d2;
		}
		void macro_cbk_size(double const& d1, double const& d2)
		{
			m_macro.size[0] = d1;
			m_macro.size[1] = d2;
		}
		void macro_cbk_site(string const& s1)
		{
			m_macro.site_name = s1;
		}
		void macro_cbk_symmetry(vector<string> const& s1)
		{
			m_macro.vSymmetry = s1;
		}
		// callbacks for pin in macro
		void macro_cbk_pin(string const& s1)
		{
			assert(m_pin.name == s1);
			m_macro.vPin.push_back(m_pin);
			// reset 
			m_pin.reset();
		}
		void macro_cbk_pin_name(string const& s1)
		{
			m_pin.name = s1;
		}
		void macro_cbk_pin_direct(string const& s1)
		{
			m_pin.direct = s1;
		}
		void macro_cbk_pin_use(string const& s1)
		{
			assert(iequals(s1, "SIGNAL") || iequals(s1, "POWER") || iequals(s1, "GROUND") || iequals(s1, "CLOCK"));
			m_pin.use = s1;
		}
		void macro_cbk_pin_shape(string const& s1)
		{
			m_pin.shape = s1;
		}
		void macro_cbk_pin_layer(string const& s1)
		{
			m_pin.layer_name = s1;
		}
		void macro_cbk_pin_port_shape(string const& s1, vector<double> const& v2)
		{
			m_pin.port_shape = s1;
			m_pin.vBBox.assign(v2.begin(), v2.end());
			if (s1 == "RECT")
				assert(v2.size() == 4);
			else if (s1 == "POLYGON")
				assert(v2.size() >= 4);
			else assert(0);
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
	static bool read(DataBaseType& db, const string& lefFile)
	{
		std::ifstream in;

		in.open(lefFile.c_str(), std::ios::in);
		if (!in.is_open())
		{
			cout << "Unable to open input file " << lefFile << endl;
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
		LefGrammar<string::const_iterator, skip_type, DataBaseType> lg(db, error_handler);

		//TitleWriter().print("parsing lef file", 50, "#");
		//bool r = qi::phrase_parse(iter, end, lg, boost::spirit::ascii::space);
		bool r = qi::phrase_parse(iter, end, lg, skip);
		//TitleWriter().print((r && iter == end)? "parsing succeed" : "parsing failed", 50, "#");

		return r && iter == end;
	}

};

#endif 
