/**
 * @file   lp/test_bison.cpp
 * @brief  test lp parser, see @ref LpParser::Driver and @ref LpParser::LpDataBase
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <fstream>

#include <limbo/parsers/lp/bison/LpDriver.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

/// @brief Custom class that inheritates @ref LpParser::LpDataBase 
/// with all the required callbacks defined. 
class LpDataBase : public LpParser::LpDataBase
{
	public:
        /// @brief use int64_t in base type 
		typedef LpParser::int64_t int64_t;
        /// @brief constructor 
		LpDataBase()
		{
			cout << "constructing LpDataBase" << endl;
		}
        /// @brief add variable that \a l <= \a vname <= \a r. 
        /// @param vname variable name 
        /// @param l lower bound 
        /// @param r upper bound 
		void add_variable(string const& vname, double l, double r) 
		{
			cout << l << " <= " << vname << " <= " << r << endl;
		}
        /// @brief add constraint that \a terms \a compare \a constant. 
        /// @param terms array of terms in left hand side 
        /// @param compare operator '<', '>', '='
        /// @param constant constant in the right hand side 
		void add_constraint(LpParser::TermArray const& terms, char compare, double constant) 
		{
            for (LpParser::TermArray::const_iterator it = terms.begin(); it != terms.end(); ++it)
                cout << " + " << it->coef << " " << it->var; 
            cout << " " << compare << " " << constant << endl; 
		}
        /// @brief add object terms 
        /// @param minimize true denotes minimizing object, false denotes maximizing object 
        /// @param terms array of terms 
		void add_objective(bool minimize, LpParser::TermArray const& terms)
		{
            if (minimize)
                cout << "Minimize\n"; 
            else 
                cout << "Maximize\n";
            for (LpParser::TermArray::const_iterator it = terms.begin(); it != terms.end(); ++it)
                cout << " + " << it->coef << " " << it->var; 
            cout << endl; 
            cout << "Subject To\n";
		}
        /// @brief set integer variables 
        /// @param vname integer variables  
        /// @param binary denotes whether they are binary variables 
        void set_integer(string const& vname, bool binary)
        {
            if (binary)
                cout << vname << ": BINARY\n";
            else 
                cout << vname << ": INTEGER\n";
        }
};

/// @brief test 1: use function wrapper @ref LpParser::read  
void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	LpDataBase db;
	LpParser::read(db, filename);
}

/// @brief test 2: use class wrapper @ref LpParser::Driver 
void test2(string const& filename)
{
	cout << "////////////// test2 ////////////////" << endl;
	LpDataBase db;
	LpParser::Driver driver (db);
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
