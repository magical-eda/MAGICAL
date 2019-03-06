/**
 * @file DetailedGridlessAStar.h
 * @brief Gridless A* search sequential routing for detailed routing. A medium-sized grid is used to record the history cost. The design is routed sequentially
 * @author anoynmous anoynmous
 * @date 10/26/2018
 */

// Effort level:
//     0: only legal and follow the gloabal routing guide
//     1: allow detouring in the upper and lower level
//     2: allow wrong way routing 
//     3: 1+2
//     4: allow overlap between routed shapes, but detour through fixed objects
//     5: 1+4
//     6: 2+4
//     7: 1+2+4

#ifndef AROUTER_DETAILED_GRIDLESS_ASTAR_H_
#define AROUTER_DETAILED_GRIDLESS_ASTAR_H_

#include "dr/FixedViaSelector.h"
#include "dr/ShapeMapBase.h"
#include "grdr/GridlessAStarBase.h"
#include "grdr/PathSearchAlgo.h"
#include "dr/HisCostMap.h"

PROJECT_NAMESPACE_BEGIN

struct NodeAffixDR
{
    public:
        NodeAffixDR() = default;
        NodeKey sourceNode() const { return _sourceNode; }
        void setSourceNode(NodeKey sourceNode) { _sourceNode = sourceNode; }
        void setSourceShape(ShapeID shapeID)   { _originShapeID = shapeID; }
        ShapeID sourceShape() const { return _originShapeID; }
        /// @brief the type of via this node has 
        /// @return the index of this via type in the target lower metal layer
        IndexType viaType() const { return _viaType; }
        /// @brief whether this via is valid
        /// @return whether this via is valid
        bool hasVia() const { return _viaType != INDEX_TYPE_MAX; }
        /// @brief add a via to the from side
        /// @param the index of the via type, the from and to nodekey
        void addVia(IndexType viaType);
        /// @brief determine if this is the secondary net in the symmetry pair
        /// @return if this is the secondary net in the symmetry pair
        //bool isSecondaryNet() const { return _secondaryNet; }
        /// @brief set if this node is the secondary net in the symmetry pair
        /// @param if this is the secondary net in the symmetry pair
        //void setIsSecondaryNet(bool secondaryNet) { _secondaryNet = secondaryNet; }
    private:
        NodeKey _sourceNode = NodeKey(); ///< The node of the beginning of the shape
        IndexType _viaType = INDEX_TYPE_MAX; ///< The index of via type selected. If equals INDEX_TYPE_MAX, then this is not a via. (the via type is only valid on the "to" side)
        //bool _viaUp = true; ///< Wether this via is from lower to upper, used to determine whether this node is on the "from" or the "to" side
        ShapeID _originShapeID =  ShapeID(); ///< The shape ID that the origin of the path has
};

inanoynmouse void NodeAffixDR::addVia(IndexType viaType)
{
    _viaType = viaType;
#if 0
    if (from.layer < to.layer)
    {
        _viaType = viaType;
        _viaUp = true;
    }
    else
    {
        _viaType = viaType;
        _viaUp = false;
    }
#endif
}

