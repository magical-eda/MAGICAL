//
// Created by anoynmous on 2018/08/20
//

#include "db/drDB/DatabaseDR.h"

/// ================================================================================ 
/// SearchGraphConstructor.h
///
/// Used to construct the detailed routing search graph construction
/// Could use an external graph?
/// Here I just use a std::vector<other nodes>, like a simple list_graph
///
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

/// Use as the node in the searching graph
#ifndef USE_XYZ_NODEIDEN
class NodeIdentifier
{
    public:
        explicit NodeIdentifier() = default;
        explicit NodeIdentifier(IndexType idx2DP, IndexType layerP) : idx2D(idx2DP), layer(layerP) {} 

        bool operator< (const NodeIdentifier &rhs) const { return layer == rhs.layer? idx2D < rhs.idx2D : layer < rhs.layer; }
        bool operator== (const NodeIdentifier &rhs) const { return layer == rhs.layer && idx2D == rhs.idx2D; }

        std::string toStr() const
        {
            std::ostringstream oss;
            oss<< "("<<idx2D<<","<<layer<<")";
            return oss.str();
        }

        IndexType idx2D = 0;
        IndexType layer = 0;
};
#endif

#ifdef USE_XYZ_NODEIDEN
class NodeIdentifier
{
    public:
        explicit NodeIdentifier() = default;
        explicit NodeIdentifier(XYZ<IndexType> xyz) : _idx3D(xyz) {}
        explicit NodeIdentifier(XY<IndexType> xy, IndexType layer) : _idx3D(xy, layer) {}

        const XYZ<IndexType> & idx3D() const { return _idx3D; }
        XYZ<IndexType> & idx3D() { return _idx3D; }
        IndexType layer() const { return _idx3D.z(); }

        void setX(IndexType x) { _idx3D.setX(x); }
        void setY(IndexType y) { _idx3D.setY(y); }
        void setLayer(IndexType layer) { _idx3D.setZ(layer); }
        
        bool operator< (const NodeIdentifier &rhs) const { return this->idx3D() < rhs.idx3D(); }
        bool operator== (const NodeIdentifier &rhs) const { return this->idx3D() == rhs.idx3D(); }

        std::string toStr() const
        {
            std::ostringstream oss;
            oss<< "("<<idx3D().toStr()<<")";
            return oss.str();
        }

    private:
        XYZ<IndexType> _idx3D;
};
#endif

class SearchGraphConstructor
{
    public:
        explicit SearchGraphConstructor(DatabaseDR &db) : drDB(db) {}

        void clear() { _listGraph.clear(); }

        //////////////
        /// Getters
        //////////////
        const std::map<NodeIdentifier, std::set<NodeIdentifier>> & listGraph() const { return _listGraph; }
        std::map<NodeIdentifier, std::set<NodeIdentifier>> & listGraph() { return _listGraph; }

        /////////////////////////
        /// Construct the graph
        /////////////////////////
        void constructNetGraph(IndexType netIdx);
        
        ////////////////////////
        /// Print
        ////////////////////////
        std::string node2Str(const NodeIdentifier &node);

        /// Convert a xyidx to a Node
        NodeIdentifier xyidx2Node(XY<IndexType> xyIdx, IndexType layer) const;
        /// Convert a node to xyIdx
        XY<IndexType> node2XYIdx(const NodeIdentifier &node) const;
        XYZ<IndexType> node2XYZIdx(const NodeIdentifier &node) const;

        //////////////////////////////
        /// COnstruct the graph
        //////////////////////////////
        /// Add one GR grid cell to the graph and add vertical/horizontal edges to them
        void addGRGrid(const XYZ<IndexType> &grNode);

    private:

