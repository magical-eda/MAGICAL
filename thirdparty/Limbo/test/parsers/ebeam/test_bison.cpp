/**
 * @file   ebeam/test_bison.cpp
 * @brief  test ebeam parser, see @ref EbeamParser::Driver and @ref EbeamParser::EbeamDataBase
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <fstream>

#include <limbo/parsers/ebeam/bison/EbeamDriver.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

/// @brief Custom class that inheritates @ref EbeamParser::EbeamDataBase 
/// with all the required callbacks defined. 
class EbeamDataBase : public EbeamParser::EbeamDataBase
{
	public:
        /// @brief constructor 
		EbeamDataBase()
		{
			cout << "constructing EbeamDataBase" << endl;
		}
        /// @brief set database unit
        /// @param token unit  
		virtual void set_ebeam_unit(int token) 
		{
			cout << __func__ << " => " << token << endl;
		}
        /// @brief set ebeam boundary 
		virtual void set_ebeam_boundary(EbeamParser::EbeamBoundary const&) 
		{
			cout << __func__ << endl;
		}
        /// @brief set ebeam macro 
		virtual void add_ebeam_macro(EbeamParser::Macro const&) 
		{
			cout << __func__ << endl;
		}
};

/// @brief test 1: use function wrapper @ref EbeamParser::read  
void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	EbeamDataBase db;
	EbeamParser::read(db, filename);
}

/// @brief test 2: use class wrapper @ref EbeamParser::Driver 
void test2(string const& filename)
{
	cout << "////////////// test2 ////////////////" << endl;
	EbeamDataBase db;
	EbeamParser::Driver driver (db);
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
