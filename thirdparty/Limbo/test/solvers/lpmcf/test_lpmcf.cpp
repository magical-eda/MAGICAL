/**
 * @file   test_lpmcf.cpp
 * @brief  test @ref limbo::solvers::lpmcf::Lgf and @ref limbo::solvers::lpmcf::LpDualMcf
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <boost/algorithm/string/predicate.hpp>
#include <limbo/solvers/lpmcf/Lgf.h>
#include <limbo/solvers/lpmcf/LpDualMcf.h>

using std::cout;
using std::endl;

/// @brief main function 
/// 
/// If given .lgf file, solve min-cost flow of the graph by @ref limbo::solvers::lpmcf::Lgf; 
/// If given .lp file, solver linear programming problem with dual min-cost flow by @ref limbo::solvers::lpmcf::LpDualMcf. 
/// Here we assume .lgf and .lp inputs describe different sets of problems that can be solved by the mathematical solver. 
/// .Lgf format is defined by Lemon to describe a network flow graph 
/// 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 
int main(int argc, char** argv)
{
	if (argc > 1)
	{
		limbo::solvers::lpmcf::Lgf<long> lpsolver1;
		limbo::solvers::lpmcf::LpDualMcf<long> lpsolver2;

		string filename = argv[1];
		if (boost::ends_with(filename, ".lgf"))
		{
			lpsolver1(filename);
			lpsolver1.print_graph("graph1");
		}
		else if (boost::ends_with(filename, ".lp"))
		{
			lpsolver2(filename);
			lpsolver2.print_graph("graph2");
		}
		else 
			cout << "only support .lgf and .lp file formats" << endl;

		//lpsolver();
	}
	else 
		cout << "at least 1 argument required\n";

	return 0;
}
