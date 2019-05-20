/**
 * @file   EbeamDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref EbeamParser::Driver
 */

#include "EbeamDriver.h"
#include "EbeamScanner.h"

namespace EbeamParser {

Driver::Driver(EbeamDataBase& db)
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

//////////////////////////////////// callbacks //////////////////////////////////
void Driver::units_cbk(int d1)
{
	//std::cerr << __func__ << std::endl;
	// call db
	m_db.set_ebeam_unit(d1);
#ifdef DEBUG_EBEAMPARSER
	std::cerr << "ebeam unit = " << d1 << std::endl;
#endif 
}
void Driver::ebeam_cbk()
{
	//std::cerr << __func__ << std::endl;
	// call db
	m_db.set_ebeam_boundary(m_ebeam_boundary);
#ifdef DEBUG_EBEAMPARSER
	std::cerr << m_ebeam_boundary << std::endl;
#endif 
	m_ebeam_boundary.reset();
}
void Driver::ebeam_cbk_offset(double d1)
{
	//std::cerr << __func__ << std::endl;
	m_ebeam_boundary.offset = d1;
}
void Driver::ebeam_cbk_width(double d1)
{
	//std::cerr << __func__ << std::endl;
	m_ebeam_boundary.width = d1;
}
void Driver::ebeam_cbk_step(double d1)
{
	//std::cerr << __func__ << std::endl;
	m_ebeam_boundary.step = d1;
}
void Driver::ebeam_cbk_layerid(vector<int> const& v1)
{
	//std::cerr << __func__ << std::endl;
	m_ebeam_boundary.vLayerId = v1;
}
void Driver::ebeam_cbk_layer(vector<string> const& v1)
{
	//std::cerr << __func__ << std::endl;
	m_ebeam_boundary.vLayer = v1;
}
// callbacks for macro 
void Driver::macro_cbk(string const& s1)
{
	//std::cerr << __func__ << std::endl;
	assert(m_macro.macro_name == s1);
	// call db 
	m_db.add_ebeam_macro(m_macro);
#ifdef DEBUG_EBEAMPARSER
	std::cerr << m_macro << std::endl;
#endif 
	// reset 
	m_macro.reset();
}
void Driver::macro_cbk_name(string const& s1)
{
	//std::cerr << __func__ << std::endl;
	m_macro.macro_name = s1;
}
void Driver::macro_cbk_confsite_name(string const& s1)
{
	//std::cerr << __func__ << std::endl;
	m_macro.vConfSite.push_back(ConfSite());
	m_macro.vConfSite.back().confsite_name = s1;
}
void Driver::macro_cbk_confsite_layerid(int const& d1)
{
	//std::cerr << __func__ << std::endl;
	m_macro.vConfSite.back().layer_id = d1;
}
void Driver::macro_cbk_confsite_layer(string const& s1)
{
	//std::cerr << __func__ << std::endl;
	m_macro.vConfSite.back().layer = s1;
}
void Driver::macro_cbk_confsite_site(vector<int> const& v1)
{
	//std::cerr << __func__ << std::endl;
	m_macro.vConfSite.back().vSiteCnt = v1;
}
void Driver::macro_cbk_confsite(string const& s1)
{
	//std::cerr << __func__ << std::endl;
	assert(m_macro.vConfSite.back().confsite_name == s1);
}

//////////////////////////////////// api //////////////////////////////////
bool read(EbeamDataBase& db, const string& ebeamFile)
{
	Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	return driver.parse_file(ebeamFile);
}

} // namespace example
