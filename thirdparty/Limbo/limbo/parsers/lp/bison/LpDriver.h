/**
 * @file   LpDriver.h
 * @brief  Driver for Lp parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef LPPARSER_DRIVER_H
#define LPPARSER_DRIVER_H

#include "LpDataBase.h"

/** The example namespace is used to encapsulate the three parser classes
 * example::Parser, example::Scanner and example::Driver */
/** @brief namespace for LpParser */
namespace LpParser {

using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;

/** 
 * @class LpParser::Driver
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
    Driver(LpDataBase& db);

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
    LpDataBase& m_db;

    /// @cond 
	// coef*var 
	void obj_cbk(bool minimize, TermArray const& terms);
    // sum coef*var compare_op constant 
	void constraint_cbk(TermArray& terms, char compare, double constant);
    // sum coef*var compare_op constant 
	void constraint_cbk(string const& name, TermArray& terms, char compare, double constant);
	// var compare_op constant 
	void bound_cbk(string const& var, char compare, double constant);
	// constant compare_op var
	void bound_cbk(double constant, char compare, string const& var);
    // constant1 compare_op1 var compare_op2 constant2 
	void bound_cbk(double constant1, char compare1, string const& var, char compare2, double constant2);
	// generals type (integer)
	void generals_cbk(StringArray const&);
	// binary type  
	void binary_cbk(StringArray const&);
    /// @endcond 
protected:
};

/// @brief API for LpParser. 
/// Read LP file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref LpParser::LpDataBase
/// @param lpFile LP file 
bool read(LpDataBase& db, const string& lpFile);

} // namespace example

#endif // EXAMPLE_DRIVER_H
