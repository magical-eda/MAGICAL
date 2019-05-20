/**
 * @file   BookshelfDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref BookshelfParser::Driver
 */

#include "BookshelfDriver.h"
#include "BookshelfScanner.h"
#include <limbo/string/String.h>
#include <algorithm>
#if ZLIB == 1 
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#endif

namespace BookshelfParser {

Driver::Driver(BookshelfDataBase& db)
    : trace_scanning(false),
      trace_parsing(false),
      m_db(db), 
      m_plFlag(false)
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
#if ZLIB == 1
    if (limbo::get_file_suffix(filename) == "gz") // detect .gz file 
    {
        boost::iostreams::filtering_istream in; 
        in.push(boost::iostreams::gzip_decompressor());
        in.push(boost::iostreams::file_source(filename.c_str()));

        if (!in.good()) return false;
        return parse_stream(in, filename);
    }
#endif
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

// control m_plFlag
void Driver::setPlFlag(bool flag) 
{
    m_plFlag = flag;
}

// .nodes file 
void Driver::numNodeTerminalsCbk(int nn, int nt)
{
    m_db.resize_bookshelf_node_terminals(nn, nt);
}
void Driver::terminalEntryCbk(string& name, int w, int h)
{
    m_db.add_bookshelf_terminal(name, w, h);
}
void Driver::nodeEntryCbk(string& name, int w, int h, string&)
{
    m_db.add_bookshelf_node(name, w, h);
}
void Driver::nodeEntryCbk(string& name, int w, int h)
{
    m_db.add_bookshelf_node(name, w, h);
}
// .nets file 
void Driver::numNetCbk(int n)
{
    m_db.resize_bookshelf_net(n);
}
void Driver::numPinCbk(int n)
{
    m_db.resize_bookshelf_pin(n);
}
void Driver::netPinEntryCbk(string& node_name, char direct, double offsetX, double offsetY, double w, double h, string& pin_name)
{
    // not sure whether w or h has the correct meaning 
    m_net.vNetPin.push_back(NetPin(node_name, direct, offsetX, offsetY, w, h, pin_name));
}
void Driver::netPinEntryCbk(string& node_name, char direct, double offsetX, double offsetY, double w, double h)
{
    // not sure whether w or h has the correct meaning 
    m_net.vNetPin.push_back(NetPin(node_name, direct, offsetX, offsetY, w, h));
}
void Driver::netNameAndDegreeCbk(string& net_name, int degree)
{
    m_net.net_name.swap(net_name);
    m_net.vNetPin.clear();
    m_net.vNetPin.reserve(degree);
}
void Driver::netEntryCbk()
{
    m_db.add_bookshelf_net(m_net);
    m_net.reset();
}
// .pl file 
void Driver::plNodeEntryCbk(string& node_name, double x, double y, string& orient, string& status)
{
    m_db.set_bookshelf_node_position(node_name, x, y, orient, status, m_plFlag);
}
void Driver::plNodeEntryCbk(string& node_name, double x, double y, string& orient)
{
    m_db.set_bookshelf_node_position(node_name, x, y, orient, "", m_plFlag);
}
// .scl file 
void Driver::sclNumRows(int n)
{
    m_db.resize_bookshelf_row(n);
}
void Driver::sclCoreRowStart(string const& orient)
{
    m_row.orient = orient;
}
void Driver::sclCoreRowCoordinate(int y)
{
    m_row.origin[1] = y;
}
void Driver::sclCoreRowHeight(int h)
{
    m_row.height = h;
}
void Driver::sclCoreRowSitewidth(int site_width)
{
    m_row.site_width = site_width;
}
void Driver::sclCoreRowSitespacing(int site_spacing)
{
    m_row.site_spacing = site_spacing;
}
void Driver::sclCoreRowSiteorient(int site_orient)
{
    m_row.site_orient = site_orient;
}
void Driver::sclCoreRowSitesymmetry(int site_symmetry)
{
    m_row.site_symmetry = site_symmetry;
}
void Driver::sclCoreRowSubRowOrigin(int x)
{
    m_row.origin[0] = x;
}
void Driver::sclCoreRowNumSites(int site_num)
{
    m_row.site_num = site_num;
}
void Driver::sclCoreRowEnd()
{
    m_db.add_bookshelf_row(m_row);
    m_row.reset();
}
// .wts file 
// .aux file 
void Driver::auxCbk(string& design_name, vector<string>& vBookshelfFiles)
{
    m_db.set_bookshelf_design(design_name);
    m_vBookshelfFiles.swap(vBookshelfFiles);
}

/// a local helper function 
struct SortByPairFirst
{
    inline bool operator()(std::pair<int, int> const& p1, std::pair<int, int> const& p2) const 
    {
        return p1.first < p2.first; 
    }
};

bool read(BookshelfDataBase& db, const string& auxFile)
{
    // first read .aux 
	Driver driverAux (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

    bool gzFlag = limbo::iequals(limbo::get_file_suffix(auxFile), "gz"); // compressed or not 
#if ZLIB == 0
    if (gzFlag)
    {
        std::cerr << "compile with ZLIB_DIR defined to read .gz files\n";
        return false; 
    }
#endif
	bool flagAux = driverAux.parse_file(auxFile);
    if (!flagAux)
        return false;

    string auxPath = limbo::get_file_path(auxFile);

    // (visit_order, index)
    vector<std::pair<int, int> > vOrder (driverAux.bookshelfFiles().size());
    for (unsigned i = 0; i < vOrder.size(); ++i)
    {
        string const& filename = driverAux.bookshelfFiles().at(i);
        string suffix = limbo::get_file_suffix(filename);
        if (limbo::iequals(suffix, "scl"))
            vOrder[i].first = 0;
        else if (limbo::iequals(suffix, "nodes"))
            vOrder[i].first = 1;
        else if (limbo::iequals(suffix, "nets"))
            vOrder[i].first = 2;
        else if (limbo::iequals(suffix, "wts"))
            vOrder[i].first = 3;
        else if (limbo::iequals(suffix, "pl"))
            vOrder[i].first = 4;
        else 
            vOrder[i].first = vOrder.size();
        vOrder[i].second = i;
    }
    // order by visit_order 
    std::sort(vOrder.begin(), vOrder.end(), SortByPairFirst());

    // start parsing 
    for (vector<std::pair<int, int> >::const_iterator it = vOrder.begin(); it != vOrder.end(); ++it)
    {
        std::pair<int, int> const& order = *it;
        string filename = driverAux.bookshelfFiles().at(order.second);
        if (gzFlag && !limbo::iequals(limbo::get_file_suffix(filename), "gz"))
        {
            filename += ".gz";
        }

        Driver driver (db);
        bool flag = driver.parse_file(auxPath + "/" + filename);
        if (!flag)
            return false;
    }
    // inform database that parsing is completed 
    db.bookshelf_end(); 

    return true;
}

/// read .pl file only, the callback only provide positions and orientation 
bool readPl(BookshelfDataBase& db, const string& plFile)
{
    // start parsing 
    string const& filename = plFile;

    Driver driver (db);
    driver.setPlFlag(true);
    bool flag = driver.parse_file(filename);
    if (!flag)
        return false;
    // do not inform the ending 
    // inform database that parsing is completed 
    //db.bookshelf_end(); 

    return true;
}


} // namespace example
