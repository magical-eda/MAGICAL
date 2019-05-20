/**
 * @file   DualMinCostFlow.h
 * @author Yibo Lin
 * @date   Feb 2017
 * @brief  Solve a special case of linear programming with dual min-cost flow.
 *         A better implementation of @ref LpDualMcf.h
 */
#ifndef LIMBO_SOLVERS_DUALMINCOSTFLOW_H
#define LIMBO_SOLVERS_DUALMINCOSTFLOW_H

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

/// @class limbo::solvers::DualMinCostFlow
/// @brief LP solved with min-cost flow. A better implementation of @ref limbo::solvers::lpmcf::LpDualMcf
/// 
/// The dual problem of this LP is a min-cost flow problem, 
/// so we can solve the graph problem and then 
/// call shortest path algrithm to calculate optimum of primal problem. 
///
/// 1. Primal problem \n
/// \f{eqnarray*}{
/// & min. & \sum_{i=1}^{n} c_i \cdot x_i - \sum_{i,j} u_{ij} \alpha_{ij}, \\
/// & s.t. & x_i - x_j - \alpha_{ij} \ge b_{ij}, \forall (i, j) \in E,  \\
/// &     & d_i \le x_i \le u_i, \forall i \in [1, n], \\
/// &     & \alpha_{ij} \ge 0, \forall (i, j) \in A.  
/// \f}
/// \n
/// 2. Introduce new variables \f$y_i\f$ in \f$[0, n]\f$, set \f$x_i = y_i - y_0\f$, \n
/// \f{eqnarray*}{
/// & min. & \sum_{i=1}^{n} c_i \cdot (y_i-y_0) - \sum_{i,j} u_{ij} \alpha_{ij}, \\
/// & s.t. & y_i - y_j -\alpha_{ij} \ge b_{ij}, \forall (i, j) \in E \\
/// &      & d_i \le y_i - y_0 - \alpha_{ij} \le u_i, \forall i \in [1, n], \\
/// &      & y_i \textrm{ is unbounded integer}, \forall i \in [0, n], \\
/// &      & \alpha_{ij} \ge 0, \forall (i, j) \in A.  
/// \f}
/// \n
/// 3. Re-write the problem \n
/// \f{eqnarray*}{
/// & min. & \sum_{i=0}^{n} c_i \cdot y_i - \sum_{i,j} u_{ij} \alpha_{ij}, \textrm{ where } \
///   c_i = \begin{cases}
///             c_i, & \forall i \in [1, n],  \\
///             - \sum_{j=1}^{n} c_i, & i = 0, \\
///           \end{cases} \\
/// & s.t. & y_i - y_j \ge \
///        \begin{cases}
///            b_{ij}, & \forall (i, j) \in E, \\
///            d_i,  & \forall j = 0, i \in [1, n], \\
///            -u_i, & \forall i = 0, j \in [1, n], \\
///        \end{cases} \\
/// &      & y_i \textrm{ is unbounded integer}, \forall i \in [0, n], \\
/// &      & \alpha_{ij} \ge 0, \forall (i, j) \in A.  
/// \f}
/// \n
/// 4. Map to dual min-cost flow problem. \n
///    Let's use \f$c'_i\f$ for generalized \f$c_i\f$ and \f$b'_{ij}\f$ for generalized \f$b_{ij}\f$. \n
///    Then \f$c'_i\f$ is node supply. 
///    For each \f$(i, j) \in E'\f$, an arc from i to j with cost \f$-b'_{ij}\f$ and flow range \f$[0, u_{ij}]\f$. 
///    The variable \f$\alpha_{ij}\f$ denotes the slackness in the primal problem, but the capacity constraint 
///    in the dual problem. We can set the edge capacity as \f$u_{ij}\f$. We can also leave the edge uncapacited (\f$\infty\f$)
///    if there are no such variables. \n
///    Some concolusions from \cite FLOW_B2005_Ahuja where \f$f_{ij}^*\f$ denotes the optimal flow on edge \f$(i, j)\f$ 
///    and \f$c_{ij}^\pi\f$ denotes the reduced cost in the residual network. \n
///    - If \f$c_{ij}^\pi > 0\f$, then \f$f_{ij}^* = 0\f$. 
///    - If \f$ 0 < f_{ij}^* < u_{ij} \f$, then \f$ c_{ij}^\pi = 0 \f$. 
///    - If \f$c_{ij}^\pi < 0\f$, then \f$f_{ij}^* = u_{ij}\f$. 
/// 
///    These conclusions might be useful to check optimality for the primal problem. 
/// \n
/// Caution: this mapping of LP to dual min-cost flow results may result in negative arc cost which is not supported 
/// by all the algorithms (only capacity scaling algorithm supports). Therefore, graph transformation is introduced 
/// to convert arcs with negative costs to positive costs with arc inversal. 
/// 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class DualMinCostFlow 
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
		typedef graph_type::ArcMap<value_type> arc_value_map_type;
		typedef graph_type::ArcMap<value_type> arc_cost_map_type;
		typedef graph_type::ArcMap<value_type> arc_flow_map_type;
		typedef graph_type::NodeMap<value_type> node_pot_map_type; // potential of each node 

        typedef MinCostFlowSolver<coefficient_value_type, variable_value_type> solver_type; 
        /// @endnowarn

        /// @brief constructor 
        /// @param model pointer to the model of problem 
        DualMinCostFlow(model_type* model);
        /// @brief destructor 
        ~DualMinCostFlow(); 
        
        /// @brief API to run the algorithm 
        /// @param solver an object to solve min cost flow, use default updater if NULL  
        SolverProperty operator()(solver_type* solver = NULL); 

        /// @return big M for differential constraints 
        value_type diffBigM() const; 
        /// @brief set big M as a large number for differential constraints 
        /// @param v value 
        void setDiffBigM(value_type v);
        /// @return big M for bound constraints 
        value_type boundBigM() const; 
        /// @brief set big M as a large number for bound constraints  
        /// @param v value 
        void setBoundBigM(value_type v);

        /// @return graph 
        graph_type const& graph() const; 
        /// @return arc lower bound map 
        //arc_value_map_type const& lowerMap() const; 
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
        DualMinCostFlow(DualMinCostFlow const& rhs);
        /// @brief assignment, forbidden  
        /// @param rhs right hand side 
        DualMinCostFlow& operator=(DualMinCostFlow const& rhs);

        /// @brief kernel function to solve the problem 
        /// @param solver an object to solve min cost flow, use default updater if NULL  
        SolverProperty solve(solver_type* solver);
        /// @brief prepare data like big M 
        void prepare(); 
        /// @brief build dual min-cost flow graph 
        void buildGraph(); 
        /// @brief map variables and the objective to graph nodes 
        void mapObjective2Graph();
        /// @brief map differential constraints to graph arcs 
        /// @param countArcs flag for counting arcs mode, if true, only count arcs and no actual arcs are added; otherwise, add arcs 
        /// @return number of arcs added 
        unsigned int mapDiffConstraint2Graph(bool countArcs);
        /// @brief map bound constraints to graph arcs 
        /// @param countArcs flag for counting arcs mode, if true, only count arcs and no actual arcs are added; otherwise, add arcs 
        /// @return number of arcs added 
        unsigned int mapBoundConstraint2Graph(bool countArcs);
        /// @brief generalized method to add an arc for differential constraint \f$ x_i - x_j \ge c_{ij} \f$, resolve negative arc costs by arc reversal 
        /// @param xi node corresponding to variable \f$ x_i \f$
        /// @param xj node corresponding to variable \f$ x_j \f$
        /// @param cij constant at right hand side 
        /// @param bigM large number for upper bound of capacity 
        void addArcForDiffConstraint(node_type xi, node_type xj, value_type cij, value_type bigM); 
        /// @brief apply solutions to model 
        void applySolution(); 

        model_type* m_model; ///< model for the problem 

		graph_type m_graph; ///< input graph 
		//arc_value_map_type m_mLower; ///< lower bound of flow, usually zero  
		arc_value_map_type m_mUpper; ///< upper bound of flow, arc capacity in min-cost flow  
		arc_cost_map_type m_mCost; ///< arc cost in min-cost flow 
		node_value_map_type m_mSupply; ///< node supply in min-cost flow 
		value_type m_totalFlowCost; ///< total cost after solving 
        value_type m_diffBigM; ///< a big number for infinity for differential constraints
        value_type m_boundBigM; ///< a big number for infinity for bound constraints 
        value_type m_reversedArcFlowCost; ///< normalized flow cost of overall reversed arcs to resolve negative arc cost, to get total flow cost of reversed arcs, it needs to times with big M 

		arc_flow_map_type m_mFlow; ///< solution of min-cost flow, which is the dual solution of LP 
		node_pot_map_type m_mPotential; ///< dual solution of min-cost flow, which is the solution of LP 
};

