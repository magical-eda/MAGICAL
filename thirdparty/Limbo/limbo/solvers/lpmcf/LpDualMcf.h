/**
 * @file   LpDualMcf.h
 * @brief  solve linear programming problem with dual min-cost flow 
 *
 * [Lemon](https://lemon.cs.elte.hu) is used as min-cost flow solver 
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _LIMBO_SOLVERS_LPMCF_LPDUALMCF_H
#define _LIMBO_SOLVERS_LPMCF_LPDUALMCF_H

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>
#include <cctype>
#include <ctime>
#include <boost/cstdint.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <limbo/solvers/lpmcf/Lgf.h>
#include <limbo/parsers/lp/bison/LpDriver.h>
#include <limbo/math/Math.h>

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::make_pair;
using boost::int32_t;
using boost::int64_t;
using boost::unordered_map;
using boost::iequals;

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace for Limbo.Solvers 
namespace solvers 
{ 
/// namespace for Limbo.Solvers.lpmcf 
namespace lpmcf 
{

/// @class limbo::solvers::lpmcf::hash_pair
/// @brief hash calculator for pairs 
/// @tparam T1 first data type 
/// @tparam T2 second data type 
template <typename T1, typename T2>
struct hash_pair : pair<T1, T2>
{
    /// @nowarn
	typedef pair<T1, T2> base_type;
	using typename base_type::first_type;
	using typename base_type::second_type;
    /// @endnowarn

    /// @brief constructor 
	hash_pair() : base_type() {}
    /// @brief constructor 
    /// @param a first data value 
    /// @param b second data value 
	hash_pair(first_type const& a, second_type const& b) : base_type(a, b) {}
    /// @brief copy constructor 
    /// @param rhs a pair of data 
	hash_pair(base_type const& rhs) : base_type(rhs) {}

    /// @brief override equality comparison 
    /// @param rhs a pair of data in the right hand side 
    /// @return true if two pairs are equal 
	bool operator==(base_type const& rhs) const 
	{return this->first == rhs.first && this->second == rhs.second;}

    /// @brief compute hash value for a pair of data 
    /// @param key a pair of data 
    /// @return hash value 
	friend std::size_t hash_value(base_type const& key) 
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, key.first);
		boost::hash_combine(seed, key.second);
		return seed;
	}
};

/// @class limbo::solvers::lpmcf::LpDualMcf
/// @brief LP solved with min-cost flow. 
/// 
/// The dual problem of this LP is a min-cost flow problem, 
/// so we can solve the graph problem and then 
/// call shortest path algrithm to calculate optimum of primal problem. 
///
/// 1. Primal problem \n
/// \f{eqnarray*}{
/// & min. & \sum_{i=1}^{n} c_i \cdot x_i, \\
/// & s.t. & x_i - x_j \ge b_{ij}, \forall (i, j) \in E,  \\
/// &     & d_i \le x_i \le u_i, \forall i \in [1, n].  
/// \f}
/// \n
/// 2. Introduce new variables \f$y_i\f$ in \f$[0, n]\f$, set \f$x_i = y_i - y_0\f$, \n
/// \f{eqnarray*}{
/// & min. & \sum_{i=1}^{n} c_i \cdot (y_i-y_0), \\
/// & s.t. & y_i - y_j \ge b_{ij}, \forall (i, j) \in E \\
/// &      & d_i \le y_i - y_0 \le u_i, \forall i \in [1, n], \\
/// &      & y_i \textrm{ is unbounded integer}, \forall i \in [0, n].  
/// \f}
/// \n
/// 3. Re-write the problem \n
/// \f{eqnarray*}{
/// & min. & \sum_{i=0}^{n} c_i \cdot y_i, \textrm{ where } \
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
/// &      & y_i \textrm{ is unbounded integer}, \forall i \in [0, n].  
/// \f}
/// \n
/// 4. Map to dual min-cost flow problem. \n
///    Let's use \f$c'_i\f$ for generalized \f$c_i\f$ and \f$b'_{ij}\f$ for generalized \f$b_{ij}\f$. \n
///    Then \f$c'_i\f$ is node supply. 
///    For each \f$(i, j) \in E'\f$, an arc from i to j with cost \f$-b'_{ij}\f$ and flow range \f$[0, \infty]\f$. \n
/// \n
/// Caution: the cost-scaling algorithm in lemon cannot take an arc with negative cost but unlimited capacity.
/// So here I introduce a member variable m_M to represent unlimit, but it is much smaller than real bound of integer.
/// But there may be problem if potential overflow appears. 
/// 
/// @tparam T data type 
template <typename T = int64_t>
class LpDualMcf : public Lgf<T>, public LpParser::LpDataBase
{
	public:
		/// value_type can only be integer types 
		typedef T value_type;
        /// inherit from limbo::solvers::lpmcf::Lgf 
		typedef Lgf<value_type> base_type1;
        /// inherit from LpParser::LpDataBase
		typedef LpParser::LpDataBase base_type2;
        /// @nowarn
		using typename base_type1::cost_type;
		using typename base_type1::graph_type;
		using typename base_type1::node_type;
		using typename base_type1::arc_type;
		using typename base_type1::node_value_map_type;
		using typename base_type1::node_name_map_type;
		using typename base_type1::arc_value_map_type;
		using typename base_type1::arc_cost_map_type;
		using typename base_type1::arc_flow_map_type;
		using typename base_type1::node_pot_map_type;

		// I don't know why it does not work with 
		// using typename base_type1::alg_type;
		typedef typename base_type1::alg_type alg_type;
        /// @endnowarn

        /// @class limbo::solvers::lpmcf::LpDualMcf::variable_type
		/// @brief variable \f$x_i\f$ in the primal linear programming problem.
        /// standard format: \f$l_i \le x_i \le u_i\f$
		/// maps to 
		/// node \f$i\f$, 
		/// arcs from node \f$i\f$ to node \f$st\f$. 
		struct variable_type 
		{
			string name; ///< name of variable 
			pair<value_type, value_type> range; ///< pair of \f$(l_i, u_i)\f$
			value_type weight; ///< weight in the objective, i.e., \f$c_i\f$
			value_type value; ///< solved value 
			node_type node; ///< node \f$i\f$ in the graph 

            /// @brief constructor 
            /// @param n name 
            /// @param l lower bound of range, i.e., \f$l_i\f$
            /// @param r upper bound of range, i.e., \f$u_i\f$
            /// @param w weight in the objective, i.e., \f$c_i\f$
            /// @param v solved value 
			variable_type(string const& n, 
					value_type const& l = 0, 
					value_type const& r = std::numeric_limits<value_type>::max(),
					value_type const& w = 0, 
					value_type const& v = 0)
				: name(n), range(make_pair(l, r)), weight(w), value(v) {}

            /// @brief check if the variable is lower bounded 
            /// @return true if the variable has a lower bound 
			bool is_lower_bounded() const {return range.first != limbo::lowest<value_type>();}
            /// @brief check if the variable is upper bounded 
            /// @return true if the variable is an upper bound 
			bool is_upper_bounded() const {return range.second != std::numeric_limits<value_type>::max();}
            /// @brief check if the variable is bounded 
            /// @return true if the variable is eitehr lower bounded or upper bounded 
			bool is_bounded() const {return is_lower_bounded() || is_upper_bounded();}
		};

        /// @class limbo::solvers::lpmcf::LpDualMcf::constraint_type
        /// @brief constraint object in the primal linear programming problem. 
		/// standard format: \f$x_i - x_j \ge c_{ij}\f$
		/// maps to 
		/// arc \f$x_i \rightarrow x_j, \textrm{cost} = -c_{ij}\f$. 
		struct constraint_type 
		{
			pair<string, string> variable; ///< variable \f$x_i\f$ and \f$x_j\f$
			value_type constant; ///< constant in the right hand side, i.e., \f$c_{ij}\f$
			arc_type arc; ///< arc \f$x_i \rightarrow x_j\f$ in the graph 
			
            /// @brief constructor 
            /// @param xi \f$x_i\f$
            /// @param xj \f$x_j\f$
            /// @param c \f$c_{ij}\f$
			constraint_type(string const& xi, string const& xj, value_type const& c)
				: variable(make_pair(xi, xj)), constant(c) {}
		};

		/// @brief constructor 
		/// @param max_limit represents unlimited arc capacity, default value is \f$2^{32 \times \frac{3}{4}}\f$ for int32_t, \f$2^{64 \times \frac{3}{4}}\f$ for int64_t...
		LpDualMcf(value_type max_limit = (value_type(2) << (sizeof(value_type)*8*3/4))) 
			: base_type1(), 
			base_type2(),
			m_is_bounded(false), 
			m_M(max_limit) // use as unlimited number 
		{
			if (m_M < 0) m_M = -m_M; // make sure m_M is positive 
		}

		/// @brief add variable with range. 
		/// default range is 
		/// \f$-\infty \le x_i \le \infty\f$. 
        /// @param xi variable \f$x_i\f$
        /// @param l lower bound \f$l_i\f$
        /// @param r upper bound \f$u_i\f$
		virtual void add_variable(string const& xi, 
				double l = limbo::lowest<double>(), 
				double r = std::numeric_limits<value_type>::max())
		{
            // in case of overflow 
            value_type lb = l; 
            value_type ub = r;
            if (l <= (double)limbo::lowest<value_type>())
                lb = limbo::lowest<value_type>();
            if (r >= (double)std::numeric_limits<value_type>::max())
                ub = std::numeric_limits<value_type>::max();
			assert_msg(lb <= ub, "failed to add bound " << lb << " <= " << xi << " <= " << ub);

			// no variables with the same name is allowed 
			BOOST_AUTO(found, m_hVariable.find(xi));
			if (found == m_hVariable.end())
				assert_msg(m_hVariable.insert(make_pair(xi, variable_type(xi, lb, ub, 0))).second,
						"failed to insert variable " << xi << " to hash table");
			else 
			{
				found->second.range.first = std::max(found->second.range.first, (value_type)lb);
				found->second.range.second = std::min(found->second.range.second, (value_type)ub);
				assert_msg(found->second.range.first <= found->second.range.second, 
						"failed to set bound " << found->second.range.first << " <= " << xi << " <= " << found->second.range.second);
			}
			// if user set bounds to variables 
			// switch to bounded mode, which means there will be an additional node to the graph 
			if (lb != limbo::lowest<value_type>() || ub != std::numeric_limits<value_type>::max())
				this->is_bounded(true);
		}
        /// @brief add constraint callback for LpParser::LpDataBase
        /// @param terms array of terms in left hand side 
        /// @param compare operator '<', '>', '='
        /// @param constant constant in the right hand side 
        virtual void add_constraint(std::string const& /*cname*/, LpParser::TermArray const& terms, char compare, double constant)
        {
            assert(terms.size() == 2 && terms[0].coef*terms[1].coef < 0);
            // in case some variables are not added yet 
            add_variable(terms[0].var); 
            add_variable(terms[1].var); 
            string xi = terms[0].var;
            string xj = terms[1].var; 
            if (compare == '<' || compare == '=')
            {
                if (terms[0].coef > 0)
                {
                    xi = terms[1].var; 
                    xj = terms[0].var; 
                }
                else 
                {
                    xi = terms[0].var; 
                    xj = terms[1].var; 
                }
                constant = -constant;
                add_constraint(xi, xj, constant);
            }
            if (compare == '>' || compare == '=')
            {
                if (terms[0].coef > 0)
                {
                    xi = terms[0].var; 
                    xj = terms[1].var; 
                }
                else 
                {
                    xi = terms[1].var; 
                    xj = terms[0].var; 
                }
                add_constraint(xi, xj, constant);
            }
        }
        /// @brief add object callback for LpParser::LpDataBase 
        /// @param minimize true denotes minimizing object, false denotes maximizing object 
        /// @param terms array of terms 
		virtual void add_objective(bool minimize, LpParser::TermArray const& terms) 
        {
            for (LpParser::TermArray::const_iterator it = terms.begin(); it != terms.end(); ++it)
            {
                // in case variable is not added yet 
                add_variable(it->var);

                if (minimize) // minimize objective 
                    add_objective(it->var, it->coef); 
                else // maximize objective 
                    add_objective(it->var, -it->coef); 
            }
        }
		/// @brief add constraint 
		/// \f$x_i - x_j \ge c_{ij}\f$. 
        /// 
		/// Assume there's no duplicate.  
        /// @param xi variable \f$x_i\f$
        /// @param xj variable \f$x_j\f$
        /// @param cij constant \f$c_{ij}\f$
		virtual void add_constraint(string const& xi, string const& xj, cost_type const& cij)
		{
			BOOST_AUTO(found, m_hConstraint.find(make_pair(xi, xj)));
			if (found == m_hConstraint.end())
				assert_msg(m_hConstraint.insert(
							make_pair(
								make_pair(xi, xj), 
								constraint_type(xi, xj, cij)
								)
							).second,
						"failed to add constraint for " << xi << " - " << xj << " >= " << cij
						);
			else // automatically reduce constraints 
				found->second.constant = std::max(found->second.constant, cij);
		}
		/// @brief add linear terms for objective function of the primal linear programming problem. 
        /// 
        /// Assume the variable is already been setup. 
		/// We allow repeat adding weight and the weight will be accumulated. 
        /// @param xi variable \f$x_i\f$
        /// @param w weight 
		virtual void add_objective(string const& xi, value_type const& w)
		{
			if (w == 0) return;

			BOOST_AUTO(found, m_hVariable.find(xi));
			assert_msg(found != m_hVariable.end(), "failed to add objective " << w << " " << xi);

			found->second.weight += w;
		}
        /// @brief set integer variables 
        /// param vname integer variables  
        /// param binary denotes whether they are binary variables 
        void set_integer(std::string const& /*vname*/, bool /*binary*/)
        {
            // ignored 
        }

		/// @brief check if lp problem is bounded 
        /// @return true if the problem is bounded 
		bool is_bounded() const {return m_is_bounded;}
        /// @brief set if the problem is bounded 
        /// @param v flag for whether the problem is bounded 
		void is_bounded(bool v) {m_is_bounded = v;}

		/// @brief API to run the algorithm with input file. 
        /// 
        /// Read primal problem in lp format and then dump solution. 
        /// @param filename input file name, the output file will be dumped to filename+".sol"
        /// @return solving status 
		typename alg_type::ProblemType operator()(string const& filename)
		{
			read_lp(filename);
			typename alg_type::ProblemType status = (*this)();
			if (status == alg_type::OPTIMAL)
				this->print_solution(filename+".sol");

			return status;
		}
		/// @brief API to run the algorithm.
        /// 
		/// Execute solver 
		/// and write out solution file if provided. 
        /// @return solving status 
		typename alg_type::ProblemType operator()() 
		{
			prepare();
#ifdef DEBUG 
			this->print_graph("graph.lp");
#endif 
			return run();
		}
        /// @brief get solution to \f$x_i\f$ 
        /// @param xi variable \f$x_i\f$
		/// @return solution 
		value_type solution(string const& xi) const 
		{
			BOOST_AUTO(found, m_hVariable.find(xi));
			assert_msg(found != m_hVariable.end(), "failed to find variable " << xi);

			return found->second.value;
		}
		/// @brief read lp format 
        /// @param filename input file in lp format 
		/// initializing graph 
		void read_lp(string const& filename) 
		{
			LpParser::read(*this, filename);
		}
		/// @brief check empty
		/// @return true if there's no variable created
		bool empty() const {return m_hVariable.empty();}

		/// @brief print solutions into a file 
		/// including primal problem and dual problem
        /// @param filename output file name 
		virtual void print_solution(string const& filename) const 
		{
			this->base_type1::print_solution(filename);

			std::ofstream out (filename.c_str(), std::ios::app);
			if (!out.good())
			{
				cout << "failed to open " << filename << endl;
				return;
			}

			out << "############# LP Solution #############" << endl;
			for (BOOST_AUTO(it, this->m_hVariable.begin()); it != this->m_hVariable.end(); ++it)
			{
				variable_type const& variable = it->second;
				out << this->m_hName[variable.node] << ": " << variable.value << endl;
			}

			out.close();
		}
		/// @brief print primal problem in LP format to a file 
        /// @param filename output file name 
		void print_problem(string const& filename) const 
		{
			std::ofstream out (filename.c_str());
			if (!out.good())
			{
				cout << "failed to open " << filename << endl;
				return;
			}

			// print objective 
			out << "Minimize\n";
			for (BOOST_AUTO(it, this->m_hVariable.begin()); it != this->m_hVariable.end(); ++it)
			{
				variable_type const& variable = it->second;
				if (variable.weight == 0) continue;

				out << "\t" << " + " << variable.weight << " " << variable.name << endl;
			}
			// print constraints 
			out << "Subject To\n";
			for (BOOST_AUTO(it, this->m_hConstraint.begin()); it != this->m_hConstraint.end(); ++it)
			{
				constraint_type const& constraint = it->second;
				out << "\t" << constraint.variable.first 
					<< " - " << constraint.variable.second 
					<< " >= " << constraint.constant << endl;
			}
			// print bounds 
			out << "Bounds\n";
			for (BOOST_AUTO(it, this->m_hVariable.begin()); it != this->m_hVariable.end(); ++it)
			{
				variable_type const& variable = it->second;
				out << "\t";
				// both lower bound and upper bound 
				if (variable.range.first != limbo::lowest<value_type>()
						&& variable.range.second != std::numeric_limits<value_type>::max())
					out << variable.range.first << " <= " 
						<< variable.name << " <= " << variable.range.second << endl;
				// lower bound only 
				else if (variable.range.first != limbo::lowest<value_type>())
					out << variable.name << " >= " << variable.range.first << endl;
				// upper bound only 
				else if (variable.range.second != std::numeric_limits<value_type>::max())
					out << variable.name << " <= " << variable.range.second << endl;
				// no bounds 
				else 
					out << variable.name << " free\n";
			}
			// print data type (integer)
			out << "Generals\n";
			for (BOOST_AUTO(it, this->m_hVariable.begin()); it != this->m_hVariable.end(); ++it)
			{
				variable_type const& variable = it->second;
				out << "\t" << variable.name << endl;
			}
			out << "End";
			out.close();
		}
	protected:
		/// @brief prepare before run 
		void prepare()
		{
			// 1. preparing nodes 
			// set supply to its weight in the objective 
			for (BOOST_AUTO(it, m_hVariable.begin()); it != m_hVariable.end(); ++it)
			{
				variable_type& variable = it->second;
				node_type const& node = this->m_graph.addNode();
				variable.node = node;
				this->m_hSupply[node] = variable.weight; 
				this->m_hName[node] = variable.name;
			}

			// 2. preparing arcs 
			// arcs constraints like xi - xj >= cij 
			// add arc from node i to node j with cost -cij and capacity unlimited 
			for (BOOST_AUTO(it, m_hConstraint.begin()); it != m_hConstraint.end(); ++it)
			{
				constraint_type& constraint = it->second;
				BOOST_AUTO(found1, m_hVariable.find(constraint.variable.first));
				BOOST_AUTO(found2, m_hVariable.find(constraint.variable.second));
				assert_msg(found1 != m_hVariable.end(), "failed to find variable1 " << constraint.variable.first << " in preparing arcs"); 
				assert_msg(found2 != m_hVariable.end(), "failed to find variable2 " << constraint.variable.second << " in preparing arcs"); 

				variable_type const& variable1 = found1->second;
				variable_type const& variable2 = found2->second;

				node_type const& node1 = variable1.node;
				node_type const& node2 = variable2.node;

				arc_type const& arc = this->m_graph.addArc(node1, node2);
				constraint.arc = arc;

				this->m_hCost[arc] = -constraint.constant;
				this->m_hLower[arc] = 0;
				//m_hUpper[arc] = std::numeric_limits<value_type>::max();
				this->m_hUpper[arc] = m_M;
			}
			// 3. arcs for variable bounds 
			// from node to additional node  
			if (this->is_bounded())
			{
				// 3.1 create additional node 
				// its corresponding weight is the negative sum of weight for other nodes 
				m_addl_node = this->m_graph.addNode();
				value_type addl_weight = 0;
				for (BOOST_AUTO(it, m_hVariable.begin()); it != m_hVariable.end(); ++it)
					addl_weight -= it->second.weight;
				this->m_hSupply[m_addl_node] = addl_weight; 
				this->m_hName[m_addl_node] = "lpmcf_additional_node";

				for (BOOST_AUTO(it, m_hVariable.begin()); it != m_hVariable.end(); ++it)
				{
					variable_type const& variable = it->second;
					// has lower bound 
					// add arc from node to additional node with cost d and cap unlimited
					if (variable.is_lower_bounded())
					{
						arc_type const& arc = this->m_graph.addArc(variable.node, m_addl_node);
						this->m_hCost[arc] = -variable.range.first;
						this->m_hLower[arc] = 0;
						this->m_hUpper[arc] = m_M;
					}
					// has upper bound 
					// add arc from additional node to node with cost u and capacity unlimited
					if (variable.is_upper_bounded())
					{
						arc_type const& arc = this->m_graph.addArc(m_addl_node, variable.node);
						this->m_hCost[arc] = variable.range.second;
						this->m_hLower[arc] = 0;
						this->m_hUpper[arc] = m_M;
					}
				}
			}
		}
		/// @brief kernel function to run algorithm 
        /// @return solving status, OPTIMAL, INFEASIBLE, UNBOUNDED 
		typename alg_type::ProblemType run()
		{
			// 1. choose algorithm 
			alg_type alg (this->m_graph);

			// 1.1 for robustness 
			// if problem is empty, also return OPTIMAL
			if (this->empty())
				return alg_type::OPTIMAL;

			// 2. run 
			typename alg_type::ProblemType status = alg.reset().resetParams()
				.lowerMap(this->m_hLower)
				.upperMap(this->m_hUpper)
				.costMap(this->m_hCost)
				.supplyMap(this->m_hSupply)
				.run();

			// 3. check results 
#ifdef DEBUG
			switch (status)
			{
				case alg_type::OPTIMAL:
					cout << "OPTIMAL" << endl;
					break;
				case alg_type::INFEASIBLE:
					cout << "INFEASIBLE" << endl;
					break;
				case alg_type::UNBOUNDED:
					cout << "UNBOUNDED" << endl;
					break;
			}

			assert_msg(status == alg_type::OPTIMAL, "failed to achieve OPTIMAL solution");
#endif 
			// 4. update solution 
			if (status == alg_type::OPTIMAL)
			{
				this->m_totalcost = alg.template totalCost<cost_type>();
				// get dual solution of LP, which is the flow of mcf, skip this if not necessary
				alg.flowMap(this->m_hFlow);
				// get solution of LP, which is the dual solution of mcf 
				alg.potentialMap(this->m_hPot);

				// update solution 
				value_type addl_value = 0;
				if (this->is_bounded())
					addl_value = this->m_hPot[m_addl_node];
				for (BOOST_AUTO(it, m_hVariable.begin()); it != m_hVariable.end(); ++it)
				{
					variable_type& variable = it->second;
					variable.value = this->m_hPot[variable.node]-addl_value;
				}
			}

			return status;
		}

		bool m_is_bounded; ///< whether the problem is bounded or not 
		node_type m_addl_node; ///< additional node, only valid when m_is_bounded = true  

		value_type m_M; ///< a very large number to deal with ranges of variables 
						///< reference from MIT paper: solving the convex cost integer dual network flow problem 
		unordered_map<string, variable_type> m_hVariable; ///< variable map 
		unordered_map<hash_pair<string, string>, constraint_type> m_hConstraint; ///< constraint map 
};

}}} // namespace lpmcf // namespace solvers // limbo

#endif 
