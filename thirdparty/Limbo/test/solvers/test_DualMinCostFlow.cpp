/**
 * @file   test_DualMinCostFlow.cpp
 * @author Yibo Lin
 * @date   Feb 2017
 * @brief  Test dual min-cost flow solver @ref limbo::solvers::DualMinCostFlow
 */

#include <iostream>
#include <limbo/solvers/DualMinCostFlow.h>

/// @brief test file API 
/// @param filename input lp file 
/// @param alg algorithm type, 0 for cost scaling, 1 for capacity scaling, 2 for network simplex, 3 for cycle canceling 
void test(std::string const& filename, int alg)
{
    typedef limbo::solvers::LinearModel<int, int> model_type; 
    model_type optModel; 
    optModel.read(filename); 

    // print problem 
    optModel.print(std::cout); 

    // solve 
    limbo::solvers::MinCostFlowSolver<int, int>* minCostFlowSolver = NULL; 
    switch (alg)
    {
        case 0:
            minCostFlowSolver = new limbo::solvers::CostScaling<int, int>(); 
            break;
        case 1:
            minCostFlowSolver = new limbo::solvers::CapacityScaling<int, int>(); 
            break;
        case 2:
            minCostFlowSolver = new limbo::solvers::NetworkSimplex<int, int>(); 
            break;
        case 3:
        default:
            minCostFlowSolver = new limbo::solvers::CycleCanceling<int, int>(); 
            break; 
    }
    limbo::solvers::DualMinCostFlow<int, int> solver (&optModel); 
    limbo::solvers::SolverProperty status = solver(minCostFlowSolver);
    //limbo::solvers::SolverProperty status = solver();
    std::cout << "Problem solved " << limbo::solvers::toString(status) << "\n";
    delete minCostFlowSolver;

    // print solutions 
    optModel.printSolution(std::cout);
    // print problem 
    optModel.print(std::cout); 

    // print graph with solution information 
    solver.printGraph(true);
}

/// @brief main function 
/// 
/// Given .lp file, solver linear programming problem with dual min-cost flow by @ref limbo::solvers::DualMinCostFlow. 
/// 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 
int main(int argc, char** argv)
{
	if (argc > 1)
	{
        int alg = 0; 
        if (argc > 2)
            alg = atoi(argv[2]);
        // test file API 
        test(argv[1], alg);
	}
	else 
		std::cout << "at least 1 argument required\n";

	return 0;
}
