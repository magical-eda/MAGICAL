/**
 * @file NetNode.h
 * @brief Class for defining the description of shape tree for net intercell connection and intracell connection
 * @author Keren Zhu
 * @date 12/07/2018
 */

#ifndef AROUTER_NETNODE_H_
#define AROUTER_NETNODE_H_

#include "global/global.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::NetNodeType
/// @brief the type of the nodes in the NetDR
enum NetNodeType : Byte
{
    PHYSICAL = 0, ///< this is connect to a physical pin
    MEDIUM  = 1, ///< this is a medium point (turning point, via etc)
};

/// @class AROUTER::NetNodeLoc
/// @brief the geometic location and layer of a node
struct NetNodeLoc
{
    public:
        explicit NetNodeLoc() = default;
        explicit NetNodeLoc(XY<LocType> loc, IndexType layer) : _loc(loc), _layer(layer) {}
        XY<LocType> loc() const { return _loc; }
        XY<LocType> & loc() { return _loc; }
        IndexType layer() const { return _layer; }
        void setLoc(XY<LocType> loc) { _loc = loc; }
        void setLayer(IndexType layer) { _layer = layer; }
    XY<LocType> _loc = XY<LocType>(-1, -1); ///< the 2D location of this node (center line)
    IndexType _layer = INDEX_TYPE_MAX; ///< The layer of this node
};

/// @class AROUTER::NetNodeWire
/// @brief the specification for the wire
struct NetNodeWire
{
    public: 
        explicit NetNodeWire() = default;
        explicit NetNodeWire(LocType halfWidth, LocType extension) : _extension(extension), _halfWidth(halfWidth) {}
        LocType extension() const { return _extension; }
        LocType halfWidth() const { return _halfWidth; }
        void setExtension(LocType extension) { _extension = extension; }
        void setHalfWidth(LocType halfWidth) { _halfWidth = halfWidth; }
        bool isFake() const { return _extension  == -1 && _halfWidth == -1; }
        void setFake() { _extension = -1; _halfWidth = -1; }

    LocType _extension = -1; ///< The extension length of this segment
    LocType _halfWidth = -1; ///< The wire half-width of this segment
};

/// @class AROUTER::NetNodeVIA
/// @brief the specification for the VIA
struct NetNodeVIA
{
    public:
        explicit NetNodeVIA() = default;
        explicit NetNodeVIA(IndexType viaType) : _viaType(viaType) {}
        /// @brief determine whether this is a valid VIA
        bool valid() const { return _viaType != INDEX_TYPE_MAX; }
        /// @brief set this via invalid
        void setInvalid() { _viaType = INDEX_TYPE_MAX; }
        IndexType viaType() const { return _viaType; }
        void setViaType(IndexType viaType) { _viaType = viaType; }
        IndexType _viaType = INDEX_TYPE_MAX; ///< The via type on this one. If not via, then equal to INDEX_TYPE_MAX
};

