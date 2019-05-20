/**
 * @file   verilog/test_bison.cpp
 * @brief  test verilog parser, see @ref VerilogParser::Driver and @ref VerilogParser::VerilogDataBase
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <fstream>

#include <limbo/parsers/verilog/bison/VerilogDriver.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

/// @brief Custom class that inheritates @ref VerilogParser::VerilogDataBase 
/// with all the required callbacks defined. 
class VerilogDataBase : public VerilogParser::VerilogDataBase
{
	public:
        /// @brief constructor 
		VerilogDataBase()
		{
			cout << "VerilogDataBase::" << __func__ << endl;
		}
		//////////////////// required callbacks from abstract VerilogParser::VerilogDataBase ///////////////////
        /// @brief read an instance. 
        /// 
        /// NOR2_X1 u2 ( .a(n1), .b(n3), .o(n2) );
        /// 
        /// @param macro_name standard cell type or module name 
        /// @param inst_name instance name 
        /// @param vNetPin array of pairs of net and pin 
        virtual void verilog_instance_cbk(std::string const& macro_name, std::string const& inst_name, std::vector<VerilogParser::NetPin> const& vNetPin)
        {
			cout << __func__ << " => " << macro_name << ", " << inst_name << ", ";
            for (std::vector<VerilogParser::NetPin>::const_iterator it = vNetPin.begin(); it != vNetPin.end(); ++it)
                cout << it->pin << "(" << it->net << ")" << "[" << it->range.low << ":" << it->range.high << "] ";
            cout << endl;
        }
        /// @brief read an net declaration 
        /// 
        /// wire aaa[1];
        /// 
        /// @param net_name net name 
        /// @param range net range, negative infinity if either low or high value of the range is not defined 
        virtual void verilog_net_declare_cbk(std::string const& net_name, VerilogParser::Range const& range)
		{
			cout << __func__ << " => " << net_name << " (" << range.low << ", " << range.high << ")" << endl;
		}
        /// @brief read an pin declaration 
        /// 
        /// input inp2;
        /// 
        /// @param pin_name pin name 
        /// @param type type of pin, refer to @ref VerilogParser::PinType
        /// @param range pin range, negative infinity if either low or high value of the range is not defined 
        virtual void verilog_pin_declare_cbk(std::string const& pin_name, unsigned type, VerilogParser::Range const& range)
        {
			cout << __func__ << " => " << pin_name << " " << type << " (" << range.low << ", " << range.high << ")" << endl;
        }
        /// @brief read an assignment 
        /// 
        /// assign exu_mmu_early_va_e[0] = exu_mmu_early_va_e[0];
        /// 
        /// @param target_name name of left hand side 
        /// @param target_range range of left hand side, negative infinity if either low or high value of the range is not defined 
        /// @param source_name name of right hand side 
        /// @param source_range range of right hand side, negative infinity if either low or high value of the range is not defined 
        virtual void verilog_assignment_cbk(std::string const& target_name, VerilogParser::Range const& target_range, std::string const& source_name, VerilogParser::Range const& source_range)
        {
            cout << __func__ << " => " << target_name << " (" << target_range.low << ", " << target_range.high << ")" << " = " 
                << source_name << " (" << source_range.low << ", " << source_range.high << ")" << endl;
        }
};

/// @brief test 1: use function wrapper @ref VerilogParser::read  
void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	VerilogDataBase db;
	VerilogParser::read(db, filename);
}

/// @brief test 2: use class wrapper @ref VerilogParser::Driver 
void test2(string const& filename)
{
	cout << "////////////// test2 ////////////////" << endl;
	VerilogDataBase db;
	VerilogParser::Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	driver.parse_file(filename);
}

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 
int main(int argc, char** argv)
{

	if (argc > 1)
	{
		test1(argv[1]);
		test2(argv[1]);
	}
	else 
		cout << "at least 1 argument is required" << endl;

	return 0;
}
