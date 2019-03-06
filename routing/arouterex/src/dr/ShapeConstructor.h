/**
 * @file ShapeConstructor.h
 * @brief The class for computing the shape of the wire/via
 * @author anoynmous anoynmous
 * @date 10/31/2018
 */

#ifndef AROUTER_SHAPE_CONSTRUCTOR_H_
#define AROUTER_SHAPE_CONSTRUCTOR_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN


/// @class AROUTER::ShapeConstructor
/// @brief the class for computing the shape of the net node
class ShapeConstructor
{
    public:
        typedef std::pair<Box<LocType>, IndexType> ShapeConstType; ///< .first = rectangle shape .second  = layer index 
        explicit ShapeConstructor(Database &db) : _db(db) {}
        /*------------------------------*/ 
        /* Shape Compute wrapper        */
        /*------------------------------*/ 
        /// @brief compute the shapes for the shapeID and return the results
        /// @param the shaped ID 
        /// @return the vector of shape and layer
        const std::vector<ShapeConstType> & computeShape(ShapeID shapeID);
        /// @brief compute the metal/masterslice wire
        /// @param the shape ID
        void computeWire(ShapeID shapeID);
        /// @brief compute the VIA shapes
        /// @param the shapeID
        void computeVIA(ShapeID shapeID);
        /// @brief compute the fixed shapes
        /// @param the shapeID
        void computeFixedShape(ShapeID shapeID);
        /*------------------------------*/ 
        /* Get the computing results    */
        /*------------------------------*/ 
        /// @brief return the results of the last computation
        /// @return the results from the last computation
        const std::vector<ShapeConstType> & result() { return _result; }
        /*------------------------------*/ 
        /* Seperated Shape compute      */
        /*------------------------------*/ 
        /// @brief compute the wire box 
        /// @param the two nodes as starting and end point and the specification for the wire
        /// @return the shape of the wire segment
        Box<LocType> computeWireShape(XY<LocType> from, XY<LocType> to, NetNodeWire wire) const;
        /// @brief compute the lower level metal shape of a VIAs
        /// @param loc: the location of the center point of the VIA. bottomLayer: the index in routable layer of the bottom metal. via: the specification of the via
        /// @return the shapes of the VIA on the lower layer metal
        Box<LocType> computeViaBottomMetalShape(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via) const;  
        /// @brief compute the CUT layer shapes of a VIA
        /// @param shapeID
        /// @return the shapes of the VIA on the middle CUT layer
        void computeViaCutShape(ShapeID shapeID, std::vector<Box<LocType>> &resultList) const;
        /// @brief compute the CUT layer shapes of a VIA
        /// @param loc: the location of the center point of the VIA. bottomLayer: the index in routable layer of the bottom metal. via: the specification of the via
        /// @return the shapes of the VIA on the middle CUT layer
        void computeViaCutShape(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via, std::vector<Box<LocType>> &resultList) const;
        /// @brief compute the top layer metal shape of a VIA
        /// @param loc: the location of the center point of the VIA. bottomLayer: the index in routable layer of the bottom metal. via: the specification of the via
        /// @return the shape of the VIA on the top metal layer
        Box<LocType> computeViaTopMetalShape(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via) const;
        /// @brief compute the CUT layer bounding box of a VIA
        /// @param loc: the location of the center point of the VIA. bottomLayer: the index in routable layer of the bottom metal. via: the specification of the via
        /// @return the bounding box of the VIA on the middle cut layer
        Box<LocType> computeViaCutBBox(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via) const;
        /*------------------------------*/ 
        /* Misc. query                  */
        /*------------------------------*/ 
        /// @brief determine whether the proposing routing path is on track with an existing shape
        /// @param shapeID: the ID of a existing shape. fromNode, toNode: the location of the two nodes. fromRoutelayer, toRotueLayer: the index of the node layer in routable layers (masterslice + metal)
        /// @return true: if the shape is on-track.
        bool onTrack(ShapeID shapeID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer);
        /// @brief determine whether the proposing routing path has reached the terminal
        bool reachTerminal(ShapeID terminalID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer);
        /// @brief whether two nodes are on-track with a wire
        bool onTrackWire(ShapeID shapeID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer);
        /// @brief whether the two nodes are on-track with a via
        bool onTrackVIA(ShapeID shapeID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer);
        /// @brief whether the rect is inside the target terminal
        bool rectInTerminal(ShapeID terminal, const Box<LocType> &rect, IndexType routeLayer);
    private:
        Database &_db; ///< The database of the router
        std::vector<ShapeConstType> _result; ///< put the computed shapes in here
};

