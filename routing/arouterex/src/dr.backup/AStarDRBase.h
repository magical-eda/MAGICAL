r/
// Created by Keren on 2018/08/21
//

#ifndef AROUTER_ASTARDRBASE_H_
#define AROUTER_ASTARDRBASE_H_


#include <boost/heap/fibonacci_heap.hpp>
#include "global/global.h"

/// ================================================================================ 
///  AstarDRBase.h
///
/// A similar A-star solver base class to shallowrouter/gr/AstarGRBase
/// It use an adjacent list for graph instead of built-in vector3D  
/// std::map<NodeIdentifier, std::set<NodeIdentifier>> _listGraph
///
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

/// NodeIdentyType is for identifying the node. A common (default) choice is XYZ
/// NodeAffix is the additional information that Attached to the NodeInfo 
template<typename NodeIdentyType, typename NodeAffix = IntType>
class AStarDRBase
{
    /////////////////////////
    /// Supporting Structs 
    /////////////////////////
    public:
        class NodeInfo;
        /// Used to stored in the heap
        struct NodeRef
        {
            //explicit NodeRef(NodeIdentyType node, const std::map<NodeIdentyType, std::set<NodeInfo>> &augListGraph) : nodeIdentyRef(node), listGraphRef(augListGraph) {}
            NodeRef(NodeIdentyType node, const std::map<NodeIdentyType, NodeInfo> &augListGraph);


            bool operator< (const NodeRef &rhs) const { return nodeIdentyRef < rhs.nodeIdentyRef; }
            NodeIdentyType nodeIdentyRef;
            const std::map<NodeIdentyType, NodeInfo> &listGraphRef;
        };

        /// Used to rank nodes in the heap
        struct compareNode
        {
            bool operator()(const NodeRef &n1, const NodeRef &n2) const
            {
                IntType gCost1 = n1.listGraphRef.find(n1.nodeIdentyRef)->second.gCost;
                IntType hCost1 = n1.listGraphRef.find(n1.nodeIdentyRef)->second.hCost;
                IntType gCost2 = n2.listGraphRef.find(n2.nodeIdentyRef)->second.gCost;
                IntType hCost2 = n2.listGraphRef.find(n2.nodeIdentyRef)->second.hCost;
                return (gCost1 + hCost1) > (gCost2 + hCost2);
                //return (n1.listGraphRef[n1.nodeIdentyRef].gCost +  n1.listGraphRef[n1.nodeIdentyRef].hCost) > (n2.listGraphRef[n2.nodeIdentyRef].gCost +  n2.listGraphRef[n2.nodeIentyRef].hCost);
            }
        };

        /// Handle type for accessing/updating the element in the fibonacci_heap
        typedef typename boost::heap::fibonacci_heap<NodeRef, boost::heap::compare<compareNode>>::handle_type HandleType;
        
        class NodeInfo
        {
            public:
                explicit NodeInfo() = default;
                
                void reset()
                {
                    gCost = INT_TYPE_MAX;
                    hCost = INT_TYPE_MAX;
                    inList = false;
                    isSource = false;
                }

                //NodeIdentyType current = NULL;
                NodeIdentyType parent = NodeIdentyType();
                IntType gCost = INT_TYPE_MAX;
                IntType hCost = INT_TYPE_MAX;
                /// Whether this node is in openList
                bool inList = false;
                /// Whether this node is a source
                bool isSource = false;
                HandleType handle;
                /// Additional information from the external (derived) class
                NodeAffix affix;
        };


        //////////////////////
        /// Class functions
        //////////////////////
    public:
        ///////////////////
        /// Constructor
        ///////////////////
        explicit AStarDRBase(const std::map<NodeIdentyType, std::set<NodeIdentyType>> &listGraph) : _listGraph(listGraph) { 
#ifndef USE_XYZ_NODEIDEN
            initAugListGraph(listGraph); 
#endif
        }


        //////////////////
        /// Init
        //////////////////
        void initAugListGraph(const std::map<NodeIdentyType, std::set<NodeIdentyType>> &listGraph);

        //////////////////
        /// Reset
        //////////////////
        /// Reset the changed Node in _augListGraph and clear the heap
        void reset();

        //////////////////
        /// Getters
        //////////////////
        const NodeInfo &                        node(const NodeIdentyType &nodeIdenty) const                                { return _augListGraph.at(nodeIdenty); } 
        NodeInfo &                              node(const NodeIdentyType &nodeIdenty)                                      { return _augListGraph.at(nodeIdenty); } 
        const std::vector<NodeIdentyType> &     results() const                                                             { return _results; }
        std::vector<NodeIdentyType> &           results()                                                                   { return _results; }
        
