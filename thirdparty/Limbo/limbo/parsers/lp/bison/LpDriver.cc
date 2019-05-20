/**
 * @file   LpDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref LpParser::Driver
 */

#include <limits>
#include "LpDriver.h"
#include "LpScanner.h"

namespace LpParser {

Driver::Driver(LpDataBase& db)
    : trace_scanning(false),
      trace_parsing(false),
      m_db(db)
{
}

bool Driver::parse_stream(std::istream& in, const std::string& sname)
{
    streamname = sname;

    Scanner scanner(&in);
    scanner.set_debug(trace_scanning);
    this->lexer = &scanner;

    Parser parser(*this);
    parser.set_debug_level(trace_parsing);
    return (parser.parse() == 0);
}

bool Driver::parse_file(const std::string &filename)
{
    std::ifstream in(filename.c_str());
    if (!in.good()) return false;
    return parse_stream(in, filename);
}

bool Driver::parse_string(const std::string &input, const std::string& sname)
{
    std::istringstream iss(input);
    return parse_stream(iss, sname);
}

void Driver::error(const class location& l,
		   const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}

void Driver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}

// sum coef*var compare_op constant 
void Driver::obj_cbk(bool minimize, TermArray const& terms) 
{
    m_db.add_objective(minimize, terms); 
}
void Driver::constraint_cbk(TermArray& terms, char compare, double constant)
{
    m_db.add_constraint("", terms, compare, constant); 
}
void Driver::constraint_cbk(string const& name, TermArray& terms, char compare, double constant)
{
    m_db.add_constraint(name, terms, compare, constant); 
}
// var compare_op constant 
void Driver::bound_cbk(string const& var, char compare, double constant)
{
    double lb = limbo::lowest<double>(); 
    double ub = std::numeric_limits<double>::max(); 

    switch (compare)
    { 
        case '>':
            lb = constant; 
            break; 
        case '<':
            ub = constant; 
            break; 
        case '=':
        default:
            lb = ub = constant; 
    }
    m_db.add_variable(var, lb, ub);
}
// constant compare_op var
void Driver::bound_cbk(double constant, char compare, string const& var) 
{
    double lb = limbo::lowest<double>(); 
    double ub = std::numeric_limits<double>::max(); 

    switch (compare)
    { 
        case '>':
            ub = constant; 
            break; 
        case '<':
            lb = constant; 
            break; 
        case '=':
        default:
            lb = ub = constant; 
            break; 
    }
    m_db.add_variable(var, lb, ub);
}
// constant1 compare_op1 var compare_op2 constant2 
void Driver::bound_cbk(double constant1, char compare1, string const& var, char compare2, double constant2)
{
    double lb = limbo::lowest<double>(); 
    double ub = std::numeric_limits<double>::max(); 

    // compare1 is different from compare2 
    switch (compare1)
    { 
        case '>':
            ub = constant1; 
            break; 
        case '<':
            lb = constant1; 
            break; 
        case '=':
        default:
            lb = ub = constant1; 
            break; 
    }
    switch (compare2)
    { 
        case '>':
            lb = constant2; 
            break; 
        case '<':
            ub = constant2; 
            break; 
        case '=':
        default:
            lb = ub = constant2; 
            break; 
    }
    m_db.add_variable(var, lb, ub);
}
// generals type (integer)
void Driver::generals_cbk(StringArray const& vIntegerVar)
{
    for (StringArray::const_iterator it = vIntegerVar.begin(); it != vIntegerVar.end(); ++it)
        m_db.set_integer(*it, false);
}
// binary type  
void Driver::binary_cbk(StringArray const& vIntegerVar)
{
    for (StringArray::const_iterator it = vIntegerVar.begin(); it != vIntegerVar.end(); ++it)
        m_db.set_integer(*it, true);
}

bool read(LpDataBase& db, const string& lpFile)
{
	Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	return driver.parse_file(lpFile);
}

} // namespace example
