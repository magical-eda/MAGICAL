/**
 * @file   GdfDriver.h
 * @brief  Driver for Gdf parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef GDFPARSER_DRIVER_H
#define GDFPARSER_DRIVER_H

#include "GdfDataBase.h"

/** The example namespace is used to encapsulate the three parser classes
 * example::Parser, example::Scanner and example::Driver */
/** @brief namespace for GdfParser */
namespace GdfParser {

	using std::cout;
	using std::endl;
	using std::cerr;
	using std::string; 
	using std::vector;
	using std::pair;
	using std::make_pair;
	using std::ostringstream;

/** 
 * @class GdfParser::Driver
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
    Driver(GdfDataBase& db);

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
    GdfDataBase& m_db;

    /// @cond 
    void cellPortCbk(std::string& name, CellPort::PortTypeEnum type, std::string& layer, double x, double y);
    void cellInstanceCbk(std::string& name, std::string& cellType, double x, double y, int32_t orient);
    void textCbk(Text::TextTypeEnum textType, std::string const& name, std::string& content);
    void pathObjCbk(PathObj::PathObjTypeEnum pathObjType, std::string const& name, std::string& layer, double width, double xl, double yl, double xh, double yh);
    void pathCbk(std::string& name); 
    void netPortCbk(std::string& name, std::string& instName);
    void netPortCbk(std::string& name);
    void netCbk(std::string& name);
    /// @endcond 

    /// cellCbk is called after all the contents in the cell is initialized 
    /// this is different from some other callbacks 
    void cellCbk(std::string& name);

protected:
    /// @brief temporary storage of cell  
    Cell m_cell;
};

/// @brief API for GdfParser. 
/// Read GDF file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref GdfParser::GdfDataBase
/// @param filename GDF file 
bool read(GdfDataBase& db, const string& filename);

} // namespace example

#endif // EXAMPLE_DRIVER_H