        //////////////////
        /// Setters
        //////////////////
        /// Add a node as a source
        void addSource(NodeIdentyType nodeIdenty);
        void setGoal(NodeIdentyType nodeIdenty) { AssertMsg(_augListGraph.find(nodeIdenty) != _augListGraph.end(), "%s: node not found in graph , graph size %u \n", __PRETTY_FUNCTION__, _augListGraph.size()); _goal = nodeIdenty; }

        ////////////////////
        /// Heap operation
        ////////////////////
        /// Push node to heap
        void pushNodeToOpen(NodeIdentyType nodeIdenty)
        {
            NodeInfo &nodeInfo = _augListGraph[nodeIdenty];
            nodeInfo.inList = true;
            nodeInfo.handle = _openList.push(NodeRef(nodeIdenty, _augListGraph));
            /// Also add to resetList
            _resetNodeList.emplace_back(nodeIdenty);
        }
        /// Update the node (Update the node in heap with changed cost)
        void updateOpen(NodeInfo &nodeInfo) { _openList.update(nodeInfo.handle); }
        void updateOpen(NodeIdentyType &nodeIdenty) { updateOpen(_augListGraph[nodeIdenty]); }
        /// Decrease the node (update with reduced key, faster than update)
        void decreaseOpen(const NodeInfo &nodeInfo) { _openList.decrease(nodeInfo.handle); }
        void decreaseOpen(NodeIdentyType nodeIdenty) { decreaseOpen(_augListGraph[nodeIdenty]); }
        /// Pop a node
        NodeIdentyType popNode() { NodeIdentyType nodeIdenty = _openList.top().nodeIdentyRef; _augListGraph[nodeIdenty].inList = false; _openList.pop(); return nodeIdenty; }
        

        /////////////////////////
        /// Virtual functions///
        ///////////////////////
        /// Using virtual function for the features that should be flexible in different scenarios, and only has the simplest implementation.
        /// Other class can override these functions to implement A* search with different specifications.
        /// Is given xyz a valid coordinate?
        virtual bool        isValid(const NodeIdentyType &nodeIdenty) const                             { return true; }
        /// Is the node blocked?
        virtual bool        isBlocked(const NodeIdentyType &nodeIdenty) const                           { return false; }
        /// Is the node occupied?
        virtual bool        isOccupied(const NodeIdentyType &nodeIdenty) const                          { return false; }
        /// Is destination?
        virtual bool        isGoal(const NodeIdentyType &nodeIdenty) const                              { return nodeIdenty == _goal; }
        /// Is in the closed list (default not using closed list)
        virtual bool        isClosed(const NodeIdentyType &nodeIdenty) const                            { return false; }
        /// Add the node to closed list
        virtual void        addToClosed(NodeIdentyType &nodeIdenty)                                     { }
        /// Evaluate distance between node (used to calculate gCost)
        /// Default all the nodes has distance of 1 between any other node
        virtual IntType   distance(const NodeIdentyType &nodeFrom, const NodeIdentyType &nodeTo) const  { return 1; }
        /// Heuristic cost
        virtual IntType   heuristicCostEstimate(const NodeIdentyType &nodeIdenty) const                 { return 1; }
        /// The neighbors, should be valid(safe for vector access), not neccessarily of unblocked
        virtual const std::set<NodeIdentyType> &neighbors(const NodeIdentyType &nodeIdenty) const;
        /// Whether grid edge is enough/existing for routing
        virtual bool        edgeAdm(const NodeIdentyType &nodeFrom, const NodeIdentyType &nodeTo) const { return true; }
        /// Update the node if in solving, this node's cost is reduced. In other words, its parent node change
        virtual void updateNodeParent(const NodeIdentyType &from, const NodeIdentyType &to, IntType tentativeGCost);
        /// Evaluate gCost
        virtual IntType evaluateGCost(const NodeIdentyType &from, const NodeIdentyType &to) const { return node(from).gCost + distance(from, to); }


        /////////////////////
        /// Core fuctions///
        ///////////////////
        /// Process node (call isGoal, decide with push to open list, calculate cost etc. )
        /// return true to terminate the search
        void        processNode(const NodeIdentyType &from, const NodeIdentyType &to);
        /// Solve the search, return false if the seach fails
        bool        solve();
        /// Retrace the result and clear the data
        void  retraceResult();
        /// Get the cost 
        IndexType costResult() { return node(_goal).gCost; }
        
