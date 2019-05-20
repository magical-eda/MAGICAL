/**
 * @file   GurobiApi.h
 * @brief  Gurobi API by writing out problem file in LP format and read solution. 
 *         Consider the file IO overhead before using it. 
 * @author Yibo Lin
 * @date   Nov 2014
 */

#ifndef LIMBO_SOLVERS_API_GUROBIAPI_H
#define LIMBO_SOLVERS_API_GUROBIAPI_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp> 
#include <limbo/solvers/Solvers.h>
// make sure gurobi is configured properly 
extern "C" 
{
#include "gurobi_c.h"
}

/// namespace for Limbo
namespace limbo 
{ 
/// namespace for Solvers 
namespace solvers 
{

/// @brief Base class for custom Gurobi parameters 
class GurobiParameters
{
    public:
        /// @brief constructor 
        GurobiParameters() 
            : m_outputFlag(0)
            , m_numThreads(std::numeric_limits<int>::max())
        {
        }
        /// @brief destructor 
        virtual ~GurobiParameters() {}
        /// @brief customize environment 
        /// @param env Gurobi environment 
        virtual void operator()(GRBenv* env) const 
        {
            // mute the log from the LP solver
            GRBsetintparam(env, GRB_INT_PAR_OUTPUTFLAG, m_outputFlag); 
            if (m_numThreads > 0 && m_numThreads != std::numeric_limits<int>::max())
                GRBsetintparam(env, GRB_INT_PAR_THREADS, m_numThreads);
        }
        /// @brief customize model 
        /// 
        /// param model Gurobi model 
        virtual void operator()(GRBmodel* /*model*/) const 
        {
        }

        /// @brief set output flag 
        /// @param v value 
        void setOutputFlag(int v) {m_outputFlag = v;}
        /// @brief set number of threads 
        /// @param v value 
        void setNumThreads(int v) {m_numThreads = v;}

    protected:
        int m_outputFlag; ///< control log from Gurobi 
        int m_numThreads; ///< number of threads 
};

/// @brief Gurobi API with @ref limbo::solvers::LinearModel
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class GurobiLinearApi 
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
        typedef GurobiParameters parameter_type; 
        /// @endnowarn
        
        /// @brief constructor 
        /// @param model pointer to the model of problem 
        GurobiLinearApi(model_type* model);
        /// @brief destructor 
        ~GurobiLinearApi(); 
        
        /// @brief API to run the algorithm 
        /// @param param set additional parameters, use default if NULL 
        SolverProperty operator()(parameter_type* param = NULL); 

        /// @brief error handler 
        /// @param env environment 
        /// @param error error type 
        void errorHandler(GRBenv* env, int error) const; 
    protected:
        /// @brief copy constructor, forbidden 
        /// @param rhs right hand side 
        GurobiLinearApi(GurobiLinearApi const& rhs);
        /// @brief assignment, forbidden 
        /// @param rhs right hand side 
        GurobiLinearApi& operator=(GurobiLinearApi const& rhs);

