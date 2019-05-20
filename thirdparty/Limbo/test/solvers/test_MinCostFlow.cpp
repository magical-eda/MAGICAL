/**
 * @file   test_MinCostFlow.cpp
 * @author Yibo Lin
 * @date   Nov 2017
 * @brief  Test min-cost flow solver @ref limbo::solvers::MinCostFlow
 */

#include <iostream>
#include <limbo/solvers/MinCostFlow.h>

/// @brief test file API 
/// @param filename input lp file 
/// @param alg algorithm type, 0 for cost scaling, 1 for capacity scaling, 2 for network simplex, 3 for cycle canceling 
void test(std::string const& filename, int alg)
{
    typedef limbo::solvers::LinearModel<double, int> model_type; 
    model_type optModel; 
    optModel.read(filename); 

    // print problem 
    optModel.print(std::cout); 

    // solve 
    limbo::solvers::MinCostFlowSolver<double, int>* minCostFlowSolver = NULL; 
    switch (alg)
    {
        case 0:
            minCostFlowSolver = new limbo::solvers::CostScaling<double, int>(); 
            break;
        case 1:
            minCostFlowSolver = new limbo::solvers::CapacityScaling<double, int>(); 
            break;
        case 2:
            minCostFlowSolver = new limbo::solvers::NetworkSimplex<double, int>(); 
            break;
        case 3:
        default:
            minCostFlowSolver = new limbo::solvers::CycleCanceling<double, int>(); 
            break; 
    }
    limbo::solvers::MinCostFlow<double, int> solver (&optModel); 
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
/// Given .lp file, solver linear programming problem with min-cost flow by @ref limbo::solvers::MinCostFlow. 
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
