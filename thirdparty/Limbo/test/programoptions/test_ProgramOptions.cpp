/**
 * @file   test_ProgramOptions.cpp
 * @brief  test usage of program options
 * @author Yibo Lin
 * @date   Jul 2015
 */

#include <iostream>
#include <string>
#include <vector>
#include <limbo/programoptions/ProgramOptions.h>
using std::cout;
using std::endl;

/**
 * @brief test usage of program options, such as help, boolean, integer, floating point number, and vector
 * @param argc number of arguments
 * @param argv values of arguments
 * @return 0 if parsing succeeds, 1 if help message is specified 
 */
int main(int argc, char** argv)
{
    using namespace limbo;
    using namespace limbo::programoptions;

    bool help = false;
    int i = 0;
    double fp = 0;
    std::string str;
    std::vector<int> vInteger;
    std::vector<std::string> vString;

    ProgramOptions po ("My options");
    po.add_option(Value<bool>("-help", &help, "print help message").toggle(true).default_value(false).toggle_value(true).help(true))
        .add_option(Value<int>("-i", &i, "an integer").default_value(100, "1.0.0"))
        .add_option(Value<double>("-f", &fp, "a floating point").required(true))
        .add_option(Value<std::string>("-s", &str, "a string").required(true))
        .add_option(Value<std::vector<int> >("-vi", &vInteger, "vector of integers"))
        .add_option(Value<std::vector<std::string> >("-vs", &vString, "vector of string"))
        ;

    po.print();
    try 
    {
        bool flag = po.parse(argc, argv);
        if (flag) 
            cout << "parsing succeeded\n";
    }
    catch (std::exception& e)
    {
        cout << "parsing failed\n";
        cout << e.what() << "\n";
    }

    cout << "help = " << ((help)? "true" : "false") << endl;
    cout << "i = " << i << endl;
    cout << "fp = " << fp << endl;
    cout << "str = " << str << endl;
    cout << "vInteger = ";
    for (std::vector<int>::const_iterator it = vInteger.begin(); it != vInteger.end(); ++it)
        cout << *it << " ";
    cout << endl;
    cout << "vString = ";
    for (std::vector<std::string>::const_iterator it = vString.begin(); it != vString.end(); ++it)
        cout << *it << " ";
    cout << endl;

    assert_msg(help == false, "help turned to true");

    return 0;
}
