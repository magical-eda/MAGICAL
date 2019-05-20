/**
 * @file   MinCostFlow.h
 * @author Yibo Lin
 * @date   Nov 2017
 * @brief  Solve a special case of linear programming with primal min-cost flow.
 */
#ifndef LIMBO_SOLVERS_MINCOSTFLOW_H
#define LIMBO_SOLVERS_MINCOSTFLOW_H

#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>
#include <lemon/cost_scaling.h>
#include <lemon/capacity_scaling.h>
#include <lemon/cycle_canceling.h>
#include <lemon/lgf_writer.h>

#include <limbo/solvers/Solvers.h>

/// namespace for Limbo 
namespace limbo 
{
/// namespace for Limbo.Solvers 
namespace solvers 
{

// forward declaration 
// Only CapacityScaling supports real value costs. 
// All other methods require integer costs. 
template <typename T, typename V>
class MinCostFlowSolver;
template <typename T, typename V>
class CapacityScaling;
template <typename T, typename V>
class CostScaling;
template <typename T, typename V>
class NetworkSimplex;
template <typename T, typename V>
class CycleCanceling;

/// @class limbo::solvers::MinCostFlow
/// @brief LP solved with min-cost flow. 
/// 
/// The primal problem of this LP is a min-cost flow problem, 
/// so we can solve the graph problem. 
///
/// 1. Primal problem \n
/// \f{eqnarray*}{
/// & min. & \sum_{i, j \in E} c_{ij} \cdot x_{ij}, \\
/// & s.t. & l_{ij} \le x_{ij} \le u_{ij}, \forall (i, j) \in E,  \\
/// &     & \sum_{j \in V} x_{ij} = s_i, \forall i \ne t, \in V, \\
/// &     & x_{ij} = -x_{ji}, \forall (i, j) \in E. (implicit) 
/// \f}
/// \n
/// Only CapacityScaling algorithm supports real-value costs. 
/// All other algorithms require integer costs, supply and capacity. 
/// 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class MinCostFlow 
{
    public:
        /// @brief linear model type for the problem 
        typedef LinearModel<T, V> model_type; 
        /// @nowarn
        typedef typename model_type::coefficient_value_type coefficient_value_type; 
        typedef typename model_type::variable_value_type variable_value_type; 
        typedef variable_value_type value_type; // use only one kind of value type 
        typedef typename model_type::variable_type variable_type;
        typedef typename model_type::constraint_type constraint_type; 
        typedef typename model_type::expression_type expression_type; 
        typedef typename model_type::term_type term_type; 
        typedef typename model_type::property_type property_type;

		typedef lemon::SmartDigraph graph_type;
		typedef graph_type::Node node_type;
		typedef graph_type::Arc arc_type;
		typedef graph_type::NodeMap<value_type> node_value_map_type;
		typedef graph_type::NodeMap<std::string> node_name_map_type;
		typedef graph_type::ArcMap<std::string> arc_name_map_type;
		typedef graph_type::ArcMap<value_type> arc_value_map_type;
		typedef graph_type::ArcMap<value_type> arc_cost_map_type;
		typedef graph_type::ArcMap<value_type> arc_flow_map_type;
		typedef graph_type::NodeMap<value_type> node_pot_map_type; // potential of each node 

        typedef MinCostFlowSolver<coefficient_value_type, variable_value_type> solver_type; 
        /// @endnowarn

        /// @brief constructor 
        /// @param model pointer to the model of problem 
        MinCostFlow(model_type* model);
        /// @brief destructor 
        ~MinCostFlow(); 
        
        /// @brief API to run the algorithm 
        /// @param solver an object to solve min cost flow, use default updater if NULL  
        SolverProperty operator()(solver_type* solver = NULL); 

        /// @return graph 
        graph_type const& graph() const; 
        /// @return arc lower bound map 
        arc_value_map_type const& lowerMap() const; 
        /// @return arc upper bound map 
        arc_value_map_type const& upperMap() const; 
        /// @return arc cost map 
        arc_cost_map_type const& costMap() const; 
        /// @return node supply map 
        node_value_map_type const& supplyMap() const; 
        /// @return arc flow map 
        arc_flow_map_type& flowMap();
        /// @return node potential map 
        node_pot_map_type& potentialMap(); 
        /// @return total flow cost 
        value_type totalFlowCost() const; 
        /// @param cost total cost of min-cost flow graph 
        void setTotalFlowCost(value_type cost); 
        /// @return total cost of the original LP problem 
        value_type totalCost() const; 

        /// @name print functions to debug.lgf 
        ///@{
        /// @brief print graph 
        /// @param writeSol if true write flow and potential as well 
        void printGraph(bool writeSol) const; 
        ///@}
    protected:
        /// @brief copy constructor, forbidden  
        /// @param rhs right hand side 
        MinCostFlow(MinCostFlow const& rhs);
        /// @brief assignment, forbidden  
        /// @param rhs right hand side 
        MinCostFlow& operator=(MinCostFlow const& rhs);

        /// @brief kernel function to solve the problem 
        /// @param solver an object to solve min cost flow, use default updater if NULL  
        SolverProperty solve(solver_type* solver);
        /// @brief build min-cost flow graph 
        void buildGraph(); 
        /// @brief set objective, support incremental set 
        void setObjective(expression_type const& obj); 
        /// @brief apply solutions to model 
        void applySolution(); 

        model_type* m_model; ///< model for the problem 

		graph_type m_graph; ///< input graph 
		arc_value_map_type m_mLower; ///< lower bound of flow, usually zero  
		arc_value_map_type m_mUpper; ///< upper bound of flow, arc capacity in min-cost flow  
		arc_cost_map_type m_mCost; ///< arc cost in min-cost flow 
		node_value_map_type m_mSupply; ///< node supply in min-cost flow 
		value_type m_totalFlowCost; ///< total cost after solving 

		arc_flow_map_type m_mFlow; ///< solution of min-cost flow, which is the solution of LP 
		node_pot_map_type m_mPotential; ///< solution of min-cost flow, which is the dual solution of LP 
};

template <typename T, typename V>
MinCostFlow<T, V>::MinCostFlow(typename MinCostFlow<T, V>::model_type* model)
    : m_model(model)
    , m_graph()
    , m_mLower(m_graph)
    , m_mUpper(m_graph)
    , m_mCost(m_graph)
    , m_mSupply(m_graph)
    , m_totalFlowCost(std::numeric_limits<typename MinCostFlow<T, V>::value_type>::max())
    , m_mFlow(m_graph)
    , m_mPotential(m_graph)
{
}
template <typename T, typename V>
MinCostFlow<T, V>::~MinCostFlow() 
{
}
template <typename T, typename V>
SolverProperty MinCostFlow<T, V>::operator()(typename MinCostFlow<T, V>::solver_type* solver)
{
    return solve(solver);
}
template <typename T, typename V>
typename MinCostFlow<T, V>::graph_type const& MinCostFlow<T, V>::graph() const 
{
    return m_graph;
}
template <typename T, typename V>
typename MinCostFlow<T, V>::arc_value_map_type const& MinCostFlow<T, V>::lowerMap() const
{
    return m_mLower;
}
template <typename T, typename V>
typename MinCostFlow<T, V>::arc_value_map_type const& MinCostFlow<T, V>::upperMap() const 
{
    return m_mUpper;
}
template <typename T, typename V>
typename MinCostFlow<T, V>::arc_cost_map_type const& MinCostFlow<T, V>::costMap() const 
{
    return m_mCost;
}
template <typename T, typename V>
typename MinCostFlow<T, V>::node_value_map_type const& MinCostFlow<T, V>::supplyMap() const 
{
    return m_mSupply;
}
template <typename T, typename V>
typename MinCostFlow<T, V>::arc_flow_map_type& MinCostFlow<T, V>::flowMap() 
{
    return m_mFlow;
}
template <typename T, typename V>
typename MinCostFlow<T, V>::node_pot_map_type& MinCostFlow<T, V>::potentialMap() 
{
    return m_mPotential;
}
template <typename T, typename V>
typename MinCostFlow<T, V>::value_type MinCostFlow<T, V>::totalFlowCost() const
{
    return m_totalFlowCost; 
}
template <typename T, typename V>
void MinCostFlow<T, V>::setTotalFlowCost(typename MinCostFlow<T, V>::value_type cost)
{
    m_totalFlowCost = cost; 
}
template <typename T, typename V>
typename MinCostFlow<T, V>::value_type MinCostFlow<T, V>::totalCost() const 
{
    return totalFlowCost(); 
}
template <typename T, typename V>
void MinCostFlow<T, V>::printGraph(bool writeSol) const
{
    if (writeSol)
        limboPrint(kDEBUG, "totalFlowCost = %ld, totalCost = %ld\n", (long)totalFlowCost(), (long)totalCost()); 

    node_name_map_type nodeNameMap (m_graph); 
    for (unsigned int i = 0, ie = m_model->constraints().size(); i < ie; ++i)
        nodeNameMap[m_graph.nodeFromId(i)] = m_model->constraintNames().at(i); 
    nodeNameMap[m_graph.nodeFromId(m_graph.maxNodeId())] = "st";

    arc_name_map_type arcNameMap (m_graph); 
    for (unsigned int i = 0, ie = m_model->numVariables(); i < ie; ++i)
        arcNameMap[m_graph.arcFromId(i)] = m_model->variableName(variable_type(i)); 

    // dump lgf file 
    lemon::DigraphWriter<graph_type> writer(m_graph, "debug.lgf");
    writer.nodeMap("supply", m_mSupply)
        .nodeMap("name", nodeNameMap)
        .arcMap("name", arcNameMap)
        .arcMap("capacity_lower", m_mLower)
        .arcMap("capacity_upper", m_mUpper)
        .arcMap("cost", m_mCost);
    if (writeSol)
        writer.nodeMap("potential", m_mPotential)
            .arcMap("flow", m_mFlow);
    writer.run();
}
template <typename T, typename V>
SolverProperty MinCostFlow<T, V>::solve(typename MinCostFlow<T, V>::solver_type* solver)
{
    bool defaultSolver = false; 
    // use default solver if NULL 
    if (solver == NULL)
    {
        solver = new CostScaling<coefficient_value_type, variable_value_type>(); 
        defaultSolver = true; 
    }

    // skip empty problem 
    if (m_model->numVariables() == 0)
        return OPTIMAL; 

    // build graph if no nodes, I know in corner cases it may be called repeatedly 
    // but this seems to be the best way 
    if (m_graph.nodeNum() == 0)
        buildGraph(); 
    setObjective(m_model->objective());
#ifdef DEBUG_MINCOSTFLOW
    printGraph(false);
    // total supply must be zero 
    coefficient_value_type totalSupply = 0; 
    for (graph_type::NodeIt it (m_graph); it != lemon::INVALID; ++it)
        totalSupply += m_mSupply[it]; 
    limboAssert(totalSupply == 0);
#endif
    // solve min-cost flow problem 
    SolverProperty status = solver->operator()(this); 
    // apply solution 
    applySolution(); 

#ifdef DEBUG_MINCOSTFLOW
    printGraph(true);
#endif

    if (defaultSolver)
        delete solver; 

    return status; 
}
template <typename T, typename V>
void MinCostFlow<T, V>::buildGraph() 
{
    // compute how many nodes 
    // regard equality constraints as nodes except for s and t  
    unsigned int numNodes = 0; 
    for (typename std::vector<constraint_type>::const_iterator it = m_model->constraints().begin(), ite = m_model->constraints().end(); it != ite; ++it)
    {
        // assume only equality constraints 
        limboAssertMsg(it->sense() == '=', "only support equality constraints %s", m_model->constraintName(*it).c_str()); 
        // equality constraints 
        numNodes += 1; 
    }

    // construct nodes 
    coefficient_value_type totalSupply = 0; 
    m_graph.reserveNode(numNodes+1); // additional nodes for s and t
    for (unsigned int i = 0, ie = numNodes+1; i < ie; ++i)
        m_graph.addNode(); 
    // node st 
    node_type st = m_graph.nodeFromId(numNodes); 

    // a map record arc to its two nodes 
    // arc id, source node id, target node id 
    // variable id, constraint id positive, constraint id negative 
    // I assume vVar2Constr sorts according to variable id 
    std::vector<std::pair<unsigned int, unsigned int> > vVar2Constr (m_model->numVariables(), std::make_pair(std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max())); 
    unsigned int constrIndex = 0; 
    for (typename std::vector<constraint_type>::const_iterator it = m_model->constraints().begin(), ite = m_model->constraints().end(); it != ite; ++it)
    {
        constraint_type constr = *it; 
        // equality constraints 
        //if (constr.sense() == '=')
        {
            // whether need to inverse the sign of the constraint 
            // only need to check the first found term 
            for (typename std::vector<term_type>::const_iterator itt = constr.expression().terms().begin(), itte = constr.expression().terms().end(); itt != itte; ++itt)
            {
                term_type const& term = *itt; 
                std::pair<unsigned int, unsigned int> const& value = vVar2Constr[term.variable().id()];
                if (term.coefficient() == 1)
                {
                    // I want value.first is not set 
                    if (value.first != std::numeric_limits<unsigned int>::max())
                    {
                        constr.scale(-1); 
                        break; 
                    }
                    else if (value.second != std::numeric_limits<unsigned int>::max())
                    {
                        break; 
                    }
                }
                else if (term.coefficient() == -1)
                {
                    // I want value.first is not set 
                    if (value.second != std::numeric_limits<unsigned int>::max())
                    {
                        constr.scale(-1); 
                        break; 
                    }
                    else if (value.first != std::numeric_limits<unsigned int>::max())
                    {
                        break; 
                    }
                }
                else 
                {
                    limboAssertMsg(0, "unsupported coefficient %g in constraint %d\n", double(term.coefficient()), constr.id()); 
                }
            }
            // record map 
            for (typename std::vector<term_type>::const_iterator itt = constr.expression().terms().begin(), itte = constr.expression().terms().end(); itt != itte; ++itt)
            {
                term_type const& term = *itt; 
                std::pair<unsigned int, unsigned int>& value = vVar2Constr[term.variable().id()];
                if (term.coefficient() == 1)
                {
                    limboAssertMsg(value.first == std::numeric_limits<unsigned int>::max(), 
                            "variable %s appears in more than 1 constraint with coefficient 1", m_model->variableName(term.variable()).c_str());
                    value.first = constrIndex; 
                }
                else if (term.coefficient() == -1)
                {
                    limboAssertMsg(value.second == std::numeric_limits<unsigned int>::max(), 
                            "variable %s appears in more than 1 constraint with coefficient -1", m_model->variableName(term.variable()).c_str());
                    value.second = constrIndex; 
                }
            }
            // compute supply 
            // since here we know whether this constraint is inversed
            m_mSupply[m_graph.nodeFromId(constrIndex)] = constr.rightHandSide(); 
            totalSupply += constr.rightHandSide(); 
            // next cosntraint 
            ++constrIndex; 
        }
    }
    // supply for s and t 
    m_mSupply[st] = -totalSupply; 

    // construct arcs 
    m_graph.reserveArc(vVar2Constr.size()); 
    // I assume vVar2Constr sorts according to variable id 
    unsigned int var = 0; 
    for (std::vector<std::pair<unsigned int, unsigned int> >::const_iterator it = vVar2Constr.begin(), ite = vVar2Constr.end(); it != ite; ++it, ++var)
    {
        unsigned int constrSrc = it->first;
        unsigned int constrTgt = it->second;

        arc_type arc; 
        if (constrSrc == std::numeric_limits<unsigned int>::max()) // from s 
        {
            arc = m_graph.addArc(st, m_graph.nodeFromId(constrTgt));
        }
        else if (constrTgt == std::numeric_limits<unsigned int>::max()) // to t 
        {
            arc = m_graph.addArc(m_graph.nodeFromId(constrSrc), st);
        }
        else 
        {
            arc = m_graph.addArc(m_graph.nodeFromId(constrSrc), m_graph.nodeFromId(constrTgt));
        }
        m_mLower[arc] = m_model->variableLowerBound(m_model->variable(var));
        m_mUpper[arc] = m_model->variableUpperBound(m_model->variable(var));
#ifdef DEBUG_MINCOSTFLOW
        limboAssert(arc == m_graph.arcFromId(var));
#endif
    }

    // construct cost map 
    // everytime solver is called 
}
template <typename T, typename V>
void MinCostFlow<T, V>::setObjective(typename MinCostFlow<T, V>::expression_type const& obj)
{
    for (typename std::vector<term_type>::const_iterator it = obj.terms().begin(), ite = obj.terms().end(); it != ite; ++it)
    {
        term_type const& term = *it; 
        switch (m_model->optimizeType())
        {
            case MIN:
                m_mCost[m_graph.arcFromId(term.variable().id())] = term.coefficient(); 
                break; 
            case MAX:
                m_mCost[m_graph.arcFromId(term.variable().id())] = -term.coefficient(); 
                break; 
            default:
                limboAssertMsg(0, "Unknown optimization type"); 
                break; 
        }
    }
}
template <typename T, typename V>
void MinCostFlow<T, V>::applySolution()
{
    unsigned int i = 0; 
    for (typename std::vector<value_type>::iterator it = m_model->variableSolutions().begin(), ite = m_model->variableSolutions().end(); it != ite; ++it, ++i)
        *it = m_mFlow[m_graph.arcFromId(i)]; 
}

/// @brief A base class of min-cost flow solver 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class MinCostFlowSolver
{
    public:
        /// @brief dual min-cost flow solver type 
        typedef MinCostFlow<T, V> primalsolver_type; 

        /// @brief constructor 
        MinCostFlowSolver() {} 
        /// @brief copy constructor 
        /// @param rhs right hand side 
        MinCostFlowSolver(MinCostFlowSolver const& rhs) 
        {
            copy(rhs);
        }
        /// @brief assignment 
        /// @param rhs right hand side 
        MinCostFlowSolver& operator=(MinCostFlowSolver const& rhs)
        {
            if (this != &rhs)
                copy(rhs);
            return *this;
        }
        /// @brief destructor 
        virtual ~MinCostFlowSolver() {}

        /// @brief API to run min-cost flow solver 
        /// @param d dual min-cost flow object 
        virtual SolverProperty operator()(primalsolver_type* d) = 0; 
    protected:
        /// @brief copy object 
        void copy(MinCostFlowSolver const& /*rhs*/) {} 
};

/// @brief Capacity scaling algorithm for min-cost flow 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class CapacityScaling : public MinCostFlowSolver<T, V>
{
    public:
        /// @brief value type 
        typedef T value_type;
        /// @brief base type 
        typedef MinCostFlowSolver<T, V> base_type; 
        /// @brief dual min-cost flow solver type 
        typedef typename base_type::primalsolver_type primalsolver_type; 
        /// @brief algorithm type 
        typedef lemon::CapacityScaling<typename primalsolver_type::graph_type, 
                value_type, 
                value_type> alg_type;

        /// @brief constructor 
        /// @param factor scaling factor 
        CapacityScaling(int factor = 4)
            : base_type()
            , m_factor(factor)
        {
        }
        /// @brief copy constructor 
        /// @param rhs right hand side 
        CapacityScaling(CapacityScaling const& rhs)
            : CapacityScaling::base_type(rhs)
        {
            copy(rhs);
        }
        /// @brief assignment 
        /// @param rhs right hand side 
        CapacityScaling& operator=(CapacityScaling const& rhs)
        {
            if (this != &rhs)
            {
                this->base_type::operator=(rhs);
                copy(rhs);
            }
            return *this;
        }

        /// @brief API to run min-cost flow solver 
        /// @param d dual min-cost flow object 
        virtual SolverProperty operator()(primalsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                .lowerMap(d->lowerMap())
                .upperMap(d->upperMap())
                .costMap(d->costMap())
                .supplyMap(d->supplyMap())
                .run(m_factor);

            // 3. check results 
            SolverProperty solverStatus; 
            switch (status)
            {
                case alg_type::OPTIMAL:
                    solverStatus = OPTIMAL; 
                    break;
                case alg_type::INFEASIBLE:
                    solverStatus = INFEASIBLE; 
                    break;
                case alg_type::UNBOUNDED:
                    solverStatus = UNBOUNDED; 
                    break;
                default:
                    limboAssertMsg(0, "unknown status");
            }

            // 4. apply results 
            // get dual solution of LP, which is the flow of min-cost flow, skip this if not necessary
            alg.flowMap(d->flowMap());
            // get solution of LP, which is the dual solution of min-cost flow 
            alg.potentialMap(d->potentialMap());
            // set total cost of min-cost flow 
            d->setTotalFlowCost(alg.totalCost());

            return solverStatus; 
        }
    protected:
        /// @brief copy object 
        void copy(CapacityScaling const& rhs)
        {
            m_factor = rhs.m_factor;
        }

        int m_factor; ///< scaling factor for the algorithm 
};

/// @brief Cost scaling algorithm for min-cost flow 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class CostScaling : public MinCostFlowSolver<T, V>
{
    public:
        /// @brief value type 
        typedef T value_type;
        /// @brief base type 
        typedef MinCostFlowSolver<T, V> base_type; 
        /// @brief dual min-cost flow solver type 
        typedef typename base_type::primalsolver_type primalsolver_type; 
        /// @brief algorithm type 
        typedef lemon::CostScaling<typename primalsolver_type::graph_type, 
                value_type, 
                value_type> alg_type;

        /// @brief constructor 
        /// @param method internal method 
        /// @param factor scaling factor 
        CostScaling(typename alg_type::Method method = alg_type::PARTIAL_AUGMENT, int factor = 16)
            : base_type()
            , m_method(method)
            , m_factor(factor)
        {
        }
        /// @brief copy constructor 
        /// @param rhs right hand side 
        CostScaling(CostScaling const& rhs)
            : base_type(rhs)
        {
            copy(rhs);
        }
        /// @brief assignment 
        /// @param rhs right hand side 
        CostScaling& operator=(CostScaling const& rhs)
        {
            if (this != &rhs)
            {
                this->base_type::operator=(rhs);
                copy(rhs);
            }
            return *this;
        }

        /// @brief API to run min-cost flow solver 
        /// @param d dual min-cost flow object 
        virtual SolverProperty operator()(primalsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                .lowerMap(d->lowerMap())
                .upperMap(d->upperMap())
                .costMap(d->costMap())
                .supplyMap(d->supplyMap())
                .run(m_method, m_factor);

            // 3. check results 
            SolverProperty solverStatus; 
            switch (status)
            {
                case alg_type::OPTIMAL:
                    solverStatus = OPTIMAL; 
                    break;
                case alg_type::INFEASIBLE:
                    solverStatus = INFEASIBLE; 
                    break;
                case alg_type::UNBOUNDED:
                    solverStatus = UNBOUNDED; 
                    break;
                default:
                    limboAssertMsg(0, "unknown status");
            }

            // 4. apply results 
            // get dual solution of LP, which is the flow of min-cost flow, skip this if not necessary
            alg.flowMap(d->flowMap());
            // get solution of LP, which is the dual solution of min-cost flow 
            alg.potentialMap(d->potentialMap());
            // set total cost of min-cost flow 
            d->setTotalFlowCost(alg.totalCost());

            return solverStatus; 
        }
    protected:
        /// @brief copy object 
        void copy(CostScaling const& rhs)
        {
            m_method = rhs.m_method;
            m_factor = rhs.m_factor;
        }

        typename alg_type::Method m_method; ///< PUSH, AUGMENT, PARTIAL_AUGMENT
        int m_factor; ///< scaling factor for the algorithm 
};

/// @brief Network simplex algorithm for min-cost flow 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class NetworkSimplex : public MinCostFlowSolver<T, V>
{
    public:
        /// @brief value type 
        typedef T value_type;
        /// @brief base type 
        typedef MinCostFlowSolver<T, V> base_type; 
        /// @brief dual min-cost flow solver type 
        typedef typename base_type::primalsolver_type primalsolver_type; 
        /// @brief algorithm type 
        typedef lemon::NetworkSimplex<typename primalsolver_type::graph_type, 
                value_type, 
                value_type> alg_type;

        /// @brief constructor 
        /// @param pivotRule pivot rule 
        NetworkSimplex(typename alg_type::PivotRule pivotRule = alg_type::BLOCK_SEARCH)
            : base_type()
            , m_pivotRule(pivotRule)
        {
        }
        /// @brief copy constructor 
        /// @param rhs right hand side 
        NetworkSimplex(NetworkSimplex const& rhs)
            : base_type(rhs)
        {
            copy(rhs);
        }
        /// @brief assignment 
        /// @param rhs right hand side 
        NetworkSimplex& operator=(NetworkSimplex const& rhs)
        {
            if (this != &rhs)
            {
                this->base_type::operator=(rhs); 
                copy(rhs);
            }
            return *this;
        }

        /// @brief API to run min-cost flow solver 
        /// @param d dual min-cost flow object 
        virtual SolverProperty operator()(primalsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                .lowerMap(d->lowerMap())
                .upperMap(d->upperMap())
                .costMap(d->costMap())
                .supplyMap(d->supplyMap())
                .run(m_pivotRule);

            // 3. check results 
            SolverProperty solverStatus; 
            switch (status)
            {
                case alg_type::OPTIMAL:
                    solverStatus = OPTIMAL; 
                    break;
                case alg_type::INFEASIBLE:
                    solverStatus = INFEASIBLE; 
                    break;
                case alg_type::UNBOUNDED:
                    solverStatus = UNBOUNDED; 
                    break;
                default:
                    limboAssertMsg(0, "unknown status");
            }

            // 4. apply results 
            // get dual solution of LP, which is the flow of min-cost flow, skip this if not necessary
            alg.flowMap(d->flowMap());
            // get solution of LP, which is the dual solution of min-cost flow 
            alg.potentialMap(d->potentialMap());
            // set total cost of min-cost flow 
            d->setTotalFlowCost(alg.totalCost());

            return solverStatus; 
        }
    protected:
        /// @brief copy object 
        void copy(NetworkSimplex const& rhs)
        {
            m_pivotRule = rhs.m_pivotRule;
        }

        typename alg_type::PivotRule m_pivotRule; ///< pivot rule for the algorithm, FIRST_ELIGIBLE, BEST_ELIGIBLE, BLOCK_SEARCH, CANDIDATE_LIST, ALTERING_LIST
};

/// @brief Cycle canceling algorithm for min-cost flow 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class CycleCanceling : public MinCostFlowSolver<T, V>
{
    public:
        /// @brief value type 
        typedef T value_type;
        /// @brief base type 
        typedef MinCostFlowSolver<T, V> base_type; 
        /// @brief dual min-cost flow solver type 
        typedef typename base_type::primalsolver_type primalsolver_type; 
        /// @brief algorithm type 
        typedef lemon::CycleCanceling<typename primalsolver_type::graph_type, 
                value_type, 
                value_type> alg_type;

        /// @brief constructor 
        /// @param method method
        CycleCanceling(typename alg_type::Method method = alg_type::CANCEL_AND_TIGHTEN)
            : base_type()
            , m_method(method)
        {
        }
        /// @brief copy constructor 
        /// @param rhs right hand side 
        CycleCanceling(CycleCanceling const& rhs)
            : base_type(rhs)
        {
            copy(rhs);
        }
        /// @brief assignment 
        /// @param rhs right hand side 
        CycleCanceling& operator=(CycleCanceling const& rhs)
        {
            if (this != &rhs)
            {
                this->operator=(rhs);
                copy(rhs);
            }
            return *this;
        }

        /// @brief API to run min-cost flow solver 
        /// @param d dual min-cost flow object 
        virtual SolverProperty operator()(primalsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                .lowerMap(d->lowerMap())
                .upperMap(d->upperMap())
                .costMap(d->costMap())
                .supplyMap(d->supplyMap())
                .run(m_method);

            // 3. check results 
            SolverProperty solverStatus; 
            switch (status)
            {
                case alg_type::OPTIMAL:
                    solverStatus = OPTIMAL; 
                    break;
                case alg_type::INFEASIBLE:
                    solverStatus = INFEASIBLE; 
                    break;
                case alg_type::UNBOUNDED:
                    solverStatus = UNBOUNDED; 
                    break;
                default:
                    limboAssertMsg(0, "unknown status");
            }

            // 4. apply results 
            // get dual solution of LP, which is the flow of min-cost flow, skip this if not necessary
            alg.flowMap(d->flowMap());
            // get solution of LP, which is the dual solution of min-cost flow 
            alg.potentialMap(d->potentialMap());
            // set total cost of min-cost flow 
            d->setTotalFlowCost(alg.totalCost());

            return solverStatus; 
        }
    protected:
        /// @brief copy object 
        void copy(CycleCanceling const& rhs)
        {
            m_method = rhs.m_method;
        }

        typename alg_type::Method m_method; ///< method for the algorithm, SIMPLE_CYCLE_CANCELING, MINIMUM_MEAN_CYCLE_CANCELING, CANCEL_AND_TIGHTEN
};


} // namespace solvers 
} // namespace limbo 

#endif