    protected:
        //////////////////
        /// Members
        //////////////////
        /// A reference to the original listGraph
        const std::map<NodeIdentyType, std::set<NodeIdentyType>>                              &_listGraph;
        /// Collection of the nodes, in a augmented std::map/listGraph
        std::map<NodeIdentyType, NodeInfo>                                              _augListGraph;
        /// Open list for discovered nodes
        boost::heap::fibonacci_heap<NodeRef, boost::heap::compare<compareNode>>         _openList;
        /// Set of changed Nodes. Being used for reset
        std::vector<NodeIdentyType>                                                     _resetNodeList;
        /// Source of the search
        std::vector<NodeIdentyType>                                                     _source;
        /// Destination/goal
        NodeIdentyType                                                                  _goal;
        /// The searching results from last solve()
        std::vector<NodeIdentyType>                                                     _results;

#ifdef DEBUG_ASTAR
    private:
        std::string node2Str(NodeIdentyType node)
        {
            /// divided by ySize (# of rows = dieHeight / trackWidth) 
            std::ostringstream oss;
            oss << ")";
            return oss.str();
        }
#endif
};

template<typename NodeIdentyType, typename NodeAffix>
AStarDRBase<NodeIdentyType, NodeAffix>::NodeRef::NodeRef(NodeIdentyType node, const std::map<NodeIdentyType, NodeInfo> &augListGraph)
    : nodeIdentyRef(node), listGraphRef(augListGraph) {}

template<typename NodeIdentyType, typename NodeAffix>
void AStarDRBase<NodeIdentyType, NodeAffix>::addSource(NodeIdentyType nodeIdenty)
{
    auto nodeIter = _augListGraph.find(nodeIdenty);
    if (nodeIter == _augListGraph.end())
    {
        AssertMsg(false, "%s: node not found in the graph \n", __PRETTY_FUNCTION__);
    }
    else
    {
        nodeIter->second.isSource = true;
        nodeIter->second.parent = nodeIdenty;
        _source.emplace_back(nodeIdenty);
    }
}


template<typename NodeIdentyType, typename NodeAffix>
inline const std::set<NodeIdentyType> & AStarDRBase<NodeIdentyType, NodeAffix>::neighbors(const NodeIdentyType &nodeIdenty) const 
{
    /*
    std::set<NodeIdentyType> neighbor;
    neighbor = _listGraph.find(nodeIdenty)->second;
    return neighbor;
    */
    return _listGraph.find(nodeIdenty)->second;
}

template<typename NodeIdentyType, typename NodeAffix>
inline void AStarDRBase<NodeIdentyType, NodeAffix>::updateNodeParent(const NodeIdentyType &from, const NodeIdentyType &to, IntType tentativeGCost)
{
    /// Note: gCost of each cost is initialized as INT_TYPE_MAX, so that it should be larger than any actual cost.
    /// If this assumption fails, than undefined behavior.
    /// Update: 2018/08/22 Disabled here
    //AssertMsg(!klib::idxAddOverflow(node(from).gCost, distance(from, to)), "%s gCost overflow! %u %u \n", __PRETTY_FUNCTION__, node(from).gCost, distance(from, to));
    if (tentativeGCost >= node(to).gCost)
    {
        return;
    }
    
    /// Record this path and push to the openList
    IntType hCost = heuristicCostEstimate(to);
    /// Update: 2018/08/22 Disabled
    //AssertMsg(!klib::idxAddOverflow(tentativeGCost, heuristicCostEstimate(to)), "%s hCost overflow! %u %u \n", __PRETTY_FUNCTION__, tentativeGCost, hCost);
    node(to).parent = from;
    node(to).gCost = tentativeGCost;
    node(to).hCost = hCost;

    /// Push/update openList
    if (node(to).inList)
    {
        decreaseOpen(to);
    }
    else /// Not in open list
    {
        pushNodeToOpen(to);
    }
}

template<typename NodeIdentyType, typename NodeAffix>
inline void AStarDRBase<NodeIdentyType, NodeAffix>::reset()
{
    for (auto &nodeIdenty : _resetNodeList)
    {
        node(nodeIdenty) = NodeInfo();
    }
    _resetNodeList.clear();
    _openList.clear();
    _source.resize(0);
}

