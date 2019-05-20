
#include <iostream>
#include <fstream>

#include <limbo/parsers/lef/spirit/LefParser.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

/// this is an recommended API for LefParser, which also matches to potential bison version in the future
/// actually spirit version of parser is based on template, so user does not necessarily need to inherit LefDataBase
class DataBase : public LefParser::LefDataBase
{
	public:

		DataBase()
		{
			cout << "constructing DataBase" << endl;
		}
		///////////////// required callback ////////////////
		virtual void set_lef_version(string const& v) 
		{
			cout << "version = " << v << endl;
		}
		virtual void set_lef_unit(int32_t const& u) 
		{
			cout << "unit = " << u << endl;
		}
		virtual void set_lef_site(LefParser::Site const& s) 
		{
			cout << s << endl;
		}
		virtual void add_lef_layer(LefParser::Layer const& l) 
		{
			cout << l << endl;
		}
		virtual void add_lef_macro(LefParser::Macro const& m) 
		{
			cout << m << endl;
		}
};

void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	DataBase db;
	if (LefParser::read(db, filename))
		cout << "read successfully" << endl;
	else cout << "read failed" << endl;
}

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
