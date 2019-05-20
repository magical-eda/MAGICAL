/**
 * @file DetailedNCR.h
 * @brief Negotiated congestion routing for detailed router
 * @author Keren Zhu
 * @date 11/04/2018
 */

#ifndef AROUTER_DETAILED_NCR_H_
#define AROUTER_DETAILED_NCR_H_

#include "grdr/RoutingAlgo.h" // This is a routing algorithm
#include "dr/DRBase.h" // This is a detailed router. Used DRBase functions for database-related operations
#include "dr/DetailedGridlessAStar.h" // The result of path Search should be in location and layer. Using this to wrap
#include "dr/HisCostMapRoughGrid.h" // The his cost map implementation
#include "dr/ShapeMapRtree.h" // The implementation of shape map

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::NCR_NOPATH_EXCEPTION
/// @brief exception for no path is found even if allowing overlaping
class NCR_NOPATH_EXCEPTION : public std::exception
{
    virtual const char* what() const throw()
    {
        return "No path was found even if allowing overlapping. There is no legal path";
    }
};

/// @class AROUTER::DetailedNCR
/// @brief general negotialtion-congestion based routing
class DetailedNCR : public RoutingAlgo, public DRBase
{

    protected:
        using RoutingAlgo::_db; // RoutingAlgo::_db and DRBase::_db point to the same database. using one of them for convenience
    typedef GridlessAStarBase<NodeAffixDR> GridlessBase;
    typedef PathSearchAlgoAnaDR<NodeKey> PathSearch; ///< Using the gridless detailed routing path search algorithm


