/**
 * @file   lef/test_adapt.cpp
 * @brief  test lef parser, see @ref LefParser::Driver and @ref LefParser::LefDataBase
 * @author Yibo Lin
 * @date   Dec 2014
 */

#include <iostream>
#include <fstream>
#include <string>

#include <limbo/parsers/lef/adapt/LefDriver.h>

using std::cout;
using std::endl;
using std::string;

/// @brief Custom class that inheritates @ref LefParser::LefDataBase 
/// with all the required callbacks defined. 
/// This is an recommended API for LefParser, which also matches to potential bison version in the future. 
/// Actually spirit version of parser is based on template, so user does not necessarily need to inherit LefDataBase. 
/// 
/// With the runtime reminder scheme, not all callbacks need to be defined in the derived database because most benchmarks only use a few keywords. 
/// If the parser detects any keyword whose callback function is not defined, it produces runtime assertion failure to remind the developer. 
class LefDataBase : public LefParser::LefDataBase
{
	public:
        /// base type 
		typedef LefParser::LefDataBase base_type;

        /// @brief constructor 
		LefDataBase() : base_type()
		{
			cout << "constructing LefDataBase" << endl;
		}
		//////////////////// required callbacks from abstract LefParser::LefDataBase ///////////////////
        /// @brief set LEF version 
        /// @param v string of LEF version 
		virtual void lef_version_cbk(string const& v)
		{
			cout << "lef version = " << v << endl;
		}
        /// @brief set LEF version 
        /// @param v floating point number of LEF version 
		virtual void lef_version_cbk(double v) 
		{
			cout << "lef version = " << v << endl;
		}
        /// @brief set divider characters 
        /// @param v divider characters
		virtual void lef_dividerchar_cbk(string const& v)
		{
			cout << "lef dividechar = " << v << endl;
		}
        /// @brief set unit 
        /// @param v an object for unit 
		virtual void lef_units_cbk(LefParser::lefiUnits const& v)
		{
			v.print(stdout);
		}
        /// @brief set manufacturing entry 
        /// @param v manufacturing entry 
		virtual void lef_manufacturing_cbk(double v)
		{
			cout << "lef manufacturing = " << v << endl;
		}
        /// @brief set bus bit characters 
        /// @param v but bit characters 
		virtual void lef_busbitchars_cbk(string const& v)
		{
			cout << "lef busbitchars = " << v << endl;
		}
        /// @brief add layer 
        /// @param v an object for layer 
		virtual void lef_layer_cbk(LefParser::lefiLayer const& v)
		{
			v.print(stdout);
		}
        /// @brief add via 
        /// @param v an object for via 
		virtual void lef_via_cbk(LefParser::lefiVia const& v)
		{
			v.print(stdout);
		}
        /// @brief add via rule 
        /// @param v an object for via rule 
		virtual void lef_viarule_cbk(LefParser::lefiViaRule const& v)
		{
			v.print(stdout);
		}
        /// @brief spacing callback 
        /// @param v an object for spacing 
		virtual void lef_spacing_cbk(LefParser::lefiSpacing const& v)
		{
			v.print(stdout);
		}
        /// @brief site callback 
        /// @param v an object for site 
		virtual void lef_site_cbk(LefParser::lefiSite const& v)
		{
			v.print(stdout);
		}
        /// @brief macro begin callback, describe standard cell type 
        /// @param v name of macro 
		virtual void lef_macrobegin_cbk(std::string const& v)
        {
            cout << __func__ << " => " << v << endl; 
        }
        /// @brief macro callback, describe standard cell type 
        /// @param v an object for macro 
		virtual void lef_macro_cbk(LefParser::lefiMacro const& v)
		{
			v.print(stdout);
		}
        /// @brief property callback 
        /// @param v an object for property 
		virtual void lef_prop_cbk(LefParser::lefiProp const& v)
        {
            v.print(stdout);
        }
        /// @brief noise margin callback 
        /// @param v an object for noise margin 
		virtual void lef_maxstackvia_cbk(LefParser::lefiMaxStackVia const& v)
        {
            v.print(stdout);
        }
        /// @brief obstruction callback 
        /// @param v an object for obstruction 
		virtual void lef_obstruction_cbk(LefParser::lefiObstruction const& v)
		{
			v.print(stdout);
		}
        /// @brief pin callback, describe pins in a standard cell 
        /// @param v an object for pin 
		virtual void lef_pin_cbk(lefiPin const& v)
        {
            v.print(stdout);
        }
};

/// @brief test 1: use function wrapper @ref LefParser::read  
void test1(std::string const& filename)
{
	cout << "////////////// test1 ////////////////" << endl;
	LefDataBase db;
	if (LefParser::read(db, filename))
		cout << "read successfully" << endl;
	else cout << "read failed" << endl;
}

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 
int main(int argc, char** argv)
{

	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
			test1(argv[i]);
	}
	else 
		cout << "at least 1 argument is required" << endl;

	return 0;
}