        //////////////////////////
        /// Construct the graph
        //////////////////////////
        /// Add edge between two GR grid cell
        void connectGRCell(const XYZ<IndexType> &grNodeFrom, const XYZ<IndexType> &grNodeTo);
        /// Add inter-layer edges to the graph
        void addInterLayerEdge(const XYZ<IndexType> &grNode, IndexType targetLayer);
        /// Add edges on the west/east/south/north boundary of a GR grid cell
        void addWestBoundaryEdge(const XYZ<IndexType> &grNode);
        void addEastBoundaryEdge(const XYZ<IndexType> &grNode);
        void addNorthBoundaryEdge(const XYZ<IndexType> &grNode);
        void addSouthBoundaryEdge(const XYZ<IndexType> &grNode);
    private:
        DatabaseDR &drDB;
        std::map<NodeIdentifier, std::set<NodeIdentifier>> _listGraph; /// Represent the graph as adjacent edges

};

inanoynmouse void SearchGraphConstructor::constructNetGraph(IndexType netIdx)
{

    clear();
    const auto &netDR = drDB.netArray().at(netIdx);
    ///std::map<XYZ<IndexType>, std::set<XYZ<IndexType>>>
    for (const auto &grSol : netDR.grid2GridGR())
    {
#ifdef DEBUG_SEARCH_GRAPH_
        DBG("Add grSol: %s \n", grSol.first.toStr().c_str());
#endif
        addGRGrid(grSol.first);

        for (const auto &adjGC : grSol.second)
        {
#ifdef DEBUG_SEARCH_GRAPH_
            DBG("Add edge: %s to %s \n", grSol.first.toStr().c_str(), adjGC.toStr().c_str());
#endif
            connectGRCell(grSol.first, adjGC);
        }
    }
}

inanoynmouse void SearchGraphConstructor::connectGRCell(const XYZ<IndexType> &grNodeFrom, const XYZ<IndexType> &grNodeTo)
{
    if (grNodeFrom.z() != grNodeTo.z())
    {
        Assert(grNodeFrom.xy() == grNodeTo.xy());
        addInterLayerEdge(grNodeFrom, grNodeTo.z());
    }
    else if (grNodeFrom.east() == grNodeTo)
    {
        addEastBoundaryEdge(grNodeFrom);
    }
    else if (grNodeFrom.south() == grNodeTo)
    {
        addSouthBoundaryEdge(grNodeFrom);
    }
    else if (grNodeFrom.west() == grNodeTo)
    {
        addWestBoundaryEdge(grNodeFrom);
    }
    else if (grNodeFrom.north() == grNodeTo)
    {
        addNorthBoundaryEdge(grNodeFrom);
    }
    else
    {
        AssertMsg(false, "Not adjacent grid cell, from %s to %s \n", grNodeFrom.toStr().c_str(), grNodeTo.toStr().c_str());
    }
}

inanoynmouse NodeIdentifier SearchGraphConstructor::xyidx2Node(XY<IndexType> xyIdx, IndexType layer) const 
{
#ifndef USE_XYZ_NODEIDEN
    IndexType idx = drDB.grid3D().gridVec2D(layer).xyIdx2FlatIdx(xyIdx);
    return NodeIdentifier(idx, layer);
#else
    return NodeIdentifier(xyIdx, layer);
#endif
}

inanoynmouse XY<IndexType> SearchGraphConstructor::node2XYIdx(const NodeIdentifier &node) const
{
#ifndef USE_XYZ_NODEIDEN
    return drDB.grid3D().gridVec2D(node.layer).flatIdx2XYIdx(node.idx2D);
#else
    return node.idx3D().xy();
#endif
}

inanoynmouse XYZ<IndexType> SearchGraphConstructor::node2XYZIdx(const NodeIdentifier &node) const
{
#ifndef USE_XYZ_NODEIDEN
    return XYZ<IndexType>(node2XYIdx(node), node.layer());
#else
    return node.idx3D();
#endif
}

