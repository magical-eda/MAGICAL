/**
 * @file GridlessAStarBase.h
 * @brief Gridless A* search base class
 * @author Keren Zhu
 * @date 10/26/2018
 */

#ifndef AROUTER_GRIDLESS_ASTARBASE_H_
#define AROUTER_GRIDLESS_ASTARBASE_H_

//#include <unordered_map>
#include <sparsehash/dense_hash_map> //google dense map
#include <boost/heap/fibonacci_heap.hpp>
#include "global/global.h"
#include "util/Vector2D.h"
#include "util/XY.h"
#include "util/Box.h"


PROJECT_NAMESPACE_BEGIN

/// @brief the key to identify the node
struct NodeKey
{
    explicit NodeKey() = default;
    explicit NodeKey(const XY<LocType> &loc_p, IndexType layer_p) : loc(loc_p), layer(layer_p) {}
    bool operator==(const NodeKey &rhs) const { return this->loc == rhs.loc && this->layer == rhs.layer; }
    std::string toStr() const
    {
        std::ostringstream oss;
        oss << loc.toStr();
        oss<<" layer: "<<layer;
        return oss.str();
    }
    XY<LocType> loc = XY<LocType>(-1, -1); ///< The location of this node
    IndexType layer = INDEX_TYPE_MAX; ///< the layer this node is on
};

/// @class AROUTER::dGridlessAStarBase
/// @brief the gridless A* path search base class
/// @tparam NodeAffix: some additional things attached to each node information
template<typename NodeAffix = IntType>
class GridlessAStarBase
{
    public:
        struct NodeValue;
    /*------------------------------*/ 
    /* Supporting structs           */
    /*------------------------------*/ 
        /// @brief the node stored in the candidate heap
        struct NodeRef
        {
            /// @brief constructor
            /// @param a Node this Node Ref corresponding to
            explicit NodeRef(const NodeKey &nodeKey_p, const NodeValue &nodeValue) : nodeKey(nodeKey_p), ghCost(nodeValue.gCost + nodeValue.hCost) {}
            //XY<LocType> loc = XY<LocType>(-1, -1); ///< The location of this node
            //IndexType layer = INDEX_TYPE_MAX; ///< The layer of this node
            NodeKey nodeKey; ///< identify the node
            CostType ghCost = COST_TYPE_MAX; ///< The current cost + heuristic cost
        };
        /// @brief Conpare structor for comparing two NodeRef (used in the heap)
        struct compareNode
        {
            bool operator()(const NodeRef &n1, const NodeRef &n2) const
            {
                return n1.ghCost > n2.ghCost;
            }
        };
        /// @brief Handle type for accessing/updating the element in the fibonacci_heap
        typedef typename boost::heap::fibonacci_heap<NodeRef, boost::heap::compare<compareNode>>::handle_type HandleType;