    /// @brief Stuctor for maintaining the subnets in heap for routing orders
    /// TODO: add things that differentiate the netOrdering
    struct NetNode
    {
        explicit NetNode(Database &db_p, IndexType netIdx_p, IndexType iteration_p) : db(db_p), netIdx(netIdx_p), iteration(iteration_p) {}
        IntType hpwl() const 
        {
            //const auto &netBBox = db.grDB().nets().at(netIdx).gridBBox();
            return db.hpwl(netIdx);
        }
        IntType priority() const 
        {
            //const auto &net = db.grDB().nets().at(netIdx);
            if (db.drDB().net(netIdx).isSelfSym()) { return 0; }
            if (db.drDB().net(netIdx).isSym()) { return 1; }
            return 2;
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
        explicit DetailedNCR(Database &db) : RoutingAlgo(db), DRBase(db), _instr(db.searchInstruct()), _shapeMap(db, _instr), _shapeComp(db),  _viaSelect(db) { }
        /*------------------------------*/ 
        /* Solve                        */
        /*------------------------------*/ 
        bool runKernel() override { return ncrFlow(); }
        /*------------------------------*/ 
        /* Getter                       */
        /*------------------------------*/ 
        /// @brief get the shape map object pointer in the solver
        /// @return the shape map object poinrter in the solver
        std::shared_ptr<ShapeMap> shapeMapPtr() { return ::klib::createSharedPtr<ShapeMap>(_shapeMap); }
    protected:
        /*------------------------------*/ 
        /* Initialization               */
        /*------------------------------*/ 
        /// @brief the wrapper of the overall initialization
        /// @return if sucessful
        bool init();
        /// @brief initialization of the histCostMap
        bool initHisCostMap();
        /// @brief initialization of the shape map
        bool initShapeMap();
        /// @brief initialization of the via selector
        bool initViaSelect();
        /// @brief initialization the net ordering for routing
        bool initNetOrdering();
        /// @brief insert dummy pin shape around the pins
        void dummyPinShape();
        /*------------------------------*/ 
        /* Init path search             */
        /*------------------------------*/ 
        /// @brief wrapper of initializing the path search (gridless A*)
        /// @return if successful
        bool initPathSearchGridlessAstar();
        /// @brief wrapper of initializing the path search (boost A*). Just for comparision!
        bool initPathSearchBoostAStar();
        /*------------------------------*/ 
        /* Gridless A star              */
        /*------------------------------*/ 
        /// @brief initializing the search region, routing direction and other design informawtion for the path search solver
        /// @param the ptr to the solver
        /// @return if successful
        bool initGridlessAstarDesign(std::shared_ptr<DetailedGridlessAStar> pSolver);
        /*------------------------------*/ 
        /* Routing flow                 */
        /*------------------------------*/ 
        /// @brief assign the search results to the net
        void assignSol();
        /// @brief route a sub net
        bool routeSubNet(IndexType netIdx, IndexType sourceTermIdx, IndexType targetTermIdx);
        /// @brief route a normal net
        bool routeOneNormalNet(IndexType netIdx);
        /// @brief route a symmetry net
        bool routeSymNet(IndexType netIdx);
        /// @brief route a self symmetry net
        bool routeSelfSymNet(IndexType netIdx);
        /// @brief routing ncr flow
        bool ncrFlow();
        /// @brief rip-up the current path search result
        void ripupPathSearchResult();
    private:
        /// @brief assign solution to a net with vector of nodes
        /// @param the net index 
        /// @param a vector of nodes
        void assignSolNodes(IndexType netIdx, const std::vector<NodeKey> &nodeVec, bool sym);
        /// @brief configure the path search source/target/shapes
        /// @param the net index
        void configurePathSearch(IndexType netIdx, IndexType sourceTermIdx, IndexType targetTermIdx);
        /// @brief configure the wiring settings
        /// @param the net index
        void configureWireType(IndexType netIdx);
        /// @brief waive the symmetry constraints for a symmetry pair
        /// @param the index of the SymNet
        void waiveSymPair(IndexType symIdx);
        /// @brief waive the self symmetry constraint for a self-sym net
        /// @param the index of the SelfSym
        void waiveSelfSymNet(IndexType selfSymIdx);

        /// @brief configure the waive global routing properties
        /// @param the index of net
        void configureWaiveRouting(IndexType netIdx);

        /// @brief waive a global routing net
        /// @param the index of the net
        void waiveGlobalRouting(IndexType netIdx);

        /*------------------------------*/ 
        /* Misc. functions              */
        /*------------------------------*/ 
        /// @brief Check whether a node should be added to path search and add source to _pathSearch
        void checkSymSourceAndAdd(const NodeKey &node, const SymNet &symNet, ShapeID sourceShapeID, ShapeID sourceShapeSym);
    private:
        std::shared_ptr<PathSearch> _pathSearch; ///< Solver for path search
        /*------------------------------*/ 
        /* Technology information       */
        /*------------------------------*/ 
        SearchInstruct &_instr; ///< Include the processed design rule information
        /*------------------------------*/ 
        /* Needed by the pathSearch     */
        /*------------------------------*/ 
        HisCostMapRoughGrid _hisCostMap; ///< The map for history cost
        ShapeMapRtree _shapeMap; ///< The map for querying the shapes in the design
        ShapeConstructor _shapeComp; ///< The shape constructor
        FixedViaSelector _viaSelect; ///< The processed via information
        /*------------------------------*/ 
        /* Net ordering                 */
        /*------------------------------*/ 
        boost::heap::fibonacci_heap<NetNode, boost::heap::compare<compareNet>> _netHeap; ///< Use heap to determine the order for routing
        std::vector<IndexType> _netIterations; ///< The iterations of each net. Used to help determining the net ordering 
        /*------------------------------*/ 
        /* Net and routing settings     */
        /*------------------------------*/ 
        IndexType _netIdx = INDEX_TYPE_MAX; ///< The net that is currently being routed
        std::vector<NetNodeWire> _wireTypes; ///< The specification of the wire that is using
        /*------------------------------*/ 
        /* Misc.                        */
        /*------------------------------*/ 
        NetNodeLoc _primarySource; ///< Primary search point
        NetNodeLoc _parimaryTarget; ///< primary target search point
        SymNet _symNet; ///< The symnet is currently being routed


};


PROJECT_NAMESPACE_END

#endif ///AROUTER_DETAILED_NCR_H_
