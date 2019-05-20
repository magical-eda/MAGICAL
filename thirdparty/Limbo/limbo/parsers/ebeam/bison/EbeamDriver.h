/**
 * @file   EbeamDriver.h
 * @brief  Driver for Ebeam parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef EBEAMPARSER_DRIVER_H
#define EBEAMPARSER_DRIVER_H

#include "EbeamDataBase.h"

/** The example namespace is used to encapsulate the three parser classes
 * example::Parser, example::Scanner and example::Driver */
/** @brief namespace for EbeamParser */
namespace EbeamParser {

using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;

/** 
 * @class EbeamParser::Driver
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
    Driver(EbeamDataBase& db);

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

    /// @cond 
	void units_cbk(int d1);
	void ebeam_cbk();
	void ebeam_cbk_offset(double d1);
	void ebeam_cbk_width(double d1);
	void ebeam_cbk_step(double d1);
	void ebeam_cbk_layerid(vector<int> const& v1);
	void ebeam_cbk_layer(vector<string> const& v1);
	void macro_cbk(string const& s1);
	void macro_cbk_name(string const& s1);
	void macro_cbk_confsite_name(string const& s1);
	void macro_cbk_confsite_layerid(int const& d1);
	void macro_cbk_confsite_layer(string const& s1);
	void macro_cbk_confsite_site(vector<int> const& v1);
	void macro_cbk_confsite(string const& s1);
    /// @endcond 
protected:
    /** Reference to the database filled during parsing of the
     * expressions. */
    EbeamDataBase& m_db;

    /// temporary storage of ebeam boundary 
	EbeamBoundary m_ebeam_boundary;
    /// temporary storage of ebeam macro for a standard cell 
	Macro m_macro;

};

/// @brief API for EbeamParser. 
/// Read Ebeam file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref EbeamParser::EbeamDataBase
/// @param ebeamFile Ebeam file 
bool read(EbeamDataBase& db, const string& ebeamFile);

} // namespace example

#endif // EXAMPLE_DRIVER_H