template<typename NodeIdentyType, typename NodeAffix>
inline void AStarDRBase<NodeIdentyType, NodeAffix>::retraceResult()
{
    _results.clear();
    _results.emplace_back(_goal);
    bool search = true;
    if (node(_results.back()).isSource)
    {
        search = false;
    }
    while(search)
    {
        auto &current = node(_results.back());
        _results.emplace_back(current.parent);
        if (node(_results.back()).isSource) 
        {
            search  = false;
            break;
        }
    }
    reset();
}




template<typename NodeIdentyType, typename NodeAffix>
inline void AStarDRBase<NodeIdentyType, NodeAffix>::initAugListGraph(const std::map<NodeIdentyType, std::set<NodeIdentyType>> &listGraph)
{
    //Assert(0);
    //_listGraph = listGraph;
    _augListGraph.clear();
    for (auto &edgeList : listGraph)
    {
        auto &nodeIden = edgeList.first;
        //auto &nodeIdenSet = edgeList.second;
        //NodeInfo nodeInfo = NodeInfo();
        //_augListGraph[nodeIden] = nodeInfo;
        _augListGraph[nodeIden] = NodeInfo();
    }
}


template<typename NodeIdentyType, typename NodeAffix>
inline void AStarDRBase<NodeIdentyType, NodeAffix>::processNode(const NodeIdentyType &from, const NodeIdentyType &to)
{
#ifdef DEBUG_ASTAR
    DBG("%s: Processing %s %s \n", __FUNCTION__, node2Str(from).c_str(), node2Str(to).c_str());
    DBG("From Node gCost %d hCost %d \n", node(from).gCost, node(from).hCost);
#endif
    /// From and to should be verified to be valid before calling this function
    AssertMsg(isValid(from), "%s invalid \"from\" node %s \n", __PRETTY_FUNCTION__, from.toStr().c_str());
    AssertMsg(isValid(to), "%s invalid \"to\" node %s \n", __PRETTY_FUNCTION__, to.toStr().c_str());

    /// Check whether "to" is in closedlist
    if (isClosed(to)) {
        return;
    }

    /// Evalute gCost
    //IntType tentativeGCost = node(from).gCost + distance(from, to);
    /// Use the virtual function to replace
    IntType tentativeGCost = evaluateGCost(from, to);



    /// Use the virtual function updateNodeParent to handle the change of cost and so on
    updateNodeParent(from, to, tentativeGCost);
}


template<typename NodeIdentyType, typename NodeAffix>
inline bool AStarDRBase<NodeIdentyType, NodeAffix>::solve()
{
#ifdef DEBUG_ASTAR
    DBG("%s: From %s to %s \n", __FUNCTION__, node2Str(_source[0]).c_str(), node2Str(_goal).c_str());
#endif
    /// Check source and goal are valid nodes
    AssertMsg(_source.size() > 0, "%s No source node. \n", __PRETTY_FUNCTION__);
    AssertMsg(isValid(_goal), "%s Goal Node %s not valid \n", __PRETTY_FUNCTION__, _goal.toStr().c_str());
    /// Initialize source node cost and push it to openList
    for (const auto &source : _source)
    {
         
        AssertMsg(isValid(source), "%s Source Node %s not valid \n", __PRETTY_FUNCTION__, source.toStr().c_str());
        node(source).gCost = 0;
        node(source).hCost = heuristicCostEstimate(source);
        node(source).parent = source;
        pushNodeToOpen(source);
    }

    bool findSolution  = false;
    while(!_openList.empty())
    {
        NodeIdentyType current = popNode();
        if (isGoal(current))
        {
            findSolution = true;
            break;
        }
        
        /// Add to closed list
        addToClosed(current);

        /// Extract the neighbor of the current node
        /// These nodes are valid but not neccessarily admissible
        const std::set<NodeIdentyType> &neighborList = neighbors(current);

        for (auto &neighbor : neighborList)
        {
            if (!isBlocked(neighbor) && !isOccupied(neighbor) && edgeAdm(current, neighbor))
            {
                processNode(current, neighbor);
            }
        } /// end of neighbor : neighborList
    } /// end of while(!_openList.empty())
    if (!findSolution)
    {
        //INF("%s: Didn't find solution!  \n", __FUNCTION__);
        _results.clear();
    }
    else 
    {
        retraceResult();
    }
    return findSolution;
}


PROJECT_NAMESPACE_END

#endif ///AROUTER_ASTARDRBASE_H_
