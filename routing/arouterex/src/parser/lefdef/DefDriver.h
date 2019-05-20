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

    /// accessors for temporary data members 
    /// @cond 
    Row const& row() const {return m_row;}
    Row& row() {return m_row;}
    Component const& component() const {return m_comp;}
    Component& component() {return m_comp;}
    Pin const& pin() const {return m_pin;}
    Pin& pin() {return m_pin;}
    Net const& net() const {return m_net;}
    Net& net() {return m_net;}
    Track const& track() const {return m_track;}
    Track& track() {return m_track;}
    Snetp const& snetp() const {return m_snetp;}
    Snetp& snetp() {return m_snetp;}
    Region const& region() const {return m_region;}
    Region& region() {return m_region;}
    Group const& group() const {return m_group;}
    Group& group() {return m_group;}

    Via const &via() const { return m_via; }

    Via &via() { return m_via; }
    /// @endcond 
protected:
    /** Reference to the database filled during parsing of the
     * expressions. */
    DefDataBase& m_db;

    /// @brief temporary row 
	Row m_row;
    /// @brief temporary component 
	Component m_comp;
    /// @brief temporary pin 
	Pin m_pin;
    /// @brief temporary net 
	Net m_net;

    Track m_track;

    Snetp m_snetp;

    Region m_region;

    Group m_group;

    Via m_via;
};

/// @brief API for DefParser. 
/// Read DEF file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref DefParser::DefDataBase
/// @param defFile DEF file 
bool read(DefDataBase& db, const string& defFile);

} // namespace example

#endif // EXAMPLE_DRIVER_H
