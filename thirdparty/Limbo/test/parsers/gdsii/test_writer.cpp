/**
 * @file   gdsii/test_writer.cpp
 * @brief  test @ref GdsParser::GdsWriter 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <vector>
#include <limbo/parsers/gdsii/stream/GdsWriter.h>

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 if succeed 
int main( int argc, char *argv[] )
{
    
    int 
        x[5],
        y[5];

	if (argc < 2) 
	{
		printf("need a output file name\n");
		return 1;
	}
    // start the gds library with HEADER, BGNLIB, LIBNAME, and UNITS

	GdsParser::GdsWriter gw (argv[1]);
	// two different wrappers 
    //gw.gds_create_lib(  "dogs", 0.001 /* um per bit */ );
	gw.create_lib("dogs", 0.001, 1.0e-9);

    // create a cell called "hotdogs"

    gw.gds_write_bgnstr(  );
    gw.gds_write_strname(  "hotdogs" );

	/////////////////////////// 3 ways to create polygons ///////////////////////////
    //-----------------------------------------------------------------------------
    // create a polygon 1

    gw.gds_write_boundary(  );       // write just the token
    gw.gds_write_layer(  10001 );       // layer 0, for example
    gw.gds_write_datatype(  0 );    // datatype 1, for example

    x[0] =    0;  y[0] =   0;       // signed four-byte integers
    x[1] =    0;  y[1] = 500;       
    x[2] = 1000;  y[2] = 500;       // in this example 1 integer unit = 1 nm
    x[3] = 1000;  y[3] =   0;
    x[4] =    0;  y[4] =   0;       // required repetition of first point (yup, that's stupid)

    gw.gds_write_xy(  x, y, 5 );    // polygon, four vertices, first vertex repeated => 5 points
    gw.gds_write_endel(  );          // end of element
    //-----------------------------------------------------------------------------
    // create a polygon 2
	// use high-level interfaces 

	std::vector<int> vx(4);
	std::vector<int> vy(4);
    vx[0] =    0;  vy[0] =   0;       // signed four-byte integers
    vx[1] =    0;  vy[1] = 500;       
    vx[2] = 1000;  vy[2] = 500;       // in this example 1 integer unit = 1 nm
    vx[3] = 1000;  vy[3] =   0;

	gw.write_boundary(10002, 0, vx, vy, false);
    //-----------------------------------------------------------------------------
    // create a polygon 3
	// for rectangles 
	// use high-level interfaces 

    for (int i = 0; i < 1; ++i)
        gw.write_box(10003, 0, 10, 10, 20, 20);

    //-----------------------------------------------------------------------------
    // create some text, reflected about the x axis

    gw.gds_write_text(  );
    gw.gds_write_layer(  1 );
    gw.gds_write_texttype(  0 );
    gw.gds_write_presentation(  0, 1, 1 );  //  font, hp, vp
    gw.gds_write_width(  500 );
    gw.gds_write_strans(  1, 0, 0 );        //  reflect, abs_angle, abs_mag
    x[0] = 2000;  
    y[0] = 2000;
    gw.gds_write_xy(  x, y, 1 );  
    gw.gds_write_string(  "reflected" );
    gw.gds_write_endel(  );

    //-----------------------------------------------------------------------------
    // create some text, using the helper function instead.
    // arguments: file-descriptor, string, x, y, layer, size
    // where x, y, and size are in database units (nanometers, usually)

    gw.gds_create_text(  "not reflected", 2000, 1500, 2, 500 );  

    //-----------------------------------------------------------------------------
    // create a path

    gw.gds_write_path(  );
    gw.gds_write_layer(  3 );               // layer 3
    gw.gds_write_datatype(  4 );            // datatype 4
    gw.gds_write_pathtype(  2 );            // extended square ends
    gw.gds_write_width(  200 );             // 200 nm wide
    x[0] = 2000;  y[0] = 3000;
    x[1] = 2000;  y[1] = 4000;
    x[2] = 2500;  y[2] = 3500;
    gw.gds_write_xy(  x, y, 3 );
    gw.gds_write_endel(  );

    //-----------------------------------------------------------------------------
    // create a box, which is stupid

    gw.gds_write_box(  );            // write just the token
    gw.gds_write_layer(  6 );       // layer 6, for example
    gw.gds_write_boxtype(  12 );    // boxtype 12, for example same as datatype

    x[0] = 3000;  y[0] =   0;       // signed four-byte integers
    x[1] = 3000;  y[1] = 500;       
    x[2] = 4000;  y[2] = 500;       // in this example 1 integer unit = 1 nm
    x[3] = 4000;  y[3] =   0;
    x[4] = 3000;  y[4] =   0;       // required repetition of first point (yup, that's stupid)

    gw.gds_write_xy(  x, y, 5 );    // polygon, four vertices, first vertex repeated => 5 points
    gw.gds_write_endel(  );          // end of element


    // end the structure (the cell hotdogs)

    gw.gds_write_endstr(  );

    //-----------------------------------------------------------------------------
    // Create a new cell, which will contain an instance of the previous cell 

    gw.gds_write_bgnstr(  );                  // new cell (structure)
    gw.gds_write_strname(  "sausage" );      // called "sausage"
    gw.gds_write_sref(  );                    // contains an instance of...
    gw.gds_write_sname(  "hotdogs" );        // the cell hotdogs
    gw.gds_write_mag(  5.0 );                // which will be 5 times larger
    gw.gds_write_angle(  15.4 );             // and tilted at some weird angle
    x[0] =  2000;
    y[0] = -2000;
    gw.gds_write_xy(  x, y, 1 );             // at these coordinates (database units)
    gw.gds_write_endel(  );                   // end of element
    gw.gds_write_endstr(  );                  // end of structure (cell)

    //-----------------------------------------------------------------------------
    // Create a new cell "meatball" containing an array of the cell "sausage"

    gw.gds_write_bgnstr(  );                  // new cell
    gw.gds_write_strname(  "meatball" );     // called "meatball"
    gw.gds_write_aref(  );                    // containing an array of...
    gw.gds_write_sname(  "sausage" );        // the cell "sausage"
    gw.gds_write_colrow(  2, 5 );            // 2 columns, 5 rows

    x[0] =   5000;  y[0] =   5000;           // array anchor point

    x[1] =  85000;  y[1] =   5000;           // displacement from anchor plus ncols*pitch
                                             // which makes the column pitch 40 um
    x[2] =   5000;  y[2] = 205000;           // displacement from anchor plus nrows*pitch
                                             // which makes the row pitch 40 um
    gw.gds_write_xy(  x, y, 3 );             // See how you could have a diagonal matrix? 
                                             // That would be so goofy!
    gw.gds_write_endel(  );                   // end of element
    gw.gds_write_endstr(  );                  // end of structure (cell) "meatball"

    // end of library

    gw.gds_write_endlib(  );    

    printf( "\nDone. Look at %s\n\n", argv[1] );

    return 0; 
}
