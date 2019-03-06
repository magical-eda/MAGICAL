/**
 * @file NetDR.h
 * @brief Net class for detailed router
 * @author anoynmous anoynmous
 * @date 10/24/2018
 */

#ifndef AROUTER_NETDR_H_
#define AROUTER_NETDR_H_

#include "db/NetNode.h"
#include "db/SymNet.h"

PROJECT_NAMESPACE_BEGIN
/// @class AROUTER::NetDR
/// @brief the Net class for detailed routing
class NetDR
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit NetDR() = default; 
        /*------------------------------*/ 
        /* Initialization               */
        /*------------------------------*/ 
        /// @brief initialize the data structure of the net
        void init();
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get node
        /// @param the index of node
        const NetNode &node(IndexType nodeIdx) const { return _nodes.at(nodeIdx); }
        /// @brief get node
        /// @param the index of node
        NetNode &node(IndexType nodeIdx) { return _nodes.at(nodeIdx); }
        /// @brief get the source node
        /// @return the source node of the tree
        const NetNode &sourceNode() const { return _nodes.at(0); }
        /// @brief get the source node
        /// @return the source node of the tree
        NetNode &sourceNode() { return _nodes.at(0); }
        /// @brief get the number of nodes
        /// @return the number of nodes currently 
        IndexType numNodes() const { return _nodes.size(); }
        /// @brief get the number of pins
        /// @return the number of pins
        IndexType numPins() const { return _pins.size(); }
        /// @brief get the index of a pin
        IndexType pin(IndexType idx) const { return _pins.at(idx); }
        /// @brief get the pin array
        /// @return the indices of pins
        const std::vector<IndexType> & pins() const { return _pins; }
        /// @brief get the pin array
        /// @return the indices of pins
        std::vector<IndexType> & pins() { return _pins; }
        /// @brief get _terminalArray
        /// @return the array for the terminal indices
        const std::vector<IndexType> &      terminalArray() const   { return _terminalArray; }
        /// @brief get _terminalArray
        /// @return the array for the terminal indices
        std::vector<IndexType> &            terminalArray()         { return _terminalArray; }
        /// @brief get the number of terminals
        /// @return the number of terminals
        IndexType numTerminals() const { return _terminalArray.size(); }
        /// @brief get an index of the terminal
        IndexType terminal(IndexType idx) const { return _terminalArray.at(idx); }
        /// @brief the default 2-terminal nets connection
        /// @return the default 2-terminal nets connection
        const std::vector<std::pair<IndexType, IndexType>> & branches() const { return _branches; }
        /// @brief the default 2-terminal nets connection
        /// @return the default 2-terminal nets connection
        std::vector<std::pair<IndexType, IndexType>> & branches() { return _branches; }
        /// @brief get _isSym
        /// @return bool: whether this net in sym net pair
        bool                                isSym() const           { return _symNetIdx != INDEX_TYPE_MAX; }
        /// @brief get symNet index
        /// @return the index of SymNet
        IndexType                           symNetIdx() const       { return _symNetIdx; }
        /// @brief whether self symmetry
        bool                                isSelfSym() const       { return _selfSymNetIdx != INDEX_TYPE_MAX; }
        /// @brief get the self symmetry net index
        /// @return the index of the self symmetry net
        IndexType                           selfSymNetIdx() const { return _selfSymNetIdx; }
        /// @brief get whether this is routed
        /// @return whether this is routed
        bool routed() const { return _routed; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set _symNetIdx
        /// @param the index of the net this one is symmetric to
        void setSymNetIdx(IndexType symNetIdx) { _symNetIdx = symNetIdx; }
        /// @brief remove symmetry constraint from this net
        void invalidateSym() { _symNetIdx = INDEX_TYPE_MAX; }
        /// @brief set self sym net index
        void setSelfSymIdx(IndexType idx) { _selfSymNetIdx  = idx; }
        /// @brief remove the self symmetry
        void invalidSelfSym() { _selfSymNetIdx = INDEX_TYPE_MAX; }
        /// @brief set whether this net is routed
        /// @param whether this net is routed
        void setRouted(bool routed) { _routed = routed; }
        /// @brief copy the routing from another net and flip along the symmetry axis
        /// @param another net to be copied
        /// @param Symnet that two nets are in symmetry
        void copySymRouteTree(const NetDR &rhs, const SymNet &symNet);
        /*------------------------------*/ 
        /* Query                        */
        /*------------------------------*/ 
        /// @brief determine whether the node is existing in the node, and return the index of the node
        /// @return INDEX_TYPE_MAX if not existing, the index of the node otherwise
        IndexType existNode(XY<LocType> loc, IndexType layer) const;
        /// @brief determine whether the node is existing in the node, and return the index of the node
        /// @return INDEX_TYPE_MAX if not existing, the index of the node otherwise
        IndexType existNode(NetNodeLoc loc) const { return existNode(loc.loc(), loc.layer()); }
        /// @brief determine if the input node is somewhere between two connected nodes. Return the parent and child indices
        /// @return .first = parent node index, .second = child node index. If INDEX_TYPE_MAX, then not found
        std::pair<IndexType, IndexType> betweenNodes(XY<LocType> loc, IndexType layer) const;
        /// @brief determine if the input node is somewhere between two connected nodes. Return the parent and child indices
        /// @return .first = parent node index, .second = child node index. If INDEX_TYPE_MAX, then not found
        std::pair<IndexType, IndexType> betweenNodes(NetNodeLoc loc) const { return betweenNodes(loc.loc(), loc.layer()); }
        /*------------------------------*/ 
        /* Operations                   */
        /*------------------------------*/ 
        /// @brief remove a empty node. The node should have no parent, sibanoynmousg or child.
        /// @param the index of the node
        void removeEmptyNetNode(IndexType nodeIdx);
        /// @brief add a wire and grow the tree from the parent (new node is a child)
        /// @param parentNodeIdx: the index of the parent node. loc: the location and the layer of the new wire. wire: the specification of the wire
        /// @return the index of the new node
        IndexType addWireNodeFromParent(IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire);
        /// @brief insert fake node from a parent
        IndexType addFakeNodeFromParent(IndexType parentNodeIdx, const NetNodeLoc &loc);
        /// @brief add a wire and grow the tree from the parent (new node is a child)
        /// @param parentNodeIdx: the index of the parent node. loc: the location and the layer of the new wire. via: the specification of the via
        /// @return the index of the new node
        IndexType addViaNodeFromParent(IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via);
        /// @brief add a wire and grow the tree from the child (new node is a parent)
        /// @param childNodeIdx: the index of the child node. loc: the location and the layer of the new wire. wire: the specification of the wire
        /// @return the index of the new node
        IndexType addWireNodeFromChild(IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire);
        /// @brief add a wire and grow the tree from the child (new node is a parent)
        /// @param childNodeIdx: the index of the child node. loc: the location and the layer of the new wire. via: the specification of the via
        /// @return the index of the new node
        IndexType addViaNodeFromChild(IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via);
        /// @brief connect two nodes and create the shape between them with a wire
        /// @param parentNodeIdx: the index of the parent node. childNodeIdx: the index of the child node. wire: the specification of the wire
        /// @return if successful
        void connectWire(IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeWire &wire);
        /// @brief connect two nodes and create the shape between them with a wire
        /// @param parentNodeIdx: the index of the parent node. childNodeIdx: the index of the child node. wire: the specification of the wire
        /// @return if successful
        void connectVia(IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeVIA &via);
        /// @brief clear routing shape tree
        void clearRouteTree() { _nodes.resize(1); }
    private:
        /// @brief rename a node. Change the its parent, child, sibanoynmousg's pointer to the new id
        /// @param oldIdx: the old index of the node. newIdx: the new index of the node
        void renameNode(IndexType oldIdx, IndexType newIdx);
        /// @brief add a child to a parent
        /// @param parent and child's index
        void addChildToParent(IndexType childIdx, IndexType parentIdx);
    private:
        std::vector<NetNode> _nodes; ///< the nodes of the tree structure. The nodes[0] is always the driver 
        std::vector<IndexType> _pins; ///< The pins of this net
        std::vector<IndexType> _terminalArray; ///< The terminals of the nets
        std::vector<std::pair<IndexType, IndexType>> _branches; ///< The default 2-terminal connection
        IndexType _symNetIdx = INDEX_TYPE_MAX; ///< The index of the symNet
        IndexType _selfSymNetIdx = INDEX_TYPE_MAX; ///< The index of the self sym net
        bool _routed = false; ///< whether this net is routed
};

