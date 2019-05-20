/**
 * @file   VerilogDataBase.h
 * @brief  Database for Verilog parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef VERILOGPARSER_DATABASE_H
#define VERILOGPARSER_DATABASE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <limits>

/// namespace for VerilogParser
namespace VerilogParser {

/// @brief pin type 
enum PinType
{
    kINPUT = 0x1, 
    kOUTPUT = 0x2,
    kREG = 0x4
};

/// @brief range with pair of low and high values 
struct Range 
{
    int low; ///< low value, min infinity if not specified
    int high; ///< high value, min infinity if not specified 
    /// @brief constructor 
    Range() {low = high = std::numeric_limits<int>::min();}
    /// @brief constructor 
    /// @param l low value 
    /// @param h high value 
    Range(int l, int h) : low(l), high(h) {}
};

/// @class VerilogParser::GeneralName 
/// @brief name with a range 
struct GeneralName
{
    std::string name; ///< name string, empty if not specified
    Range range; ///< min infinity if not specified 

    /// @brief constructor 
    /// @param n name 
    GeneralName(std::string const& n = "") {name = n; range.low = range.high = std::numeric_limits<int>::min();}
    /// @brief constructor 
    /// @param n name 
    /// @param low low value of range 
    /// @param high high value of range 
    GeneralName(std::string const& n, int low, int high) {name = n; range.low = low; range.high = high;}
};

/// @brief Describe the connection of pins to nets. 
/// 
/// NOR2_X1 u2 ( .a(n1), .b(n3), .o(n2) );
///
/// Each of .a(n1), .b(n3) and .o(n2) generates an object of net and pin. 
struct NetPin
{
    std::string net; ///< net name 
    std::string pin; ///< pin name 
    Range range; ///< range of net 

    /// @brief constructor 
    /// @param n net name 
    /// @param p pin name 
    /// @param r net range 
    NetPin(std::string& n, std::string& p, Range const& r = Range())
    {
        net.swap(n);
        pin.swap(p);
        range = r;
    }
};

/// @brief bison does not support vector very well, 
/// so here create a dummy class for string array. 
class StringArray : public std::vector<std::string>
{
	public: 
        /// @nowarn 
		typedef std::vector<std::string> base_type;
		using base_type::size_type;
		using base_type::value_type;
		using base_type::allocator_type;
        /// @endnowarn

        /// @brief constructor 
        /// @param alloc memory allocator 
		StringArray(const allocator_type& alloc = allocator_type())
			: base_type(alloc) {}
        /// @brief constructor 
        /// @param n number of values 
        /// @param val data value
        /// @param alloc memory allocator 
		StringArray(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
			: base_type(n, val, alloc) {}
};

/// @brief bison does not support vector very well, 
/// so here create a dummy class for @ref VerilogParser::GeneralName array. 
class GeneralNameArray : public std::vector<GeneralName>
{
	public: 
        /// @nowarn 
		typedef std::vector<GeneralName> base_type;
		using base_type::size_type;
		using base_type::value_type;
		using base_type::allocator_type;
        /// @endnowarn

        /// @brief constructor 
        /// @param alloc memory allocator 
		GeneralNameArray(const allocator_type& alloc = allocator_type())
			: base_type(alloc) {}
        /// @brief constructor 
        /// @param n number of values 
        /// @param val data value
        /// @param alloc memory allocator 
		GeneralNameArray(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
			: base_type(n, val, alloc) {}
};

// forward declaration
/// @class VerilogParser::VerilogDataBase
/// @brief Base class for verilog database. 
/// Only pure virtual functions are defined.  
/// User needs to inheritate this class and derive a custom database type with all callback functions defined.  
class VerilogDataBase
{
	public:
        /// @brief read an instance. 
        /// 
        /// NOR2_X1 u2 ( .a(n1), .b(n3), .o(n2) );
        /// 
        /// @param macro_name standard cell type or module name 
        /// @param inst_name instance name 
        /// @param vNetPin array of pairs of net and pin 
        virtual void verilog_instance_cbk(std::string const& macro_name, std::string const& inst_name, std::vector<NetPin> const& vNetPin) = 0;
        /// @brief read an net declaration 
        /// 
        /// wire aaa[1];
        /// 
        /// @param net_name net name 
        /// @param range net range, negative infinity if either low or high value of the range is not defined 
        virtual void verilog_net_declare_cbk(std::string const& net_name, Range const& range) = 0;
        /// @brief read an pin declaration 
        /// 
        /// input inp2;
        /// 
        /// @param pin_name pin name 
        /// @param type type of pin, refer to @ref VerilogParser::PinType
        /// @param range pin range, negative infinity if either low or high value of the range is not defined 
        virtual void verilog_pin_declare_cbk(std::string const& pin_name, unsigned type, Range const& range) = 0;
        /// @brief read an assignment 
        /// 
        /// assign exu_mmu_early_va_e[0] = exu_mmu_early_va_e[0];
        /// 
        /// @param target_name name of left hand side 
        /// @param target_range range of left hand side, negative infinity if either low or high value of the range is not defined 
        /// @param source_name name of right hand side 
        /// @param source_range range of right hand side, negative infinity if either low or high value of the range is not defined 
        virtual void verilog_assignment_cbk(std::string const& target_name, Range const& target_range, std::string const& source_name, Range const& source_range); 

    protected:
        /// @brief remind users to define some optional callback functions at runtime 
        /// @param str message including the information to the callback function in the reminder 
        void verilog_user_cbk_reminder(const char* str) const; 
};

} // namespace VerilogParser

#endif