class DetailedGridlessAStar : public GridlessAStarBase<NodeAffixDR>, public PathSearchAlgoAnaDR<NodeKey>
{
    public:
    /*------------------------------*/ 
    /* Member functions             */
    /*------------------------------*/ 
        /// @brief default constructor
        explicit DetailedGridlessAStar() = default;
        /*------------------------------*/ 
        /* Interface for PathSearchAlgo */
        /*------------------------------*/ 
        /// @brief add a source
        void addSourceShapeAlgo(const NodeKey &nodeKey, ShapeID shapeID) override 
        { 
            addSourceAlgo(nodeKey); 
            node(nodeKey).affix.setSourceShape(shapeID); 
#ifndef NDEBUG
            if (this->isSymMode())
            {
                auto symNode = _symNet.symLoc(nodeKey.loc);
                if (_symNet.symType() == SymType::HORIZONTAL)
                {
                    AssertMsg(symNode.x() >= nodeKey.loc.x(), "node loc %s \n", nodeKey.loc.toStr().c_str());
                }
                else if (_symNet.symType() == SymType::VERTICAL)
                {
                    Assert(symNode.y() >= nodeKey.loc.y());
                }
            }
#endif
        }
        /// @brief add a source
        void addSourceAlgo(const NodeKey &nodeKey) override { this->addSource(nodeKey); node(nodeKey).affix.setSourceNode(nodeKey); }
        /// @brief set the target
        void setGoalAlgo(const NodeKey &nodeKey) override { this->setGoal(nodeKey); }
        /// @brief solve path search
        bool solvePathSearchAlgo() override 
        { 
            if (_sources.size() == 0)
            {
                WRN("%s: no valid source \n", __FUNCTION__);
                return false;
            }
            initBeforeSolve();
            bool success = this->pathSearchKernal(); 
#if 0
            auto filename =  "debug/" + std::to_string(_netIdx) + ".gds";
            drawGdsExploredNode(filename);
#endif
            if (success) { compressResult(); }  
            if (this->isSymMode()) { this->copySymPathResult(); }
            return success;
        }
        /// @brief reset the path search solver
        void resetAlgo() override { this->resetSym(); this->reset(); }
        /// @brief return the result as a vector of Node
        const std::vector<NodeKey> & resultAlgo() override { return this->solution(); }
        /// @brief return the result as a vector of Node for the seondary symmetry net
        const std::vector<NodeKey> & symResultAlgo() override { return _symResult; }
        /// @brief the via at the node. If no via, then it is a wire
        /// @param the NodeKey for the node
        /// @return index of via type. if INDEX_TYPE_MAX, then there is no via
        IndexType viaType(const NodeKey &nodeKey) override { return this->node(nodeKey).affix.viaType(); }
        /// @brief set the net index that being routed
        /// @param the index of the net
        void setNetIdx(IndexType netIdx) override { _netIdx = netIdx; }
        /// @brief set wire type
        /// @param the specification for the wire
        void setWireType(const std::vector<NetNodeWire> &wire) override { _wireTypes = wire;}
        /// @brief set effort level
        /// @param how much effort the path search should do
        void setEffortMode(IntType effort) override;
        /// @brief debug interface
        /// @param file name
        void debugInterface(const std::string &fileName) override
        {
            this->drawGdsExploredNode(fileName);
        }
        /*------------------------------*/ 
        /* Interface for analog DR      */
        /*------------------------------*/ 
        /// @brief add the source shape ID for the symmetry net. NOTE: the location is for the primary/default/before-flipping net, not the one for after the symmetry
        /// @param first: the node identity for the source node PRIMARY for SYMNET to be added
        /// @param second: the shape identity for the source node to be added SECONDARY FOR SYMNET
        void addSymNodeSourceShape(NodeKey primaryNode, ShapeID secondaryShape) override
        {
            Assert(this->isSymMode());
            // Simply map them
            _sourceShapeMap.insert(std::make_pair(node(primaryNode).affix.sourceShape(), secondaryShape));
        }

