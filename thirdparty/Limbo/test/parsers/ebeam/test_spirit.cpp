
#include <iostream>
#include <fstream>

#include <limbo/parsers/ebeam/spirit/EbeamParser.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

class DataBase : public EbeamParser::EbeamDataBase
{
	public:
		DataBase()
		{
			cout << "constructing DataBase" << endl;
		}
		virtual void set_ebeam_unit(int token) 
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void set_ebeam_boundary(EbeamParser::EbeamBoundary const&) 
		{
			cout << __func__ << endl;
		}
		virtual void add_ebeam_macro(EbeamParser::Macro const&) 
		{
			cout << __func__ << endl;
		}
};

void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	DataBase db;
	EbeamParser::read(db, filename);
}

#if 0
void test2(string const& filename)
{
	cout << "////////////// test2 ////////////////" << endl;
	DataBase db;
	EbeamParser::Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	driver.parse_file(filename);
}
#endif 

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