        model_type* m_model; ///< model for the problem 
        GRBmodel* m_grbModel; ///< model for Gurobi 
};

template <typename T, typename V>
GurobiLinearApi<T, V>::GurobiLinearApi(GurobiLinearApi<T, V>::model_type* model)
    : m_model(model)
    , m_grbModel(NULL)
{
}
template <typename T, typename V>
GurobiLinearApi<T, V>::~GurobiLinearApi()
{
}
template <typename T, typename V>
SolverProperty GurobiLinearApi<T, V>::operator()(GurobiLinearApi<T, V>::parameter_type* param)
{
    bool defaultParam = false; 
    if (param == NULL)
    {
        param = new GurobiParameters;
        defaultParam = true; 
    }

	// ILP environment
	GRBenv* env = NULL; 
    m_grbModel = NULL; 
    // Create environment
    int error = GRBloadenv(&env, NULL);
    errorHandler(env, error);
    param->operator()(env); 
    // Create an empty model 
    error = GRBnewmodel(env, &m_grbModel, "GurobiLinearApi", m_model->numVariables(), NULL, NULL, NULL, NULL, NULL);
    errorHandler(env, error);

    // create variables 
    error = GRBupdatemodel(m_grbModel);
    errorHandler(env, error);
    for (unsigned int i = 0, ie = m_model->numVariables(); i < ie; ++i)
    {
        variable_type var (i);
        error = GRBsetdblattrelement(m_grbModel, GRB_DBL_ATTR_LB, var.id(), m_model->variableLowerBound(var));
        errorHandler(env, error);
        error = GRBsetdblattrelement(m_grbModel, GRB_DBL_ATTR_UB, var.id(), m_model->variableUpperBound(var));
        errorHandler(env, error);
        error = GRBsetdblattrelement(m_grbModel, GRB_DBL_ATTR_START, var.id(), m_model->variableSolution(var));
        errorHandler(env, error);
        error = GRBsetcharattrelement(m_grbModel, GRB_CHAR_ATTR_VTYPE, var.id(), m_model->variableNumericType(var) == CONTINUOUS? GRB_CONTINUOUS : GRB_INTEGER);
        errorHandler(env, error);
        error = GRBsetstrattrelement(m_grbModel, GRB_STR_ATTR_VARNAME, var.id(), m_model->variableName(var).c_str());
        errorHandler(env, error);
    }

    // create constraints 
    std::vector<int> vIdx; 
    std::vector<double> vValue; 
    for (unsigned int i = 0, ie = m_model->constraints().size(); i < ie; ++i)
    {
        constraint_type const& constr = m_model->constraints().at(i);

        vIdx.clear(); 
        vValue.clear();
        for (typename std::vector<term_type>::const_iterator it = constr.expression().terms().begin(), ite = constr.expression().terms().end(); it != ite; ++it)
        {
            vIdx.push_back(it->variable().id()); 
            vValue.push_back(it->coefficient());
        }

        error = GRBaddconstr(m_grbModel, constr.expression().terms().size(), &vIdx[0], &vValue[0], constr.sense(), constr.rightHandSide(), m_model->constraintName(constr).c_str());
        errorHandler(env, error);
    }

    // create objective 
    for (typename std::vector<term_type>::const_iterator it = m_model->objective().terms().begin(), ite = m_model->objective().terms().end(); it != ite; ++it)
    {
        error = GRBsetdblattrelement(m_grbModel, GRB_DBL_ATTR_OBJ, it->variable().id(), it->coefficient());
        errorHandler(env, error);
    }
    error = GRBsetintattr(m_grbModel, GRB_INT_ATTR_MODELSENSE, m_model->optimizeType() == MIN? GRB_MINIMIZE : GRB_MAXIMIZE);
    errorHandler(env, error);

    // call parameter setting before optimization 
    param->operator()(m_grbModel); 
    error = GRBupdatemodel(m_grbModel);
    errorHandler(env, error);

#ifdef DEBUG_GUROBIAPI
    GRBwrite(m_grbModel, "problem.lp");
#endif 
    error = GRBoptimize(m_grbModel);

    int status = 0; 
    error = GRBgetintattr(m_grbModel, GRB_INT_ATTR_STATUS, &status);
    errorHandler(env, error);

    if (status == GRB_INFEASIBLE)
    {
        error = GRBcomputeIIS(m_grbModel);
        errorHandler(env, error); 
        GRBwrite(m_grbModel, "problem.ilp");
        limboPrint(kERROR, "Model is infeasible, compute IIS and write to problem.ilp\n");
    }
#ifdef DEBUG_GUROBIAPI
    GRBwrite(m_grbModel, "problem.sol");
#endif 

    for (unsigned int i = 0; i < m_model->numVariables(); ++i)
    {
        variable_type var = m_model->variable(i); 
        double value = 0; 
        error = GRBgetdblattrelement(m_grbModel, GRB_DBL_ATTR_X, var.id(), &value);
        errorHandler(env, error);
        m_model->setVariableSolution(m_model->variable(i), value);
    }

    if (defaultParam)
        delete param; 
    // Free model 
    GRBfreemodel(m_grbModel);
    // Free environment 
    GRBfreeenv(env);

    switch (status)
    {
        case GRB_OPTIMAL:
            return OPTIMAL;
        case GRB_INFEASIBLE:
            return INFEASIBLE;
        case GRB_INF_OR_UNBD:
        case GRB_UNBOUNDED:
            return UNBOUNDED;
        default:
            limboAssertMsg(0, "unknown status %d", status);
    }
}

template <typename T, typename V>
void GurobiLinearApi<T, V>::errorHandler(GRBenv* env, int error) const 
{
    // Error reporting 
    if (error) 
        limboAssertMsg(0, "%s", GRBgeterrormsg(env)); 
}

#if GUROBIFILEAPI == 1
#include "gurobi_c++.h"
/// This api needs a file in LP format as input.
/// And then read the solution output by gurobi 
/// I plan to come up with more efficient api without file io operations 
template <typename T>
struct GurobiFileApi 
{
    /// @brief value type 
	typedef T value_type;

