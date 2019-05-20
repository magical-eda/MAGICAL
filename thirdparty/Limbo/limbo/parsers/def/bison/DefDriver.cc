/**
 * @file   DefDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref DefParser::Driver
 */

#include "DefDriver.h"
#include "DefScanner.h"

namespace DefParser {

Driver::Driver(DefDataBase& db)
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

void Driver::dividerchar_cbk(string const& s) 
{
	m_db.set_def_dividerchar(s);
}
void Driver::busbitchars_cbk(string const& s)
{
	m_db.set_def_busbitchars(s);
}
void Driver::version_cbk(double v) 
{
	// use stringstream to convert double to string 
	std::stringstream ss;
	ss << v;
	m_db.set_def_version(ss.str());
}
void Driver::design_cbk(string const& s) 
{
	m_db.set_def_design(s);
}
void Driver::unit_cbk(int v) 
{
	m_db.set_def_unit(v);
}
void Driver::diearea_cbk(int xl, int yl, int xh, int yh) 
{
	m_db.set_def_diearea(xl, yl, xh, yh);
}

void Driver::row_cbk(string& row_name, string& macro_name, 
		int originx, int originy, string& orient, 
		int repeatx, int repeaty, int stepx, int stepy) 
{
	std::swap(m_row.row_name, row_name); 
	std::swap(m_row.macro_name, macro_name);
	m_row.origin[0] = originx; 
	m_row.origin[1] = originy; 
	std::swap(m_row.orient, orient);
	m_row.repeat[0] = repeatx; m_row.repeat[1] = repeaty;
	m_row.step[0] = stepx; m_row.step[1] = stepy;
	m_db.add_def_row(m_row);
#ifdef DEBUG_DEFPARSER
	std::cerr << m_row << std::endl;
#endif 
	m_row.reset();
}
void Driver::track_cbk(string const& /*orient*/, int /*origin*/, 
		int /*repeat*/, int /*step*/, vector<string> const& /*vLayerName*/) 
{
	// leave it empty here
	// add something if needed
}
void Driver::gcellgrid_cbk(string const& /*orient*/, int /*origin*/, 
		int /*repeat*/, int /*step*/) 
{
	// leave it empty here
	// add something if needed
}
void Driver::component_cbk_size(int size) 
{
	m_db.resize_def_component(size);
}
void Driver::component_cbk_position(string& status, int originx, int originy, string& orient) 
{
	std::swap(m_comp.status, status);
	m_comp.origin[0] = originx; m_comp.origin[1] = originy;
	std::swap(m_comp.orient, orient);
}
void Driver::component_cbk_position(string& status) 
{
	std::swap(m_comp.status, status);
}
void Driver::component_cbk_source(string const&) 
{
	// no use 
}
void Driver::component_cbk(string& comp_name, string& macro_name) 
{
	std::swap(m_comp.comp_name, comp_name);
	std::swap(m_comp.macro_name, macro_name);
	m_db.add_def_component(m_comp);
#ifdef DEBUG_DEFPARSER
	std::cerr << m_comp << std::endl;
#endif 
	m_comp.reset();
}
void Driver::pin_cbk_size(int size) 
{
	m_db.resize_def_pin(size);
}
void Driver::pin_cbk(string& pin_name) // remember to reset in this function 
{
	std::swap(m_pin.pin_name, pin_name);
	m_db.add_def_pin(m_pin);
#ifdef DEBUG_DEFPARSER
	std::cerr << m_pin << std::endl;
#endif 
	m_pin.reset();
}
void Driver::pin_cbk_net(string& net_name)
{
	std::swap(m_pin.net_name, net_name);
}
void Driver::pin_cbk_direction(string& direct)
{
	std::swap(m_pin.direct, direct);
}
void Driver::pin_cbk_position(string& status, int originx, int originy, string& orient)
{
	std::swap(m_pin.status, status);
	m_pin.origin[0] = originx; m_pin.origin[1] = originy;
	std::swap(m_pin.orient, orient);
}
void Driver::pin_cbk_bbox(string& layer_name, int xl, int yl, int xh, int yh)
{
	std::swap(m_pin.layer_name, layer_name);
	m_pin.bbox[0] = xl; m_pin.bbox[1] = yl;
	m_pin.bbox[2] = xh; m_pin.bbox[3] = yh;
}
void Driver::pin_cbk_use(string& use)
{
	std::swap(m_pin.use, use);
}
void Driver::net_cbk_name(string& net_name) 
{
	// due to the feature of LL 
	// net_cbk_pin will be called before net_cbk_name 
	std::swap(m_net.net_name, net_name);
	std::swap(m_net.vNetPin, m_vNetPin);
	m_db.add_def_net(m_net);
#ifdef DEBUG_DEFPARSER
	std::cerr << m_net << std::endl;
#endif 
	m_net.reset();
	// remember to clear m_vNetPin
	m_vNetPin.clear();
}
void Driver::net_cbk_pin(string& node_name, string& pin_name) 
{
    m_vNetPin.push_back(pair<string, string>());
    std::swap(m_vNetPin.back().first, node_name);
    std::swap(m_vNetPin.back().second, pin_name);
}
void Driver::net_cbk_size(int size) 
{
	m_db.resize_def_net(size);
}
void Driver::blockage_cbk_size(int n) 
{
    m_db.resize_def_blockage(n);
}
void Driver::blockage_cbk_placement(int xl, int yl, int xh, int yh)
{
    m_db.add_def_placement_blockage(xl, yl, xh, yh);
}
void Driver::blockage_cbk_routing(int xl, int yl, int xh, int yh)
{
    m_db.add_def_routing_blockage(xl, yl, xh, yh);
}

bool read(DefDataBase& db, const string& defFile)
{
	Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	return driver.parse_file(defFile);
}

} // namespace example