inanoynmouse const std::vector<ShapeConstructor::ShapeConstType> & ShapeConstructor::computeShape(ShapeID shapeID)
{
    AssertMsg(shapeID.valid(), "%s: Invalid shape! \n", __FUNCTION__);
    _result.clear();
    /// First determine whether the shape is a fixed shape
    if (!shapeID.isRouteShape())
    {
        computeFixedShape(shapeID);
    }
    else
    {
        /// The child node has only one parent. And the connection can only be either a VIA edge or a wire edge
        if (_db.drDB().netNode(shapeID).hasVia())
        {
            computeVIA(shapeID);
        }
        else if (!_db.drDB().netNode(shapeID).isFake())
        {
            computeWire(shapeID);
        }
        else
        {
            /// Fake
        }
    }
    return _result;
}

inanoynmouse void ShapeConstructor::computeWire(ShapeID shapeID)
{
    _result.clear();
    const auto &child = _db.drDB().netNode(shapeID);
    const auto &parent = _db.drDB().net(shapeID.netIdx).node(child.parent());
    Assert(child.layer() == parent.layer());
    _result.emplace_back(std::make_pair(this->computeWireShape(child.location(), parent.location(), child.netNodeWire()), DRUtil::routeLayer2AllLayer(child.layer())));
}

inanoynmouse void ShapeConstructor::computeVIA(ShapeID shapeID)
{
    _result.clear();
    const auto &child = _db.drDB().netNode(shapeID);
    const auto &parent = _db.drDB().net(shapeID.netIdx).node(child.parent());
    Assert(child.location() == parent.location());

    IndexType lowerLayer = std::min(child.layer(), parent.layer());
    /// Bottom metal
    _result.emplace_back(std::make_pair(this->computeViaBottomMetalShape(child.location(), lowerLayer, child.netNodeVia()),
                                        DRUtil::routeLayer2AllLayer(lowerLayer)));
    /// CUT
    std::vector<Box<LocType>> cutRects;
    this->computeViaCutShape(child.location(), lowerLayer, child.netNodeVia(), cutRects);
    for (const auto &cut : cutRects)
    {
        _result.emplace_back(std::make_pair(cut,
                                            DRUtil::routeLayer2AllLayer(lowerLayer) + 1));
    }
    /// Top metal
    _result.emplace_back(std::make_pair(this->computeViaTopMetalShape(child.location(), lowerLayer, child.netNodeVia()),
                                        DRUtil::routeLayer2AllLayer(lowerLayer) + 2));
}

inanoynmouse void ShapeConstructor::computeFixedShape(ShapeID shapeID)
{
    _result.clear();
    /// First determine whether this shape belongs to one net
    if (shapeID.isBlockage())
    {
        IndexType blockIdx = shapeID.blockageIdx();
        const auto &block = _db.blockage(blockIdx);
        _result.emplace_back(std::make_pair(block.rect(), block.layer()));
    }
    else if (shapeID.isTerminal())
    {
        /// A terminal shape belongs to a net
        IndexType termIdx = shapeID.terminalIdx();
        for (const auto termRect : _db.terminal(termIdx).intraCellRoute())
        {
            _result.emplace_back(std::make_pair( termRect.box(), termRect.layer())); /// this layer is already all layers
        }
    }
    else if (shapeID.isPinShape())
    {
        IndexType pinIdx = shapeID.pinIdx();
        for (const auto pinRect : _db.drDB().pin(pinIdx).shapeRects())
        {
            _result.emplace_back(std::make_pair( pinRect, DRUtil::routeLayer2AllLayer(_db.drDB().pin(pinIdx).layer()))); 
        }
    }
    else
    {
        Assert(false);
    }
}

