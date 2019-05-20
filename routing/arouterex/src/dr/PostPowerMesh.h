/**
 * @file PostPowerMesh.h
 * @brief Build a power mesh as post processing step
 * @author Keren Zhu
 * @date 03/15/2019
 */

#ifndef AROUTER_POST_POWER_MESH_H_
#define AROUTER_POST_POWER_MESH_H_

#include "db/Database.h"
#include "dr/DRBase.h"
#include "dr/DetailedGridlessAStar.h" // The result of path Search should be in location and layer. Using this to wrap
#include "dr/HisCostMapRoughGrid.h" // The his cost map implementation
#include "dr/ShapeMapRtree.h" // The implementation of shape map

PROJECT_NAMESPACE_BEGIN

/// @class PROJECT_NAMESPACE::PostPowerMesh
/// @brief building up a power mesh as a post-processing step
class PostPowerMesh : public DRBase
{
    typedef GridlessAStarBase<NodeAffixDR> GridlessBase;
    typedef PathSearchAlgoAnaDR<NodeKey> PathSearch; ///< Using the gridless detailed routing path search algorithm
    public:
        explicit PostPowerMesh(Database &db) : DRBase(db), _db(db), _instr(db.searchInstruct()),  _shapeComp(db),  _viaSelect(db){}
        /// @brief post-processing for a power/ground net
        /// @param first: the index of the net (should follow the drDB)
        /// @param second: the indices of the ports, follow the net.terminal
        /// @param third: some shape map implementation pointer
        /// @return if successful
        bool build(IndexType netIdx, const std::vector<IndexType> &ports, std::shared_ptr<ShapeMap> shapeMapPtr);
    private:
        struct GuidePath
        {
            explicit GuidePath() = default;
            IndexType portTermIdx = INDEX_TYPE_MAX; ///< The terminal index for the port
            NetNodeLoc targetLoc; ///< The target search point
            IndexType pinTermIdx = INDEX_TYPE_MAX;  ///< The pin terminal index
            std::vector<IndexType> netNodes; ///< The nodes found to be candidates of searching source
            std::string toStr() const 
            {
                std::stringstream oss;
                oss<< "Port "<< portTermIdx << " Point "<< targetLoc.loc().toStr()<<" " << targetLoc.layer() <<" ";
                oss<< "Pin "<< pinTermIdx << " nodes ";
                for (auto node : netNodes) { oss <<node <<" ";}
                return oss.str();
            }
        };
        /// @brief just a simple struct for containing the information needed in guidePath
        struct PowerMeshSearchNode
        {
            explicit PowerMeshSearchNode() = default;
            bool empty() const { return sourcePort == INDEX_TYPE_MAX && pins.empty(); }
            IndexType sourcePort = INDEX_TYPE_MAX; ///< If MAX, no port, o.w. terminal index
            std::set<IndexType> pins; ///< regular pins (net.terminal index)
        };

        struct NodePair
        {
            explicit NodePair(const XY<IndexType> &node1, const XY<IndexType> &node2)
            {
                Assert(node1 != node2);
                if (node1 <  node2) { nodeLo = node1; nodeHi = node2; }
                else  { nodeLo = node2; nodeHi = node1; }
            }
            bool operator<(const NodePair &rhs) const { return nodeLo == rhs.nodeLo ? nodeHi < rhs.nodeHi : nodeLo < rhs.nodeLo; }
            XY<IndexType> nodeLo;
            XY<IndexType> nodeHi;
        };

        struct TermPair
        {
            explicit TermPair(IndexType term1, IndexType term2)
            {
                if (term1 < term2) { termLo = term1; termHi = term2; }
                else { termLo = term2; termHi = term1; }
            }
            bool operator<(const TermPair &rhs) const { return termLo == rhs.termLo ? termHi < rhs.termHi : termLo < rhs.termLo; }
            IndexType termLo;
            IndexType termHi;
        };

        /*------------------------------*/ 
        /* Pair generation function     */
        /*------------------------------*/ 
        /// @brief find which pairs of terminals need to be connected and in which rough paths
        /// @param first: the index of the net (should follow the drDB)
        /// @param second: the indices of the ports, follow the net.terminals
        /// @param third: a reference to the vector to store the pair connection and the path into
        /// @return if successful
        bool guidePath(IndexType netIdx, const std::vector<IndexType> &ports, std::vector<GuidePath> &guide); 
        /*------------------------------*/ 
        /* Routing function             */
        /*------------------------------*/ 
        /// @brief routing for the found paths
        bool routePath(IndexType netIdx, std::vector<GuidePath> &guides);
        /// @brief path search initialization for the routing
        void routePathInit();
        /// @brief route a guide
        void routeCore(GuidePath &guide);
        /// @brief configure path search options
        void routeConfigurePathSearch(GuidePath &guide);
        /// @brief assign routing solution
        void routeAssignSol();

