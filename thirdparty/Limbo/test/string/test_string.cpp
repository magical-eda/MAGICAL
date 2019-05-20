/**
 * @file   test_string.cpp
 * @brief  test string functions 
 * @author Yibo Lin
 * @date   Dec 2014
 */

#include <iostream>
#include <string>
#include <limbo/string/String.h>
#include <limbo/preprocessor/AssertMsg.h>
#include <limbo/math/Math.h>
using std::cout;
using std::endl;
using std::string;

/// @brief main function 
/// @return 0 
int main()
{
	string s1 = "limbo2343slimbo";
	string s2 = "LiMbo2343SliMbo";

	assert_msg(limbo::toupper(s1) == "LIMBO2343SLIMBO", "limbo::toupper failed");
	assert_msg(limbo::tolower(s2) == "limbo2343slimbo", "limbo::tolower failed");
	assert_msg(limbo::iequals(s1, s2), "limbo::iequals failed");

	string tmp;
	tmp = limbo::to_string(limbo::lowest<int>());
	cout << tmp << "(" << tmp.size() << ")" << " == " << limbo::lowest<int>() << endl;
	tmp = limbo::to_string(std::numeric_limits<int>::max());
	cout << tmp << "(" << tmp.size() << ")" << " == " << std::numeric_limits<int>::max() << endl;
	tmp = limbo::to_string(limbo::lowest<float>());
	cout << tmp << "(" << tmp.size() << ")" << " == " << limbo::lowest<float>() << endl;
	tmp = limbo::to_string(std::numeric_limits<float>::max());
	cout << tmp << "(" << tmp.size() << ")" << " == " << std::numeric_limits<float>::max() << endl;

	return 0;
}
