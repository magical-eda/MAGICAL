/**
 * @file GlobalRoutingAStarSequential.h
 * @brief A* search sequential routing for global routing
 * @author Keren Zhu
 * @date 10/12/2018
 */

#ifndef AROUTER_GRASTARSEQUENTIAL_H_
#define AROUTER_GRASTARSEQUENTIAL_H_

#include "grdr/AStarBase.h"
#include "gr/GlobalRoutingAlgo.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::GR_NCR_NOPATH_EXCEPTION
/// @brief exception for no path is found even if allowing overlaping
class GR_NCR_NOPATH_EXCEPTION : public std::exception
{
    virtual const char* what() const throw()
    {
        return "No path was found even if allowing overlapping. There is no legal path";
    }
};

/// @class AROUTER::GlobalRoutingAStarSequential
/// @brief Sequential routing with A* search
class GlobalRoutingAStarSequential : public GlobalRoutingAlgo, public AStarBase<bool>
{
    /// @brief Stuctor for maintaining the subnets in heap for routing orders
    struct SubNetNode
    {
        explicit SubNetNode(Database &db_p, IndexType subNetIdx_p) : db(db_p), subNetIdx(subNetIdx_p) {}
        IntType hpwl() const 
        {
            const auto &subNetBBox = db.grDB().subNets().at(subNetIdx).gridBBox();
            return subNetBBox.hpwl();
        }
        IntType priority() const 
        {
            const auto &subNet = db.grDB().subNets().at(subNetIdx);
            if (subNet.isSym()) { return 0; }
            return 2;
        }
        Database &db;
        IndexType subNetIdx;
        IndexType iteration = 0; // the number of rip-up of this subnet
    };
    /// @brief the compare struct for the SubNetNode
    struct compareSubNet
    {
        bool operator()(const SubNetNode &subNet1, const SubNetNode &subNet2) const 
        {
            if (subNet1.iteration != subNet2.iteration) { return subNet1.iteration > subNet2.iteration; }
            if (subNet1.priority() != subNet2.priority()) { return subNet1.priority() > subNet2.priority(); }
            return subNet1.hpwl() > subNet2.hpwl();
        }
    };
    /// @brief Stuctor for maintaining the subnets in heap for routing orders
    struct NetNode
    {
        explicit NetNode(Database &db_p, IndexType netIdx_p, IndexType iteration_p) : db(db_p), netIdx(netIdx_p), iteration(iteration_p) {}
        IntType hpwl() const 
        {
            const auto &netBBox = db.grDB().nets().at(netIdx).gridBBox();
            return netBBox.hpwl();
        }
        IntType priority() const 
        {
            const auto &net = db.grDB().nets().at(netIdx);
            if (net.isSym()) { return 0; }
            return 1;
        }
        Database &db;
        IndexType netIdx;
        IndexType iteration = 0; ///< the number of rip-up of this subnet
    };
    /// @brief the compare struct for the SubNetNode
    struct compareNet
    {
        bool operator()(const NetNode &net1, const NetNode &net2) const 
        {
            if (net1.iteration != net2.iteration) { return net1.iteration > net2.iteration; }
            if (net1.priority() != net2.priority()) { return net1.priority() > net2.priority(); }
            return net1.hpwl() > net2.hpwl();
        }
    };
    public:
        /// @brief constructor
        /// @param reference to the database
        explicit GlobalRoutingAStarSequential(Database &db) : GlobalRoutingAlgo(db) {}
        /// @brief run the kernel function
        /// @return if succesful
        bool runKernel() override;
    private:
        /*------------------------------*/ 
        /* Settings                     */
        /*------------------------------*/ 
        /*------------------------------*/ 
        /* Router flow                  */
        /*------------------------------*/ 
        /// @brief initialization of needed data
        /// @return if successful
        bool init();
        /// @brief negotation-based routing
        /// @return if successful
        bool ncrRouting();
        /// @brief route a symmetric net 
        /// @param the index of the SymNet 
        /// @return if sucessful
        bool routeOneSymNet(IndexType symNetIdx);
        /// @brief route one normal net
        /// @param the index of the net
        /// @return if successful
        bool routeOneNormalNet(IndexType netIdx);
        /// @brief route a subnet
        /// @param the index of the subnet(2-pin net) want to route
        /// @return if successful
        bool routeSubNet(IndexType subNetIdx);
        /// @brief clear the edges and update the history cost for congestion. And add the erased subNets back to the heap 
        /// @param the subNet is currently not routable
        void ripUpSubNet(IndexType subNetIdx);
        /// @brief init the beginning history cost of the node (depending on the pin count?)
        void initGCells();
        /// @brief assign the search results to the subnet
        void assignSol();
        /// @brief match the routing path of net2 to net1 in a SymNet pair
        void matchSymRoutingPath();
        /*------------------------------*/ 
        /* Overrided from AStarBase     */
        /*------------------------------*/ 
        /// @brief the distance between the two nodes
        /// @param the two nodes
        /// @return the distance between two nodes
        IntType distance(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const override; 
        /// @brief get the neighbors of the input node, should be valid(safe for vector access), not neccessarily of unblocked
        /// @param the node want to be evaluted
        /// @return the vector of indices representing the list of neighbor nodes
        std::vector<XYZ<IndexType>> neighbors(const XYZ<IndexType> &xyz) const override;
        /// @brief calculate the heuristic cost
        /// @param the node want to be evaluted
        /// @return the heuristic cost of the node
        IntType   heuristicCostEstimate(const XYZ<IndexType> &xyz) const override;
        /// @brief whether the edge is addimissive 
        /// @return if the edge should be considered
        bool edgeAdm(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const override;
        /// @brief Is destination?
        /// @param the node want to be evaluted
        /// @return if the node is the goal/target/destination
        bool isGoal(const XYZ<IndexType> &xyz) override
        {
            bool reach =  _db.grDB().gridCell(xyz).existTerm(_targetTermIdx);
            if (reach) { _goal = xyz; }
            return reach;
        }
        /// @brief Is the node blocked?
        /// @param the node want to be evaluated
        /// @return if the given node is blocked
        bool        isBlocked(const XYZ<IndexType> &xyz) const override  { return _db.grDB().gridCell(xyz).isBlocked(); }
        /*------------------------------*/ 
        /* Private misc.                */
        /*------------------------------*/ 
        /// @brief waive a net in the global routing
        /// @param the net index
        void waiveNetInGr(IndexType netIdx);
        /// @brief if inside the searching range
        /// @param the index of to
        bool withinSearchRange(const XYZ<IndexType> &to) const
        {
            /// TODO:
            return true;
            if (_searchBBox.contain(to.x(), to.y()))
            {
                return true;
            }
            return false;
        }
        /// @brief determine whether a grid is in one terminal
        /// @param first: indices of a global routing grid
        /// @param second: a terminal
        /// @return if the global routing grid is overlapping with the terminal search points
        bool gridInTerminal(XYZ<IndexType> gridIdx, const Terminal &term) const 
        {
            if (_db.grDB().gridIdx(term.primarySearchPoint().loc()) == gridIdx.xy() && term.primarySearchPoint().layer() ==  gridIdx.z())
            {
                return true;
            }
            for (const auto &otherPt : term.otherSearchPoints())
            {
                if (_db.grDB().gridIdx(otherPt.loc()) == gridIdx.xy() && otherPt.layer() == gridIdx.z())
                {
                    return true;
                }
            }
            return false;
        }

    private:
        boost::heap::fibonacci_heap<NetNode, boost::heap::compare<compareNet>> _netHeap; ///< Use heap to determine the order for routing
        std::vector<IndexType> _netIterations; ///< The iterations of each net
        IndexType _subNetIdx = INDEX_TYPE_MAX; ///< The subNet Index is currently being routed
        bool _forceRoute = false; ///< whether to ignore the edge capacity 
        std::unique_ptr<SymNet> _symNetPtr = nullptr; ///< Used to figure out which symnet is being routed
        bool _symRoutingMode = false; ///< whether the global router is doing symmetric routing
        Box<IntType> _searchBBox; ///< The bounding box for searching 
        IndexType _sourceTermIdx = INDEX_TYPE_MAX; ///< The index for the source terminal
        IndexType _targetTermIdx = INDEX_TYPE_MAX; ///< The index for the target terminal
        IndexType _netIdx = INDEX_TYPE_MAX; ///< The net is currently being routed
        IndexType _netIdxSym = INDEX_TYPE_MAX; ///< The net that is at the other side of symmetry
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_GRASTARSEQUENTIAL_H_