/// @class AROUTER::NetNode
/// @brief the node class stored in NetDR to reconstruct the route of the net
class NetNode
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit NetNode() = default;
        /// @brief constructor with loc
        /// @param location and layer
        explicit NetNode(const NetNodeLoc &locLayer) : _locLayer(locLayer) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _parent
        /// @return the index of the parent node
        IndexType               parent() const    { return _parent; }
        /// @brief get _child
        /// @return the child node of this node
        IndexType               child() const     { return _child; }
        /// @brief get _sbling
        /// @return the sibling node of this one
        IndexType               sibling() const   { return _sibling; }
        /// @brief get _type
        /// @return the type of this node
        NetNodeType             type() const      { return _type; }
        /// @brief get _loc
        /// @return the location of the node
        const XY<LocType>       location() const  { return _locLayer.loc(); }
        /// @brief get _loc
        /// @return the location of the node
        XY<LocType>             location()        { return _locLayer.loc(); }
        /// @brief get _layer
        /// @return the layer this node is on
        IndexType               layer() const     { return _locLayer.layer(); }
        /// @brief get _extension
        /// @return the line extension length of this node
        LocType                 extension() const { return _wire.extension(); }
        /// @brief get _width
        /// @return the metal/masterslice width of this node
        LocType                 halfWidth() const     { return _wire.halfWidth(); }
        /// @brief get _viaType
        /// @return the index of via type that is on this node (the via between two nodes should only has viaType on the child node)
        IndexType               viaType() const   { return _via.viaType(); }
        /// @brief get the wire type
        /// @return the specification for the wire 
        NetNodeWire             netNodeWire() const { return _wire; }
        /// @brief get the via speicfication
        /// @return the specification for the via
        NetNodeVIA              netNodeVia() const { return _via; }
        /// @brief get the net node location bundle
        /// @return the bundle of location and layer
        NetNodeLoc              netNodeLoc() const { return _locLayer; }
        /// @brief set if this shape is a fake shape
        bool isFake() const { return _wire.isFake() && !this->hasVia();}
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set _parent
        /// @param the index of the parent node of this one
        void        setParent(IndexType parent)     { _parent = parent; }
        /// @brief set _child
        /// @param the index of the first child node of this node
        void        setChild(IndexType child)       { _child = child; }
        /// @brief set _sibling
        /// @param the index of the first sibling of this node
        void        setSibling(IndexType sibling)   { _sibling = sibling; }
        /// @brief set _type
        /// @param the type of this node
        void        setType(NetNodeType type)       { _type = type; }
        /// @brief set _layer
        /// @param the layer this node is on
        void        setLayer(IndexType layer)       { _locLayer.setLayer(layer); }
        /// @brief set location of the node
        /// @param location of the node
        void        setLocation(XY<LocType> loc)    { _locLayer.setLoc(loc); }
        /// @brief set _extension
        /// @param the line extension length of this node
        void        setExtension(LocType extension) { _wire.setExtension(extension); }
        /// @brief set _width
        /// @param the metal/masterslice width of this node
        void        setHalfWidth(LocType halfWidth)         { _wire.setHalfWidth(halfWidth); }
        /// @brief set _viaType
        /// @param the index of via type that is on this node
        void        setViaType(IndexType viaType)   { _via.setViaType(viaType); }
        /// @brief set _via
        /// @param set the via type
        void        setVia(NetNodeVIA via) { _via = via; }
        /// @brief set _wire
        /// @param the wire specifiction
        void        setWire(NetNodeWire wire) { _wire = wire; }
        /// @brief set fake, make this node not a valid shape
        void        setFake() { _wire.setFake(); _via.setInvalid();  }

        /*------------------------------*/ 
        /* Check valid                  */
        /*------------------------------*/ 
        /// @brief whether has parent
        /// @return true: has parent, false: no
        bool        hasParent() const             { return _parent != INDEX_TYPE_MAX; }
        /// @brief whether has child
        /// @return true: has child, false no
        bool        hasChild() const              { return _child != INDEX_TYPE_MAX; }
        /// @brief whether has sibling
        /// @return true: has sibling, false no
        bool        hasSibling() const            { return _sibling != INDEX_TYPE_MAX; }
        /// @brief whether has via
        /// @return true: if has via on this node, false: no
        bool        hasVia() const                { return _via.valid(); }
        /*------------------------------*/ 
        /* Operations                   */
        /*------------------------------*/ 
        /// @brief reset/remove shape
        void removeShape() { _wire = NetNodeWire(); _via = NetNodeVIA(); _parent = INDEX_TYPE_MAX; }

    private:
        /*------------------------------*/ 
        /* Construct the tree           */
        /*------------------------------*/ 
        IndexType _parent = INDEX_TYPE_MAX; ///< the parent of this node. If no parent, equal to INDEX_TYPE_MAX
        IndexType _child = INDEX_TYPE_MAX; ///< the first child of this node. If no child, equal to INDEX_TYPE_MAX. To get the other children, search the siblings of the child
        IndexType _sibling = INDEX_TYPE_MAX; ///< the first sibling of this node. If no sibling, equal to INDEX_TYPE_MAX. To get the other siblings, search tAfterling of sibling
        /*------------------------------*/ 
        /* Building up the route        */
        /*------------------------------*/ 
        NetNodeType _type = NetNodeType::MEDIUM; ///< The type of this node
        NetNodeLoc _locLayer; ///< The location and layer of the node
        NetNodeWire _wire = NetNodeWire(); ///< The specification for the wire shape
        NetNodeVIA _via = NetNodeVIA(); ///< The specification for the via
};