        /// @brief the node value (key is the NodeKey) that contain the needed informations
        struct NodeValue 
        {
            explicit NodeValue() = default;
            NodeKey parentKey() { return NodeKey(parentLoc, parentLayer); }
            //XY<LocType> loc = XY<LocType>(-1, -1); ///< The location of this node
            //IndexType layer = INDEX_TYPE_MAX; ///< The layer of this node
            XY<LocType> parentLoc = XY<LocType>(-1, -1); ///< The location of its parent node
            IndexType parentLayer = INDEX_TYPE_MAX; ///< The layer of its parent layer
            CostType gCost = COST_TYPE_MAX; ///< The current cost
            CostType hCost = COST_TYPE_MAX; ///< The heuristic cost
            bool isInList = false; ///< Is this node in the open list?
            bool isSource = false; ///< Is this node a source?
            HandleType handle; /// Handle type to access the node in the heap
            NodeAffix affix = NodeAffix(); ///< Some additional information. Nonsense in base class. Could/should be overrieded by the derived classes
        };
        /// @brief struct for hashing the NodeKey
        struct NodeKeyHashFunc
        {
            std::size_t operator() (const NodeKey &nodeKey) const
            {
                std::size_t seed = 0;
                boost::hash_combine(seed, nodeKey.loc.x());
                boost::hash_combine(seed, nodeKey.loc.y());
                boost::hash_combine(seed, nodeKey.layer);
                return seed;
            }
        };
        struct NodeKeyEqStr
        {
            bool operator()(const NodeKey &k1, const NodeKey &k2) const 
            {
                return k1==k2;
            }
        };
        typedef typename google::dense_hash_map<NodeKey, NodeValue, NodeKeyHashFunc, NodeKeyEqStr>::iterator NodeIterator; 
    /*------------------------------*/ 
    /* Member functions             */
    /*------------------------------*/ 
    //public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit GridlessAStarBase() { _nodeMap.set_empty_key(NodeKey()); };
        /*------------------------------*/ 
        /* Core Interface               */
        /*------------------------------*/ 
        /// @brief add source nodes
        /// @param the node want to add as source
        void addSource(const NodeKey &node);
        /// @brief set the goal for the search
        /// @param the node want to be set as the goal
        void setGoal(const NodeKey &node);
        /// @brief set _searchBox
        /// @param the search region need to be considered
        void setSearchBox(const Box<LocType> &searchBox) { _searchBox = searchBox; }
        /// @brief set _layerLo
        /// @param the lowest layer need to be considered
        void setLayerLo(IndexType layerLo) { _layerLo = layerLo; }
        /// @brief set _layerHi
        /// @param the highest layer need to be considered
        void setLayerHi(IndexType layerHi) { _layerHi = layerHi; }
        /// @brief solve the searching problem
        /// @return true if successful, false if not
        bool solve() { return pathSearchKernal(); }
        /// @brief get the results and re-init the solver
        /// @param the searching results in a vector of nodes
        const std::vector<NodeKey> & solution();
        /// @brief get the cost of the path
        /// @return the cost of the searching results. If infeasiable, return INT_TYPE_MAX
        IntType costSolution() const { return node(_goal).gCost; }
    //protected:
        /*------------------------------*/ 
        /* Init and reset               */
        /*------------------------------*/ 
        /// @brief reset the nodes and clear the heap
        void reset();
        /*------------------------------*/ 
        /* Virtual functions            */
        /*------------------------------*/ 
        /// Using virtual function for the features that should be flexible in different scenarios, and only has the simplest implementation.
        /// Other class can override these functions to implement A* search with different specifications.
        /// @brief Is given xyz a valid coordinate?
        /// @param the node want to be evaluated
        /// @return if the given node is valid
        virtual bool        isValid(const NodeKey &nodeKey) const;
        /// @brief Is the node blocked?
        /// @param the node want to be evaluated
        /// @return if the given node is blocked
        virtual bool        isBlocked(const NodeKey &nodeKey) const                                      { return false; }
        /// @breif Is the node occupied?
        /// @param the node want to be evaluated
        /// @return if the given node is occupied
        virtual bool        isOccupied(const NodeKey &from, const NodeKey &to)                          { return false; }
        /// @brief Is destination?
        /// @param the node want to be evaluted
        /// @return if the node is the goal/target/destination
        virtual bool        isGoal(const NodeKey &nodeKey) { return nodeKey == _goal; }
        /// @brief Is in the closed list (default not using closed list)
        /// @param the node want to be evalued
        /// @return if the node is inside the closed list
        virtual bool        isClosed(const NodeKey &nodeKey) const                                       { return false; }
        /// @brief Add the node to closed list
        /// @param the node want to be added to the closed list
        virtual void        addToClosed(const NodeKey &nodeKey)                                          { }
        /// @brief Evaluate distance between node (used to calculate gCost)
        /// @param the two nodes from/to want to be evaluted
        /// @return the distance between these two nodes
        /// Default all the nodes has distance of 1 between any other node
        virtual CostType   distance(const NodeKey &from, const NodeKey &to)                    { return 1; }
        /// @brief calculate the heuristic cost
        /// @param the node want to be evaluted
        /// @return the heuristic cost of the node
        virtual CostType   heuristicCostEstimate(const NodeKey &nodeKey) const;
        /// @brief get the neighbors of the input node, should be valid(safe for vector access), not neccessarily of unblocked
        /// @param the node want to be evaluted.  the vector of indices representing the list of neighbor nodes
        virtual void neighbors(const NodeKey &nodeKey, std::vector<NodeKey> &list);
        /// @brief whether the edge is addimissive 
        virtual bool        edgeAdm(const NodeKey &from, const NodeKey &to)                           { return true; }
        /// @brief Whether grid edge is enough/existing for routing
        /// @brief Update the node if in solving, this node's cost is reduced. In other words, its parent node change
        /// @param from: the "from" node. to: the "to" node. tentatuveGCost: the calculated g cost for to
        virtual void updateNodeParent(const NodeKey &from, const NodeKey &to, CostType tentativeGCost);
        /// @brief Evaluate gCost for the "to"
        /// @param the "from" node and "to" node
        virtual IntType evaluateGCost(const NodeKey &from, const NodeKey &to) { return node(from).gCost + distance(from, to); }
        /*------------------------------*/ 
        /* Get NodeValue                */
        /*------------------------------*/ 
        /// @brief get the NodeInfo 
        /// @param the indices of the node
        /// @return the corresponding NodeInfo
        //const NodeValue & node(const NodeKey &nodeKey) const;
        /// @brief get the NodeInfo 
        /// @param the indices of the node
        /// @return the corresponding NodeInfo
        NodeValue & node(const NodeKey &nodeKey);
        /*------------------------------*/ 
        /* Heap operations              */
        /*------------------------------*/ 
        /// @brief push a node to to open
        /// @param the NodeKey to the node
        void pushNodeToOpen(const NodeKey &nodeKey);
        /// @brief Decrease the node cost in the heap
        /// @param the NodeKey to the node
        void decreaseOpen(const NodeKey &nodeKey);
        /// @brief pop a node from the heap and find the nodeValue corresponding to it
        /// @return the interator to the node in the map
        NodeKey popNode();
        /*------------------------------*/ 
        /* Kernals                      */
        /*------------------------------*/ 
        /// @brief Process node (call isGoal, decide with push to open list, calculate cost etc. )
        void        processNode(const NodeKey &from, const NodeKey &to);
        /// @brief the core A* search kernal
        /// @return if search succeeded
        bool        pathSearchKernal();
        /// @brief back trace to get the solution
        void        traceSolution();
    /*------------------------------*/ 
    /* Member variables             */
    /*------------------------------*/ 
    //protected:
        /*------------------------------*/ 
        /* Setting of the search        */
        /*------------------------------*/ 
        Box<LocType> _searchBox = Box<LocType>(-1, -1, -1, -1); ///< The boundary of the search region
        IndexType _layerLo = INDEX_TYPE_MAX; /// The lowest layer allowed to be used
        IndexType _layerHi = INDEX_TYPE_MAX;  /// The highest layer allowed to be used
        /*------------------------------*/ 
        /* Searching step and his cost  */
        /*------------------------------*/ 
        /*------------------------------*/ 
        /* The target of path search    */
        /*------------------------------*/ 
        std::vector<NodeKey> _sources; ///< the list of search starting point
        NodeKey _goal; ///< The goal for the path search
        /*------------------------------*/ 
        /* Supporting variables         */
        /*------------------------------*/ 
        boost::heap::fibonacci_heap<NodeRef, boost::heap::compare<compareNode>> _openList; /// Open list for the discovered Nodes
        //std::vector<NodeKey> _resetNodeList; ///< The list for the nodes need to be reset
        std::vector<NodeKey> _result; ///< Store the result of the found path
        google::dense_hash_map<NodeKey, NodeValue, NodeKeyHashFunc, NodeKeyEqStr> _nodeMap; ///< the map of node. .first = NodeKey, .second = NodeValue

};


