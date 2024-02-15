/**
 * @file CGLegalizer.h
 * @brief The "legalization" solver with constraint graph + LP
 * @author Keren Zhu
 * @date 11/25/2019
 */

#ifndef IDEAPLACE_CG_LEGALIZER_H_
#define IDEAPLACE_CG_LEGALIZER_H_

#include "ConstraintGraph.h"
#include "db/Database.h"
#include "util/linear_programming.h"

PROJECT_NAMESPACE_BEGIN

/// @brief a directed edge representing a constraint
class ConstraintEdge
{
    public:
        explicit ConstraintEdge(IndexType source, IndexType target):
            _source(source), _target(target)
    {}
        /// @brief Get the index of source vertex
        /// @return the index of source vertex
        IndexType source() const { return _source; }
        /// @brief Get the index of target vertex
        /// @return the index of target vertex
        IndexType target() const { return _target; }
        /// @brief Get the weight of this edge
        /// @return the weight of this edge
        IntType weight() const { return 1; }
        /// @brief to string for debuging
        std::string toStr() const 
        {
            std::stringstream ss;
            ss << "source "<< _source <<" target "<< _target ;
            return ss.str();
        }
        bool operator<(const ConstraintEdge &rhs) const
        {
            if (_source == rhs.source())
            {
                return _target < rhs.target();
            }
            return _source < rhs.source();
        }
        bool operator==(const ConstraintEdge &rhs) const 
        {
            return _source == rhs.source() && _target == rhs.target();
        }
    private:
        IndexType _source;  ///< The index of source vertex
        IndexType _target; ///< The index of target vertex
        //IntType _weight;  ///< The weight of this edge
};


/// @brief Just a wrappers for ConstraintEdge
class Constraints
{

    public:
        /// @brief default constructor
        explicit Constraints() = default;
        /// @breif clear the constraint edges
        void clear() { _edges.clear(); }
        /// @brief get the constraint edges
        /// @return the constraint edges
        const std::set<ConstraintEdge> & edges() const { return _edges; }
        /// @brief get the constraint edges
        /// @return the constraint edges
        std::set<ConstraintEdge> & edges() { return _edges; }
        /// @brief add a constraint edge
        /// @param the source cell index
        /// @param the target cell index
        /// @param the weight of the edge
        void addConstraintEdge(IndexType sourceIdx, IndexType targetIdx, IntType weight)
        {
            _edges.insert(ConstraintEdge(sourceIdx, targetIdx));
        }
        bool hasEdgeNoDirection(IndexType sourceIdx, IndexType targetIdx)
        {
            auto it = _edges.find(ConstraintEdge(sourceIdx, targetIdx));
            if (it != _edges.end())
            {
                return true;
            }
            it = _edges.find(ConstraintEdge(targetIdx, sourceIdx));
            if (it != _edges.end())
            {
                return true;
            }
            return false;
        }
        void removeConstraintEdge(IndexType sourceIdx, IndexType targetIdx)
        {
            auto it = _edges.find(ConstraintEdge(sourceIdx, targetIdx));
            if (it != _edges.end())
            {
                _edges.erase(it);
            }
            it = _edges.find(ConstraintEdge(targetIdx, sourceIdx));
            if (it != _edges.end())
            {
                _edges.erase(it);
            }

        }
        
    private:
        std::set<ConstraintEdge> _edges; ///< The constraint edges
};

