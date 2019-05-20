#include "DRBase.h"

PROJECT_NAMESPACE_BEGIN

void DRBase::addPinsAndTerminalsToShapeMap()
{
    for (IndexType netIdx = 0; netIdx < _db.drDB().numNets(); ++netIdx)
    {
        const auto &net = _db.drDB().net(netIdx);
        for (IndexType termIdx = 0; termIdx < net.numTerminals(); ++ termIdx)
        {
            const auto &terminal = _db.terminal(net.terminal(termIdx));
            ShapeID terminalShapeID = ShapeID();
            // Add the shapes belonging to the terminals
            for (IndexType shapeIdx = 0; shapeIdx < terminal.intraCellRoute().size(); ++shapeIdx)
            {
                const auto &boxLayer = terminal.intraShape(shapeIdx);
                if (terminal.isIntraShapeConnect(shapeIdx))
                {
                    terminalShapeID.configTerminal(net.terminal(termIdx), shapeIdx, true);
                    // This should be used as a connection points 
                    _shapeMapPtr->insertFixedShape(boxLayer.box(), boxLayer.layer(), terminalShapeID); 
                }
                else
                {
                    terminalShapeID.configTerminal(net.terminal(termIdx), shapeIdx, false);
                    // This should be used as a blockage
                    _shapeMapPtr->insertFixedShape(boxLayer.box(), boxLayer.layer(), terminalShapeID); 
                }
            }
            // Add the remaining pin shapes related to this terminal
            for (IndexType pinIdx : terminal.pinArray())
            {
                const auto &pin = _db.drDB().pin(pinIdx);
                ShapeID pinShapeID = ShapeID();
                pinShapeID.configpin(pinIdx);
                for (IndexType pinShapeIdx = 0; pinShapeIdx <  pin.shapeRects().size(); ++pinShapeIdx)
                {
                    if (!pin.isIncludeTerm(pinShapeIdx))
                    {
                        // If it is not included in the terminal connection, add it as a blockage
                        const auto &pinShape  =  pin.shapeRects().at(pinShapeIdx);
                        _shapeMapPtr->insertFixedShape(pinShape, DRUtil::routeLayer2AllLayer(pin.layer()), pinShapeID);
                    }
                }
            }
        }
    }
}

void DRBase::addBlockagesToShapeMap()
{
    // Query the shapeMap and only save the ones not overlapping with pins
    /// TODO: Here just using the bruteforce comparision, if the run time is intolerantable, use r tree instead
#if 1
    for (auto &terminal : _db.terminalArray())
    {
        std::set<IndexType> removeBlockageList;
        std::set<IndexType> boxesToAdd;
        for (IndexType blockIdx = 0; blockIdx < _db.numBlockages(); ++blockIdx)
        {
            const auto &block = _db.blockage(blockIdx);
            for (const auto &rect : terminal.intraCellRoute())
            {
                // If the two shapes are identical, remove the blockage
                if (block.layer() == rect.layer() && rect.box().intersect(block.rect()))
                {
                    removeBlockageList.insert(blockIdx);
                    // Handle the small shape between horizontal pin segment and fingers
                    //if (!rect.box().overlap(block.rect()))
                    if (1)
                    {
                        boxesToAdd.insert(blockIdx);
                    }
                }
            }
        }
        // Add intersect shapes to the terminals
        for (IndexType blockIdx : boxesToAdd)
        {
            const auto &block = _db.blockage(blockIdx);
            terminal.addIntraBox(block.rect(), block.layer(), true, false);
        }
        // The removeBlockageList is in accending order, and therefore can be safely erased one by one in the reverse order
        for (auto it = removeBlockageList.rbegin(); it != removeBlockageList.rend(); ++it)
        {
            ::klib::eraseElementInVec<Blockage>(_db.blockageArray(), *it);
        }
    }
#endif
    // Add to the shapemap
    for (IndexType blockIdx = 0; blockIdx < _db.numBlockages(); ++blockIdx)
    {
        const auto &block = _db.blockage(blockIdx);
        ShapeID shapeID = ShapeID();
        shapeID.configBlockage(blockIdx);
        // This should be used as a blockage
        _shapeMapPtr->insertFixedShape(block.rect(), std::min(block.layer(), _shapeMapPtr->numLayers() - 1), shapeID); 
    }
}
bool DRBase::removeShape(ShapeID shapeID)
{
    // remove it from the shapeMap
    _shapeMapPtr->removeShape(shapeID);

    auto &net = _db.drDB().net(shapeID.netIdx);
    IndexType nodeIdx = shapeID.nodeIdx;
    IndexType parentIdx = net.node(nodeIdx).parent();
    net.node(nodeIdx).removeShape();

    // Resolve the node index
    IndexType childIdx = net.node(parentIdx).child();
    if (childIdx != nodeIdx)
    {
        // the child of the parent is not this node. Then need to find the node that point to nodeIdx as sibling
        IndexType childNode = childIdx;
        while(net.node(childNode).sibling() != nodeIdx)
        {
            childNode = net.node(childNode).sibling();
            Assert(net.node(childNode).sibling() != INDEX_TYPE_MAX);
        }
        // update the sibling
        net.node(childNode).setSibling(net.node(nodeIdx).sibling());
    }
    else
    {
        // if the parnet.child ==  this node, then also need to update the parent.child
        net.node(parentIdx).setChild(net.node(nodeIdx).sibling());
    }
    // Also clear the sibling of this node
    net.node(nodeIdx).setSibling(INDEX_TYPE_MAX);
    // If the node has no connection in the tree, remove it
    if (!net.node(nodeIdx).hasChild())
    {
        removeEmptyNetNode(shapeID.netIdx, shapeID.nodeIdx);
    }
    return true;
}

