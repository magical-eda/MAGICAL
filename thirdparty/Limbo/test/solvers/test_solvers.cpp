/**
 * @file   test_solvers.cpp
 * @author Yibo Lin
 * @date   Feb 2017
 * @brief  test API of solvers in @ref limbo::solvers::LinearModel
 */
#include <iostream>
#include <limbo/solvers/Solvers.h>

/// @brief test function API 
void test1()
{
    typedef limbo::solvers::LinearModel<float, float> model_type; 
    model_type optModel; 
    model_type::expression_type expr; 
    // create variables 
    std::vector<model_type::variable_type> vVar; 
    for (int i = 0; i < 5; ++i)
        vVar.push_back(optModel.addVariable(0, 5, limbo::solvers::INTEGER, "")); 
    // create constraints 
    limboAssertMsg(optModel.addConstraint(vVar[0]+2*vVar[1]-vVar[2]*10 <= 10), "fail to add constraint");
    limboAssertMsg(optModel.addConstraint(-3*vVar[0]-vVar[1]+vVar[3] >= 5), "fail to add constraint");
    limboAssertMsg(optModel.addConstraint(-vVar[0]-2*(vVar[1]+vVar[3]) == 5), "fail to add constraint");
    expr = (vVar[4]*4-vVar[2]*3)/2; 
    expr = vVar[1]*2 + expr; 
    expr /= 2; 
    expr *= 4; 
    limboAssertMsg(optModel.addConstraint(expr >= 1), "fail to add constraint");
    limboAssertMsg(optModel.addConstraint(vVar[1]-vVar[0]-vVar[1] <= -1), "fail to add constraint");
    // create objective 
    expr.clear();
    expr = vVar[0]*4 + vVar[1]*2 + vVar[2]/2 + (vVar[3]/4 - (-vVar[4])*5); 
    expr -= vVar[0] + -vVar[2]/4; 
    optModel.setObjective(expr); 
    optModel.setOptimizeType(limbo::solvers::MAX);

    std::cout << "////////////////////// " << __func__ << "//////////////////////\n";
    optModel.print(std::cout); 
}

/// @brief test file API 
/// @param filename input lp file 
void test2(std::string const& filename)
{
    typedef limbo::solvers::LinearModel<float, float> model_type; 
    model_type optModel; 
    optModel.read(filename); 

    std::cout << "////////////////////// " << __func__ << "//////////////////////\n";
    optModel.print(std::cout); 
}

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments
int main(int argc, char** argv)
{
    // test function API 
    test1(); 
    if (argc > 1)
    {
        // test file API 
        test2(argv[1]);
    }
    else std::cout << "at least one argument is required to test file API\n";

    return 0; 
}