	/// @brief data structure to save solution
	struct solution_type 
	{
		value_type obj; ///< objective value 
		std::list<std::pair<std::string, value_type> > vVariable; ///< std::list of (variable, solution) pairs 
	};
	/// @brief API function 
    /// @param fileName input file name 
    /// @return solution 
	virtual boost::shared_ptr<solution_type> operator()(std::string const& fileName, bool = true) const 
	{
		// better to use full path for file name 
		boost::shared_ptr<solution_type> pSol (new solution_type);
		// remove previous solution file 
		std::cout << "rm -rf "+fileName+".sol" << std::endl;
		std::cout << system(("rm -rf "+fileName+".sol").c_str()) << std::endl;;

		std::cout << "solve linear program "+fileName << std::endl;
		this->solve_lp(fileName);

		// read rpt 
		{
			std::ifstream solFile ((fileName+".sol").c_str(), std::ifstream::in);
			if (!solFile.good()) BOOST_ASSERT_MSG(false, ("failed to open " + fileName + ".sol").c_str());

			std::string var;
			double value;

			// read objective value 
			solFile >> var >> var >> var >> var >> value;
			pSol->obj = value;

			while (!solFile.eof())
			{
				solFile >> var >> value;
				pSol->vVariable.push_back(make_pair(var, value));
			}
			solFile.close();
		}

		return pSol;
	}
	/// Core function to solve lp problem with Gurobi. 
	/// It is modified from examples of Gurobi. 
	/// Basically it reads input problem file, and output solution file. 
    /// @param fileName input file 
	virtual void solve_lp(std::string fileName) const 
	{
		try 
		{
			GRBEnv env = GRBEnv();
			GRBModel model = GRBModel(env, fileName+".lp");

			model.optimize();

			int optimstatus = model.get(GRB_IntAttr_Status);

			if (optimstatus == GRB_INF_OR_UNBD) 
			{
				model.getEnv().set(GRB_IntParam_Presolve, 0);
				model.optimize();
				optimstatus = model.get(GRB_IntAttr_Status);
			}

			if (optimstatus == GRB_OPTIMAL) 
			{
				double objval = model.get(GRB_DoubleAttr_ObjVal);
				std::cout << "Optimal objective: " << objval << std::endl;
				// write result 
				model.write(fileName+".sol");
			} 
			else if (optimstatus == GRB_INFEASIBLE) 
			{
				std::cout << "Model is infeasible" << std::endl;

				// compute and write out IIS

				model.computeIIS();
				model.write(fileName+".ilp");
			} 
			else if (optimstatus == GRB_UNBOUNDED) 
			{
				std::cout << "Model is unbounded" << std::endl;
			} 
			else 
			{
				std::cout << "Optimization was stopped with status = "
					<< optimstatus << std::endl;
			}
		} 
		catch(GRBException e) 
		{
			std::cout << "Error code = " << e.getErrorCode() << std::endl;
			std::cout << e.getMessage() << std::endl;
		} 
		catch (...) 
		{
			std::cout << "Error during optimization" << std::endl;
		}
	}

};
#endif

} // namespace solvers
} // namespace limbo

#endif 