inanoynmouse void SearchGraphConstructor::addGRGrid(const XYZ<IndexType> &grNode)
{


    /// Convert GR node to range of DR nodes
    Box<LocType> locBox = drDB.grIdx2Loc().xyIdx2Loc(grNode.xy());
    Box<IndexType> drIdxBox = drDB.grid3D().gridVec2D(grNode.z()).locBox2XYIdx(locBox);

    /// Add them to the graph
    for (IndexType xIdx = drIdxBox.xLo(); xIdx <= drIdxBox.xHi(); ++xIdx)
    {
        for (IndexType yIdx = drIdxBox.yLo(); yIdx <= drIdxBox.yHi(); ++yIdx)
        {
#ifndef USE_XYZ_NODEIDEN
            IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, yIdx));
            //_listGraph.insert(NodeIdentifier(idx2D, grNode.z()));
            _listGraph[NodeIdentifier(idx2D, grNode.z())]; /// default vector constructor
#else
            _listGraph[NodeIdentifier(grNode)];
#endif
        }
    }

    /// Add the edge in horizontal/vertical way
    if (drDB.grid3D().gridVec2D(grNode.z()).routeDir() == RouteDirType::HORIZONTAL)
    {
        for (IndexType xIdx = drIdxBox.xLo(); xIdx < drIdxBox.xHi(); ++xIdx)
        {
            for (IndexType yIdx = drIdxBox.yLo(); yIdx <= drIdxBox.yHi(); ++yIdx)
            {
#ifndef USE_XYZ_NODEIDEN
                IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, yIdx));
                IndexType idx2DNext = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx + 1, yIdx));
                _listGraph[NodeIdentifier(idx2D, grNode.z())].emplace(NodeIdentifier(idx2DNext, grNode.z()));
                _listGraph[NodeIdentifier(idx2DNext, grNode.z())].emplace(NodeIdentifier(idx2D, grNode.z()));
#else
                _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, yIdx, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx + 1, yIdx, grNode.z())));
                _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx + 1, yIdx, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx, yIdx, grNode.z())));
#endif
            }
        }
         
    }
    else if ((drDB.grid3D().gridVec2D(grNode.z()).routeDir() == RouteDirType::VERTICAL))
    {
        for (IndexType xIdx = drIdxBox.xLo(); xIdx <= drIdxBox.xHi(); ++xIdx)
        {
            for (IndexType yIdx = drIdxBox.yLo(); yIdx < drIdxBox.yHi(); ++yIdx)
            {
#ifndef USE_XYZ_NODEIDEN
                IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, yIdx));
                IndexType idx2DNext = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, yIdx + 1));
                _listGraph[NodeIdentifier(idx2D, grNode.z())].emplace(NodeIdentifier(idx2DNext, grNode.z()));
                _listGraph[NodeIdentifier(idx2DNext, grNode.z())].emplace(NodeIdentifier(idx2D, grNode.z()));
#else
                _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, yIdx, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx, yIdx + 1, grNode.z())));
                _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, yIdx + 1, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx, yIdx, grNode.z())));
#endif
            }
        }
    } /// end of if (drDB.grid3D().at(grNode.z()).routeDir() == RouteDirType::HORIZONTAL)
    else if (drDB.grid3D().gridVec2D(grNode.z()).routeDir() == RouteDirType::FORBID)
    {
        /// do nothing. Forbidded to route
    }
    else
    {
        AssertMsg(drDB.grid3D().gridVec2D(grNode.z()).routeDir() == RouteDirType::VERTICAL, "Haven't consider other routing direction yet");
    }


}

