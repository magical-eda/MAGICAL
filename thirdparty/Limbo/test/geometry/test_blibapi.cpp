/**
 * @file   test_blibapi.cpp
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include "bLibPoint.h"
#include "bLibShape.h"
#include <limbo/geometry/Polygon2Rectangle.h>
#include <limbo/geometry/api/bLibApi.h>
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::list;
using std::set;

namespace lg = limbo::geometry; 

/**
 * @brief test bLib API 
 *
 * use bLib::bPoint and bLib::bBox as data type for polygon-to-rectangle conversion
 */

/**
 * @brief test std::vector 
 */
void test1(string const& filename)
{
    std::vector<bLib::bBox> vRect; 
	lg::Polygon2Rectangle<vector<bLib::bPoint>, vector<bLib::bBox> > p2r (vRect, lg::HOR_VER_SLICING);
	assert(p2r.read(filename));
	assert(p2r());
	p2r.print("p2r1.gp");
}

/**
 * @brief test std::list
 */
void test2(string const& filename)
{
    std::vector<bLib::bBox> vRect; 
	lg::Polygon2Rectangle<list<bLib::bPoint>, vector<bLib::bBox> > p2r (vRect, lg::HOR_VER_SLICING);
	assert(p2r.read(filename));
	assert(p2r());
	p2r.print("p2r2.gp");
}

/**
 * @brief test std::set
 */
void test3(string const& filename)
{
    std::vector<bLib::bBox> vRect; 
	lg::Polygon2Rectangle<set<bLib::bPoint, lg::point_compare_type>, vector<bLib::bBox> > p2r (vRect, lg::HOR_VER_SLICING);
	assert(p2r.read(filename));
	assert(p2r());
	p2r.print("p2r3.gp");
}

/**
 * @brief main function requires an input benchmark in gnuplot format  
 * @param argc number of arguments 
 * @param argv values of arguments 
 * @return 0
 */
int main(int argc, char** argv)
{
	if (argc > 1)
	{
		test1(argv[1]);
		test2(argv[1]);
		test3(argv[1]);
	}
	else cout << "at least 1 argument is required" << endl;

	return 0;
}