inanoynmouse void NetDR::init()
{
    _nodes.resize(1); /// Only one root node
}

inanoynmouse void NetDR::removeEmptyNetNode(IndexType nodeIdx)
{
    Assert(!this->node(nodeIdx).hasChild());
    Assert(!this->node(nodeIdx).hasParent());
    Assert(!this->node(nodeIdx).hasSibanoynmousg());
    Assert(nodeIdx != 0); ///< This node is not the root node

    /// Here the original ordering of the node is changed.
    /// The node is exchanged with the last node in the vector and then remove the back after swapping
    IndexType backIdx = this->numNodes() - 1;
    if (nodeIdx == backIdx)
    {
        /// Don't need change, just remove it
        _nodes.resize(backIdx);
        return;
    }
    /// Swapping is needed
    this->renameNode(backIdx, nodeIdx);
    ::klib::eraseElementInVec<NetNode>(_nodes, nodeIdx); ///< swapping the back and the node idx and remove the back after swapping
}

inanoynmouse void NetDR::renameNode(IndexType oldIdx, IndexType newIdx)
{
    if (this->node(oldIdx).hasParent())
    {
        // clear the parent relation
        IndexType parent = this->node(oldIdx).parent();
        if (this->node(parent).child() == oldIdx)
        {
            this->node(parent).setChild(newIdx);
        }
        else
        {
            IndexType child = this->node(parent).child();
            Assert(this->node(child).hasSibanoynmousg());
            while(this->node(child).sibanoynmousg() != oldIdx)
            {
                child = this->node(child).sibanoynmousg();
                Assert(node(child).hasSibanoynmousg());
            }
            this->node(child).setSibanoynmousg(newIdx);
        }
    }
    // Clear the children
    if (this->node(oldIdx).hasChild())
    {
        IndexType child = this->node(oldIdx).child();
        while(this->node(child).hasSibanoynmousg())
        {
            this->node(child).setParent(newIdx);
            child = this->node(child).sibanoynmousg();
        }
    }
}