        /*------------------------------*/ 
        /* Misc.                        */
        /*------------------------------*/ 
        /// @brief check the ports are valid in the net
        /// @param first: the index of the net (should follow the drDB)
        /// @param second: the indices of the ports, follow the net.terminals
        /// @return are all the ports inside the net
        bool checkPortsValid(IndexType netIdx, const std::vector<IndexType> &ports) const;
        /// @brief check the terminal pair candidates are not routed before
        bool checkTerminalPairValid(const std::set<TermPair> &pairs, IndexType netIdx) const;
        /// @brief using the teminal pair candidates to generate search points
        void guidePathPlanPath(IndexType netIdx, std::vector<GuidePath> &guide, std::set<TermPair> &pairs, const std::vector<IndexType> &ports);
        /// @brief find the correct searching point for the pair
        void guidePathPlanPathSearchPoint(IndexType netIdx, GuidePath &guide);
        /// @brief prepared the grid for guidePath
        /// @return if successful
        bool guidePathPrepareGrid(IndexType netIdx, const std::vector<IndexType> &ports, Vector2D<PowerMeshSearchNode> &grid, std::set<XY<IndexType>> &sources);
        /// @brief find candidate pairs of terminals
        bool guidePathCandidates(Vector2D<PowerMeshSearchNode> &grid, std::set<XY<IndexType>> &sources, std::set<TermPair> &pairs, IndexType netIdx);
        /// @brief BFS the grid and find the candidate pairs of nodes
        void guidePathCandidateSearch(Vector2D<PowerMeshSearchNode> &grid, XY<IndexType> source, std::set<NodePair> &pairs, IndexType netIdx);
        /// @brief some supporting function to guidePathCandidateSearch, visit the node
        bool guidePathCandidateSearchVisit(XY<IndexType> node, std::list<XY<IndexType>> &queue, Vector2D<PowerMeshSearchNode> &grid, IndexType netIdx, Vector2D<IntType> &visited);
        /// @brief prune the node pairs with some connection has already been assigned
        void guidePathPruneNodePair(std::set<NodePair> &nodePairs, IndexType netIdx, Vector2D<PowerMeshSearchNode> &grid);
        /// @brief build up candidate pairs of terminals from node pairs
        void guidePathCandidateTerms(std::set<TermPair> &candidates, IndexType netIdx, std::set<NodePair> &nodePairs, Vector2D<PowerMeshSearchNode> &grid);
        /// @brief prune the term LUT
        void guidePathCandidateTermsPruneLUT(Vector2D<IntType> &lut, Vector2D<PowerMeshSearchNode> &grid);
        /// @brief find a pair of terminals (port and non-port) from a node pair
        void guidePathCandidateTermsPortAndPin(std::set<TermPair> &candidates, IndexType netIdx, XY<IndexType> nodeSource, XY<IndexType> nodePin, Vector2D<PowerMeshSearchNode> &grid);
    private: 
        Database &_db; ///< The database of the router
        /*------------------------------*/ 
        /* Routing                      */
        /*------------------------------*/ 
        std::shared_ptr<PathSearch> _pathSearch; ///< Solver for path search
        /*------------------------------*/ 
        /* Technology information       */
        /*------------------------------*/ 
        SearchInstruct &_instr; ///< Include the processed design rule information
        /*------------------------------*/ 
        /* Routing                      */
        /*------------------------------*/ 
        std::shared_ptr<ShapeMap> _shapeMap; ///< used to query the existence of other shapes
        ShapeConstructor _shapeComp; ///< The shape constructor
        FixedViaSelector _viaSelect; ///< The processed via information
        std::vector<NetNodeWire> _wireTypes; ///< The specification of the wire that is using
        NetNodeLoc _primarySource; ///< Primary search point
        NetNodeLoc _parimaryTarget; ///< primary target search point
        IndexType _netIdxInRoute = INDEX_TYPE_MAX; ///< The current net index
};

PROJECT_NAMESPACE_END

#endif  //AROUTER_POST_POWER_MESH_H_
