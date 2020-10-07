/**
 * @file lp_limbo.h
 * @brief The linear programming implementation using limbo::LPSolveApi and limbo::GurobiLinearApi
 * @author Keren Zhu
 * @date 03/10/2020
 */

#pragma once

#include <limbo/solvers/Solvers.h>
#ifndef LP_NOT_USE_LPSOLVE
#include <limbo/solvers/api/LPSolveApi.h>
#endif
#ifndef LP_NOT_USE_GUROBI
#include <limbo/solvers/api/GurobiApi.h>
#endif
#include "linear_programming_trait.h"

namespace klib {

namespace _lp
{

template<typename limbo_lp_api>
struct _limbo_lp_api_trait
{
    typedef typename limbo_lp_api::param_type param_type;
    typedef typename limbo_lp_api::limbo_solver_type limbo_solver_type;
    typedef typename limbo_lp_api::value_type value_type;
    
    static void setDefaultParams(param_type & param);
    static void setNumThreads(param_type &param, std::uint32_t numThreads);
};

template<typename limbo_lp_api_type>
class _limbo_lp_solver
{
    friend linear_programming_trait<_limbo_lp_solver<limbo_lp_api_type>>;
    public:
        typedef typename _limbo_lp_api_trait<limbo_lp_api_type>::value_type value_type;
        typedef typename limbo::solvers::LinearModel<value_type, value_type> model_type;
        typedef typename model_type::variable_type variable_type;
        typedef typename model_type::expression_type expr_type;
        typedef typename model_type::constraint_type constr_type;
        typedef limbo::solvers::SolverProperty status_type;
        typedef typename _limbo_lp_api_trait<limbo_lp_api_type>::param_type param_type;
        typedef typename _limbo_lp_api_trait<limbo_lp_api_type>::limbo_solver_type limbo_solver_type;
    private:
        model_type _model; ///< The LP problem model
        status_type _status; ///< The result status. e.g. OPTIMAL
        param_type _params; ///< The parameters for LIMBO solver
};

#ifndef LP_NOT_USE_LPSOLVE


template<typename value_type>
struct _limbo_lp_api_lpsove
{
};

template<typename coe_value_type>
struct _limbo_lp_api_trait<_limbo_lp_api_lpsove<coe_value_type>>
{
    typedef coe_value_type value_type;
    typedef typename limbo::solvers::LPSolveParameters param_type;
    typedef limbo::solvers::LPSolveLinearApi<value_type, value_type> limbo_solver_type;
    
    static void setDefaultParams(param_type & param)
    {
        param.setVerbose(2); // SERVE
    }
    static void setNumThreads(param_type &, std::uint32_t) {}
};

#endif

#ifndef LP_NOT_USE_GUROBI


template<typename value_type>
struct _limbo_lp_api_gurobi
{
};

template<typename coe_value_type>
struct _limbo_lp_api_trait<_limbo_lp_api_gurobi<coe_value_type>>
{
    typedef coe_value_type value_type;
    typedef typename limbo::solvers::GurobiParameters param_type;
    typedef limbo::solvers::GurobiLinearApi<value_type, value_type> limbo_solver_type;
    
    static void setDefaultParams(param_type &) {}
    static void setNumThreads(param_type &param, std::uint32_t numThreads) { param.setNumThreads(numThreads); }
};

#endif

template<typename limbo_lp_api_type>
struct linear_programming_trait<_limbo_lp_solver<limbo_lp_api_type>>
{
    typedef _limbo_lp_solver<limbo_lp_api_type> solver_type;
    typedef typename _limbo_lp_api_trait<limbo_lp_api_type>::value_type value_type;
    typedef typename solver_type::variable_type variable_type;
    typedef typename solver_type::expr_type expr_type;
    typedef typename solver_type::constr_type constr_type;
    typedef typename solver_type::status_type status_type;