inanoynmouse Box<LocType> ShapeConstructor::computeWireShape(XY<LocType> from, XY<LocType> to, NetNodeWire wire) const 
{
    LocType xLo = std::min(from.x(), to.x());
    LocType xHi = std::max(from.x(), to.x());
    LocType yLo = std::min(from.y(), to.y());
    LocType yHi = std::max(from.y(), to.y());
    if (xLo == xHi)
    {
        /// Horizontal
        xLo -= wire.extension();
        xHi += wire.extension();
        yLo -= wire.halfWidth();
        yHi += wire.halfWidth();
    }
    else
    {
        Assert(yLo ==  yHi);
        /// vertical
        xLo -= wire.halfWidth();
        xHi += wire.halfWidth();
        yLo -= wire.extension();
        yHi += wire.extension();
    }
    return Box<LocType>(xLo, yLo, xHi, yHi);
}

inanoynmouse Box<LocType> ShapeConstructor::computeViaBottomMetalShape(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via) const 
{
    Box<LocType> rect = _db.macroDB().viaMacro(bottomLayer, via.viaType()).bottomRect();
    rect.offsetBy(loc);
    return rect;
}

inanoynmouse void ShapeConstructor::computeViaCutShape(ShapeID shapeID, std::vector<Box<LocType>> &resultList) const
{
    if (shapeID.isRouteShape())
    {
        const auto &child = _db.drDB().netNode(shapeID);
        const auto &parent = _db.drDB().net(shapeID.netIdx).node(child.parent());
        Assert(child.location() == parent.location());

        IndexType lowerLayer = std::min(child.layer(), parent.layer());
        this->computeViaCutShape(child.location(), lowerLayer, child.netNodeVia(), resultList);
    }
}

inanoynmouse void ShapeConstructor::computeViaCutShape(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via, std::vector<Box<LocType>> &resultList) const 
{
    for (auto rect : _db.macroDB().viaMacro(bottomLayer, via.viaType()).cutRectArray())
    {
        resultList.emplace_back(rect.offsetBox(loc));
    }
}

inanoynmouse Box<LocType> ShapeConstructor::computeViaTopMetalShape(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via) const
{
    Box<LocType> rect = _db.macroDB().viaMacro(bottomLayer, via.viaType()).topRect();
    rect.offsetBy(loc);
    return rect;
}

inanoynmouse Box<LocType> ShapeConstructor::computeViaCutBBox(XY<LocType> loc, IndexType bottomLayer, NetNodeVIA via) const
{
    Box<LocType> cutBBox = _db.macroDB().viaMacro(bottomLayer, via.viaType()).cutBBox();
    cutBBox.offsetBy(loc);
    return cutBBox;
}

inanoynmouse bool ShapeConstructor::reachTerminal(ShapeID shapeID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer)
{
    Assert(shapeID.isTerminal());
    /// If the node is within the shape of pin, then it is on track
    const auto &term = _db.terminal(shapeID.terminalIdx());
    for (IndexType rectIdx = 0; rectIdx < term.numIntraShapes(); ++rectIdx)
    {
        /// Check  the connecting segments
        if (!term.isIntraShapeConnect(rectIdx))
        {
            continue;
        }
        const auto &boxLayer = term.intraShape(rectIdx);
        if (boxLayer.box().contain(toNode) && toRouteLayer ==  DRUtil::allLayer2RouteLayer(boxLayer.layer()))
        {
            return true;
        }
    }
    return false;
}
inanoynmouse bool ShapeConstructor::onTrack(ShapeID shapeID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer)
{
    Assert(shapeID.valid());
    if (shapeID.isTerminal())
    {
        /// If the node is within the shape of pin, then it is on track
        const auto &term = _db.terminal(shapeID.terminalIdx());
        for (IndexType rectIdx = 0; rectIdx < term.numIntraShapes(); ++rectIdx)
        {
            /// Check  the connecting segments
            if (!term.isIntraShapeConnect(rectIdx))
            {
                return false;
            }
            const auto &boxLayer = term.intraShape(rectIdx);
            if (boxLayer.box().contain(fromNode) && fromRouteLayer ==  DRUtil::allLayer2RouteLayer(boxLayer.layer()))
            {
                return true;
            }
            if (boxLayer.box().contain(toNode) && toRouteLayer ==  DRUtil::allLayer2RouteLayer(boxLayer.layer()))
            {
                return true;
            }
        }
        return false;
    }
    else if (shapeID.isPinShape())
    {
        return false;
    }
    else if (shapeID.isBlockage())
    {
        return false;
    }
    else if (_db.drDB().netNode(shapeID).hasVia())
    {
        /// Via
        return onTrackVIA(shapeID, fromNode, fromRouteLayer, toNode, toRouteLayer);
    }
    else 
    {
        /// wire
        return onTrackWire(shapeID, fromNode, fromRouteLayer, toNode, toRouteLayer);
    }
}