        /// @brief set the source/target to the shape map with record shape (primary in the symmetry)
        void configureShapeMapID(NodeKey from) override
        {
            _shapeMap->setSourceShapeID(node(from).affix.sourceShape());
            // Set the deafult primary target shape
            _shapeMap->setTargetShapeID(_targetShape);
        }
        /// @brief set the source/target to the shape map with record shape (secondary in symmetry)
        /// @param source node for the secondary shape source 
        void configureShapeMapIDSym(NodeKey primFrom) override
        {
            _shapeMap->setSourceShapeID(_sourceShapeMap.at(node(primFrom).affix.sourceShape()));
            // Set the deafult primary target shape
            _shapeMap->setTargetShapeID(_symTargetShape);
        }
        /*------------------------------*/ 
        /* Init and reset               */
        /*------------------------------*/ 
        /// @brief init the misc information, called before solve
        void initBeforeSolve(); 
        /// @brief init the history cost grid
        /// @param hisCostMap: the ptr to the hisCostMap
        void initHisCostMap(std::shared_ptr<HisCostMap> hisCostMap) { _hisCostMap = hisCostMap; }
        /// @brief init the shape map for querying other shapes
        /// @param shapeMap: the ptr to the shape map
        void initShapeMap(std::shared_ptr<ShapeMap> shapeMap) { _shapeMap = shapeMap; }
        /// @brief init the shape constructor
        /// @param shapeMap: the ptr to the shape constructor
        void initShapeConstrutor(std::shared_ptr<ShapeConstructor> shapeConstructor) { _shapeComp = shapeConstructor; }
        /// @brief set _roughGrid
        /// @param roughGrid: the ptr to the Roughgrid which contain the global routing solution
        void initRoughGrid(std::shared_ptr<RoughGrid> roughGrid) { _roughGrid = roughGrid; }
        /// @brief set _stepSizeX
        /// @param the minimum step size in x direction
        void setStepSize(std::vector<LocType> stepSize) { _stepSize = stepSize; }
        /// @brief set _steps
        /// @param how many steps should to be taken each time
        void setSteps(const std::vector<IndexType> &steps) { _steps = steps; }
        /// @brief set _routingDir
        /// @param the vector of routing direction that specify the preferred routing direction of each layer
        void setRoutingDirection(const std::vector<RouteDirType> &routingDir) { _routingDir = routingDir; }
        /// @brief init _viaSelect, which include the processed information for selecting vias
        /// @param viaSelect: the ptr to a FixedViaSelector
        void initViaSelect(std::shared_ptr<FixedViaSelector> viaSelect) { _viaSelect = viaSelect; }
        /// @brief reset the symmetry
        void resetSym()
        {
            _symResult.clear();
            _sourceShapeMap.clear();
            _symNet = SymNet();
        }
    protected:
        /*------------------------------*/ 
        /* Override Function            */
        /*------------------------------*/ 
        /// @brief Is given xyz a valid coordinate?
        /// @param the node want to be evaluated
        /// @return if the given node is valid
        bool        isValid(const NodeKey &nodeKey) const override;
        /// @brief Evaluate distance between node (used to calculate gCost)
        /// @param the two nodes from/to want to be evaluted
        /// @return the distance between these two nodes
        CostType distance(const NodeKey &from, const NodeKey &to) override;
        /// @brief get the neighbors of the input node, should be valid(safe for vector access), not neccessarily of unblocked
        /// @param the node want to be evaluted.  the vector of indices representing the list of neighbor nodes
        void neighbors(const NodeKey &nodeKey, std::vector<NodeKey> &list) override;
        /// @breif Is the segment occupied?
        /// @param the node want to be evaluated
        /// @return if the given node is occupied
        bool isOccupied(const NodeKey &from, const NodeKey &to) override;
        /// @brief Original: Is in the closed list (default not using closed list) Here: whether it is in global routing guide
        /// @param the node want to be evalued
        /// @return if the node is inside the closed list
        bool isClosed(const NodeKey &nodeKey) const override;
        /// @brief Whether grid edge is enough/existing for routing
        /// @brief Update the node if in solving, this node's cost is reduced. In other words, its parent node change
        /// @param from: the "from" node. to: the "to" node. tentatuveGCost: the calculated g cost for to
        void updateNodeParent(const NodeKey &from, const NodeKey &to, CostType tentativeGCost) override;
        /// @brief whether the edge is addimissive; 
        bool edgeAdm(const NodeKey &from, const NodeKey &to) override;
        /// @brief calculate the heuristic cost
        /// @param the node want to be evaluted
        /// @return the heuristic cost of the node
        CostType   heuristicCostEstimate(const NodeKey &nodeKey) const override
        { return static_cast<CostType>(::klib::manhattanDistance<LocType>(nodeKey.loc, _goal.loc)) + static_cast<CostType>(::klib::idxDif(nodeKey.layer, _goal.layer)); }
        /// @brief Is destination?
        /// @param the node want to be evaluted
        /// @return if the node is the goal/target/destination
        bool        isGoal(const NodeKey &nodeKey) override;
    private:
        /*------------------------------*/ 
        /* Geometric shape checking     */
        /*------------------------------*/ 
        /// @brief a wire segment on a layer is legal
        /// @param the two end points of the segment
        /// @param whether the one need to be checked is the symmetry one
        /// @return if the segment is legal
        bool wireLegal(const NodeKey &from, const NodeKey &to, bool sym);
        /// @brief a via segment is legal
        /// @param the two end point of the segment.
        /// @return if the via is legal
        bool viaLegal(const NodeKey &from, const NodeKey &to, bool sym);
        /*------------------------------*/ 
        /* Config & init                */
        /*------------------------------*/ 
        /// @brief configure the modes
        void configEffortModes();
        /// @brief calculating the minimum segment lengths for each routable layer
        void calculateMinSegLens();
        /*------------------------------*/ 
        /* Compress the path search     */
        /*------------------------------*/ 
        /// @brief compress the path search results and remove the redundant nodes
        void compressResult();
        /// @brief get the relation between two NodeKey. Just for helping the compressResult and determine whether the two nodes should be in a different shape
        /// @return 1: horizontal 2: vertical 3:via
        IndexType nodeKeyRelation(const NodeKey &node1, const NodeKey &node2) const;
        /*------------------------------*/ 
        /* Process node information     */
        /*------------------------------*/ 
        /// @brief determine whether the new node is belonging to a new shape of the from
        /// @param from: the existing node. to: the new node
        /// @return true if a new shape is needed, false if they can be in the same shape
        bool isNodeNewShape(const NodeKey &from, const NodeKey &to);
        /*------------------------------*/ 
        /* Misc.                        */
        /*------------------------------*/ 
        /// @brief whether need via
        /// @return whet,er need via
        bool needVia(const NodeKey &from, const NodeKey &to) const { return from.layer != to.layer; }
        /// @brief choose a via
        /// @param routable layer
        IndexType chooseVIA(IndexType layer) const;
        /// @brief calculate the history cost for a path
        /// @param first: the from node
        /// @param second: the to node
        /// @return the history cost from "from" to "to"
        CostType hisCost(NodeKey from, NodeKey to) const;
        /*------------------------------*/ 
        /* Symmetry                      */
        /*------------------------------*/ 
        /// @brief get the symmetry point
        /// @param a point on one side of the symmetry
        /// @return the point corresponding to the input point with respect to the symmetry axis
        NodeKey symmetryPoint(NodeKey pt) const { return NodeKey(_symNet.symLoc(pt.loc), pt.layer); }
        /// @brief copy the path result from primary to secondary symmetry net
        void copySymPathResult()
        {
            _symResult.clear();
            for (auto & node : _result)
            {
                _symResult.emplace_back(this->symmetryPoint(node));
            }
        }
        /*------------------------------*/ 
        /* Debug                        */
        /*------------------------------*/ 
        /// @brief draw the explored node for the current search
        /// @param filename: the filename for the output gds file.
        void drawGdsExploredNode(const std::string &filename) const;
    /*------------------------------*/ 
    /* Member variables             */
    /*------------------------------*/ 
    private:
        /*------------------------------*/ 
        /* Setting of the search        */
        /*------------------------------*/ 
        std::vector<RouteDirType> _routingDir; ///< The routing directions
        bool _allowWrongWay = false; ///< Whether allow wrong way
        bool _allowDetouringNeighborLayer = false; ///< whether allow detouring to neighbor layer
        bool _allowOverLap = false; ///< Allow overlap between routed shapes. not including fixed objects
        /*------------------------------*/ 
        /* Searching step and his cost  */
        /*------------------------------*/ 
        std::shared_ptr<HisCostMap> _hisCostMap; ///< the history cost map
        std::shared_ptr<RoughGrid> _roughGrid; ///< The map for querying global routing guide
        std::vector<LocType> _stepSize; ///< The minimum step size for each layer
        std::vector<IndexType> _steps; ///< How many steps should be taken each time
        /*------------------------------*/ 
        /* Shape map                    */
        /*------------------------------*/ 
        std::shared_ptr<ShapeMap> _shapeMap; ///< used to query the existence of other shapes
        std::shared_ptr<ShapeConstructor> _shapeComp; ///< Shape constructor
        /*------------------------------*/ 
        /* Symmetry support             */
        /*------------------------------*/ 
        std::map<ShapeID,ShapeID> _sourceShapeMap; ///< The map between the primary net source node shapeIDs and the secondary
        std::vector<NodeKey> _symResult; ///< Store the result of the found path for the secondary net
        /*------------------------------*/ 
        /* Supporting variables         */
        /*------------------------------*/ 
        IndexType _netIdx = INDEX_TYPE_MAX; ///< The current net that is being routed
        std::vector<NetNodeWire> _wireTypes; ///< The type of wire used for routing the current net, including the information such as width
        std::shared_ptr<FixedViaSelector> _viaSelect; ///< The selector of the via
        Box<LocType> _searchRange; ///< The size down searching range
        std::vector<LocType> _minSegmentLength; ///< The minimum segment length for each layer
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_DETAILEDGRIDLESSASTAR_H_
