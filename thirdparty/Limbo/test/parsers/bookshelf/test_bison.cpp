/**
 * @file   bookshelf/test_bison.cpp
 * @brief  test bookshelf parser, see @ref BookshelfParser::Driver and @ref BookshelfParser::BookshelfDataBase
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <fstream>

#include <limbo/parsers/bookshelf/bison/BookshelfDriver.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

/// @brief Custom class that inheritates @ref BookshelfParser::BookshelfDataBase 
/// with all the required callbacks defined. 
class BookshelfDataBase : public BookshelfParser::BookshelfDataBase
{
	public:
        /// constructor 
		BookshelfDataBase()
		{
			cout << "BookshelfDataBase::" << __func__ << endl;
		}
		//////////////////// required callbacks from abstract BookshelfParser::BookshelfDataBase ///////////////////
        /// @param nn number of nodes 
        /// @param nt number of terminals 
        virtual void resize_bookshelf_node_terminals(int nn, int nt) 
        {
            cout << __func__ << " => " << nn << ", " << nt << endl;
        }
        /// @param n number of nets 
        virtual void resize_bookshelf_net(int n)
        {
            cout << __func__ << " => " << n << endl;
        }
        /// @param n number of sets 
        virtual void resize_bookshelf_pin(int n)
        {
            cout << __func__ << " => " << n << endl;
        }
        /// @param n number of rows 
        virtual void resize_bookshelf_row(int n)
        {
            cout << __func__ << " => " << n << endl;
        }
        /// @param name terminal name 
        /// @param w width 
        /// @param h height 
        virtual void add_bookshelf_terminal(string& name, int w, int h)
        {
            cout << __func__ << " => " << name << ", " << w << ", " << h << endl;
        }
        /// @param name node name 
        /// @param w width 
        /// @param h height 
        virtual void add_bookshelf_node(string& name, int w, int h)
        {
            cout << __func__ << " => " << name << ", " << w << ", " << h << endl;
        }
        /// @param net net object 
        virtual void add_bookshelf_net(BookshelfParser::Net const& net)
        {
            net.print(cout);
        }
        /// @param row row object 
        virtual void add_bookshelf_row(BookshelfParser::Row const& row)
        {
            row.print(cout);
        }
        /// @param name node name 
        /// @param x, y node position 
        /// @param orient orientation 
        /// @param status placement status 
        /// @param plFlag if true the function is called from .pl file 
        virtual void set_bookshelf_node_position(string const& name, double x, double y, string const& orient, string const& status, bool plFlag)
        {
            cout << __func__ << " => " << name << ", " << x << ", " << y << ", " << orient << ", " << status << ", plFlag = " << plFlag << endl;
        }
        /// @param name design name 
        virtual void set_bookshelf_design(string& name)
        {
            cout << __func__ << " => " << name << endl;
        }
        /// end of bookshelf 
        virtual void bookshelf_end()
        {
            cout << __func__ << endl;
        }
};

/// @brief test 1: use function wrapper @ref BookshelfParser::read  
void test1(string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	BookshelfDataBase db;
	BookshelfParser::read(db, filename);
}

/// @brief test 2: use class wrapper @ref BookshelfParser::Driver 
void test2(string const& filename)
{
	cout << "////////////// test2 ////////////////" << endl;
	BookshelfDataBase db;
	BookshelfParser::Driver driver (db);
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
