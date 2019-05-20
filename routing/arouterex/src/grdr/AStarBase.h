/**
 * @file AStarBase.h
 * @brief Base class for AStar search routing
 * Use vector<vector2D<NodeInfo>> and XYZ<uint> 
 * @author Keren Zhu
 * @date 10/08/2018
 */

#ifndef AROUTER_ASTARBASE_H_
#define AROUTER_ASTARBASE_H_

#include <boost/heap/fibonacci_heap.hpp>
#include "global/global.h"
#include "util/Vector2D.h"
#include "util/XYZ.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::AStarBase
/// @brief the base class for astar routing
/// @tparam NodeAffix: some additional things attached to each node information
template<typename NodeAffix = IntType>
class AStarBase
{
    /*------------------------------*/ 
    /* Supporting structs           */
    /*------------------------------*/ 
    protected:
        /// Forward declaration of nodes stored in vector<vector2D<NodeInfo>>
        struct NodeInfo;
        /// @brief A reference to the nodeInfo to be stored in the heap
        struct NodeRef {
            explicit NodeRef(NodeInfo &node) : nodeRef(node) {}
            NodeInfo &nodeRef;
        };
        /// @brief Conpare structor for comparing two NodeRef (used in the heap)
        struct compareNode
        {
            bool operator()(const NodeRef &n1, const NodeRef &n2) const
            {
                return (n1.nodeRef.gCost +  n1.nodeRef.hCost) > (n2.nodeRef.gCost +  n2.nodeRef.hCost);
            }
        };
        /// @brief Handle type for accessing/updating the element in the fibonacci_heap
        typedef typename boost::heap::fibonacci_heap<NodeRef, boost::heap::compare<compareNode>>::handle_type HandleType;

        /// @brief used to stored in vector<vector2D>
        struct NodeInfo
        {
            /// @breif default constructor
            explicit NodeInfo() = default;
            /// @brief constructor
            /// @param the indices of the node
            explicit NodeInfo(const XYZ<IndexType> &xyzz) : xyz(xyzz) { reset(); }
            /// @brief constructor
            /// @param the indices of the node
            explicit NodeInfo(IndexType x, IndexType y, IndexType z) : xyz(x, y, z) { reset(); }
            /// @brief reset the node
            void reset()
            {
                gCost = INT_TYPE_MAX;
                hCost = INT_TYPE_MAX;
                inList = false;
                isSource = false;
            }

            XYZ<IndexType> xyz = XYZ<IndexType>(INDEX_TYPE_MAX, INDEX_TYPE_MAX, INDEX_TYPE_MAX); ///< the indices of this node
            XYZ<IndexType> parentXYZ = XYZ<IndexType>(INDEX_TYPE_MAX, INDEX_TYPE_MAX, INDEX_TYPE_MAX); ///< the indices of the parent node of this node
            IntType gCost = INT_TYPE_MAX; ///< the current cost 
            IntType hCost = INT_TYPE_MAX; ///< the heuristic cost
            bool inList = false; ///< Whether this node is in openList
            bool isSource = false; ///< Whether this node is a source
            HandleType handle; ///< handle type to access the node in the heap
            NodeAffix affix = NodeAffix(); ///< Some additional information. Nonsense in base class. Could/should be overrieded by the derived classes
        };

