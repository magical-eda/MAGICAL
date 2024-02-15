/**
 * @file linear_programming_trait.h
 * @brief The linear programming concept
 * @author Keren Zhu
 * @date 03/10/2020
 */

#ifndef KLIB_LINEAR_PROGRAMMING_TRAIT_H_
#define KLIB_LINEAR_PROGRAMMING_TRAIT_H_

#include "Assert.h"
#include <string>

namespace klib
{

namespace _lp
{

struct _undefined
{
    typedef int variable_type;
    typedef double value_type;
    typedef double expr_type;
    typedef double constr_type;
    typedef int status_type;

    variable_type addVar() { Assert(false); return 0;}
    void addConstr(const constr_type &) {}
    void setVarLowerBound(const variable_type &, const value_type &) {}
    void setVarUpperBound(const variable_type &, const value_type &) {}
    void setVarInteger(const variable_type &) {}
    void setVarContinuous(const variable_type &) {}
    void setObjectiveMaximize() {}
    void setObjectiveMinimize() {}
    void setObjective(const expr_type &) {}
    void solve() {}
    status_type status() { return 0;}
    bool isOptimal() { return true; }
    bool isSuboptimal() { return true; }
    bool isUnbounded() { return true; }
    bool isInfeasible() { return false;}
    value_type evaluateExpr(const expr_type &) const {return 0;}
    value_type solution(const variable_type &) const { return 0;}
    std::string statusStr() const { return "";}
    void setNumThreads(std::uint32_t) {}
};

template<typename solver_type>
struct linear_programming_trait
{
    typedef typename solver_type::variable_type variable_type; ///< The LP variable type
    typedef typename solver_type::value_type value_type; ///< The value type of LP problem. Usually variety of floats
    typedef typename solver_type::expr_type expr_type; ///< The type for expressions
    typedef typename solver_type::constr_type constr_type; ///< The type for constraints
    typedef typename solver_type::status_type status_type; ///< The LP solving status

    static variable_type addVar(solver_type &solver)
    {
        return solver.addVar();
    }
    static void addConstr(solver_type &solver, const constr_type &constr)
    {
        solver.addConstr(constr);
    }
    static void setVarLowerBound(solver_type &solver, const variable_type &var, const value_type &val)
    {
        solver.setVarLowerBound(var, val);
    }
    static void setVarUpperBound(solver_type &solver, const variable_type &var, const value_type &val)
    {
        solver.setVarUpperBound(var, val);
    }
    static void setVarInteger(solver_type &solver, const variable_type &var)
    {
        solver.setVarInteger(var);
    }
    static void setVarContinuous(solver_type &solver, const variable_type &var)
    {
        solver.setVarContinuous(var);
    }
    static void setObjectiveMaximize(solver_type &solver)
    {
        solver.setObjectiveMaximize();
    }
    static void setObjectiveMinimize(solver_type &solver)
    {
        solver.setObjectiveMinimize();
    }
    static void setObjective(solver_type &solver, const expr_type &expr)
    {
        solver.setObjective(expr);
    }
    static void solve(solver_type &solver)
    {
        solver.solve();
    }
    static status_type status(solver_type &solver)
    {
        return solver.status();
    }
    static bool isOptimal(solver_type &solver)
    {
        return solver.isOptimal();
    }
    static bool isSuboptimal(solver_type &solver)
    {
        return solver.isSuboptimal();
    }
    static bool isUnbounded(solver_type &solver)
    {
        return solver.isUnbounded();
    }
    static bool isInfeasible(solver_type &solver)
    {
        return solver.isInfeasible();
    }
    static value_type evaluateExpr(const solver_type &solver, const expr_type &expr)
    {
        return solver.evaluateExpr(expr);
    }
    static value_type solution(const solver_type &solver, const variable_type &var)
    {
        return solver.solution(var);
    }
    static std::string statusStr(const solver_type &solver)
    {
        return solver.statusStr();
    }
    static void setNumThreads(solver_type &solver, std::uint32_t numThreads)
    {
        solver.setNumThreads(numThreads);
    }
};

} //namespace _lp

} // namespace klib

#endif // KLIB_LINEAR_PROGRAMMING_BASE_H_
