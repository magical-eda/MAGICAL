/**
 * @file   GdfDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref GdfParser::Driver
 */

#include "GdfDriver.h"
#include "GdfScanner.h"
#include <limbo/string/String.h>

namespace GdfParser {

Driver::Driver(GdfDataBase& db)
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

void Driver::cellPortCbk(std::string& name, CellPort::PortTypeEnum type, std::string& layer, double x, double y) 
{
    m_cell.vCellPort.push_back(CellPort());
    CellPort& cp = m_cell.vCellPort.back();
    cp.name.swap(name);
    cp.portType = type;
    cp.layer.swap(layer);
    cp.point.x = x;
    cp.point.y = y;
}

void Driver::cellInstanceCbk(std::string& name, std::string& cellType, double x, double y, int32_t orient) 
{
    m_cell.vCellInstance.push_back(CellInstance());
    CellInstance& ci = m_cell.vCellInstance.back();
    ci.name.swap(name);
    ci.cellType.swap(cellType);
    ci.position.x = x;
    ci.position.y = y;
    ci.orient = orient; 
}

void Driver::textCbk(Text::TextTypeEnum textType, std::string const& name, std::string& content) 
{
    m_cell.vText.push_back(Text());
    Text& t = m_cell.vText.back();
    t.textType = textType;
    t.name = name;
    t.content.swap(content);
}

void Driver::pathObjCbk(PathObj::PathObjTypeEnum pathObjType, std::string const& name, std::string& layer, double width, double xl, double yl, double xh, double yh)
{
    Path& p = m_cell.vPath.back();
    p.vPathObj.push_back(PathObj());
    PathObj& po = p.vPathObj.back();
    po.pathObjType = pathObjType;
    po.name = name;
    po.layer.swap(layer);
    po.width = width;
    po.startPoint.x = xl;
    po.startPoint.y = yl;
    po.endPoint.x = xh;
    po.endPoint.y = yh;
}

void Driver::pathCbk(std::string& name) 
{
    m_cell.vPath.push_back(Path());
    Path& p = m_cell.vPath.back();
    p.name.swap(name);
}

void Driver::netPortCbk(std::string& name, std::string& instName)
{
    Net& net = m_cell.vNet.back();
    net.vNetPort.push_back(NetPort());
    NetPort& np = net.vNetPort.back();
    np.name.swap(name);
    np.instName.swap(instName);
}

void Driver::netPortCbk(std::string& name)
{
    Net& net = m_cell.vNet.back();
    net.vNetPort.push_back(NetPort());
    NetPort& np = net.vNetPort.back();
    np.name.swap(name);
    np.instName.clear(); // PI/PO 
}

void Driver::netCbk(std::string& name) 
{
    m_cell.vNet.push_back(Net());
    Net& net = m_cell.vNet.back();
    net.name.swap(name);
}

void Driver::cellCbk(std::string& name) 
{
    m_cell.name.swap(name);
    m_db.add_gdf_cell(m_cell);
    m_cell.reset();
}

bool read(GdfDataBase& db, const string& filename)
{
    // start parsing 
    Driver driver (db);
    return driver.parse_file(filename);
}


} // namespace example
