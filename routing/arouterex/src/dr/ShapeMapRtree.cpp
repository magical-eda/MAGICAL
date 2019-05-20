#include "ShapeMapRtree.h"

PROJECT_NAMESPACE_BEGIN

void ShapeMapRtree::init()
{
    /// init the vector of layer to be the size of MASTERSLICE, METAL, CUT and LAYER
    // Last layer is the dummy blockage
    IndexType numAccessLayers = _db.techDB().layerTypeVec().size() + 1; 
    _rtreeArray.resize(numAccessLayers);    

    /// Add pin shapes to the shape map
    //initPinShapes(); 
}

void ShapeMapRtree::initPinShapes()
{
    for (IndexType netIdx =  0; netIdx < _db.drDB().numNets(); ++ netIdx)
    {
        for (IndexType pinIdx : _db.drDB().net(netIdx).pins())
        {
            const auto &pin = _db.drDB().pin(pinIdx);
            for (const auto &shape : pin.shapeRects())
            {
                /// build a rect consisting of the rectangle shape and the ID
                rectangle_type rect(shape.xLo(), shape.yLo(), shape.xHi(), shape.yHi());
                /// Use node index = INDEX_TYPE_MAX to represent this is a fixed pin shape
                rect.setShapeID(ShapeID(netIdx, INDEX_TYPE_MAX));
                /// Insert into the rtree
                _rtreeArray.at(pin.layer()).insert(rect);
            }
        }
    }
}

void ShapeMapRtree::constructRegion(const Box<LocType> &region, IndexType layer) 
{
    WRN("%s: Ignore the region construction command. Build the complete exsiting shape map with rtree\n", __FUNCTION__);
    for (IndexType netIdx = 0; netIdx < _db.drDB().nets().size(); ++netIdx)
    {
        const auto &net = _db.drDB().net(netIdx);
        for (IndexType nodeIdx = 0;  nodeIdx < net.numNodes(); ++nodeIdx)
        {
            /// Only build node with parent
            if (net.node(nodeIdx).hasParent())
            {
                const auto &shapeList = _shapeComp.computeShape(ShapeID(netIdx, nodeIdx));
                for (const auto &shape : shapeList) /// shape.first = Box .second = layer
                {
                    /// build a rect consisting of the rectangle shape and the ID
                    rectangle_type rect(shape.first.xLo(), shape.first.yLo(), shape.first.xHi(), shape.first.yHi());
                    rect.setShapeID(ShapeID(netIdx, nodeIdx));
                    /// Insert into the rtree
                    _rtreeArray.at(shape.second).insert(rect);
                }
            }
        }
    }
}

bool ShapeMapRtree::removeShape(ShapeID shapeID)
{
    Assert(_db.drDB().netNode(shapeID).hasParent());
    bool find = true;
    const auto &shapeList = _shapeComp.computeShape(shapeID);
    for (const auto &shape : shapeList) /// shape.first = Box .second = layer
    {
        /// build a rect consisting of the rectangle shape and the ID
        rectangle_type rect(shape.first.xLo(), shape.first.yLo(), shape.first.xHi(), shape.first.yHi());
        rect.setShapeID(shapeID);
        /// Remove from the rtree
        if (_rtreeArray.at(shape.second).remove(rect) == 0) /// return 1 if the value was removed, 0 otherwise.
        {
            find = false;
        }
    }
    return find;
}

