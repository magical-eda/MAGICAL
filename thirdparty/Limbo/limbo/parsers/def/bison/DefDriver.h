/**
 * @file   DefDriver.h
 * @brief  Driver for Def parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef DEFPARSER_DRIVER_H
#define DEFPARSER_DRIVER_H

#include "DefDataBase.h"

/** The example namespace is used to encapsulate the three parser classes
 * example::Parser, example::Scanner and example::Driver */
/** @brief namespace for DefParser */
namespace DefParser {

using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;

/** 
 * @class DefParser::Driver
 * The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class Driver
{
public:
    /// construct a new parser driver context
    /// @param db reference to database 
    Driver(DefDataBase& db);

    /// enable debug output in the flex scanner
    bool trace_scanning;

    /// enable debug output in the bison parser
    bool trace_parsing;

    /// stream name (file or input stream) used for error messages.
    string streamname;

    /** Invoke the scanner and parser for a stream.
     * @param in	input stream
     * @param sname	stream name for error messages
     * @return		true if successfully parsed
     */
    bool parse_stream(std::istream& in,
		      const string& sname = "stream input");

    /** Invoke the scanner and parser on an input string.
     * @param input	input string
     * @param sname	stream name for error messages
     * @return		true if successfully parsed
     */
    bool parse_string(const string& input,
		      const string& sname = "string stream");

    /** Invoke the scanner and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename	input file name
     * @return		true if successfully parsed
     */
    bool parse_file(const string& filename);

    // To demonstrate pure handling of parse errors, instead of
    // simply dumping them on the standard error output, we will pass
    // them to the driver using the following two member functions.

    /** Error handling with associated line number. This can be modified to
     * output the error e.g. to a dialog box. */
    void error(const class location& l, const string& m);

    /** General error handling. This can be modified to output the error
     * e.g. to a dialog box. */
    void error(const string& m);

    /** Pointer to the current lexer instance, this is used to connect the
     * parser to the scanner. It is used in the yylex macro. */
    class Scanner* lexer;

    /** Reference to the database filled during parsing of the
     * expressions. */
    DefDataBase& m_db;

    /// @cond
	void dividerchar_cbk(string const&) ;
	void busbitchars_cbk(string const&) ;
	void version_cbk(double) ;
	void design_cbk(string const&) ;
	void unit_cbk(int) ;
	void diearea_cbk(int, int, int, int) ;

	void row_cbk(string&, string&, int, int, string&, int, int, int, int) ;
	void track_cbk(string const&, int, int, int, vector<string> const&) ;
	void gcellgrid_cbk(string const&, int, int, int) ;
	// component cbk
	void component_cbk_size(int) ;
	void component_cbk_position(string&, int, int, string&) ;
	void component_cbk_position(string&) ;
	void component_cbk_source(string const&) ;
	void component_cbk(string&, string&) ;

	// pin cbk 
//	void pin_cbk(string const&, string const&, string const&, string const&, 
//			int, int, string const&, string const&, int, int, int, int) ;
	void pin_cbk_size(int) ;
	void pin_cbk(string&); // remember to reset in this function 
	void pin_cbk_net(string&);
	void pin_cbk_direction(string&);
	void pin_cbk_position(string&, int, int, string&);
	void pin_cbk_bbox(string&, int, int, int, int);
	void pin_cbk_use(string&);
	// net cbk 
	void net_cbk_name(string&) ;
	void net_cbk_pin(string&, string&) ;
	void net_cbk_size(int) ;
    // blockage cbk 
    void blockage_cbk_size(int);
    void blockage_cbk_placement(int, int, int, int);
    void blockage_cbk_routing(int, int, int, int);
    /// @endcond

protected:
	/// @brief Use as a stack for node and pin pairs in a net,  
	/// because net_cbk_pin will be called before net_cbk_name
	vector<pair<string, string> > m_vNetPin;
    /// @brief temporary row 
	Row m_row;
    /// @brief temporary component 
	Component m_comp;
    /// @brief temporary pin 
	Pin m_pin;
    /// @brief temporary net 
	Net m_net;
};

/// @brief API for DefParser. 
/// Read DEF file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref DefParser::DefDataBase
/// @param defFile DEF file 
bool read(DefDataBase& db, const string& defFile);

} // namespace example

#endif // EXAMPLE_DRIVER_H
