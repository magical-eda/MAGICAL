/**
 * @file   EbeamDataBase.h
 * @brief  Database for Ebeam parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef EBEAMPARSER_DATABASE_H
#define EBEAMPARSER_DATABASE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

/// namespace for EbeamParser
namespace EbeamParser {

/// @nowarn
using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;
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
	virtual void print(ostringstream&) const {};
    /// print data members with stream operator 
    /// @param os output stream 
    /// @param rhs target object 
    /// @return output stream 
	friend std::ostream& operator<<(std::ostream& os, Item const& rhs)
	{
		std::ostringstream ss;
		rhs.print(ss);
		os << ss.str();
		return os;
	}
    /// print data members with stream operator 
    /// @param ss output stream 
    /// @param rhs target object 
    /// @return output stream 
	friend ostringstream& operator<<(ostringstream& ss, Item const& rhs)
	{
		rhs.print(ss);
		return ss;
	}
};
/// @brief describe ebeam boundary 
struct EbeamBoundary : public Item
{
	double offset; ///< offset of beam 
	double width; ///< width of beam 
	double step; ///< step of beam 
	vector<int32_t> vLayerId; ///< array of layer id 
	vector<string> vLayer; ///< array of layer name 

    /// @brief constructor 
	EbeamBoundary() {reset();}
    /// @brief reset all data members 
	void reset()
	{
		offset = 0;
		width = 0;
		step = 0;
		vLayerId.clear();
		vLayer.clear();
	}
    /// @brief print data members 
    /// @param ss output stream 
	virtual void print(ostringstream& ss) const
	{
		ss << "//////// EbeamBoundary ////////" << endl
			<< "offset = " << offset << endl 
			<< "width = " << width << endl 
			<< "step = " << step << endl
			<< "vLayerId = ";
		for (vector<int32_t>::const_iterator it = vLayerId.begin(); it != vLayerId.end(); ++it)
			ss << *it << "\t";
		ss << endl;
		ss << "vLayer = ";
		for (vector<string>::const_iterator it = vLayer.begin(); it != vLayer.end(); ++it)
			ss << *it << "\t";
	}
};
/// @brief describe conflict sites 
struct ConfSite : public Item
{
	string confsite_name; ///< name 
	int32_t layer_id; ///< layer id 
	string layer; ///< layer name 
	vector<int32_t> vSiteCnt; ///< array of site indices that are conflicted 

    /// @brief constructor 
	ConfSite() {reset();}
    /// @brief reset all data members
	void reset()
	{
		confsite_name = "";
		layer_id = -1;
		layer = "";
		vSiteCnt.clear();
	}
    /// @brief print data members 
    /// @param ss output stream 
	virtual void print(ostringstream& ss) const
	{
		ss << "//////// ConfSite ////////" << endl;
		ss << "confsite_name = " << confsite_name << endl;
		ss << "layer_id = " << layer_id << endl;
		ss << "layer = " << layer << endl;
		ss << "vSiteCnt = ";
		for (vector<int32_t>::const_iterator it = vSiteCnt.begin(); it != vSiteCnt.end(); ++it)
			ss << *it << "\t";
		ss << endl;
	}
};
/// @brief describe conflict sites for each standard cell type 
struct Macro : public Item 
{
	string macro_name; ///< standard cell type 
	vector<ConfSite> vConfSite; ///< array of conflict sites at different layers 
    /// @brief reset all data members
	void reset()
	{
		macro_name = "";
		vConfSite.clear();
	}
    /// @brief print data members 
    /// @param ss output stream 
	virtual void print(ostringstream& ss) const
	{
		ss << "//////// Macro ////////" << endl;
		ss << "macro_name = " << macro_name << endl;
		for (vector<ConfSite>::const_iterator it = vConfSite.begin(); it != vConfSite.end(); ++it)
			ss << *it << endl;
	}
};

// forward declaration
/// @brief Base class for ebeam database. 
/// Only pure virtual functions are defined.  
/// User needs to inheritate this class and derive a custom database type with all callback functions defined.  
class EbeamDataBase
{
	public:
        /// @brief set unit of micron in database, 
        /// e.g., unit 1000 denotes 1000 in database unit is equal to 1 micron 
		virtual void set_ebeam_unit(int) = 0;
        /// @brief set ebeam boundary 
		virtual void set_ebeam_boundary(EbeamBoundary const&) = 0;
        /// @brief add ebeam macro for each standard cell 
		virtual void add_ebeam_macro(Macro const&) = 0;
};

} // namespace EbeamParser

#endif