template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::addSource(const NodeKey &nodeKey)
{
    _sources.emplace_back(nodeKey);
    _nodeMap[nodeKey].isSource = true;
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::setGoal(const NodeKey &nodeKey)
{
    _goal = nodeKey;
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::traceSolution()
{
    _result.clear();
    _result.emplace_back(_goal);
    bool search = true;
    if (node(_result.back()).isSource)
    {
        search = false;
    }
    while(search)
    {
#ifdef DEBUG_ASTAR
        DBG("%s: %s \n", __FUNCTION__,  _result.back().toStr().c_str());
#endif
        auto &current = node(_result.back());
        _result.emplace_back(current.parentKey());
        if (node(_result.back()).isSource) 
        {
            search  = false;
            break;
        }
        Assert(node(_result.back()).parentLoc != XY<LocType>(-1, -1));
        Assert(node(_result.back()).parentLayer != INDEX_TYPE_MAX);
    }
    //reset();
}

template<typename NodeAffix>
inline const std::vector<NodeKey> & GridlessAStarBase<NodeAffix>::solution()
{
    return _result;
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::reset()
{
    _nodeMap.clear();
    _openList.clear();
    _sources.clear();
}

template<typename NodeAffix>
inline bool GridlessAStarBase<NodeAffix>::isValid(const NodeKey &nodeKey) const 
{
    if (nodeKey.layer < _layerLo) { return false; }
    if (nodeKey.layer > _layerHi) { return false; }
    if (nodeKey.loc.x() < _searchBox.xLo()) { return false; }
    if (nodeKey.loc.y() < _searchBox.yLo()) { return false; }
    if (nodeKey.loc.x() > _searchBox.xHi()) { return false; }
    if (nodeKey.loc.y() > _searchBox.yHi()) { return false; }
    return true;
}

template<typename NodeAffix>
inline CostType GridlessAStarBase<NodeAffix>::heuristicCostEstimate(const NodeKey &nodeKey) const 
{
    return static_cast<CostType>(::klib::manhattanDistance<LocType>(nodeKey.loc, _goal.loc)) + static_cast<CostType>(::klib::idxDif(nodeKey.layer, _goal.layer));
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::neighbors(const NodeKey &nodeKey, std::vector<NodeKey> &list)
{
    //AssertMsg(false, "%s: incomplete function. Should override this \n", __FUNCTION__);
    if (nodeKey.layer - 1 >= _layerLo) { list.emplace_back(NodeKey(nodeKey.loc, nodeKey.layer - 1)); }
    if (nodeKey.layer + 1 <= _layerHi) { list.emplace_back(NodeKey(nodeKey.loc, nodeKey.layer + 1)); }
    if (nodeKey.loc.x() - 1 >= _searchBox.xLo()) { list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x() - 1, nodeKey.loc.y() ), nodeKey.layer)); }
    if (nodeKey.loc.x() + 1 <= _searchBox.xHi()) { list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x() + 1, nodeKey.loc.y() ), nodeKey.layer)); }
    if (nodeKey.loc.y() - 1 >= _searchBox.yLo()) { list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x(), nodeKey.loc.y() - 1), nodeKey.layer)); }
    if (nodeKey.loc.y() + 1 <= _searchBox.yHi()) { list.emplace_back(NodeKey(XY<LocType>(nodeKey.loc.x(), nodeKey.loc.y() + 1), nodeKey.layer)); }
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::updateNodeParent(const NodeKey &from, const NodeKey &to, CostType tentativeGCost)
{
    // Note: gCost of each cost is initialized as INT_TYPE_MAX, so that it should be larger than any actual cost.
    // If this assumption fails, than undefined behavior.
    // Update: 2018/08/22 Disabled here
    //AssertMsg(!klib::idxAddOverflow(node(from).gCost, distance(from, to)), "%s gCost overflow! %u %u \n", __PRETTY_FUNCTION__, node(from).gCost, distance(from, to));
    if (tentativeGCost >= node(to).gCost)
    {
        return;
    }
    
    // Record this path and push to the openList
    IntType hCost = heuristicCostEstimate(to);
    // Update: 2018/08/22 Disabled
    //AssertMsg(!klib::idxAddOverflow(tentativeGCost, heuristicCostEstimate(to)), "%s hCost overflow! %u %u \n", __PRETTY_FUNCTION__, tentativeGCost, hCost);
    node(to).parentLoc = from.loc;
    node(to).parentLayer = from.layer;
    node(to).gCost = tentativeGCost;
    node(to).hCost = hCost;

    // Push/update openList
    if (node(to).isInList)
    {
        decreaseOpen(to);
    }
    else // Not in open list
    {
        pushNodeToOpen(to);
    }
}