inanoynmouse void SearchGraphConstructor::addInterLayerEdge(const XYZ<IndexType> &grNode, IndexType targetLayer)
{
    Assert(klib::idxDif(grNode.z(), targetLayer) == 1);

    /// Convert GR node to range of DR nodes
    Box<LocType> locBox = drDB.grIdx2Loc().xyIdx2Loc(grNode.xy());
    Box<IndexType> drIdxBox = drDB.grid3D().gridVec2D(grNode.z()).locBox2XYIdx(locBox);

    for (IndexType xIdx = drIdxBox.xLo(); xIdx <= drIdxBox.xHi(); ++xIdx)
    {
        for (IndexType yIdx = drIdxBox.yLo(); yIdx <=  drIdxBox.yHi(); ++yIdx)
        {
#ifndef USE_XYZ_NODEIDEN
            IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, yIdx)); /// idx on the source layer
#else
#endif
            /// use layer2layer to add the edge (return std::vector<XY<IndexType>>)
            for (const auto &targetXYIdx : drDB.grid3D().layer2Layer(grNode.z(), targetLayer, XY<IndexType>(xIdx, yIdx)))
            {
#ifndef USE_XYZ_NODEIDEN
                IndexType idx2DNext = drDB.grid3D().gridVec2D(targetLayer).xyIdx2FlatIdx(targetXYIdx);
                _listGraph[NodeIdentifier(idx2D, grNode.z())].emplace(NodeIdentifier(idx2DNext, targetLayer));
                _listGraph[NodeIdentifier(idx2DNext, targetLayer)].emplace(NodeIdentifier(idx2D, grNode.z()));
#else
                _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, yIdx, grNode.z()))].emplace(NodeIdentifier(targetXYIdx, targetLayer));
                _listGraph[NodeIdentifier(targetXYIdx, targetLayer)].emplace(XYZ<IndexType>(xIdx, yIdx, grNode.z()));
#endif
            }
        }
    }
}

inanoynmouse void SearchGraphConstructor::addWestBoundaryEdge(const XYZ<IndexType> &grNode) 
{

    Assert(grNode.x() > 0);

    /// Convert GR node to range of DR nodes
    Box<LocType> locBox = drDB.grIdx2Loc().xyIdx2Loc(grNode.xy());
    Box<IndexType> drIdxBox = drDB.grid3D().gridVec2D(grNode.z()).locBox2XYIdx(locBox);

    for (IndexType yIdx = drIdxBox.yLo(); yIdx <= drIdxBox.yHi(); ++yIdx)
    {
#ifndef USE_XYZ_NODEIDEN
        IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(drIdxBox.xLo(), yIdx));
        IndexType idx2DNext = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(drIdxBox.xLo() - 1, yIdx));
        _listGraph[NodeIdentifier(idx2D, grNode.z())].emplace(NodeIdentifier(idx2DNext, grNode.z()));
        _listGraph[NodeIdentifier(idx2DNext, grNode.z())].emplace(NodeIdentifier(idx2D, grNode.z()));
#else
        _listGraph[NodeIdentifier(XYZ<IndexType>(drIdxBox.xLo(), yIdx, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(drIdxBox.xLo() - 1, yIdx, grNode.z())));
        _listGraph[NodeIdentifier(XYZ<IndexType>(drIdxBox.xLo() - 1, yIdx, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(drIdxBox.xLo(), yIdx, grNode.z())));
#endif
    }
}


inanoynmouse void SearchGraphConstructor::addEastBoundaryEdge(const XYZ<IndexType> &grNode) 
{
    Assert(grNode.x() < drDB.grIdx2Loc().xGridsSize() - 1);
#ifdef DEBUG_SEARCH_GRAPH_
    DBG("East of %s \n", grNode.toStr().c_str());
#endif

    /// Convert GR node to range of DR nodes
    Box<LocType> locBox = drDB.grIdx2Loc().xyIdx2Loc(grNode.xy());
    Box<IndexType> drIdxBox = drDB.grid3D().gridVec2D(grNode.z()).locBox2XYIdx(locBox);

    for (IndexType yIdx = drIdxBox.yLo(); yIdx <= drIdxBox.yHi(); ++yIdx)
    {
#ifndef USE_XYZ_NODEIDEN
        IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(drIdxBox.xHi(), yIdx));
        IndexType idx2DNext = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(drIdxBox.xHi() + 1, yIdx));
        _listGraph[NodeIdentifier(idx2D, grNode.z())].emplace(NodeIdentifier(idx2DNext, grNode.z()));
        _listGraph[NodeIdentifier(idx2DNext, grNode.z())].emplace(NodeIdentifier(idx2D, grNode.z()));
