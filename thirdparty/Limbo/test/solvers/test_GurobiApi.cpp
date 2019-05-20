/**
 * @file   test_GurobiApi.cpp
 * @author Yibo Lin
 * @date   Mar 2017
 * @brief  Test Gurobi API @ref limbo::solvers::GurobiLinearApi
 */
#include <iostream>
#include <limbo/solvers/api/GurobiApi.h>

/// @brief main function 
/// 
/// Solve following problem, 
/// \f{eqnarray*}{
/// & min. & x_1 + x_2 + x_3 + x_4, \\
/// & s.t. & x_1 - x_2 \ge 0.5,  \\
/// &     & x_4 - x_3 \ge 0.1, \\
/// &     & x_2 - x_3 \ge 0.2.  
/// \f}
/// 
/// @return 0 
int main()
{
    // ILP model 
    typedef limbo::solvers::LinearModel<double, long> model_type; 
    model_type optModel; 

    // create variables 
    model_type::variable_type var1 = optModel.addVariable(0, 1, limbo::solvers::CONTINUOUS, "x1");
    model_type::variable_type var2 = optModel.addVariable(0, 1, limbo::solvers::CONTINUOUS, "x2");
    model_type::variable_type var3 = optModel.addVariable(0, 1, limbo::solvers::CONTINUOUS, "x3");
    model_type::variable_type var4 = optModel.addVariable(0, 1, limbo::solvers::CONTINUOUS, "x4");

    // create objective
    optModel.setObjective(var1+var2+var3+var4); 
    optModel.setOptimizeType(limbo::solvers::MIN);

    // create constraints 
    optModel.addConstraint(var1 - var2 >= 0.5, "c1"); 
    optModel.addConstraint(var4 - var3 >= 0.1, "c2"); 
    optModel.addConstraint(var2 - var3 >= 0.2, "c3"); 

    // solve by Gurobi 
    typedef limbo::solvers::GurobiLinearApi<model_type::coefficient_value_type, model_type::variable_value_type> solver_type; 
    solver_type solver (&optModel); 
    limbo::solvers::GurobiParameters gurobiParams; 
    gurobiParams.setNumThreads(1);
    gurobiParams.setOutputFlag(1); 

    limbo::solvers::SolverProperty optStatus = solver(&gurobiParams); 

    std::cout << "optStatus = " << optStatus << std::endl; 

    return 0; 
}