bool DRBase::ripupNet(IndexType netIdx)
{
    // Rip up all the shapes from the net
    auto &net = _db.drDB().net(netIdx);
    for (IndexType nodeIdx = 1; nodeIdx < net.numNodes(); ++nodeIdx)
    {
        // remove it from the shapeMap
        if (!_shapeMapPtr->removeShape(ShapeID(netIdx, nodeIdx)))
        {
            return false;
        }
    }
    // clear the routing shape tree
    net.clearRouteTree();
    return true;
}

IndexType DRBase::addWireNodeFromParent(IndexType netIdx, IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire)
{
    IndexType nodeIdx = _db.drDB().net(netIdx).addWireNodeFromParent(parentNodeIdx, loc, wire);
    // update the shape map
    _shapeMapPtr->insertShape(ShapeID(netIdx, nodeIdx));
    return nodeIdx;
}

IndexType DRBase::addFakeNodeFromParent(IndexType netIdx, IndexType parentNodeIdx, const NetNodeLoc &loc)
{
    IndexType nodeIdx = _db.drDB().net(netIdx).addFakeNodeFromParent(parentNodeIdx, loc);
    return nodeIdx;
}

IndexType DRBase::addViaNodeFromParent(IndexType netIdx, IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via)
{
    IndexType nodeIdx = _db.drDB().net(netIdx).addViaNodeFromParent(parentNodeIdx, loc, via);
    // update the shape map
    _shapeMapPtr->insertShape(ShapeID(netIdx, nodeIdx));
    return nodeIdx;
}

IndexType DRBase::addWireNodeFromChild(IndexType netIdx, IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire)
{
    IndexType nodeIdx = _db.drDB().net(netIdx).addWireNodeFromChild(childNodeIdx, loc, wire);
    // update the shape map
    _shapeMapPtr->insertShape(ShapeID(netIdx, nodeIdx));
    return nodeIdx;
}

IndexType DRBase::addViaNodeFromChild(IndexType netIdx, IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via)
{
    IndexType nodeIdx = _db.drDB().net(netIdx).addViaNodeFromChild(childNodeIdx, loc, via);
    // update the shape map
    _shapeMapPtr->insertShape(ShapeID(netIdx, nodeIdx));
    return nodeIdx;
}

void DRBase::connectWire(IndexType netIdx, IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeWire &wire)
{
    _db.drDB().net(netIdx).connectWire(parentNodeIdx, childNodeIdx, wire);
    // Update the shape map
    _shapeMapPtr->insertShape(ShapeID(netIdx, childNodeIdx));
}

void DRBase::connectVia(IndexType netIdx, IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeVIA &via)
{
    _db.drDB().net(netIdx).connectVia(parentNodeIdx, childNodeIdx, via);
    // Update the shape map
    _shapeMapPtr->insertShape(ShapeID(netIdx, childNodeIdx));
}