/*
template<typename NodeAffix>
inline const typename GridlessAStarBase<NodeAffix>::NodeValue & GridlessAStarBase<NodeAffix>::node(const NodeKey &nodeKey) const
{
    return _nodeMap[nodeKey];
}
*/
template<typename NodeAffix>
inline typename GridlessAStarBase<NodeAffix>::NodeValue & GridlessAStarBase<NodeAffix>::node(const NodeKey &nodeKey)
{
    return _nodeMap[nodeKey];
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::pushNodeToOpen(const NodeKey &nodeKey)
{
    node(nodeKey).isInList = true; 
    node(nodeKey).handle = _openList.push(NodeRef(nodeKey, node(nodeKey))); 
     // Also add the node to reset
    //_resetNodeList.emplace_back(xyz); 
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::decreaseOpen(const NodeKey &nodeKey)
{
    _openList.decrease(node(nodeKey).handle, NodeRef(nodeKey, node(nodeKey)));
}

template<typename NodeAffix>
inline NodeKey GridlessAStarBase<NodeAffix>::popNode()
{
    NodeRef nodeRef = _openList.top();
    auto nodeIter = _nodeMap.find(nodeRef.nodeKey);
    nodeIter->second.isInList = false; 
    _openList.pop();
    return nodeRef.nodeKey;
}

template<typename NodeAffix>
inline void GridlessAStarBase<NodeAffix>::processNode(const NodeKey &from, const NodeKey &to)
{
    Assert(isValid(to));
#ifdef DEBUG_ASTAR
    DBG("%s: from %s to %s \n", __FUNCTION__, from.toStr().c_str(), to.toStr().c_str());
#endif
    // From and to should be verified to be valid before calling this function
    AssertMsg(isValid(from), "%s invalid \"from\" node %s \n", __PRETTY_FUNCTION__, from.toStr().c_str());
    AssertMsg(isValid(to), "%s invalid \"to\" node %s \n", __PRETTY_FUNCTION__, to.toStr().c_str());

    // Check whether "to" is in closedlist
    // Relax the condition if it is a goal
    if (!isGoal(to))
    {
        if (isClosed(to)) {
            return;
        }
    }

    // Evalute gCost
    //IntType tentativeGCost = node(from).gCost + distance(from, to);
    // Use the virtual function to replace
    IntType tentativeGCost = evaluateGCost(from, to);

    // Use the virtual function updateNodeParent to handle the change of cost and so on
    updateNodeParent(from, to, tentativeGCost);
}

template<typename NodeAffix>
inline bool GridlessAStarBase<NodeAffix>::pathSearchKernal()
{
#ifdef DEBUG_ASTAR
    DBG("%s: sources size : %d \n", __FUNCTION__,  _sources.size());
#endif
    // Check source and goal are valid nodes
    AssertMsg(_sources.size() > 0, "%s No source node. \n", __PRETTY_FUNCTION__);
    AssertMsg(isValid(_goal), "%s Goal Node %s not valid \n", __PRETTY_FUNCTION__, _goal.toStr().c_str());
    // Initialize source node cost and push it to openList
    for (const auto &source : _sources)
    {

        // THe source may not be the primary source
        //AssertMsg(isValid(source), "%s Source Node %s not valid \n", __PRETTY_FUNCTION__, source.toStr().c_str());
        //if (!isBlocked(source) && !isOccupied(source))
        if (!isBlocked(source))
        {

#ifdef DEBUG_ASTAR
            DBG("%s: source  %s \n", __FUNCTION__,  source.toStr().c_str());
#endif
            node(source).gCost = 0;
            node(source).hCost = heuristicCostEstimate(source);
            node(source).parentLoc = source.loc;
            node(source).parentLayer = source.layer;
            pushNodeToOpen(source);
        }
    }

    bool findSolution  = false;
    while(!_openList.empty())
    {
        NodeKey current = popNode();
#ifdef DEBUG_ASTAR
        DBG("%s: current %s \n", __FUNCTION__, current.toStr().c_str());
#endif
        if (isGoal(current))
        {
            findSolution = true;
            break;
        }
        if (current.layer > HIGHEST_LAYER)
        {
            continue;
        }
        
        // Add to closed list
        addToClosed(current);

        // Extract the neighbor of the current node
        // These nodes are valid but not neccessarily admissible
        std::vector<NodeKey> neighborList;
        neighbors(current, neighborList);

        for (auto &neighbor : neighborList)
        {
            if (!isValid(neighbor))
            {
                continue;
            }
            if (!edgeAdm(current, neighbor))
            {
                continue;
            }
#ifdef DEBUG_ASTAR
            DBG("%s: from %s to neighbor %s \n", __FUNCTION__, current.toStr().c_str(), neighbor.toStr().c_str());
            DBG("%s: neighbor %s %d %d  \n", __FUNCTION__, neighbor.toStr().c_str(), !isOccupied(current, neighbor), edgeAdm(current, neighbor));
#endif
            if (!isBlocked(neighbor) && !isOccupied(current, neighbor))
            {
#ifdef DEBUG_ASTAR
            DBG("%s: Process node  %s \n", __FUNCTION__, neighbor.toStr().c_str());
#endif
                processNode(current, neighbor);
            }
        } // end of neighbor : neighborList
    } // end of while(!_openList.empty())
    if (!findSolution)
    {
#ifdef DEBUG_ASTAR
        INF("%s: Didn't find solution! \n", __PRETTY_FUNCTION__);
        INF("%s:  to %s \n", __FUNCTION__, _goal.toStr().c_str());
#endif
    }
    if (findSolution)
    {
        traceSolution();
    }
    return findSolution;
}

PROJECT_NAMESPACE_END
#endif ///AROUTER_GRIDLESS_ASTARBASE_H_
