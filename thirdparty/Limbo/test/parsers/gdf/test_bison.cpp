/**
 * @file   gdf/test_bison.cpp
 * @brief  test gdf parser, see @ref GdfParser::Driver and @ref GdfParser::GdfDataBase
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <fstream>

#include <limbo/parsers/gdf/bison/GdfDriver.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

/// @brief Custom class that inheritates @ref GdfParser::GdfDataBase 
/// with all the required callbacks gdfined. 
class GdfDataBase : public GdfParser::GdfDataBase
{
	public:
        /// @brief constructor 
		GdfDataBase()
		{
			cout << "GdfDataBase::" << __func__ << endl;
		}
		//////////////////// required callbacks from abstract GdfParser::GdfDataBase ///////////////////
        /// @brief add routing cell 
        /// @param cell describes the whole layout for routing 
        virtual void add_gdf_cell(GdfParser::Cell& cell) 
        {
            cout << cell << endl;
        }
};

/// @brief test 1: use function wrapper @ref GdfParser::read  
void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	GdfDataBase db;
	GdfParser::read(db, filename);
}

/// @brief test 2: use class wrapper @ref GdfParser::Driver 
void test2(string const& filename)
{
	cout << "////////////// test2 ////////////////" << endl;
	GdfDataBase db;
	GdfParser::Driver driver (db);
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
		//test2(argv[1]);
	}
	else 
		cout << "at least 1 argument is required" << endl;

	return 0;
}
