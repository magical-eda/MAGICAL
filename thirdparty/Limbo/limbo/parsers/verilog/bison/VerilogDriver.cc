/**
 * @file   VerilogDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref VerilogParser::Driver
 */

#include "VerilogDriver.h"
#include "VerilogScanner.h"

namespace VerilogParser {

Driver::Driver(VerilogDataBase& db)
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

void Driver::module_instance_cbk(std::string const& macro_name, std::string const& inst_name) 
{
	// due to the feature of LL 
	// wire_pin_cbk will be called before module_instance_cbk
	m_db.verilog_instance_cbk(macro_name, inst_name, m_vNetPin);
	// remember to clear m_vNetPin
	m_vNetPin.clear();
}

void Driver::assignment_cbk(std::string const& target_name, Range const& target_range, std::string const& source_name, Range const& source_range) 
{
    m_db.verilog_assignment_cbk(target_name, target_range, source_name, source_range);
}

void Driver::wire_pin_cbk(std::string& net_name, std::string& pin_name, Range const& range)
{
	m_vNetPin.push_back(NetPin(net_name, pin_name, range));
}
void Driver::wire_declare_cbk(std::vector<GeneralName> const& vNetName, Range const& range)
{
    for (std::vector<GeneralName>::const_iterator it = vNetName.begin(); it != vNetName.end(); ++it)
    {
        if (it->range.low != std::numeric_limits<int>::min() || it->range.high != std::numeric_limits<int>::min())
            std::cerr << "warning: multiple definitions of ranges " << it->name << std::endl;
        m_db.verilog_net_declare_cbk(it->name, range);
    }
}
void Driver::wire_declare_cbk(std::vector<GeneralName> const& vNetName)
{
    for (std::vector<GeneralName>::const_iterator it = vNetName.begin(); it != vNetName.end(); ++it)
        m_db.verilog_net_declare_cbk(it->name, it->range);
}
void Driver::pin_declare_cbk(std::vector<GeneralName> const& vPinName, unsigned type, Range const& range)
{
    for (std::vector<GeneralName>::const_iterator it = vPinName.begin(); it != vPinName.end(); ++it)
    {
        if (it->range.low != std::numeric_limits<int>::min() || it->range.high != std::numeric_limits<int>::min())
            std::cerr << "warning: multiple definitions of ranges " << it->name << std::endl;
        m_db.verilog_pin_declare_cbk(it->name, type, range);
    }
}
void Driver::pin_declare_cbk(std::vector<GeneralName> const& vPinName, unsigned type)
{
    for (std::vector<GeneralName>::const_iterator it = vPinName.begin(); it != vPinName.end(); ++it)
        m_db.verilog_pin_declare_cbk(it->name, type, it->range);
}

bool read(VerilogDataBase& db, const string& verilogFile)
{
	Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	return driver.parse_file(verilogFile);
}

} // namespace example
