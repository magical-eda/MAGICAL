/**
 * @file   VerilogDriver.h
 * @brief  Driver for Verilog parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef VERILOGPARSER_DRIVER_H
#define VERILOGPARSER_DRIVER_H

#include "VerilogDataBase.h"

/** The example namespace is used to encapsulate the three parser classes
 * example::Parser, example::Scanner and example::Driver */
/** @brief namespace for VerilogParser */
namespace VerilogParser {

using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;

/** 
 * @class VerilogParser::Driver
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
    Driver(VerilogDataBase& db);

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
    VerilogDataBase& m_db;

    /// @cond
    void wire_pin_cbk(std::string&, std::string&, Range const& = Range());
    void wire_declare_cbk(std::vector<GeneralName> const&, Range const&);
    void wire_declare_cbk(std::vector<GeneralName> const& vNetName);
    void pin_declare_cbk(std::vector<GeneralName> const&, unsigned, Range const&);
    void pin_declare_cbk(std::vector<GeneralName> const&, unsigned);
    void module_instance_cbk(std::string const&, std::string const&);
    void assignment_cbk(std::string const&, Range const&, std::string const&, Range const&); 
    /// @endcond

protected:
	/// @brief Use as a stack for node and pin pairs in a net,  
	/// because wire_pin_cbk will be called before module_instance_cbk
	vector<NetPin> m_vNetPin;
};

/// @brief API for VerilogParser. 
/// Read Verilog file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref VerilogParser::VerilogDataBase
/// @param verilogFile Verilog file 
bool read(VerilogDataBase& db, const string& verilogFile);

} // namespace example

#endif // EXAMPLE_DRIVER_H
