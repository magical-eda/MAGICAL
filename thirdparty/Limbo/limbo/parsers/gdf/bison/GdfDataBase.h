/**
 * @file   GdfDataBase.h
 * @brief  Database for Gdf parser.  
 * 
 * See http://cadlab.cs.ucla.edu/~pubbench/routing/.index.html
 * for the format of GDF file. 
 * It is an academic format for routing. 
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef GDFPARSER_DATABASE_H
#define GDFPARSER_DATABASE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

/// namespace for GdfParser
namespace GdfParser {

/// @nowarn
using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostream;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long int64_t;
/// @endnowarn

/// @brief bison does not support vector very well, 
/// so here create a dummy class for integer array. 
class IntegerArray : public vector<int>
{
	public: 
        /// @nowarn 
		typedef vector<int> base_type;
		using base_type::size_type;
		using base_type::value_type;
		using base_type::allocator_type;
        /// @endnowarn

        /// constructor 
        /// @param alloc memory allocator 
		IntegerArray(const allocator_type& alloc = allocator_type())
			: base_type(alloc) {}
        /// constructor 
        /// @param n number of values 
        /// @param val data value
        /// @param alloc memory allocator 
		IntegerArray(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
			: base_type(n, val, alloc) {}
};

/// @brief bison does not support vector very well, 
/// so here create a dummy class for floating point number array. 
class NumberArray : public vector<double>
{
	public: 
        /// @nowarn 
		typedef vector<double> base_type;
		using base_type::size_type;
		using base_type::value_type;
		using base_type::allocator_type;
        /// @endnowarn

        /// constructor 
        /// @param alloc memory allocator 
		NumberArray(const allocator_type& alloc = allocator_type())
			: base_type(alloc) {}
        /// constructor 
        /// @param n number of values 
        /// @param val data value
        /// @param alloc memory allocator 
		NumberArray(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
			: base_type(n, val, alloc) {}
};

/// @brief bison does not support vector very well, 
/// so here create a dummy class for string array. 
class StringArray : public vector<string>
{
	public: 
        /// @nowarn 
		typedef vector<string> base_type;
		using base_type::size_type;
		using base_type::value_type;
		using base_type::allocator_type;
        /// @endnowarn

        /// constructor 
        /// @param alloc memory allocator 
		StringArray(const allocator_type& alloc = allocator_type())
			: base_type(alloc) {}
        /// constructor 
        /// @param n number of values 
        /// @param val data value
        /// @param alloc memory allocator 
		StringArray(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
			: base_type(n, val, alloc) {}
};

/// @brief Temporary data structures to hold parsed data. 
/// Base class for all temporary data structures. 
struct Item 
{
    /// print data members 
	virtual void print(ostream&) const {};
    /// print data members with stream operator 
    /// @param ss output stream 
    /// @param rhs target object 
    /// @return output stream 
	friend ostream& operator<<(ostream& ss, Item const& rhs)
	{
		rhs.print(ss);
		return ss;
	}
};

/// @brief class for point 
struct Point : public Item 
{
    double x; ///< x coordinate 
    double y; ///< y coordinate

    /// @brief constructor 
    Point() 
    {
        reset();
    }
    /// @brief reset all data members 
    void reset() 
    {
        x = y = 0.0;
    }
    /// @brief print data members 
    /// @param ss output stream 
    virtual void print(ostream& ss) const 
    {
        ss << "(" << x << "," << y << ")";
    }
};

/// @brief cell port 
struct CellPort : public Item
{
    /// @brief type of port 
    enum PortTypeEnum 
    {
        IN, 
        OUT, 
        INOUT, 
        POWER, 
        GROUND, 
        UNKNOWN
    };
    string name; ///< port name 
    string layer; ///< port layer 
    Point point; ///< port position 
    PortTypeEnum portType; ///< port type 

    /// @brief constructor 
    CellPort() 
    {
        reset();
    }
    /// @brief reset all data memory 
    void reset()
    {
        name.clear();
        layer.clear();
        point.reset();
        portType = UNKNOWN;
    }
    /// @brief print port type 
    /// @param ss output stream 
    /// @param portType port type 
    /// @return output stream 
    inline friend ostream& operator<<(ostream& ss, CellPort::PortTypeEnum portType) 
    {
        switch (portType) 
        {
            case CellPort::IN: 
                ss << "IN"; break; 
            case CellPort::OUT: 
                ss << "OUT"; break; 
            case CellPort::INOUT: 
                ss << "INOUT"; break; 
            case CellPort::POWER: 
                ss << "POWER"; break; 
            case CellPort::GROUND: 
                ss << "GROUND"; break; 
            case CellPort::UNKNOWN: 
            default:
                ss << "UNKNOWN"; break; 
        }
        return ss;
    }
    /// @brief print data members 
    /// @param ss output stream 
    virtual void print(ostream& ss) const 
    {
		ss << "//////// CellPort ////////" << endl
           << "name = " << name << endl 
           << "layer = " << layer << endl 
           << "point = " << point << endl 
           << "type = " << portType << endl;
    }
};

/// @brief cell instance 
struct CellInstance : public Item 
{
    std::string name; ///< instance name 
    std::string cellType; ///< standard cell type 
    Point position; ///< instance position 
    int32_t orient; ///< instance orientation 

    /// @brief reset all data members 
    void reset()
    {
        name.clear();
        cellType.clear();
        position.reset();
        orient = 0;
    }
    /// @brief print data members 
    /// @param ss output stream 
    virtual void print(ostream& ss) const 
    {
		ss << "//////// CellInstance ////////" << endl
           << "name = " << name << endl 
           << "cellType = " << cellType << endl 
           << "position = " << position << endl 
           << "orient = " << orient << endl;
    }
};

/// @brief object on a path 
struct PathObj : public Item 
{
    /// @brief type of path object 
    enum PathObjTypeEnum 
    {
        SEGMENT, 
        VIA, 
        STRING, 
        UNKNOWN
    };
    PathObjTypeEnum pathObjType; ///< path object type 
    std::string name; ///< only valid when pathObjType is STRING
    std::string layer; ///< layer name 
    double width; ///< width 
    Point startPoint; ///< starting point 
    Point endPoint; ///< not always valid 

    /// @brief constructor 
    PathObj()
    {
        reset();
    }
    /// @brief reset all data members 
    void reset()
    {
        pathObjType = UNKNOWN;
        name.clear();
        layer.clear();
        width = 0;
        startPoint.reset();
        endPoint.reset();
    }
};

/// @brief routing path 
struct Path : public Item
{
    std::string name; ///< path name 
    std::vector<PathObj> vPathObj; ///< objects on the path 

    /// @brief constructor 
    Path()
    {
        reset();
    }
    /// @brief reset all data members
    void reset()
    {
        name.clear();
        vPathObj.clear();
    }
    /// @brief print data members 
    /// @param ss output stream 
    virtual void print(ostream& ss) const 
    {
		ss << "//////// Path ////////" << endl
			<< "name = " << name << endl; 
        ss << "path objects\n";
		for (uint32_t i = 0; i < vPathObj.size(); ++i)
			ss << "name = " << vPathObj[i].name  << " "
               << "layer = " << vPathObj[i].layer << " "
               << "width = " << vPathObj[i].width  << " "
               << "startPoint = " << vPathObj[i].startPoint  << " "
               << "endPoint = " << vPathObj[i].endPoint << " "
               << endl;
    }
};

/// @brief text 
struct Text : public Item 
{
    /// @brief type of text 
    enum TextTypeEnum 
    {
        NUMBER_OF_LAYERS, 
        WIRE_SPACINGS, 
        VIA_SPACINGS, 
        WIRE_WIDTHS, 
        VIA_WIDTHS, 
        VERTICAL_WIRE_COSTS, 
        HORIZONTAL_WIRE_COSTS, 
        VIA_COSTS, 
        STRING, 
        UNKNOWN
    };

    TextTypeEnum textType; ///< test type 
    std::string name; ///< more information for textType
    std::string content; ///< content of text 

    /// @brief constructor 
    Text() 
    {
        reset();
    }
    /// @brief reset all data members 
    void reset()
    {
        textType = UNKNOWN;
        content.clear();
    }
    /// @brief print data members 
    /// @param ss output stream 
    virtual void print(ostream& ss) const 
    {
		ss << "//////// Text ////////" << endl
			<< "name = " << name << endl; 
        ss << "content = " << content << endl; 
    }
};

/// @brief port of net 
struct NetPort : public Item 
{
    std::string name; ///< port name 
    std::string instName; ///< corresponding instance name, empty for PI/PO  

    /// @brief constructor 
    NetPort()
    {
        reset();
    }
    /// @brief reset all data members 
    void reset()
    {
        name.clear();
        instName.clear();
    }
};
/// @brief net for interconnection 
struct Net : public Item 
{
    std::string name; ///< net name 
    std::vector<NetPort> vNetPort; ///< array of net ports 

    /// @brief constructor 
    Net() 
    {
        reset();
    }
    /// @brief reset all data members
    void reset()
    {
        name.clear();
        vNetPort.clear();
    }
    /// @brief print data members 
    /// @param ss output stream 
    virtual void print(ostream& ss) const 
    {
		ss << "//////// Net ////////" << endl
			<< "name = " << name << endl; 
		for (uint32_t i = 0; i < vNetPort.size(); ++i)
			ss << "(" << vNetPort[i].name << ", " << vNetPort[i].instName << ") ";
		ss << endl;
    }
};
/// @brief the whole routing layout is describe by a cell 
struct Cell : public Item 
{
    std::string name; ///< name 
    std::vector<CellPort> vCellPort; ///< array of ports
    std::vector<Path> vPath; ///< array of paths
    std::vector<CellInstance> vCellInstance; ///< array of instances 
    std::vector<Net> vNet; ///< interconnections
    std::vector<Text> vText; ///< texts 

    /// @brief constructor 
    Cell()
    {
        reset();
    }
    /// @brief reset all data members 
    void reset()
    {
        name.clear();
        vCellPort.clear();
        vPath.clear();
        vCellInstance.clear();
        vNet.clear();
        vText.clear();
    }
    /// @brief print data members 
    /// @param ss output stream 
    virtual void print(ostream& ss) const 
    {
        ss << "//////// Cell ////////" << endl
            << "name = " << name << endl; 
        for (uint32_t i = 0; i < vCellPort.size(); ++i)
            ss << vCellPort[i];
        for (uint32_t i = 0; i < vPath.size(); ++i)
            ss << vPath[i];
        for (uint32_t i = 0; i < vCellInstance.size(); ++i)
            ss << vCellInstance[i];
        for (uint32_t i = 0; i < vNet.size(); ++i)
            ss << vNet[i];
        for (uint32_t i = 0; i < vText.size(); ++i)
            ss << vText[i];
        ss << endl;
    }
};

// forward declaration
/// @class GdfParser::GdfDataBase
/// @brief Base class for def database. 
/// Only pure virtual functions are defined.  
/// User needs to inheritate this class and derive a custom database type with all callback functions defined.  
class GdfDataBase
{
	public:
        /// @brief Add routing cell.  
        /// It is safe to directly swap the contents in the cell for efficiency.  
        virtual void add_gdf_cell(Cell&) = 0;
};

} // namespace GdfParser

#endif
