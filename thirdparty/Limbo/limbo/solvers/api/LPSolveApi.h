/**
 * @file   LPSolveApi.h
 * @author Yibo Lin
 * @date   Dec 2018
 */

#ifndef LIMBO_SOLVERS_API_LPSOLVEAPI_H
#define LIMBO_SOLVERS_API_LPSOLVEAPI_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <limbo/solvers/Solvers.h>
// make sure lpsolve is configured properly 
extern "C" 
{
#include "lp_lib.h" // use lpsolve as ILP solver 
}

// lpsolve has macro definitions that cause conflicts 
// temporarily uncomment them 
#undef OPTIMAL                  
#undef SUBOPTIMAL               
#undef INFEASIBLE               
#undef UNBOUNDED                
inline limbo::solvers::SolverProperty getSolveStatus(int status)
{
    switch (status)
    {
        case 0: // OPTIMAL 
        case 8: // PRESOLVED 
            return limbo::solvers::OPTIMAL;
        case 1: // SUBOPTIMAL 
        case 6: // USERABORT 
        case 7: // TIMEOUT 
            return limbo::solvers::SUBOPTIMAL; 
        case 2: // INFEASIBLE
        case 4: // DEGENERATE 
        case 5: // NUMFAILURE, numerical failure 
        case 25: // NUMFAILURE, accuracy error encountered 
            return limbo::solvers::INFEASIBLE;
        case 3: // UNBOUNDED
            return limbo::solvers::UNBOUNDED;
        default:
            limboAssertMsg(0, "unknown status %d", status);
    }
}

/// namespace for Limbo
namespace limbo 
{ 
/// namespace for Solvers 
namespace solvers 
{

/// @brief Base class for custom LPSolve parameters 
class LPSolveParameters
{
    public:
        /// @brief constructor 
        LPSolveParameters() 
            : m_verbose(SEVERE)
            , m_bbRule(NODE_PSEUDONONINTSELECT|NODE_RCOSTFIXING)
              // various options in presolve to tune 
              // I found PRESOLVE_COLDOMINATE may result in INFEASIBLE model, which may be a bug in lpsolve
              // PRESOLVE_COLDOMINATE is buggy, do not use it 
            , m_presolve(PRESOLVE_ROWS+PRESOLVE_COLS+PRESOLVE_LINDEP+PRESOLVE_SOS+PRESOLVE_REDUCEMIP+PRESOLVE_KNAPSACK
                   +PRESOLVE_ELIMEQ2+PRESOLVE_IMPLIEDFREE+PRESOLVE_REDUCEGCD+PRESOLVE_PROBEFIX+PRESOLVE_PROBEREDUCE
                   +PRESOLVE_ROWDOMINATE/*+PRESOLVE_COLDOMINATE*/+PRESOLVE_MERGEROWS+PRESOLVE_COLFIXDUAL
                   +PRESOLVE_BOUNDS/*+PRESOLVE_DUALS+PRESOLVE_SENSDUALS*/)
        {
        }
        /// @brief destructor 
        virtual ~LPSolveParameters() {}
        /// @brief customize environment 
        /// @param lp LPSolve problem instance  
        virtual void operator()(lprec* lp) const 
        {
            set_verbose(lp, m_verbose); 
            // various options in presolve to tune 
            set_bb_rule(lp, m_bbRule); 
            set_presolve(lp, m_presolve, get_presolveloops(lp)); 
        }

        /// @brief set verbose level 
        /// @param v value 
        void setVerbose(int v) {m_verbose = v;}
        /// @brief set branch and bound rule 
        /// @param v value 
        void setBbRule(int v) {m_bbRule = v;}
        /// @brief set presolve strategy 
        /// @param v value 
        void setPresolve(int v) {m_presolve = v;}