bool ShapeMapRtree::insertShape(ShapeID shapeID)
{
    Assert(_db.drDB().netNode(shapeID).hasParent());
    const auto &shapeList = _shapeComp.computeShape(shapeID);
    for (const auto &shape : shapeList) /// shape.first = Box .second = layer
    {
        /// build a rect consisting of the rectangle shape and the ID
        rectangle_type rect(shape.first.xLo(), shape.first.yLo(), shape.first.xHi(), shape.first.yHi());
        rect.setShapeID(shapeID);
        /// Insert into the rtree
        _rtreeArray.at(shape.second).insert(rect);
        /// DEBUG
        if (shape.second == DRUtil::routeLayer2AllLayer(8))
        {
            std::vector<ShapeID> queryList;
            this->queryBox(shape.first,shape.second ,queryList);
            if (queryList.size() > 0)
            {
                for (auto overLap : queryList)
                {
                    const auto node1 = _db.drDB().netNode(shapeID);
                    const auto node2 = _db.drDB().netNode(overLap);
                    bool bothWire = !_db.drDB().netNode(shapeID).hasVia() && !_db.drDB().netNode(shapeID).hasVia();
                    if (bothWire)
                    {
                        if (node1.location().x() !=  node2.location().x() && node1.location().y() != node2.location().y())
                        {
                            DBG("%s: overlap %d %d and %d %d \n", __FUNCTION__, shapeID.netIdx, shapeID.nodeIdx, overLap.netIdx, overLap.nodeIdx);
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool ShapeMapRtree::renameShape(ShapeID oldShapeID, ShapeID newShapeID)
{
    Assert(_db.drDB().netNode(oldShapeID).hasParent());
    bool find = true;
    const auto &shapeList = _shapeComp.computeShape(oldShapeID);
    for (const auto &shape : shapeList) /// shape.first = Box .second = layer
    {
        /// build a rect consisting of the rectangle shape and the ID
        rectangle_type rect(shape.first.xLo(), shape.first.yLo(), shape.first.xHi(), shape.first.yHi());
        rect.setShapeID(oldShapeID);
        /// Remove from the rtree
        if (_rtreeArray.at(shape.second).remove(rect) == 0) /// return 1 if the value was removed, 0 otherwise.
        {
            find = false;
        }
        /// Insert into the rtree
        rect.setShapeID(newShapeID);
        _rtreeArray.at(shape.second).insert(rect);
    }
    return find;
}


bool ShapeMapRtree::clearRegion(const Box<LocType> &region)
{
    WRN("%s: Ignore the region construction command. Build the complete exsiting shape map with rtree\n", __FUNCTION__);
    return true;
}

void ShapeMapRtree::clear()
{
    for (auto &rtree : _rtreeArray)
    {
        rtree.clear();
    }
}

ShapeID ShapeMapRtree::nearest(const XY<LocType> &loc, IndexType layer, LocType range, ShapeID exclude, ShapeID exclude2) const 
{
    std::list<rectangle_type> nearest;
    point_type pt(loc.x(), loc.y());
    auto excludeFunc = [&] (const ShapeMapRtree::rectangle_type shape) { return shape.shapeID() != exclude;};
    auto excludeFunc2 = [&] (const ShapeMapRtree::rectangle_type shape) { return shape.shapeID() != exclude2;};
    if (_ignoreRoutedShape)
    {
        auto ignoreRoutedFunc = [&] (const ShapeMapRtree::rectangle_type shape) { return !shape.shapeID().isRouteShape() && shape.shapeID().net() != _netIdx;};
        _rtreeArray.at(layer).query(bgi::nearest(pt, 1) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2) && bgi::satisfies(ignoreRoutedFunc), std::back_inserter(nearest));
    }
    else
    {
        _rtreeArray.at(layer).query(bgi::nearest(pt, 1) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2), std::back_inserter(nearest));
    }
    /// check the nearest is not empty
    if (nearest.empty())
    {
        return ShapeID(INDEX_TYPE_MAX, INDEX_TYPE_MAX);
    }
    return nearest.front().shapeID();
}

ShapeID ShapeMapRtree::nearest(const XY<LocType> &loc, IndexType layer, LocType range, RouteDirType searchDir, ShapeID exclude, ShapeID exclude2) const
{
    /// This is a slow implementation. because of the limitation of rtree interface
    /// First build a segment to represent the search range
    point_type low, hi;
    if (searchDir == RouteDirType::HORIZONTAL)
    {
        low = point_type(loc.x() - range, loc.y());
        hi = point_type(loc.x() + range, loc.y());
    }
    else if (searchDir == RouteDirType::VERTICAL)
    {
        low = point_type(loc.x(), loc.y() - range);
        hi = point_type(loc.x(), loc.y() + range);
    }
    else 
    {
        Assert(false);
    }
    path_type searchRange(low, hi);
    /// Find all the intersect shapes
    std::list<rectangle_type> candidate;
    auto excludeFunc = [&] (const ShapeMapRtree::rectangle_type shape) { return shape.shapeID() != exclude;};
    auto excludeFunc2 = [&] (const ShapeMapRtree::rectangle_type shape) { return shape.shapeID() != exclude2;};
    if (_ignoreRoutedShape)
    {
        auto ignoreRoutedFunc = [&] (const ShapeMapRtree::rectangle_type shape) { return !shape.shapeID().isRouteShape() && shape.shapeID().net() != _netIdx;};
        _rtreeArray.at(layer).query(bgi::intersects(searchRange) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2) && bgi::satisfies(ignoreRoutedFunc), std::back_inserter(candidate));
    }
    else
    {
        _rtreeArray.at(layer).query(bgi::intersects(searchRange) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2), std::back_inserter(candidate));
    }

    /// check the candidate is not empty
    if (candidate.empty())
    {
        return ShapeID(INDEX_TYPE_MAX, INDEX_TYPE_MAX);
    }

    /// find the nearest one
    /// lambda function to compare the distance from the shapes to the loc
    auto func =  [&] (rectangle_type rect1, rectangle_type rect2) { 
        if (searchDir == RouteDirType::HORIZONTAL)
        {
            return std::min(abs(gtl::xl(rect1) - loc.x()), abs(gtl::xh(rect1) - loc.x())) < std::min(abs(gtl::xl(rect2) - loc.x()), abs(gtl::xh(rect2) - loc.x()));
        }
        else
        {
            return std::min(abs(gtl::yl(rect1) - loc.y()), abs(gtl::yh(rect1) - loc.y())) < std::min(abs(gtl::yl(rect2) - loc.y()), abs(gtl::yh(rect2) - loc.y()));
        }
    };

    return std::min_element(candidate.begin(), candidate.end(), func)->shapeID();
}

ShapeID ShapeMapRtree::segmentNearest(const ManSegment<LocType> &segment, IndexType layer, LocType range, ShapeID exclude, ShapeID exclude2) const
{
    /// First compute the search box
    point_type low, hi;
    if (segment.direction() == ManSegDirType::HORIZONTAL)
    {
        low = point_type(segment.begin(), segment.coordinate() - range);
        hi = point_type(segment.end(), segment.coordinate() + range);
    }
    else
    {
        low = point_type(segment.coordinate() - range, segment.begin());
        hi = point_type(segment.coordinate() + range, segment.end());
    }
    rectangle_query_type searchRange(gtl::x(low), gtl::y(low), gtl::x(hi), gtl::y(hi));
    /// Find all the intersect shapes
    std::list<rectangle_type> candidate;
    auto excludeFunc = [&] (const ShapeMapRtree::rectangle_type shape) { return shape.shapeID() != exclude;};
    auto excludeFunc2 = [&] (const ShapeMapRtree::rectangle_type shape) { return shape.shapeID() != exclude2;};
    auto ignoreRoutedFunc = [&] (const ShapeMapRtree::rectangle_type shape) { return !shape.shapeID().isRouteShape() && shape.shapeID().net() != _netIdx;};
    if (_ignoreRoutedShape)
    {
        _rtreeArray.at(layer).query(bgi::intersects(searchRange) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2) && bgi::satisfies(ignoreRoutedFunc), std::back_inserter(candidate));
    }
    else
    {
        _rtreeArray.at(layer).query(bgi::intersects(searchRange) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2), std::back_inserter(candidate));
    }

    /// check the candidate is not empty
    if (candidate.empty())
    {
        return ShapeID(INDEX_TYPE_MAX, INDEX_TYPE_MAX);
    }
    /// find the nearest one
    /// lambda function to compare the distance from the shapes to the loc
    auto func =  [&] (rectangle_type rect1, rectangle_type rect2) { 
        if (segment.direction() == ManSegDirType::VERTICAL)
        {
            return std::min(abs(gtl::xl(rect1) - segment.coordinate()), abs(gtl::xh(rect1) - segment.coordinate())) < std::min(abs(gtl::xl(rect2) - segment.coordinate()), abs(gtl::xh(rect2) - segment.coordinate()));
        }
        else
        {
            return std::min(abs(gtl::yl(rect1) - segment.coordinate()), abs(gtl::yh(rect1) - segment.coordinate())) < std::min(abs(gtl::yl(rect2) - segment.coordinate()), abs(gtl::yh(rect2) - segment.coordinate()));
        }
    };

    return std::min_element(candidate.begin(), candidate.end(), func)->shapeID();
}

void ShapeMapRtree::queryBox(const Box<LocType> &box, IndexType layer, std::vector<ShapeID> &results, ShapeID exclude, ShapeID exclude2)
{
    point_type low(box.xLo(), box.yLo());
    point_type hi(box.xHi(), box.yHi());
#if 0
    auto excludeFunc = [&] (const ShapeMapRtree::rectangle_type shape) { 
        if (exclude.isRouteShape() || exclude.isTerminal()) { if (exclude.net() == shape.shapeID().net()) { return false; }} 
        return shape.shapeID() != exclude;};
    auto excludeFunc2 = [&] (const ShapeMapRtree::rectangle_type shape) { 
        if (exclude2.isRouteShape() || exclude2.isTerminal()) { if (exclude2.net() == shape.shapeID().net()) { return false; }} 
        return shape.shapeID() != exclude2;};
#else
    auto excludeFunc = [&] (const ShapeMapRtree::rectangle_type shape) { 
        return shape.shapeID() != exclude;};
    auto excludeFunc2 = [&] (const ShapeMapRtree::rectangle_type shape) { 
        return shape.shapeID() != exclude2;};
#endif
    rectangle_query_type searchRange(gtl::x(low), gtl::y(low), gtl::x(hi), gtl::y(hi));
    std::list<rectangle_type> queryResults;
    if (_ignoreRoutedShape)
    {
        auto ignoreRoutedFunc = [&] (const ShapeMapRtree::rectangle_type shape) { return !shape.shapeID().isRouteShape() && shape.shapeID().net() != _netIdx;};
        _rtreeArray.at(layer).query(bgi::intersects(searchRange) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2) && bgi::satisfies(ignoreRoutedFunc), std::back_inserter(queryResults));
    }
    else
    {
        _rtreeArray.at(layer).query(bgi::intersects(searchRange) && bgi::satisfies(excludeFunc) && bgi::satisfies(excludeFunc2), std::back_inserter(queryResults));
    }
    for (auto &shape : queryResults)
    {
        results.emplace_back(shape.shapeID());
    }
}



