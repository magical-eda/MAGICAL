/**
 * @file   test_MultiKnapsackLagRelax.cpp
 * @author Yibo Lin
 * @date   Feb 2017
 * @brief  test @ref limbo::solvers::MultiKnapsackLagRelax algorithm 
 */
#include <iostream>
#include <limbo/solvers/MultiKnapsackLagRelax.h>


/// @brief test file API 
/// @param filename input lp file 
void test(std::string const& filename)
{
    typedef limbo::solvers::LinearModel<float, int> model_type; 
    model_type optModel; 
    optModel.read(filename); 

    // print problem 
    optModel.print(std::cout); 

    limbo::solvers::MultiKnapsackLagRelax<float, int> solver (&optModel);

    // solve 
    limbo::solvers::SolverProperty status = solver();
    std::cout << "Problem solved " << limbo::solvers::toString(status) << "\n";

    // print solutions 
    optModel.printSolution(std::cout);
    // print problem 
    optModel.print(std::cout); 
}

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments
int main(int argc, char** argv)
{
    if (argc > 1)
    {
        // test file API 
        test(argv[1]);
    }
    else std::cout << "at least one argument is required\n";

    return 0; 
}

