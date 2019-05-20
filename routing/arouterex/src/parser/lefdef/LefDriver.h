/**
 * @file   LefDriver.h
 * @brief  Driver for Lef parser 
 *
 * - LefParser is modified from Cadence open source parser.
 * Original version stays in C style and has very poor support
 * to object-oriented programming.
 * LefParser borrows its data structure and bison grammar, 
 * but wrap it in C++ style.
 * Please follow the [LICENSE](@ref Parsers_LefParser_License) agreement from Cadence when use the parser. 
 *
 * - The original data structure abuses malloc and free without 
 * proper initialization to pointers, which may lead to 
 * memory allocating problems.
 * Although I fixed some of the bugs and it looks good with current 
 * benchmarks, there may still contain problems with other benchmarks.
 *
 * - Another issue is that keywords may have conflicts with STRING 
 * in LefParser.yy, so I created a generalized string type GSTRING 
 * which includes all string and keywords. 
 * I did not replace all STRING with GSTRING, because it may result in 
 * performance degradation. 
 * If a new benchmark causes syntax error during parsing, then replace 
 * STRING of specific grammar with GSTRING. 
 * The main idea is to reduce the usage of GSTRING. 
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef LEFPARSER_DRIVER_H
#define LEFPARSER_DRIVER_H

#include <iostream>
#include <vector>
#include <string>
#include "LefDataBase.h"

/** The example namespace is used to encapsulate the three parser classes
 * example::Parser, example::Scanner and example::Driver */
/** @brief namespace for LefParser */
namespace LefParser {

using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;

/// forward declaration 
class LefDataBase;

/** 
 * @class LefParser::Driver
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
    Driver(LefDataBase& db);
	~Driver();

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

protected:
    /** Reference to the database filled during parsing of the
     * expressions. */
    LefDataBase& m_db;
};

/// @brief API for LefParser. 
/// Read LEF file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref LefParser::LefDataBase
/// @param lefFile LEF file 
bool read(LefDataBase& db, const string& lefFile);

} // namespace example

#endif // EXAMPLE_DRIVER_H