/// @class AROUTER::ShapeID
/// @brief used to identify which segnment this shape belongs to 
struct ShapeID
{
    public:
    /// @brief default constructor
    explicit ShapeID() = default;
    explicit constexpr ShapeID(IndexType netIdx_p, IntType nodeIdx_p) : netIdx(netIdx_p), nodeIdx(nodeIdx_p) {}
    /*------------------------------*/ 
    /* Type of the shape            */
    /*------------------------------*/ 
    /// @brief whether this is a valid shape ID. Could return an invalid shape ID if the query does not have any result
    /// @return if the ShapeID is valid
    bool valid() const { return netIdx != INDEX_TYPE_MAX; }
    //// @breif whether this is a blockage in router. 
    /// @return if the id is a blockage 
    bool isBlockage() const { return netIdx == INDEX_TYPE_MAX - 1; }
    /// @brief whether this is a terminal shape
    /// @return if the id is a terminal
    bool isTerminal() const { return netIdx == INDEX_TYPE_MAX - 2; }
    /// @brief whether this is a pin shape (but not in terminal)
    /// @return if the id is a pin shape
    bool isPinShape() const { return netIdx == INDEX_TYPE_MAX - 3; }
    /// @brief whether this is a normal routing shape
    /// @return if this id is a normal routing shape
    bool isRouteShape() const { return !isBlockage() && !isPinShape() && !isTerminal(); }
    /// @brief whether this is a part of routing tree or a fixed pin shape
    /// @return 1 if it is a fixed pin shape
    /*------------------------------*/ 
    /* Configure the shape          */
    /*------------------------------*/ 
    /// @brief configure the shapeID into a terminal shape
    /// @param netIdx: the netIdx it belongs to. terminalIdx: the terminal index it belongs to. shapeIdx : which shape it is belonging to. connected: if this shape is being used for connection
    void configTerminal(IndexType terminalIdx_, IndexType shapeIdx, bool connected) 
    {
        netIdx = INDEX_TYPE_MAX - 2;
        nodeIdx = terminalIdx_;
        if (connected)
        {
            _secondIdx = shapeIdx;
        }
        else
        {
            _secondIdx = -(static_cast<IntType>(shapeIdx) + 1);
        }
    }
    /// @brief configure the shapeID into a pin shape
    /// @param  pinIdx: the pin index it belongs to 
    void configpin(IndexType pinIdx_) 
    {
        netIdx = INDEX_TYPE_MAX - 3;
        nodeIdx = pinIdx_;
    }
    /// @brief configure the shapeID into a blockage shape
    /// @param the index of the blockage
    void configBlockage(IndexType blockIdx)
    {
        netIdx = INDEX_TYPE_MAX - 1;
        nodeIdx = blockIdx;
    }
    /*------------------------------*/ 
    /* Getters                      */
    /*------------------------------*/ 
    /// @brief to str
    std::string toStr() const { std::ostringstream oss; oss << netIdx <<" "<<nodeIdx<<" "<<_secondIdx << " " << INDEX_TYPE_MAX - netIdx << " ter: "<< isTerminal()<<" pin: "<<isPinShape(); return oss.str();}
    /// @brief get net index
    IndexType net() const { return netIdx; }
    /// @brief get node index
    IndexType node() const { return nodeIdx; }
    /// @brief get secondary index
    IndexType secondIdx() const { return _secondIdx; }
    bool operator==(ShapeID const& rhs)
    {
        if (rhs.netIdx !=  this->netIdx)
        {
            return false;
        }
        if (isBlockage())
        {
            return rhs.nodeIdx == this->nodeIdx;
        }
        if (isTerminal())
        {
            return rhs.nodeIdx == this->nodeIdx && rhs.isTerminalConnect() == this->isTerminalConnect();
        }
        if (isPinShape())
        {
            return rhs.nodeIdx ==  this->nodeIdx;
        }
        //if (isRouteShape())
        return rhs.netIdx == this->netIdx  && rhs.nodeIdx == this->nodeIdx;
    }
    bool operator!=(ShapeID const& rhs)
    {
        return !(*this==rhs);
    }
    bool operator<(ShapeID const & rhs) const
    {
        if (this->netIdx != rhs.netIdx)
        {
            return this->netIdx < rhs.netIdx;
        }
        if (this->nodeIdx != rhs.nodeIdx)
        {
            return this->nodeIdx < rhs.nodeIdx;
        }
        if (this->secondIdx() != rhs.secondIdx())
        {
            return this->secondIdx() < rhs.secondIdx();
        }
        return false;
    }
    /// @brief get which terminal this shape belongs to
    /// @return the index of the terminal shape
    IndexType terminalIdx() const { Assert(isTerminal()); return nodeIdx;}
    IndexType terminalShapeIdx() const 
    { 
        Assert(isTerminal());
        return _secondIdx; 
    }
    /// @brief if the terminal shape can be used as connecting point
    bool isTerminalConnect() const { Assert(isTerminal()); return nodeIdx > 0; }
    IndexType pinIdx() const { Assert(isPinShape()); return nodeIdx; }
    IndexType blockageIdx() const { Assert(isBlockage());  return nodeIdx; }
    /*------------------------------*/ 
    /* Members                      */
    /*------------------------------*/ 
    IndexType netIdx = INDEX_TYPE_MAX; ///< The net index
    IndexType nodeIdx = INDEX_TYPE_MAX; ///< The node index, the node of the child side of the shape
    private:
        IntType _secondIdx = 0; ///< the second indicator. The different shape in the terminal. Different part of via
};

/// Some helpful default shapeID
constexpr ShapeID BLOCK_SHAPEID = ShapeID(INDEX_TYPE_MAX -  1, 0);

/// @class AROUTER::BoxLayer
/// @brief rectangle + layer 
class BoxLayer
{
    public:
        explicit BoxLayer() = default;
        explicit BoxLayer(const Box<LocType> &box, IndexType layer) : _box(box), _layer(layer) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the rectangle
        /// @return the rectangle shape
        const Box<LocType> & box() const { return _box; }
        /// @brief get the rectangle
        /// @return the rectangle shape
        Box<LocType> & box() { return _box; }
        /// @brief get the layer
        /// @return the layer
        IndexType layer() const { return _layer; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the layer
        /// @param the layer
        void setLayer(IndexType layer) { _layer = layer; }


    private:
        Box<LocType> _box; ///< A rectangle shape
        IndexType _layer = INDEX_TYPE_MAX; /// The layer
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_NETNODE_H_