IndexType DRBase::insertNodeInWire(IndexType netIdx, IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeLoc &loc)
{
    auto &net = _db.drDB().net(netIdx);
    // Use the same wire specification
    auto wire = net.node(childNodeIdx).netNodeWire();
    // Add the wire between the parent and middle
    IndexType nodeIdx = this->addWireNodeFromParent(netIdx, parentNodeIdx, loc, wire); //< The shape map is updated in the function
    // connecting child and middle operations are different depending whether the child has children
    if (net.node(childNodeIdx).hasChild())
    {
        // Then remove the shape between parent and child and connect child and the middle
        // The shape map should be handled by the functions
        this->removeShape(ShapeID(netIdx, childNodeIdx)); //< because the node has child, therefore the index shouldn't change
        this->connectWire(netIdx, nodeIdx, childNodeIdx, wire);
    }
    else
    {
        // Remove the shape between child and parent will result in removing the child. Therefore need to add a new node as child
        auto childLoc =  net.node(childNodeIdx).netNodeLoc();
        // Removing the child will make the indices change, therefore add the shape first, remove next, and search the node at last
        this->addWireNodeFromParent(netIdx, nodeIdx, childLoc, wire);
        this->removeShape(ShapeID(netIdx, childNodeIdx));
        nodeIdx = net.existNode(loc);
    }
    return nodeIdx;
}


void DRBase::initNetSourceNode(IndexType netIdx, NetNodeLoc loc)
{
    auto &node = _db.drDB().net(netIdx).sourceNode();
    node = NetNode(loc);
    node.setType(NetNodeType::PHYSICAL);
}



/*------------------------------*/ 
/* Debug                        */
/*------------------------------*/ 
PROJECT_NAMESPACE_END
#include "writer/gdsii/WriteGds.h"
PROJECT_NAMESPACE_BEGIN
void DRBase::drawGdsGlobalRoutingGuide(const std::string &filename, IndexType netIdx) const
{
    const auto &net = _db.drDB().net(netIdx);
    auto _wg = std::make_shared<WriteGds>(filename);
    //_wg = std::shared_ptr<WriteGds>(new WriteGds(outputFile));
    if (!_wg->initWriter())
    {
        return;
    }
    if (!_wg->createLib("TOP", _db.techDB().units().databaseNumber, 1e-6/_db.techDB().units().databaseNumber))
    {
        return;
    }
    if (!_wg->writeCellBgn("INTERCONNECTION"))
    {
        return;
    }
    // Write Pins
    for (IndexType pinIdx : net.pins())
    {
        const auto &pin = _db.drDB().pin(pinIdx);
        Box<LocType> shape = Box<LocType> (-1000, -1000, 1000, 1000 );
        XY<LocType> origin = pin.loc();
        if (!_wg->writeRectangle(shape.offsetBox(origin), 999, 0))
        {
            std::cout<< shape.offsetBox(origin).xLo()<<"  failed"<<std::endl;
        }
    }
    // Write global routing guide
    const auto &roughGrid = _db.drDB().roughGrid();
    for (IndexType xGrid = 0; xGrid < roughGrid.sizeX(); ++xGrid)
    {
        for (IndexType yGrid = 0; yGrid < roughGrid.sizeY(); ++yGrid)
        {
            for (IndexType layer = 0; layer < roughGrid.numLayers(); ++layer)
            {
                if (roughGrid.at(xGrid, yGrid, layer).netInGlobalRoutingGuide(netIdx))
                {
                    Box<LocType> gBox = roughGrid.nodeBox(xGrid, yGrid);
                    _wg->writeRectangle(gBox, layer, 0);
                }
            }
        }
    }
    
    if (!_wg->writeCellEnd())
    {
        return;
    }
    if (!_wg->endLib())
    {
        return;
    }
    DBG("%s: debug rtree shapes saved in %s \n", __FUNCTION__, filename.c_str());
    return;
}
/*------------------------------*/ 
/* Private functions            */
/*------------------------------*/ 

void DRBase::removeEmptyNetNode(IndexType netIdx, IndexType nodeIdx)
{
    _db.drDB().net(netIdx).removeEmptyNetNode(nodeIdx);
}

PROJECT_NAMESPACE_END
