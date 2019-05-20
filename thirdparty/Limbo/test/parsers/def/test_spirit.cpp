
#include <iostream>
#include <fstream>

#include <limbo/parsers/def/spirit/DefParser.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

class DataBase : public DefParser::DefDataBase
{
	public:
		DataBase()
		{
			cout << "DataBase::" << __func__ << endl;
		}
		//////////////////// required callbacks from abstract DefParser::DefDataBase ///////////////////
		virtual void set_def_dividerchar(string const& token) 
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void set_def_busbitchars(string const& token)
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void set_def_version(string const& token) 
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void set_def_design(string const& token) 
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void set_def_unit(int token) 
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void set_def_diearea(int t1, int t2, int t3, int t4) 
		{
			cout << __func__ << " => " << t1 << "," << t2 << "," << t3 << "," << t4 << endl;
		}
		virtual void add_def_row(DefParser::Row const&) 
		{
			cout << __func__ << endl;
		}
		virtual void add_def_component(DefParser::Component const&) 
		{
			cout << __func__ << endl;
		}
		virtual void resize_def_component(int token) 
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void add_def_pin(DefParser::Pin const&) 
		{
			cout << __func__ << endl;
		}
		virtual void resize_def_pin(int token)  
		{
			cout << __func__ << " => " << token << endl;
		}
		virtual void add_def_net(DefParser::Net const&) 
		{
			cout << __func__ << endl;
		}
		virtual void resize_def_net(int token) 
		{
			cout << __func__ << " => " << token << endl;
		}
};

void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	DataBase db;
	DefParser::read(db, filename);
}

#if 0
void test2(string const& filename)
{
	cout << "////////////// test2 ////////////////" << endl;
	DataBase db;
	DefParser::Driver driver (db);
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
