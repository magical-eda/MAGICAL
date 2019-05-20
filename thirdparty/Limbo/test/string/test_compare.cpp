/**
 * @file   test_compare.cpp
 * @brief  test case-sensitivity of string comparison 
 * @author Yibo Lin
 * @date   Jan 2017
 */
#include <iostream>
#include <string>
#include <limbo/string/String.h>

/// @brief main function 
/// @return 0 
int main()
{
    std::string s1 = "limbo2343slimbo";
    std::string s2 = "LiMbo2343SliMbo";

    if (s1 == s2)
        std::cout << "string " << s1 << " and " << s2 << " is equal case-sensitive\n";
    else 
        std::cout << "string " << s1 << " and " << s2 << " is not equal case-sensitive\n";

    if (limbo::iequals(s1, s2))
        std::cout << "string " << s1 << " and " << s2 << " is equal case-insensitive\n";
    else 
        std::cout << "string " << s1 << " and " << s2 << " is not equal case-insensitive\n";

    return 0;
}