#else
        _listGraph[NodeIdentifier(XYZ<IndexType>(drIdxBox.xHi(), yIdx, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(drIdxBox.xHi() + 1, yIdx, grNode.z())));
        _listGraph[NodeIdentifier(XYZ<IndexType>(drIdxBox.xHi() - 1, yIdx, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(drIdxBox.xHi(), yIdx, grNode.z())));
#endif
    }
}

inanoynmouse void SearchGraphConstructor::addNorthBoundaryEdge(const XYZ<IndexType> &grNode) 
{
    Assert(grNode.y() < drDB.grIdx2Loc().yGridsSize() - 1);

    /// Convert GR node to range of DR nodes
    Box<LocType> locBox = drDB.grIdx2Loc().xyIdx2Loc(grNode.xy());
    Box<IndexType> drIdxBox = drDB.grid3D().gridVec2D(grNode.z()).locBox2XYIdx(locBox);

    for (IndexType xIdx = drIdxBox.xLo(); xIdx <= drIdxBox.xHi(); ++xIdx)
    {
#ifndef USE_XYZ_NODEIDEN
        IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, drIdxBox.yHi()));
        IndexType idx2DNext = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, drIdxBox.yHi() + 1));
        _listGraph[NodeIdentifier(idx2D, grNode.z())].emplace(NodeIdentifier(idx2DNext, grNode.z()));
        _listGraph[NodeIdentifier(idx2DNext, grNode.z())].emplace(NodeIdentifier(idx2D, grNode.z()));
#else
        _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yHi(), grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yHi() + 1, grNode.z())));
        _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yHi() + 1, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yHi(), grNode.z())));
#endif
    }
}

inanoynmouse void SearchGraphConstructor::addSouthBoundaryEdge(const XYZ<IndexType> &grNode) 
{
    Assert(grNode.y() > 0);

    /// Convert GR node to range of DR nodes
    Box<LocType> locBox = drDB.grIdx2Loc().xyIdx2Loc(grNode.xy());
    Box<IndexType> drIdxBox = drDB.grid3D().gridVec2D(grNode.z()).locBox2XYIdx(locBox);

    for (IndexType xIdx = drIdxBox.xLo(); xIdx <= drIdxBox.xHi(); ++xIdx)
    {
#ifndef USE_XYZ_NODEIDEN
        IndexType idx2D = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, drIdxBox.yLo()));
        IndexType idx2DNext = drDB.grid3D().gridVec2D(grNode.z()).xyIdx2FlatIdx(XY<IndexType>(xIdx, drIdxBox.yLo() - 1));
        _listGraph[NodeIdentifier(idx2D, grNode.z())].emplace(NodeIdentifier(idx2DNext, grNode.z()));
        _listGraph[NodeIdentifier(idx2DNext, grNode.z())].emplace(NodeIdentifier(idx2D, grNode.z()));
#else
        _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yLo(), grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yLo() - 1, grNode.z())));
        _listGraph[NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yLo() - 1, grNode.z()))].emplace(NodeIdentifier(XYZ<IndexType>(xIdx, drIdxBox.yLo(), grNode.z())));
#endif
    }
}


inanoynmouse std::string SearchGraphConstructor::node2Str(const NodeIdentifier &node)
{
    std::ostringstream oss;
#ifndef USE_XYZ_NODEIDEN
    XY<IndexType> xyIdx = drDB.grid3D().gridVec2D(node.layer).flatIdx2XYIdx(node.idx2D);
    oss << " ("<< xyIdx.x()<<","<<xyIdx.y()<<","<<node.layer<<") ";
#else
    oss << node.idx3D().toStr();
#endif
    return oss.str();
}

PROJECT_NAMESPACE_END