    protected:
        int m_verbose; ///< verbose level 
        int m_bbRule; ///< branch and bound rule 
        int m_presolve; ///< presolve strategy 
};
/// @brief LPSolve API with @ref limbo::solvers::LinearModel
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class LPSolveLinearApi 
{
    public:
        /// @brief linear model type for the problem 
        typedef LinearModel<T, V> model_type; 
        /// @nowarn
        typedef typename model_type::coefficient_value_type coefficient_value_type; 
        typedef typename model_type::variable_value_type variable_value_type; 
        typedef typename model_type::variable_type variable_type;
        typedef typename model_type::constraint_type constraint_type; 
        typedef typename model_type::expression_type expression_type; 
        typedef typename model_type::term_type term_type; 
        typedef typename model_type::property_type property_type;
        typedef LPSolveParameters parameter_type; 
        /// @endnowarn
        
        /// @brief constructor 
        /// @param model pointer to the model of problem 
        LPSolveLinearApi(model_type* model);
        /// @brief destructor 
        ~LPSolveLinearApi(); 
        
        /// @brief API to run the algorithm 
        /// @param param set additional parameters, use default if NULL 
        SolverProperty operator()(parameter_type* param = NULL); 
    protected:
        /// @brief copy constructor, forbidden 
        /// @param rhs right hand side 
        LPSolveLinearApi(LPSolveLinearApi const& rhs);
        /// @brief assignment, forbidden 
        /// @param rhs right hand side 
        LPSolveLinearApi& operator=(LPSolveLinearApi const& rhs);

        model_type* m_model; ///< model for the problem 
        lprec* m_lpModel; ///< model for LPSolve 
};

template <typename T, typename V>
LPSolveLinearApi<T, V>::LPSolveLinearApi(LPSolveLinearApi<T, V>::model_type* model)
    : m_model(model)
    , m_lpModel(NULL)
{
}
template <typename T, typename V>
LPSolveLinearApi<T, V>::~LPSolveLinearApi()
{
}
template <typename T, typename V>
SolverProperty LPSolveLinearApi<T, V>::operator()(LPSolveLinearApi<T, V>::parameter_type* param)
{
    bool defaultParam = false; 
    if (param == NULL)
    {
        param = new LPSolveParameters;
        defaultParam = true; 
    }

    // column indices 
    std::vector<int> vIdx; 
    // coefficients  
    std::vector<double> vValue; 
    // buffer for name 
    char buf[64]; 

    m_lpModel = make_lp(m_model->constraints().size(), m_model->numVariables()); 

	set_lp_name(m_lpModel, (char*)"LPSolveLinearApi");
	// set verbose level 
	set_verbose(m_lpModel, SEVERE);

    // create variables 
    for (unsigned int i = 0, ie = m_model->numVariables(); i < ie; ++i)
    {
        variable_type var (i);
        limboAssertMsg(set_bounds(m_lpModel, i+1, m_model->variableLowerBound(var), m_model->variableUpperBound(var)), "failed to set bounds of variable for LP");
        sprintf(buf, "%s", m_model->variableName(var).c_str());
		limboAssertMsg(set_col_name(m_lpModel, i+1, buf), "failed to set name of variable %s", m_model->variableName(var).c_str());
        switch (m_model->variableNumericType(var))
        {
            case CONTINUOUS:
                break; 
            case BINARY: 
                limboAssertMsg(set_binary(m_lpModel, i+1, TRUE), "failed to set binary variable for LP"); 
                break;
            case INTEGER:
                limboAssertMsg(set_int(m_lpModel, i+1, TRUE), "failed to set integer variable for LP"); 
                break;
            default:
                limboAssertMsg(0, "unknown numeric type");
        }
    }

    // create objective 
    vIdx.clear(); 
    vValue.clear();
    for (typename std::vector<term_type>::const_iterator it = m_model->objective().terms().begin(), ite = m_model->objective().terms().end(); it != ite; ++it)
    {
        vIdx.push_back(it->variable().id()+1); // variable id starts from 1 for LPSolve
        vValue.push_back(it->coefficient());
    }
	limboAssertMsg(set_obj_fnex(m_lpModel, vIdx.size(), &vValue[0], &vIdx[0]), "failed to set objective for LP"); 
    if (m_model->optimizeType() == MIN)
    {
        set_minim(m_lpModel); 
    }
    else 
    {
        set_maxim(m_lpModel); 
    }

    // create constraints 
    for (unsigned int i = 0, ie = m_model->constraints().size(); i < ie; ++i)
    {
        constraint_type const& constr = m_model->constraints().at(i);

        vIdx.clear(); 
        vValue.clear();
        for (typename std::vector<term_type>::const_iterator it = constr.expression().terms().begin(), ite = constr.expression().terms().end(); it != ite; ++it)
        {
            vIdx.push_back(it->variable().id()+1); // variable id starts from 1 for LPSolve
            vValue.push_back(it->coefficient());
        }

        limboAssertMsg(set_rowex(m_lpModel, i+1, vIdx.size(), &vValue[0], &vIdx[0]), "failed to set constraint for LP");
        switch (constr.sense())
        {
            case '>':
                limboAssertMsg(set_constr_type(m_lpModel, i+1, GE), "failed to set constraint type for LP");
                break;
            case '<':
                limboAssertMsg(set_constr_type(m_lpModel, i+1, LE), "failed to set constraint type for LP");
                break;
            case '=':
                limboAssertMsg(set_constr_type(m_lpModel, i+1, EQ), "failed to set constraint type for LP");
                break;
            default:
                limboAssertMsg(0, "unknown sense");
        }
        limboAssertMsg(set_rh(m_lpModel, i+1, constr.rightHandSide()), "failed to set constraint right-hand side for LP"); 
        sprintf(buf, "%s", m_model->constraintName(constr).c_str());
        limboAssertMsg(set_row_name(m_lpModel, i+1, buf), "failed to set constraint name %s for LP", m_model->constraintName(constr).c_str()); 
    }

    // solve LP 
#ifdef DEBUG_LPSOLVEAPI
    write_lp(m_lpModel, (char*)"problem.lp"); 
#endif
    set_scaling(m_lpModel, SCALE_RANGE); 
    // call parameter setting before optimization 
    param->operator()(m_lpModel); 
    int status = solve(m_lpModel); 

    // apply solution 
    for (unsigned int i = 0; i < m_model->numVariables(); ++i)
    {
        REAL value = get_var_primalresult(m_lpModel, m_model->constraints().size()+1+i);
        m_model->setVariableSolution(m_model->variable(i), value);
    }

#ifdef DEBUG_LPSOLVEAPI
    std::vector<REAL> vSol (1+m_model->constraints().size()+m_model->numVariables()); 
    limboAssertMsg(get_primal_solution(m_lpModel, &vSol[0]), "failed to get_primal_solution for LP");
    std::ofstream out ("problem.sol"); 
    limboAssertMsg(out.good(), "failed to open %s for write", "problem.sol"); 
    out << "# Solution for model " << get_lp_name(m_lpModel) << "\n"; 
    out << "# Objective value = " << get_objective(m_lpModel) << "\n"; 
    for (unsigned int i = 0; i < m_model->numVariables(); ++i)
    {
        out << get_col_name(m_lpModel, i+1) << " " << vSol[1+m_model->constraints().size()+i] << "\n";
    }
    out.close();
#endif

    if (defaultParam)
        delete param; 
    delete_lp(m_lpModel); 

    return getSolveStatus(status);
    //switch (status)
    //{
    //    case 0: // OPTIMAL 
    //    case 8: // PRESOLVED 
    //        return OPTIMAL;
    //    case 1: // SUBOPTIMAL 
    //    case 6: // USERABORT 
    //    case 7: // TIMEOUT 
    //        return SUBOPTIMAL; 
    //    case 2: // INFEASIBLE
    //    case 4: // DEGENERATE 
    //    case 5: // NUMFAILURE, numerical failure 
    //    case 25: // NUMFAILURE, accuracy error encountered 
    //        return INFEASIBLE;
    //    case 3: // UNBOUNDED
    //        return UNBOUNDED;
    //    default:
    //        limboAssertMsg(0, "unknown status %d", status);
    //}
}

} // namespace solvers
} // namespace limbo

#endif 
