/**
 * @file   tf/test_spirit.cpp
 * @brief  test tf parser, see @ref TfParser::read and @ref TfParser::TfDataBase
 * @author Yibo Lin
*  @date   Oct 2014
 */

#include <iostream>
#include <string>

#include <limbo/parsers/tf/spirit/TfParser.h>

using std::cout;
using std::endl;
using std::string;

/// @brief Custom class that inheritates @ref TfParser::TfDataBase 
/// with all the required callbacks defined. 
struct TfDataBase : public TfParser::TfDataBase
{
    /// @brief add layer name, layer id, layer abbreviation 
    /// @param s1 layer name 
    /// @param s2 layer id 
    /// @param s3 layer abbreviation 
	void add_tf_layer_id(string const& s1, int32_t const& s2, string const& s3)
	{
		cout << __func__ << endl; 
		cout << s1 << ", " << s2 << ", " << s3 << endl;
	}
};

/// @brief test 1: use function wrapper @ref TfParser::read  
void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	TfDataBase db;
	TfParser::read(db, filename);
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
	}
	else 
		cout << "at least 1 argument is required" << endl;

	return 0;
}