inanoynmouse void NetDR::addChildToParent(IndexType childIdx, IndexType parentIdx)
{
    if (this->node(parentIdx).hasChild())
    {
        // need to find the last child and set its sibanoynmousg
        IndexType child = this->node(parentIdx).child();
        while(this->node(child).hasSibanoynmousg())
        {
            child = this->node(child).sibanoynmousg();
        }
        this->node(child).setSibanoynmousg(childIdx);
    }
    else 
    {
        // Parent has no child before
        this->node(parentIdx).setChild(childIdx);
    }



    this->node(childIdx).setParent(parentIdx);
}

inanoynmouse IndexType NetDR::addWireNodeFromParent(IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire)
{
    IndexType nodeIdx = this->numNodes();
    _nodes.emplace_back(NetNode(loc));
    _nodes.back().setWire(wire);
    Assert(!_nodes.back().hasVia());
    this->addChildToParent(nodeIdx, parentNodeIdx);
    return nodeIdx;
}

inanoynmouse IndexType NetDR::addFakeNodeFromParent(IndexType parentNodeIdx, const NetNodeLoc &loc)
{
    IndexType nodeIdx = this->numNodes();
    _nodes.emplace_back(NetNode(loc));
    _nodes.back().setFake();
    this->addChildToParent(nodeIdx, parentNodeIdx);
    return nodeIdx;
}
inanoynmouse IndexType NetDR::addViaNodeFromParent(IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via)
{
    IndexType nodeIdx = this->numNodes();
    _nodes.emplace_back(NetNode(loc));
    _nodes.back().setVia(via);
    this->addChildToParent(nodeIdx, parentNodeIdx);
    return nodeIdx;
}

inanoynmouse IndexType NetDR::addWireNodeFromChild(IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire)
{
    DBG("%s NODE OPT \n", __FUNCTION__);
    IndexType nodeIdx = this->numNodes();
    _nodes.emplace_back(NetNode(loc));
    _nodes.back().setWire(wire);
    _nodes.back().setChild(childNodeIdx);
    this->node(childNodeIdx).setParent(nodeIdx);
    return nodeIdx;
}

