/**
 * @file   gdsii/test_gdsdb.cpp
 * @brief  test @ref GdsParser::GdsDB::GdsDB 
 * @author Yibo Lin
 * @date   Jan 2017
 */

#include <iostream>
#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/preprocessor/Msg.h>

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments, 4 arguments: input gds, output gds, flat output gds, flat cell name  
/// @return 0 if succeed 
int main(int argc, char** argv)
{
    GdsParser::GdsDB::GdsDB db; 
    if (argc > 2 && argc <= 4)
    {
        // test simple read and write
        GdsParser::GdsDB::GdsReader reader (db); 
        limboAssert(reader(argv[1]));

        for (std::vector<GdsParser::GdsDB::GdsCell>::const_iterator it = db.cells().begin(); it != db.cells().end(); ++it)
            std::cout << "cell: " << it->name() << std::endl; 

        // write 
        GdsParser::GdsDB::GdsWriter gw (db); 
        gw(argv[2]);

        std::cout << "4 arguments to test flatten: input gds, output gds, flat output gds, flat cell name" << std::endl;
    }
	else if (argc > 4)
    {
        // test flatten 
        GdsParser::GdsDB::GdsCell flatCell = db.extractCell(argv[4]);

        GdsParser::GdsDB::GdsDB flatDB; 
        flatDB.addCell(flatCell); 

        // write flatten cell 
        GdsParser::GdsDB::GdsWriter flatGw (flatDB); 
        flatGw(argv[3]);
    }
	else std::cout << "at least 4 arguments are required: input gds, output gds, flat output gds, flat cell name" << std::endl;

	return 0;
}