/*------------------------------*/ 
/* Debug                        */
/*------------------------------*/ 
PROJECT_NAMESPACE_END

#include "writer/gdsii/WriteGds.h"
PROJECT_NAMESPACE_BEGIN
bool ShapeMapRtree::writeGds(const std::string &filename)
{
    auto _wg = std::make_shared<WriteGds>(filename);
    //_wg = std::shared_ptr<WriteGds>(new WriteGds(outputFile));
    if (!_wg->initWriter())
    {
        return false;
    }
    if (!_wg->createLib("TOP", _db.techDB().units().databaseNumber, 1e-6/_db.techDB().units().databaseNumber))
    {
        return false;
    }
    if (!_wg->writeCellBgn("RTREE"))
    {
        return false;
    }
    /// Iterate through the shapes in the r trees and write them into gds
    for (IndexType layer = 0; layer <  _rtreeArray.size(); ++layer)
    {
        const auto &rtree = _rtreeArray.at(layer);
        for (auto iter = rtree.begin(); iter != rtree.end(); ++iter)
        {
            IntType xl = gtl::xl(*iter); IntType yl = gtl::yl(*iter);
            IntType xh = gtl::xh(*iter); IntType yh = gtl::yh(*iter);
            Box<IntType> shape = Box<IntType> (xl, yl, xh, yh);
            _wg->writeRectangle(shape, layer, 0);
        }
    }
    
    if (!_wg->writeCellEnd())
    {
        return false;
    }
    if (!_wg->endLib())
    {
        return false;
    }
    DBG("%s: debug rtree shapes saved in %s \n", __FUNCTION__, filename.c_str());
    return true;
}
PROJECT_NAMESPACE_END
