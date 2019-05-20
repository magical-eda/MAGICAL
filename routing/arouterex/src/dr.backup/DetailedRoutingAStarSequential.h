/**
 * @file DetailedRoutingAStarSequential.h
 * @brief A* search sequential routing for detailed routing
 * @author Keren Zhu
 * @date 10/18/2018
 */

#ifndef AROUTER_DRASTARSEQUENTIAL_H_
#define AROUTER_DRASTARSEQUENTIAL_H_

#include "grdr/RoutingAlgo.h"
#include "grdr/AStarBase.h"
#include "FixedViaSelector.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::NodeAttach
/// @brief Some information should be attached to the NodeInfo
class NodeAttach
{
    public:
        /// fixed via of this node, if existing 
        IndexType fixedViaID = 0; ///< record in to via
        bool insertFixedVia = false;
        bool viaUp = true; ///< true this via is from lower to higher, false from: higher to: lower

        IndexType fromVia = 0; ///< the grid this node is from last via in the search

};


/// @class AROUTER::DetailedRoutingAStarSequential
/// @brief Sequential routing with A* search
class DetailedRoutingAStarSequential : public RoutingAlgo, public AStarBase<NodeAttach>
{
    /// NOTE: the following are based on global db, which shouldn't be issue. Even the global routing is not perfomed, the bounding box can be constructed
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
        explicit DetailedRoutingAStarSequential(Database &db) : RoutingAlgo(db), _fixedViaSelector(db) {}
        /// @brief run the kernel function
        /// @return if succesful
        bool runKernel() override;
        /*------------------------------*/ 
        /* Initialization               */
        /*------------------------------*/ 
        /// @brief initialization of needed data
        /// @return if successful
        bool init();
        /// @brief init Vias
        /// @return if successful
        bool initVia();
        /// @brief init searching grid
        /// @return if successful
        bool initGrid();
        /// @brief init net ordering
        /// @return if successful
        bool initNetOrdering();
        /*------------------------------*/ 
        /* Routing flow                 */
        /*------------------------------*/ 
        /// @brief negotation-based routing
        /// @return if successful
        bool ncrRouting();
        /// @brief route one normal net
        /// @param the index of the net
        /// @return if successful
        bool routeOneNormalNet(IndexType netIdx);
        /// @brief route a subnet
        /// @param the index of the subnet(2-pin net) in the _netIdx want to route
        /// @return if successful
        bool routeSubNet(IndexType subNetIdx);
        /// @brief rip-up the net occupied the path and update the history cost
        void ripup();
        /// @brief rip-up a routed net
        /// @param the index of the net want to rip up
        void clearNet(IndexType netIdx);
        /// @brief assign the search results to the subnet
        void assignSol();
        /*------------------------------*/ 
        /* Routing support functions    */
        /*------------------------------*/ 
        /// @brief whether connecting these two nodes need a via
        /// @param from, to: the two nodes
        /// @return if they need a via
        bool needVia(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const { return from.z() != to.z(); }
        /// @briefPick a fixed for the two node connection 
        /// @param from, to: two nodes
        /// @return -1 if no possible via, otherwise return the index of via type that was picked 
        IndexType pickFixedVia(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const;
        /// @brief find the search region for the cut layer spacing checking
        /// @param the index of the layer (in cut layers)
        /// @return the enlarge distance for checking
        LocType cutLayerInquirySpacing(IndexType cutLayerIdx) const;
        /*------------------------------*/ 
        /* Overrid Function             */
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
        /// @breif Is the node occupied?
        /// @param the node want to be evaluated
        /// @return if the given node is occupied
        bool isOccupied(const XYZ<IndexType> &xyz) const override;
        /// @brief Original: Is in the closed list (default not using closed list) Here: whether it is in global routing guide
        /// @param the node want to be evalued
        /// @return if the node is inside the closed list
        bool isClosed(const XYZ<IndexType> &xyz) const override;
        /// @brief Whether grid edge is enough/existing for routing
        /// @brief Update the node if in solving, this node's cost is reduced. In other words, its parent node change
        /// @param from: the "from" node. to: the "to" node. tentatuveGCost: the calculated g cost for to
        void updateNodeParent(const XYZ<IndexType> &from, const XYZ<IndexType> &to, IntType tentativeGCost) override;
    private:
        boost::heap::fibonacci_heap<NetNode, boost::heap::compare<compareNet>> _netHeap; ///< Use heap to determine the order for routing
        std::vector<IndexType> _netIterations; ///< The iterations of each net
        bool _useGlobalGuide = true; ///< whether using the global routing guide
        bool _useGlobalLayer = true; ///< whether using the global routing layer assignment result
        IndexType _netIdx = INDEX_TYPE_MAX; ///< the index of net the A* searching is conducting
        FixedViaSelector _fixedViaSelector; ///< the FixedViaSelector (library for the VIAs)
        bool _forceRoute = false; ///< Whether to force routing it and try to get a route
        std::vector<bool> _ignoreGRList; ///< list of nets need to be ignored from gr results
#ifdef DEBUG_DR_
        XYZ<IndexType> _primarySource;
#endif
};
PROJECT_NAMESPACE_END

#endif ///AROUTER_DRASTARSEQUENTIAL_H_