/// @brief The LP solver for legalization
class LpLegalizeSolver
{
        typedef ::klib::lp::LpModel lp_solver_type;
        typedef ::klib::lp::LpTrait lp_trait;
        typedef lp_trait::variable_type lp_variable_type;
        typedef lp_trait::expr_type lp_expr_type;
    public:
        explicit LpLegalizeSolver(Database &db, Constraints &constraints, bool isHor=true,
                IntType optHpwl=0, IntType optArea=1)
            : _db(db), _constrains(constraints), _isHor(isHor), _optHpwl(optHpwl), _optArea(optArea)
        {} //_solver = SolverType(&_ilpModel); }
        /// @brief solve the problem
        bool solve();
        // @brief dump out the solutions to the database
        void exportSolution();
        /// @brief evaluate the objective function and return the value
        RealType evaluateObj();
        /// @brief set the maximum width or height (_wStar)
        /// @param the maximum width or height in the hpwl optimization problem
        void setWStar(RealType wStar) { _wStar = wStar; }
    private:
        /// @brief solve the LP
        bool solveLp();
        /* Varibles functions */
        /// @brief add ILP variables
        void addIlpVars();
        /// @brief calculate the number of variables32
        IndexType numVars() const;
        /// @brief add location variables
        void addLocVars();
        /// @brief add wirelegth variables
        void addWirelengthVars();
        /// @brief add area variables
        void addAreaVars();
        /// @brief add sym group varibales
        void addSymVars();
        /* Obj functions */
        /// @brief set the objective function
        void configureObjFunc();
        /// @brief add wire length objective
        void addWirelengthObj();
        /// @brief add area objective
        void addAreaObj();
        /// @brief add relaxed symmetric penalty
        void addSymObj();
        /* Constraint functions */
        /// @brief add constraints
        void addIlpConstraints();
        /// @brief add area constraints
        void addBoundaryConstraints();
        /// @brief add topology constraints
        void addTopologyConstraints();
        /// @brief add symmetry constraints
        void addSymmetryConstraints();
        void addSymmetryConstraintsWithEqu();
        void addSymmetryConstraintsRex();
        /// @brief add hpwl constraints
        void addHpwlConstraints();
        /// @brief add current flow constraint
        void addCurrentFlowConstraints();
    private:
        /* Configurations - Inputs */
        Database &_db; ///< The database for the Ideaplace
        Constraints &_constrains; ///< The constraints edges to be honored
        bool _isHor = true; ///< Whether solving horizontal or vertical
        IntType _optHpwl = 0; ///< Whether optimizing HPWL in ILP problems
        IntType _optArea = 1; ///< Whether optimizing area in ILP problems
        /* Optimization supporting variables */
        lp_solver_type _solver; ///<  LP sovler
        lp_expr_type _obj; ///< The objective function of the ILP model
        std::vector<lp_variable_type> _locs; ///< The location variables of the ILP model
        std::vector<lp_variable_type> _wlL; ///< The left wirelength variables of the ILP model
        std::vector<lp_variable_type> _wlR; ///< The right wirelength variables of the ILP model
        lp_variable_type _dim; ///< The variable for area optimization
        RealType _wStar = 0; ///< The optimal W found in legalization step
        std::vector<lp_variable_type> _symLocs; ///< The variable for symmetric group axises
        std::vector<lp_variable_type> _symRexLeft; ///< The variables representing the left extrems of sym axis of each group
        std::vector<lp_variable_type> _symRexRight; ///< The variables representing the left extrems of sym axis of each group
#ifdef MULTI_SYM_GROUP
        bool _isMultipleSymGrp = true;
#else
        bool _isMultipleSymGrp = false;
#endif 
        bool _relaxEqualityConstraint = false;
        bool _useCurrentFlowConstraint = false;
        //SolverType _solver; ///< Solver
        /*  Optimization Results */
        RealType _largeNum = 900000.0; ///< A large number

};