template <typename T, typename V>
DualMinCostFlow<T, V>::DualMinCostFlow(typename DualMinCostFlow<T, V>::model_type* model)
    : m_model(model)
    , m_graph()
    //, m_mLower(m_graph)
    , m_mUpper(m_graph)
    , m_mCost(m_graph)
    , m_mSupply(m_graph)
    , m_totalFlowCost(std::numeric_limits<typename DualMinCostFlow<T, V>::value_type>::max())
    , m_diffBigM(std::numeric_limits<typename DualMinCostFlow<T, V>::value_type>::max())
    , m_boundBigM(std::numeric_limits<typename DualMinCostFlow<T, V>::value_type>::max())
    , m_mFlow(m_graph)
    , m_mPotential(m_graph)
{
}
template <typename T, typename V>
DualMinCostFlow<T, V>::~DualMinCostFlow() 
{
}
template <typename T, typename V>
SolverProperty DualMinCostFlow<T, V>::operator()(typename DualMinCostFlow<T, V>::solver_type* solver)
{
    return solve(solver);
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::value_type DualMinCostFlow<T, V>::diffBigM() const
{
    return m_diffBigM; 
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::setDiffBigM(DualMinCostFlow<T, V>::value_type v)
{
    m_diffBigM = v; 
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::value_type DualMinCostFlow<T, V>::boundBigM() const
{
    return m_boundBigM; 
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::setBoundBigM(DualMinCostFlow<T, V>::value_type v)
{
    m_boundBigM = v; 
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::graph_type const& DualMinCostFlow<T, V>::graph() const 
{
    return m_graph;
}
//template <typename T, typename V>
//DualMinCostFlow<T, V>::arc_value_map_type const& DualMinCostFlow<T, V>::lowerMap() const
//{
//    return m_mLower;
//}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::arc_value_map_type const& DualMinCostFlow<T, V>::upperMap() const 
{
    return m_mUpper;
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::arc_cost_map_type const& DualMinCostFlow<T, V>::costMap() const 
{
    return m_mCost;
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::node_value_map_type const& DualMinCostFlow<T, V>::supplyMap() const 
{
    return m_mSupply;
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::arc_flow_map_type& DualMinCostFlow<T, V>::flowMap() 
{
    return m_mFlow;
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::node_pot_map_type& DualMinCostFlow<T, V>::potentialMap() 
{
    return m_mPotential;
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::value_type DualMinCostFlow<T, V>::totalFlowCost() const
{
    return m_totalFlowCost; 
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::setTotalFlowCost(typename DualMinCostFlow<T, V>::value_type cost)
{
    m_totalFlowCost = cost; 
}
template <typename T, typename V>
typename DualMinCostFlow<T, V>::value_type DualMinCostFlow<T, V>::totalCost() const 
{
    // the negation comes from we change maximization problem into minimization problem 
    // the dual LP problem should be a maximization problem, but we solve it with min-cost flow 
    return -(totalFlowCost()-m_reversedArcFlowCost);
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::printGraph(bool writeSol) const
{
    limboPrint(kDEBUG, "diffBigM = %ld, boundBigM = %ld, reversedArcFlowCost = %ld\n", (long)m_diffBigM, (long)m_boundBigM, (long)m_reversedArcFlowCost);
    if (writeSol)
        limboPrint(kDEBUG, "totalFlowCost = %ld, totalCost = %ld\n", (long)totalFlowCost(), (long)totalCost()); 

    node_name_map_type nameMap (m_graph); 
    for (unsigned int i = 0, ie = m_model->numVariables(); i < ie; ++i)
        nameMap[m_graph.nodeFromId(i)] = m_model->variableName(variable_type(i)); 
    nameMap[m_graph.nodeFromId(m_graph.maxNodeId())] = "additional";

    // dump lgf file 
    lemon::DigraphWriter<graph_type> writer(m_graph, "debug.lgf");
    writer.nodeMap("supply", m_mSupply)
        .nodeMap("name", nameMap)
        .arcMap("capacity_upper", m_mUpper)
        .arcMap("cost", m_mCost);
    if (writeSol)
        writer.nodeMap("potential", m_mPotential)
            .arcMap("flow", m_mFlow);
    writer.run();
}
template <typename T, typename V>
SolverProperty DualMinCostFlow<T, V>::solve(typename DualMinCostFlow<T, V>::solver_type* solver)
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

    // prepare 
    prepare();
    // build graph 
    buildGraph();
#ifdef DEBUG_DUALMINCOSTFLOW
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

#ifdef DEBUG_DUALMINCOSTFLOW
    printGraph(true);
#endif

    if (defaultSolver)
        delete solver; 

    return status; 
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::prepare() 
{
    // big M should be larger than the summation of all non-negative supply in the graph 
    // because this is the largest possible amount of flows. 
    // The supply for each node is the coefficient of variable in the objective. 
    if (m_diffBigM == std::numeric_limits<value_type>::max()) // if not set 
    {
        m_diffBigM = 0; 
        for (typename std::vector<term_type>::const_iterator it = m_model->objective().terms().begin(), ite = m_model->objective().terms().end(); it != ite; ++it)
        {
            if (it->coefficient() > 0)
                m_diffBigM += it->coefficient();
        }
    }
    // big M for bound constraints should be larger than that for differential constraints 
    // so the results are more controllable for INFEASIBLE models  
    if (m_boundBigM == std::numeric_limits<value_type>::max())
        m_boundBigM = m_diffBigM<<1; 

    // reset data members
    m_reversedArcFlowCost = 0; 
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::buildGraph() 
{
    // 1. preparing nodes 
    mapObjective2Graph();

    // reserve arcs 
    // use count arcs mode to compute number of arcs needed 
    unsigned int numArcs = mapDiffConstraint2Graph(true)+mapBoundConstraint2Graph(true);
    m_graph.reserveArc(numArcs); 

    // 2. preparing arcs for differential constraints 
    mapDiffConstraint2Graph(false);

    // 3. arcs for variable bounds 
    mapBoundConstraint2Graph(false);
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::mapObjective2Graph() 
{
    // preparing nodes 
    // set supply to its weight in the objective 
    m_graph.reserveNode(m_model->numVariables()+1); // in case an additional node is necessary, which will be the last node  
    for (unsigned int i = 0, ie = m_model->numVariables(); i < ie; ++i)
        m_graph.addNode();
    for (typename std::vector<term_type>::const_iterator it = m_model->objective().terms().begin(), ite = m_model->objective().terms().end(); it != ite; ++it)
        m_mSupply[m_graph.nodeFromId(it->variable().id())] = it->coefficient();
}
template <typename T, typename V>
unsigned int DualMinCostFlow<T, V>::mapDiffConstraint2Graph(bool countArcs) 
{
    // preparing arcs 
    // arcs constraints like xi - xj >= cij 
    // add arc from node i to node j with cost -cij and capacity unlimited 

    unsigned int numArcs = m_model->constraints().size(); 
    if (!countArcs) // skip in count arcs mode 
    {
        // normalize to '>' format 
        for (typename std::vector<constraint_type>::iterator it = m_model->constraints().begin(), ite = m_model->constraints().end(); it != ite; ++it)
        {
            constraint_type& constr = *it; 
            limboAssertMsg(constr.expression().terms().size() == 2, "only support differential constraints like xi - xj >= cij");
            constr.normalize('>');
        }
        for (typename std::vector<constraint_type>::const_iterator it = m_model->constraints().begin(), ite = m_model->constraints().end(); it != ite; ++it)
        {
            constraint_type const& constr = *it; 
            std::vector<term_type> const& vTerm = constr.expression().terms();
            variable_type xi = (vTerm[0].coefficient() > 0)? vTerm[0].variable() : vTerm[1].variable();
            variable_type xj = (vTerm[0].coefficient() > 0)? vTerm[1].variable() : vTerm[0].variable();

            addArcForDiffConstraint(m_graph.nodeFromId(xi.id()), m_graph.nodeFromId(xj.id()), constr.rightHandSide(), m_diffBigM);
        }
    }
    return numArcs; 
}
template <typename T, typename V>
unsigned int DualMinCostFlow<T, V>::mapBoundConstraint2Graph(bool countArcs) 
{
    // 3. arcs for variable bounds 
    // from node to additional node  

    // check whether there is node with non-zero lower bound or non-infinity upper bound 
    unsigned int numArcs = 0; 
    for (unsigned int i = 0, ie = m_model->numVariables(); i < ie; ++i)
    {
        value_type lowerBound = m_model->variableLowerBound(variable_type(i)); 
        value_type upperBound = m_model->variableUpperBound(variable_type(i)); 
        if (lowerBound != limbo::lowest<value_type>())
            ++numArcs; 
        if (upperBound != std::numeric_limits<value_type>::max())
            ++numArcs; 
    }
    if (!countArcs && numArcs) // skip in count arcs mode 
    {
        // 3.1 create additional node 
        // its corresponding weight is the negative sum of weight for other nodes 
        node_type addlNode = m_graph.addNode();
        value_type addlWeight = 0;
        for (typename std::vector<term_type>::const_iterator it = m_model->objective().terms().begin(), ite = m_model->objective().terms().end(); it != ite; ++it)
            addlWeight -= it->coefficient();
        m_mSupply[addlNode] = addlWeight; 

        // bound constraint is more important 
        // so it has higher cost than normal differential constraints  
        for (unsigned int i = 0, ie = m_model->numVariables(); i < ie; ++i)
        {
            value_type lowerBound = m_model->variableLowerBound(variable_type(i)); 
            value_type upperBound = m_model->variableUpperBound(variable_type(i)); 
            // has lower bound 
            // add arc from node to additional node with cost d and cap unlimited
            if (lowerBound != limbo::lowest<value_type>())
                addArcForDiffConstraint(m_graph.nodeFromId(i), addlNode, lowerBound, m_boundBigM);
            // has upper bound 
            // add arc from additional node to node with cost u and capacity unlimited
            if (upperBound != std::numeric_limits<value_type>::max())
                addArcForDiffConstraint(addlNode, m_graph.nodeFromId(i), -upperBound, m_boundBigM); 
        }
    }
    return numArcs; 
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::addArcForDiffConstraint(typename DualMinCostFlow<T, V>::node_type xi, typename DualMinCostFlow<T, V>::node_type xj, typename DualMinCostFlow<T, V>::value_type cij, typename DualMinCostFlow<T, V>::value_type bigM) 
{
    // add constraint xi - xj >= cij 
    //
    // negative arc cost can be fixed by arc reversal 
    // for an arc with i -> j, with supply bi and bj, cost cij, capacity uij 
    // reverse the arc to 
    // i <- j, with supply bi-uij and bj+uij, cost -cij, capacity uij 

    if (cij <= 0)
    {
        arc_type arc = m_graph.addArc(xi, xj);
        m_mCost[arc] = -cij; 
        //m_mLower[arc] = 0;
        m_mUpper[arc] = bigM; 
    }
    else // reverse arc 
    {
        arc_type arc = m_graph.addArc(xj, xi); // arc reversal 
        m_mCost[arc] = cij; 
        //m_mLower[arc] = 0;
        m_mUpper[arc] = bigM;
        m_mSupply[xi] -= bigM;
        m_mSupply[xj] += bigM; 

        m_reversedArcFlowCost += cij*bigM;
    }
}
template <typename T, typename V>
void DualMinCostFlow<T, V>::applySolution()
{
    // update solution 
    value_type addlValue = 0;
    if ((unsigned int)m_graph.nodeNum() > m_model->numVariables()) // additional node has been introduced 
        addlValue = m_mPotential[m_graph.nodeFromId(m_graph.maxNodeId())];
    unsigned int i = 0; 
    for (typename std::vector<value_type>::iterator it = m_model->variableSolutions().begin(), ite = m_model->variableSolutions().end(); it != ite; ++it, ++i)
        *it = m_mPotential[m_graph.nodeFromId(i)]-addlValue; 
}


/// @brief A base class of min-cost flow solver 
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class MinCostFlowSolver
{
    public:
        /// @brief dual min-cost flow solver type 
        typedef DualMinCostFlow<T, V> dualsolver_type; 

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
        virtual SolverProperty operator()(dualsolver_type* d) = 0; 
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
        typedef typename base_type::dualsolver_type dualsolver_type; 
        /// @brief algorithm type 
        typedef lemon::CapacityScaling<typename dualsolver_type::graph_type, 
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
        virtual SolverProperty operator()(dualsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                //.lowerMap(d->lowerMap())
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
        typedef typename base_type::dualsolver_type dualsolver_type; 
        /// @brief algorithm type 
        typedef lemon::CostScaling<typename dualsolver_type::graph_type, 
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
        virtual SolverProperty operator()(dualsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                //.lowerMap(d->lowerMap())
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
        typedef typename base_type::dualsolver_type dualsolver_type; 
        /// @brief algorithm type 
        typedef lemon::NetworkSimplex<typename dualsolver_type::graph_type, 
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
        virtual SolverProperty operator()(dualsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                //.lowerMap(d->lowerMap())
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
        typedef typename base_type::dualsolver_type dualsolver_type; 
        /// @brief algorithm type 
        typedef lemon::CycleCanceling<typename dualsolver_type::graph_type, 
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
        virtual SolverProperty operator()(dualsolver_type* d)
        {
            // 1. choose algorithm 
            alg_type alg (d->graph());

            // 2. run 
            typename alg_type::ProblemType status = alg.resetParams()
                //.lowerMap(d->lowerMap())
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