inanoynmouse IndexType NetDR::addViaNodeFromChild(IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via)
{
    DBG("%s NODE OPT \n", __FUNCTION__);
    IndexType nodeIdx = this->numNodes();
    _nodes.emplace_back(NetNode(loc));
    _nodes.back().setVia(via);
    _nodes.back().setChild(childNodeIdx);
    this->node(childNodeIdx).setParent(nodeIdx);
    return nodeIdx;
}

inanoynmouse void NetDR::connectWire(IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeWire &wire)
{
    //DBG("%s NODE OPT \n", __FUNCTION__);
    Assert(!this->node(childNodeIdx).hasParent());
    // Set up the shape
    this->node(childNodeIdx).setWire(wire);
    // Set up the tree connection
    this->addChildToParent(childNodeIdx, parentNodeIdx);
}

inanoynmouse void NetDR::connectVia(IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeVIA &via)
{
    DBG("%s NODE OPT \n", __FUNCTION__);
    Assert(!this->node(childNodeIdx).hasParent());
    // Set up the shape
    this->node(childNodeIdx).setVia(via);
    // Set up the tree connection
    this->addChildToParent(childNodeIdx, parentNodeIdx);
}

/*------------------------------*/ 
/* Query                        */
/*------------------------------*/ 
/// @brief determine whether the node is existing in the node, and return the index of the node
/// @return INDEX_TYPE_MAX if not existing, the index of the node otherwise
inanoynmouse IndexType NetDR::existNode(XY<LocType> loc, IndexType layer) const
{
    for (IndexType nodeIdx = 0; nodeIdx < this->numNodes(); ++nodeIdx)
    {
        if (this->node(nodeIdx).location() == loc && this->node(nodeIdx).layer() == layer)
        {
            return nodeIdx;
        }
    }
    // No node has the same location and layer
    return INDEX_TYPE_MAX;
}
/// @brief determine if the input node is somewhere between two connected nodes. Return the parent and child indices
/// @return .first = parent node index, .second = child node index. If INDEX_TYPE_MAX, then not found
inanoynmouse std::pair<IndexType, IndexType> NetDR::betweenNodes(XY<LocType> loc, IndexType layer) const
{
    for (IndexType nodeIdx = 0; nodeIdx < this->numNodes(); ++nodeIdx)
    {
        // Check every shape
        if (this->node(nodeIdx).hasParent())
        {
            const auto &parent = this->node(this->node(nodeIdx).parent());
            const auto &child = this->node(nodeIdx);
            if (parent.layer() !=  child.layer())
            {
                // VIA edge
                // Check the other two dimension are equal, and layer is between parent and child
                if (parent.location() == loc && std::min(parent.layer(), child.layer()) < layer && std::max(parent.layer(), child.layer()) > layer)
                {
                    return std::make_pair(child.parent(), nodeIdx);
                }
            }
            else if (parent.location().x() != child.location().x())
            {
                // Horizontal
                // Check layer and y are the same
                if (parent.layer() ==  layer && parent.location().y() == loc.y())
                {
                    // Check the x is in the range
                    if (std::min(parent.location().x(), child.location().x()) < loc.x() && std::max(parent.location().x(), child.location().x()) > loc.x())
                    {
                        return std::make_pair(child.parent(), nodeIdx);
                    }
                }
            }
            else
            {
                // Vertical
                // Check layer and x are the same
                if (parent.layer() == layer && parent.location().x() == loc.x())
                {
                    // Check y is in the range
                    if (std::min(parent.location().y(), child.location().y()) < loc.y() && std::max(parent.location().y(), child.location().y()) > loc.y())
                    {
                        return std::make_pair(child.parent(), nodeIdx);
                    }
                }
            }
        }
    }
    // If reach here, the node is not between any shape
    return std::make_pair(INDEX_TYPE_MAX, INDEX_TYPE_MAX);
}



inanoynmouse void NetDR::copySymRouteTree(const NetDR &rhs, const SymNet &symNet)
{
    _nodes.reserve(rhs.numNodes());
    for (IndexType idx = 0; idx < rhs.numNodes(); ++idx)
    {
        _nodes.emplace_back(rhs.node(idx));
        // Change the location
        _nodes.back().setLocation(symNet.symLoc(_nodes.back().location()));
    }
}
PROJECT_NAMESPACE_END
#endif ///AROUTER_NETDR_H_
