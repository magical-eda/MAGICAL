/**
 * @file   gdsii/test_gdsdb.cpp
 * @brief  test @ref GdsParser::GdsDriver 
 * @author Yibo Lin
 * @date   Nov 2014
 */

#include <iostream>
#include <limbo/parsers/gdsii/stream/GdsDriver.h>
using std::cout;
using std::endl;

/// @brief a database for GdsParser::GdsDriver
struct DataBase : public GdsParser::GdsDriverDataBase
{
    /// @brief constructor 
	DataBase()
	{
		cout << "constructing DataBase" << endl;
	}
	///////////////////// required callbacks /////////////////////
    /// @brief add GDSII library 
    /// @param lib GDSII library 
	virtual void add_gds_lib(GdsParser::GdsLib const& lib) 
	{
		cout << "library name = " << lib.lib_name << endl;
		cout << "library unit = " << lib.unit[0] << ", " << lib.unit[1] << endl;
		cout << "# CELL = " << lib.vCell.size() << endl;
		if (!lib.vCell.empty())
		{
			cout << "# BOUNDARY = " << lib.vCell.front().vBoundary.size() << endl;
			cout << "cell name = " << lib.vCell.front().cell_name << endl;
			vector<GdsParser::GdsBoundary> const& vBoundary = lib.vCell.front().vBoundary;
			for (unsigned int i = 0; i < vBoundary.size(); ++i)
			{
				cout << ">> BOUNDARY" << endl;
				cout << "XY: ";
				for (unsigned int j = 0; j < vBoundary[i].vPoint.size(); ++j)
				{
					cout << vBoundary[i].vPoint[j][0] << ", " << vBoundary[i].vPoint[j][1] << endl;
				}
			}
			vector<GdsParser::GdsText> const& vText = lib.vCell.front().vText;
			for (unsigned int i = 0; i < vText.size(); ++i)
			{
				cout << ">> TEXT" << endl;
				cout << "STRING: " << vText[i].content << endl;
				cout << "XY: " << vText[i].position[0] << ", " << vText[i].position[1] << endl;
			}
			vector<GdsParser::GdsSref> const& vSref = lib.vCell.front().vSref;
			for (unsigned int i = 0; i < vSref.size(); ++i)
			{
				cout << ">> SREF" << endl;
				cout << "SNAME: " << vSref[i].sname << endl;
				cout << "XY: " << vSref[i].position[0] << ", " << vSref[i].position[1] << endl;
			}
		}
	}
};

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 if succeed 
int main(int argc, char** argv)
{
	DataBase db;
	if (argc > 1)
		cout << GdsParser::read(db, argv[1]) << endl;
	else cout << "at least 1 argument is required" << endl;

	return 0;
}