    static variable_type addVar(solver_type &solver)
    {
        return solver._model.addVariable(0, 1e20,
                                                limbo::solvers::CONTINUOUS, 
                                                "x" + solver._model.numVariables());
    }
    static void addConstr(solver_type &solver, const constr_type &constr)
    {
        bool success = solver._model.addConstraint(constr, "CONSTR");
        AssertMsg(success, "Limbo lib LP solver add constraint failed\n");
    }
    static void setVarLowerBound(solver_type &solver, const variable_type &var, const value_type &val)
    {
        solver._model.updateVariableLowerBound(var, val);
    }
    static void setVarUpperBound(solver_type &solver, const variable_type &var, const value_type &val)
    {
        solver._model.updateVariableUpperBound(var, val);
    }
    static void setVarInteger(solver_type &solver, const variable_type &var)
    {
        solver._model.setVariableNumericType(var, limbo::solvers::INTEGER);
    }
    static void setVarContinuous(solver_type &solver, const variable_type &var)
    {
        solver._model.setVariableNumericType(var, limbo::solvers::CONTINUOUS);
    }
    static void setObjectiveMaximize(solver_type &solver)
    {
        solver._model.setOptimizeType(limbo::solvers::MAX);
    }
    static void setObjectiveMinimize(solver_type &solver)
    {
        solver._model.setOptimizeType(limbo::solvers::MIN);
    }
    static void setObjective(solver_type &solver, const expr_type &expr)
    {
        solver._model.setObjective(expr);
    }
    static void solve(solver_type &solver)
    {
        _limbo_lp_api_trait<limbo_lp_api_type>::setDefaultParams(solver._params);
        typename solver_type::limbo_solver_type sol(&solver._model);
        solver._status = sol(&solver._params);
    }
    static status_type status(solver_type &solver)
    {
        return solver._status;
    }
    static bool isOptimal(solver_type &solver)
    {
        return solver._status == limbo::solvers::OPTIMAL;
    }
    static bool isSuboptimal(solver_type &solver)
    {
        return solver._status == limbo::solvers::SUBOPTIMAL;
    }
    static bool isUnbounded(solver_type &solver)
    {
        return solver._status == limbo::solvers::UNBOUNDED;
    }
    static bool isInfeasible(solver_type &solver)
    {
        return solver._status == limbo::solvers::INFEASIBLE;
    }
    static value_type evaluateExpr(const solver_type &solver, const expr_type &expr)
    {
        return solver._model.evaluateExpression(expr, solver._model.variableSolutions());
    }
    static value_type solution(const solver_type &solver, const variable_type &var)
    {
        return solver._model.variableSolution(var);
    }
    static std::string statusStr(const solver_type &solver)
    {
        return limbo::solvers::toString(solver._status);
    }
    static void setNumThreads(solver_type &solver, std::uint32_t numThreads)
    {
        _limbo_lp_api_trait<limbo_lp_api_type>::setNumThreads(solver._params, numThreads);
    }
};

} //namespace _lp

namespace _lp
{
#ifndef LP_NOT_USE_LPSOLVE
    typedef _lp::_limbo_lp_solver<_lp::_limbo_lp_api_lpsove<double>> LimboLpsolve; ///< The lpsolve using limbo api. This one need to be constructed
    typedef _lp::linear_programming_trait<LimboLpsolve> LimboLpsolveTrait; ///< The lpsolve using limbo api. Don't construct it. Use its static methods as interface
#endif
#ifndef LP_NOT_USE_GUROBI
    typedef _lp::_limbo_lp_solver<_lp::_limbo_lp_api_gurobi<double>> LimboLpGurobi; ///< The Gurobi using limbo api. This one need to be constructed
    typedef _lp::linear_programming_trait<LimboLpGurobi> LimboLpGurobiTrait; ///< The Gurobi using limbo api. Don't construct it. Use its static methods as interface
#endif
} // namespace _lp

} //namespace klib