inanoynmouse bool ShapeConstructor::onTrackWire(ShapeID shapeID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer)
{
    /// Wire: one of the node must be on the center axis of the wire
    const auto &child = _db.drDB().netNode(shapeID);
    const auto &parent = _db.drDB().net(shapeID.netIdx).node(child.parent());
    Assert(child.layer() == parent.layer());
    LocType xLo = std::min(child.location().x(), parent.location().x());
    LocType xHi = std::max(child.location().x(), parent.location().x());
    LocType yLo = std::min(child.location().y(), parent.location().y());
    LocType yHi = std::max(child.location().y(), parent.location().y());

    if (xLo == xHi)
    {
        /// Horizontal
        if (child.layer() == fromRouteLayer)
        {
            if (xLo ==  fromNode.x())
            {
                if (yLo <= fromNode.y() && yHi >= fromNode.y())
                {
                    return true;
                }
            }
        }
        if (child.layer() == toRouteLayer)
        {
            if (xLo ==  toNode.x())
            {
                if (yLo <= toNode.y() && yHi >= toNode.y())
                {
                    return true;
                }
            }
        }
    }
    else
    {
        Assert(yLo ==  yHi);
        /// vertical
        if (child.layer() == fromRouteLayer)
        {
            if (yLo ==  fromNode.y())
            {
                if (xLo <= fromNode.x() && xHi >= fromNode.x())
                {
                    return true;
                }
            }
        }
        if (child.layer() == toRouteLayer)
        {
            if (yLo ==  toNode.y())
            {
                if (xLo <= toNode.x() && xHi >= toNode.x())
                {
                    return true;
                }
            }
        }
    }
    return false;
}


inanoynmouse bool ShapeConstructor::onTrackVIA(ShapeID shapeID, XY<LocType> fromNode, IndexType fromRouteLayer, XY<LocType> toNode, IndexType toRouteLayer)
{
    /// VIA: Shared the same center axis
    const auto &child = _db.drDB().netNode(shapeID);
    const auto &parent = _db.drDB().net(shapeID.netIdx).node(child.parent());
    Assert(child.location() == parent.location());
    IndexType layerLo = std::min(child.layer(), parent.layer());
    IndexType layerHi = std::max(child.layer(), parent.layer());

    if (fromNode ==  child.location())
    {
        if (layerLo <= fromRouteLayer && layerHi >= fromRouteLayer)
        {
            return true;
        }
    }
    if (toNode ==  child.location())
    {
        if (layerLo <= toRouteLayer && layerHi >= toRouteLayer)
        {
            return true;
        }
    }
    return false;
}


inanoynmouse bool ShapeConstructor::rectInTerminal(ShapeID terminal, const Box<LocType> &rect, IndexType routeLayer)
{
    this->computeFixedShape(terminal);
    for (auto &terShape : _result)
    {
        if (terShape.second == DRUtil::routeLayer2AllLayer(routeLayer))
        {
            if (terShape.first.overlap(rect))
            {
                return true;
            }
        }
    }
    return false;
}
PROJECT_NAMESPACE_END

#endif ///AROUTER_SHAPE_CONSTRUCTOR_H_