class CGLegalizer
{
    private:
        class BoxEdge
        {
            public:
                explicit BoxEdge(LocType coord_, IndexType cellIdx_, bool isTop_)
                    : coord(coord_), cellIdx(cellIdx_), isTop(isTop_) {}
                bool operator<(const BoxEdge &rhs) const
                {
                    if (this->coord == rhs.coord)
                    {
                        if (this->isTop == rhs.isTop)
                        {
                            return this->cellIdx < rhs.cellIdx;
                        }
                        else
                        {
                            if (!this->isTop)
                            {
                                return false;
                            }
                            else
                            {
                                return true;
                            }
                        }
                    }
                    else
                    {
                        return this->coord < rhs.coord;
                    }
                }
                std::string toStr() const
                {
                    std::stringstream ss;
                    ss << "LocType "<< coord <<" cellIdx "<< cellIdx << " isTop "<< isTop;
                    return ss.str();
                }
            public:
                LocType coord; ///< Coordinate of the edge
                IndexType cellIdx; ///< The index of the cell 
                bool isTop; ///< True: top/right edge. False: bottom/left edge

        };
    public:
        /// @brief Constructor
        /// @param The database of IdeaPlaceEx
        explicit CGLegalizer(Database &db) : _db(db) {}
        /// @brief legalize the design
        bool legalize();
    private:
        /// @brief Generate the constraints (not optimal in number of constraints). Based on sweeping algorithm
        void generateConstraints();
        void generateHorConstraints();
        void generateVerConstraints();
        /// @brief construct constraint graph from two constraints
        void constructConstraintGraphs();
        /// @brief perform DFS-based transitive reduction
        /// @param the constraint graph
        void dagTransitiveReduction(ConstraintGraph & cg);
        /// @brief remove transitive edge from the graph
        /// @param constraint graph
        /// @param Vector2D of edge matrix
        /// @param number of cells/vertices?
        /// @param vector of whether visited
        /// @param Vector2D of reachable
        /// @return has transitive edge
        bool dfsRemoveTransitiveEdge(ConstraintGraph &cg, Vector2D<IntType> &edgeMat, IndexType node, 
                std::vector<bool> &visited, Vector2D<IntType> &reachable, ConstraintGraph::IndexMap &idxMap);
        /// @brief delete edges in initializing irredundant edges
        /// @param is horizontal? false-> vertical
        /// @param orders
        /// @param cands
        /// @param index of cell
        /// @param the overlap any vector
        void initIrredundantEdgesDelete(bool isHor, std::vector<IndexType> &orders,
                std::vector<IntType> &cand, IndexType cellIdx, std::vector<bool> &overlapAny);
        /// @brief insert edges in initializing irredundant edges
        /// @param is horizontal? false -> vertical
        /// @param orders
        /// @param cands
        /// @param index of cell
        void initIrredundantEdgesInsert(bool isHor , std::vector<IndexType> &orders, 
                std::vector<IntType> &cand, IndexType cellIdx);
        /// @brief get necessary edges
        void getNecessaryEdges();
        /// @brief perform DFS on the graph
        /// @param first: dp table
        /// @param second: the visited vector
        /// @param third: the node index
        /// @param fourth: constraint graph
        /// @param fifth: IndexMap for the boost graph
        void dfsGraph(Vector2D<IntType>& dpTab, std::vector<IntType> &visited, IndexType nodeIdx, ConstraintGraph &cg,
                ConstraintGraph::IndexMap &idxMap);
        /// @brief add edge greedily
        /// @param first: node index i
        /// @param second: node index j
        void addEdgeGreedy(IndexType i, IndexType j);
        /// @brief reload the constraints from the boost-based constraint graph
        void readloadConstraints();
        /// @brief linear programming-based legalization
        /// @param if solving horizontal or vertical
        /// @return the resulting objective function. if negative, failed
        RealType lpLegalization(bool isHor);
        /// @brief LP-based detailed placement. For optimizing wire length
        bool lpDetailedPlacement();
        /// @brief force the two constraint graphs to be DAG
        /// @return if both of the two graphs are DAGs
        bool dagfyConstraintGraphs();
        /// @brief dagfy one graph
        /// @return if the graph was acyclic
        bool dagfyOneConstraintGraph(ConstraintGraph &cg);
    private:
        Database &_db; ///< The database of IdeaPlaceEx
        ConstraintGraph _hCG; ///< The horizontal constraint graph
        ConstraintGraph _vCG; ///< The vertical constraint graph
        Constraints _hConstraints; ///< The horizontal constraint edges
        Constraints _vConstraints; ///< The vertical constraint edges
        RealType _wStar; ///< The width from the objective function of the first LP
        RealType _hStar; ///< The width from the objective function of the first LP
};


PROJECT_NAMESPACE_END

#endif //IDEAPLACE_CG_LEGALIZER_H_
