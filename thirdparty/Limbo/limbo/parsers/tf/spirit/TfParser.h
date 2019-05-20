/**
 * @file   TfParser.h
 * @brief  tf parser for technology file 
 * @author Yibo Lin
 * @date   Sep 2014
 */

#ifndef _TFPARSER_H
#define _TFPARSER_H

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
#include <limbo/parsers/tf/spirit/ErrorHandler.h>
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

/// @brief an option to control case insensitivity
#ifdef CASE_INSENSITIVE
	#define TF_NO_CASE(X) no_case[X]
#else 
	#define TF_NO_CASE(X) X
#endif 

/// @class TfParser 
/// @brief Tf parser for technology file. 
/// 
/// Developed with Boost.Spirit 
struct TfParser
{
    /// @class TfParser::TfDataBase 
    /// @brief Base class for tf database. 
    /// Only pure virtual functions are defined.  
    /// User needs to inheritate this class and derive a custom database type with all callback functions defined.  
    /// 
    /// Currently only read layer mapping. 
    /// @todo parse other information besides layer mapping in tf file 
	struct TfDataBase 
	{
		// required callbacks 
        /// @brief add layer name, layer id, layer abbreviation 
        /// @param s1 layer name 
        /// @param s2 layer id 
        /// @param s3 layer abbreviation 
		virtual void add_tf_layer_id(string const& s1, int32_t const& s2, string const& s3) = 0;
	};

	/// @brief define grammar 
    /// @tparam Iterator iterator of text 
    /// @tparam Skipper skip some contens 
    /// @tparam DataBaseType database to store the parsed content 
	template <typename Iterator, typename Skipper, typename DataBaseType>
	struct TfGrammar : qi::grammar<Iterator, Skipper>
	{
		qi::rule<Iterator, Skipper> expression; ///< expression 
		qi::rule<Iterator, Skipper> block_defn; ///< layer definitions
		qi::rule<Iterator, Skipper> block_layer_id; ///< layer id 
		qi::rule<Iterator, Skipper> block_layer_purpose; ///< layer purpose 
		qi::rule<Iterator, Skipper> block_display; ///< technology display 
		qi::rule<Iterator, string(), Skipper> text; ///< text 
		qi::rule<Iterator, string(), Skipper> text_nc; ///< no constraints

		DataBaseType& m_db; ///< reference to database 

        /// @brief constructor 
        /// @param db database 
        /// @param error_handler error handler 
		TfGrammar(DataBaseType& db, ErrorHandler<Iterator>& error_handler) : TfGrammar::base_type(expression), m_db(db)
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
				*block_defn
				;
			block_defn = lexeme[TF_NO_CASE("layerDefinitions")] 
				>> lit("(") 
				>> *(
						block_layer_id
						| block_layer_purpose
						| block_display
					)
				>> lit(")")
				;
			block_layer_id = lexeme[TF_NO_CASE("techLayers")]
				>> lit("(")
				>> *(
						(lit("(") >> text >> int_ >> text > lit(")"))									[boost::phoenix::bind(&TfGrammar::layer_id_cbk, this, _1, _2, _3)]
					)
				>> lit(")")
				;
			block_layer_purpose = lexeme[TF_NO_CASE("techLayerPurposePriorities")]
				>> lit("(")
				>> *(
						(lit("(") >> text >> text > lit(")"))
					)
				>> lit(")")
				;
			block_display = lexeme[TF_NO_CASE("techDisplays")]
				>> lit("(")
				>> *(
						(lit("(") >> text >> text >> text >> repeat(5)[char_] > lit(")"))
					)
				>> lit(")")
				;

			text = lexeme[(char_("a-zA-Z") >> *char_("a-zA-Z_0-9-"))];
			text_nc = lexeme[+char_("a-zA-Z_0-9.-")]; // text no constraints

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

        /// @brief callback of layer id 
        /// @param s1 layer name 
        /// @param d2 layer id 
        /// @param s3 layer abbreviation
		void layer_id_cbk(string const& s1, int32_t const& d2, string const& s3)
		{
			m_db.add_tf_layer_id(s1, d2, s3);
		}
	};
    /// @brief grammar of skipper 
    /// @param Iterator iterator of text 
	template <typename Iterator>
	struct SkipperGrammar : qi::grammar<Iterator>
	{
		qi::rule<Iterator> skip; ///< grammar to skip text 

        /// @brief constructor 
		SkipperGrammar() : SkipperGrammar::base_type(skip)
		{
			using qi::char_;
			using qi::eol;
			using spirit::lit;

			skip = ascii::space 
				| (";" >> *(char_ - eol) >> eol )
				;
		}
	};
    /// @brief API to read tf file 
    /// @param db database 
    /// @param tfFile tf file 
	template <typename DataBaseType>
	static bool read(DataBaseType& db, const string& tfFile)
	{
		std::ifstream in;

		in.open(tfFile.c_str(), std::ios::in);
		if (!in.is_open())
		{
			cout << "Unable to open input file " << tfFile << endl;
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
		TfGrammar<string::const_iterator, skip_type, DataBaseType> lg(db, error_handler);

		//TitleWriter().print("parsing tf file", 50, "#");
		//bool r = qi::phrase_parse(iter, end, lg, boost::spirit::ascii::space);
		bool r = qi::phrase_parse(iter, end, lg, skip);
		//TitleWriter().print((r && iter == end)? "parsing succeed" : "parsing failed", 50, "#");

		return r && iter == end;
	}
};

#endif 