    /*------------------------------*/ 
    /* Member functions             */
    /*------------------------------*/ 
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit AStarBase() = default;
        /// @brief constructor
        /// @param vector of <xSize, ySize> to represent the size of grids on each layers
        explicit AStarBase(const std::vector<XY<IndexType>> &xySizes);
        /*------------------------------*/ 
        /* Core Interface               */
        /*------------------------------*/ 
        /// @brief add source nodes
        /// @param the node want to add as source
        void addSource(const XYZ<IndexType> &node);
        /// @brief set the goal for the search
        /// @param the node want to be set as the goal
        void setGoal(const XYZ<IndexType> &node);
        /// @brief solve the searching problem
        /// @return true if successful, false if not
        bool solve() { return kernal(); }
        /// @brief get the results and re-init the solver
        /// @param the searching results in a vector of nodes
        const std::vector<XYZ<IndexType>> &solution();
        /// @brief get the cost of the path
        /// @return the cost of the searching results. If infeasiable, return INT_TYPE_MAX
        IntType costSolution() const { return node(_goal).gCost; }
    protected:
        /*------------------------------*/ 
        /* Virtual functions            */
        /*------------------------------*/ 
        /// Using virtual function for the features that should be flexible in different scenarios, and only has the simplest implementation.
        /// Other class can override these functions to implement A* search with different specifications.
        /// @brief Is given xyz a valid coordinate?
        /// @param the node want to be evaluated
        /// @return if the given node is valid
        virtual bool        isValid(const XYZ<IndexType> &xyz) const;
        /// @brief Is the node blocked?
        /// @param the node want to be evaluated
        /// @return if the given node is blocked
        virtual bool        isBlocked(const XYZ<IndexType> &xyz) const                                      { return false; }
        /// @breif Is the node occupied?
        /// @param the node want to be evaluated
        /// @return if the given node is occupied
        virtual bool        isOccupied(const XYZ<IndexType> &xyz) const                          { return false; }
        /// @brief Is destination?
        /// @param the node want to be evaluted
        /// @return if the node is the goal/target/destination
        virtual bool        isGoal(const XYZ<IndexType> &xyz)                                         { return xyz == _goal; }
        /// @brief Is in the closed list (default not using closed list)
        /// @param the node want to be evalued
        /// @return if the node is inside the closed list
        virtual bool        isClosed(const XYZ<IndexType> &xyz) const                                       { return false; }
        /// @brief Add the node to closed list
        /// @param the node want to be added to the closed list
        virtual void        addToClosed(const XYZ<IndexType> &xyz)                                          { }
        /// @brief Evaluate distance between node (used to calculate gCost)
        /// @param the two nodes from/to want to be evaluted
        /// @return the distance between these two nodes
        /// Default all the nodes has distance of 1 between any other node
        virtual IntType   distance(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const                    { return 1; }
        /// @brief calculate the heuristic cost
        /// @param the node want to be evaluted
        /// @return the heuristic cost of the node
        virtual IntType   heuristicCostEstimate(const XYZ<IndexType> &xyz) const                          { return ::klib::manhattanDistance<IndexType>(xyz, _goal); }
        /// @brief get the neighbors of the input node, should be valid(safe for vector access), not neccessarily of unblocked
        /// @param the node want to be evaluted
        /// @return the vector of indices representing the list of neighbor nodes
        virtual std::vector<XYZ<IndexType>> neighbors(const XYZ<IndexType> &xyz) const;
        /// @brief whether the edge is addimissive 
        virtual bool        edgeAdm(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const                           { return true; }
        /// @brief Whether grid edge is enough/existing for routing
        /// @brief Update the node if in solving, this node's cost is reduced. In other words, its parent node change
        /// @param from: the "from" node. to: the "to" node. tentatuveGCost: the calculated g cost for to
        virtual void updateNodeParent(const XYZ<IndexType> &from, const XYZ<IndexType> &to, IntType tentativeGCost);
        /// @brief Evaluate gCost for the "to"
        /// @param the "from" node and "to" node
        virtual IntType evaluateGCost(const XYZ<IndexType> &from, const XYZ<IndexType> &to) const { return node(from).gCost + distance(from, to); }
        /*------------------------------*/ 
        /* Init and reset               */
        /*------------------------------*/ 
        /// @brief init the nodeInfo container
        /// @param the vector of xSize and ySize representing the grid size of each layer
        void initNode3D(const std::vector<XY<IndexType>> &xySizes);
        /// @brief reset the nodes and clear the heap
        void reset();
        /*------------------------------*/ 
        /* Get NodeInfo                 */
        /*------------------------------*/ 
        /// @brief get the NodeInfo 
        /// @param the indices of the node
        /// @return the corresponding NodeInfo
        const NodeInfo & node(const XYZ<IndexType> &xyz) const { return _node3D.at(xyz.z()).at(xyz.xy()); }
        /// @brief get the NodeInfo 
        /// @param the indices of the node
        /// @return the corresponding NodeInfo
        NodeInfo & node(const XYZ<IndexType> &xyz) { return _node3D.at(xyz.z()).at(xyz.xy()); }
        /*------------------------------*/ 
        /* Kernals                      */
        /*------------------------------*/ 
        /// @brief Process node (call isGoal, decide with push to open list, calculate cost etc. )
        void        processNode(const XYZ<IndexType> &from, const XYZ<IndexType> &to);
        /// @brief the core A* search kernal
        /// @return if search succeeded
        bool        kernal();
    protected:
        /*------------------------------*/ 
        /* Heap operations              */
        /*------------------------------*/ 
        /// Push node to the heap
        void                pushNodeToOpen(const XYZ<IndexType> &xyz)    
        {
            this->node(xyz).inList = true; 
            this->node(xyz).handle = _openList.push(NodeRef(this->node(xyz))); 
             /// Also add the node to reset
            _resetNodeList.emplace_back(xyz); 
        }
        /// Update the node
        void                updateOpen(NodeInfo &node)               { _openList.update(node.handle); }
        void                updateOpen(const XYZ<IndexType> &xyz)        { updateOpen(this->node(xyz)); }
        /// Decrease the node (update with reduced key, faster than update)
        void                decreaseOpen(NodeInfo &node)             { _openList.decrease(node.handle); }
        void                decreaseOpen(const XYZ<IndexType> &xyz)      { decreaseOpen(this->node(xyz)); }
        /// Pop a node
        NodeInfo &          popNode()                            { NodeInfo &node =_openList.top().nodeRef; node.inList = false;  _openList.pop(); return node; }
        

    /*------------------------------*/ 
    /* Members                      */
    /*------------------------------*/ 
    protected:
        std::vector<Vector2D<NodeInfo>> _node3D; ///< A collection/container for the NodeInfos 
        boost::heap::fibonacci_heap<NodeRef, boost::heap::compare<compareNode>> _openList; /// Open list for the discovered Nodes
        std::vector<XYZ<IndexType>> _resetNodeList; ///< Set of changed Nodes. Used for reset
        std::set<XYZ<IndexType>> _sources; ///< The source nodes for starting the search
        XYZ<IndexType> _goal; ///< The single goal/target/destination for the search
        std::vector<XYZ<IndexType>> _result;
};


template<typename NodeAffix>
inline AStarBase<NodeAffix>::AStarBase(const std::vector<XY<IndexType>> &xySizes)
{
    initNode3D(xySizes);
}

template<typename NodeAffix>
inline void AStarBase<NodeAffix>::initNode3D(const std::vector<XY<IndexType>> &xySizes)
{
    /// The size of vector denotes the number of layers
    _node3D.resize(xySizes.size());
    /// Loop in each layers to reset each node
    for (IndexType z = 0;  z < xySizes.size(); ++z)
    {
        /// Resize each layer
        _node3D.at(z).resize(xySizes.at(z).x(), xySizes.at(z).y());
        for (IndexType x = 0; x < xySizes.at(z).x(); ++x)
        {
            for (IndexType y = 0; y < xySizes.at(z).y(); ++y)
            {
                /// init each node
                _node3D.at(z).at(x, y) = NodeInfo(x, y, z);
            }
        }
    }
}

template<typename NodeAffix>
inline void AStarBase<NodeAffix>::reset()
{
    for (auto &xyz : _resetNodeList)
    {
        node(xyz) = NodeInfo(xyz);
    }
    _resetNodeList.clear();
    _openList.clear();
    _sources.clear();
}

template<typename NodeAffix>
inline void AStarBase<NodeAffix>::addSource(const XYZ<IndexType> &node)
{
    _sources.insert(node);
    this->node(node).isSource = true;
    this->node(node).parentXYZ = node;
}

template<typename NodeAffix>
inline void AStarBase<NodeAffix>::setGoal(const XYZ<IndexType> &node)
{
    _goal = node;
}

template<typename NodeAffix>
inline bool AStarBase<NodeAffix>::isValid(const XYZ<IndexType> &xyz) const 
{
    if (xyz.z() >= _node3D.size()) { return false; }
    if (xyz.x() >= _node3D.at(xyz.z()).xSize()) { return false; }
    if (xyz.y() >= _node3D.at(xyz.z()).ySize()) { return false; }
    return true;
}

template<typename NodeAffix>
inline std::vector<XYZ<IndexType>> AStarBase<NodeAffix>::neighbors(const XYZ<IndexType> &xyz) const
{
    std::vector<XYZ<IndexType>> tempResult;
    // Up
    if (isValid(xyz.top()))
    {
        tempResult.emplace_back(xyz.top());
    }
    // Down
    if (isValid(xyz.bottom()))
    {
        tempResult.emplace_back(xyz.bottom());
    }
    // East
    if (isValid(xyz.east()))
    {
        tempResult.emplace_back(xyz.east());
    }
    // South
    if (isValid(xyz.south()))
    {
        tempResult.emplace_back(xyz.south());
    }
    // West
    if (isValid(xyz.west()))
    {
        tempResult.emplace_back(xyz.west());
    }
    // North
    if (isValid(xyz.north()))
    {
        tempResult.emplace_back(xyz.north());
    }
    return tempResult;
}

template<typename NodeAffix>
inline void AStarBase<NodeAffix>::updateNodeParent(const XYZ<IndexType> &from, const XYZ<IndexType> &to, IntType tentativeGCost)
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
    node(to).parentXYZ = from;
    node(to).gCost = tentativeGCost;
    node(to).hCost = hCost;

    // Push/update openList
    if (node(to).inList)
    {
        decreaseOpen(to);
    }
    else // Not in open list
    {
        pushNodeToOpen(to);
    }
}


/*------------------------------*/ 
/* kernals                      */
/*------------------------------*/ 

template<typename NodeAffix>
void AStarBase<NodeAffix>::processNode(const XYZ<IndexType> &from, const XYZ<IndexType> &to)
{
#ifdef DEBUG_ASTAR
    DBG("%s: from %s to %s \n", __FUNCTION__, from.toStr().c_str(), to.toStr().c_str());
#endif
    // From and to should be verified to be valid before calling this function
    AssertMsg(isValid(from), "%s invalid \"from\" node %s \n", __PRETTY_FUNCTION__, from.toStr().c_str());
    AssertMsg(isValid(to), "%s invalid \"to\" node %s \n", __PRETTY_FUNCTION__, to.toStr().c_str());

    // Check whether "to" is in closedlist
    if (isClosed(to)) {
        return;
    }

    // Evalute gCost
    //IntType tentativeGCost = node(from).gCost + distance(from, to);
    // Use the virtual function to replace
    IntType tentativeGCost = evaluateGCost(from, to);

    // Use the virtual function updateNodeParent to handle the change of cost and so on
    updateNodeParent(from, to, tentativeGCost);
}

template<typename NodeAffix>
bool AStarBase<NodeAffix>::kernal()
{
    // Check source and goal are valid nodes
    AssertMsg(_sources.size() > 0, "%s No source node. \n", __PRETTY_FUNCTION__);
    AssertMsg(isValid(_goal), "%s Goal Node %s not valid \n", __PRETTY_FUNCTION__, _goal.toStr().c_str());
    // Initialize source node cost and push it to openList
    for (const auto &source : _sources)
    {
         
        AssertMsg(isValid(source), "%s Source Node %s not valid \n", __PRETTY_FUNCTION__, source.toStr().c_str());
        //if (!isBlocked(source) && !isOccupied(source))
        if (1)
        {
            node(source).gCost = 0;
            node(source).hCost = heuristicCostEstimate(source);
            node(source).parentXYZ = source;
            pushNodeToOpen(source);
        }
    }

    bool findSolution  = false;
    while(!_openList.empty())
    {
        XYZ<IndexType> &current = popNode().xyz;
        if (isGoal(current))
        {
            findSolution = true;
            break;
        }
        
        // Add to closed list
        addToClosed(current);

        // Extract the neighbor of the current node
        // These nodes are valid but not neccessarily admissible
        const std::vector<XYZ<IndexType>> &neighborList = neighbors(current);

        for (auto &neighbor : neighborList)
        {
            // It is possible that the cell is blocked but is goal
            if (isGoal(neighbor) &&  edgeAdm(current, neighbor) && ::klib::absDif<IndexType>(neighbor.z(), current.z()) == 1)
            {
                processNode(current, neighbor);
            }
            if (!isBlocked(neighbor) && !isOccupied(neighbor) && edgeAdm(current, neighbor))
            {
                processNode(current, neighbor);
            }
        } // end of neighbor : neighborList
        if (findSolution)
        {
            break;
        }
    } // end of while(!_openList.empty())
    if (!findSolution)
    {
#ifdef DEBUG_ASTAR
        INF("%s: Didn't find solution! \n", __PRETTY_FUNCTION__);
        INF("%s:  to %s \n", __FUNCTION__, _goal.toStr().c_str());
#endif
    }
    return findSolution;
}

template<typename NodeAffix>
inline const std::vector<XYZ<IndexType>> &AStarBase<NodeAffix>::solution()
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
        auto &current = node(_result.back());
        _result.emplace_back(current.parentXYZ);
        if (node(_result.back()).isSource) 
        {
            search  = false;
            break;
        }
        Assert(node(_result.back()).parentXYZ != XYZ<IndexType>(INDEX_TYPE_MAX, INDEX_TYPE_MAX, INDEX_TYPE_MAX));
    }

    reset();
    return _result;
}
PROJECT_NAMESPACE_END
#endif ///AROUTER_ASTARBASE_H_
