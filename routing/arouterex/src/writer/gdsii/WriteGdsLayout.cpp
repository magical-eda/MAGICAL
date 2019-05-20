//
// Created by Keren on 09/16/2018
//

#include "WriteGdsLayout.h"

PROJECT_NAMESPACE_BEGIN

bool WriteGdsLayout::writeLayoutFake(std::string outputFile)
{
#if 0
    _wg = std::make_shared<WriteGds>(outputFile);
    //_wg = std::shared_ptr<WriteGds>(new WriteGds(outputFile));
    if (!_wg->initWriter())
    {
        return false;
    }
    if (!_wg->createLib("TOP", 2000, 1e-6/2000))
    {
        return false;
    }
    if (!_wg->writeCellBgn("INTERCONNECTION"))
    {
        return false;
    }
    // Fake vias 
    std::vector<ViaPlaced> viaVec;
    for (IndexType viaIdx = 0; viaIdx < _db.macroDB().fixedViaVec2D().at(1).size(); ++ viaIdx)
    {
        std::cout<<"Write "<<viaIdx<<std::endl;
        ViaPlaced temp(XYZ<IndexType>(0,0,1), XYZ<IndexType>(0,0,2), viaIdx);
        temp.origin() = XY<LocType>(100000, 1000 * viaIdx);
        writeViaPlacedLayout(temp);
    }
    if (!_wg->writeCellEnd())
    {
        return false;
    }
    if (!_wg->endLib())
    {
        return false;
    }
#endif
    return true;
}

bool WriteGdsLayout::writeLayout(std::string outputFile)
{
    _wg = std::make_shared<WriteGds>(outputFile);
    //_wg = std::shared_ptr<WriteGds>(new WriteGds(outputFile));
    if (!_wg->initWriter())
    {
        return false;
    }
    if (!_wg->createLib("TOP", 0.001, 1e-6/_db.techDB().units().databaseNumber))
    {
        return false;
    }
    if (!_wg->writeCellBgn("INTERCONNECTION"))
    {
        return false;
    }
    for (IndexType netIdx = 0; netIdx < _db.drDB().numNets(); ++netIdx)
    {
        if (!writeNetLayout(netIdx))
        {
            return false;
        }
    }
    // write intra cell
    if (!writeIntraCell())
    {
        return false;
    }
    // for debug
#ifdef DEBUG_DRAW_PIN
    //writePins();
    //writeDummyPins();
#endif
    
    if (!_wg->writeCellEnd())
    {
        return false;
    }
    if (!_wg->endLib())
    {
        return false;
    }

    return true;
}


bool WriteGdsLayout::writeNetLayout(IndexType netIdx)
{
    auto &net = _db.drDB().net(netIdx);
    bool writeLabel = true;
    for (IndexType nodeIdx = 0; nodeIdx < net.numNodes(); ++nodeIdx)
    {
        auto &node = net.node(nodeIdx);
        if (node.hasVia())
        {
            if (!this->writeShape(ShapeID(netIdx, nodeIdx), writeLabel))
            {
                DBG("Write VIA fails! \n");
                return false;
            }
            writeLabel = false;
        }
        else if (!node.isFake())
        {
            if (node.hasParent())
            {
                if (!this->writeShape(ShapeID(netIdx, nodeIdx), writeLabel))
                {
                    DBG("Write wire fails! \n");
                    return false;
                }
                writeLabel = false;
            }
        }
    }
    return true;
}

bool WriteGdsLayout::writeIntraCell()
{
    for (const auto &term : _db.terminalArray())
    {
        for (IndexType shapeIdx = 0; shapeIdx <  term.intraCellRoute().size(); ++shapeIdx)
        {
            if (!term.generated(shapeIdx))
            {
                continue;
            }
            const auto &shape = term.intraShape(shapeIdx);
            if (!_wg->writeRectangle(shape.box(), _db.techDB().layerIdx2MaskID(shape.layer()), 0))
            {
                return false;
            }
        }
    }
    return true;
}

bool WriteGdsLayout::writeShape(ShapeID shapeID, bool writeLabel)
{
    _shapeComp.computeShape(shapeID);

    // For wrting labels
    XY<LocType> pt;
    IntType layer = 0;
    IntType size = 0;
    const auto &shapeVec = _shapeComp.result();
    for (const auto &shape : shapeVec)
    {
        // Update the pt and layer
        pt = shape.first.center();
        layer = _db.techDB().layerIdx2MaskID(shape.second);
        size = shape.first.xLen();
        if (!_wg->writeRectangle(shape.first, _db.techDB().layerIdx2MaskID(shape.second), 0))
        {
            return false;
        }
    }
    // Write the label
    if (writeLabel)
    {
        Assert(shapeID.isRouteShape());
        const auto &net = _db.grDB().nets().at(shapeID.net());
        std::string label = net.name();
        if (!_wg->writeText(label, pt.x(), pt.y(), layer, size))
        {
            return false;
        }
    }
    return true;
}

#if 0
bool WriteGdsLayout::writeNetLayout(IndexType netIdx) const
{
    const auto &drNodes = _db.drDB().netArray().at(netIdx).drNodes();
    for (const auto &idxNode : drNodes)
    {
        Box<LocType> metal = _db.drDB().nodeMetalShape(idxNode);
        if (!_wg->writeRectangle(metal, _db.techDB().accessLayerMaskId(idxNode.z()), 0))
        {
            return false;
        }
    }

    // Draw vias
    const auto &drViaNodes = _db.drDB().viaSetArray().at(netIdx);
    for (const auto &viaNode : drViaNodes)
    {
        if (!writeViaPlacedLayout(viaNode))
        {
            return false;
        }
    }

    return true;
}

#endif
void WriteGdsLayout::writeDummyPins()
{
    for (auto &pin : _db.grDB().phyPinArray())
    {
        Box<LocType> shape = Box<LocType> (-10, -10, 10, 10 );
        XY<LocType> origin = pin.loc();
        if (!_wg->writeRectangle(shape.offsetBox(origin), 999, 0))
        {
            std::cout<< shape.offsetBox(origin).xLo()<<"  failed"<<std::endl;
        }
    }
}

bool WriteGdsLayout::writePins()
{
    for (IndexType termIdx = 0; termIdx < _db.terminalArray().size(); ++termIdx)
    {
        const auto &terminal = _db.terminalArray().at(termIdx);
        for (IndexType pinIdx : terminal.pinArray())
        {
            const auto &pin = _db.grDB().phyPinArray().at(pinIdx);
            
            for (IndexType idx = 0; idx <  pin.shapeRects().size(); ++idx)
            {
                const auto &rect = pin.shapeRects().at(idx);
                if (pin.isIncludeTerm(idx))
                {
                    continue;
                }
                IndexType accessLayerIdx = pin.layer();
                _wg->writeRectangle(rect, _db.techDB().accessLayerMaskId(accessLayerIdx), 0);
            }
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
