/**
 * @file BoostAStar.h
 * @brief Path search algorithm with boost A* star and graph
 * @author anoynmous anoynmous
 * @date 11/09/2018
 */

#include "util/solver/AStarSolver.h"
#include "PathSearchAlgo.h"
#include "GridlessAStarBase.h" /// NodeKey

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::BoostAStar
class BoostAStar : public PathSearchAlgo<GridlessAStarBase<IntType>::NodeKey>
{
    public:
    typedef GridlessAStarBase<IntType>::NodeKey NodeKey;
    explicit BoostAStar(IndexType xSize, IndexType ySize, IndexType zSize);
    /// @brief add a source
    void addSourceAlgo(const NodeKey &nodeKey) override { _solver.setSource(idx(nodeKey.loc.x()/ _trackWidth.at(nodeKey.layer), nodeKey.loc.y()/ _trackWidth.at(nodeKey.layer), nodeKey.layer)); }
    /// @brief set the target
    void setGoalAlgo(const NodeKey &nodeKey) override { _solver.setGoal(idx(nodeKey.loc.x() / _trackWidth.at(nodeKey.layer), nodeKey.loc.y() / _trackWidth.at(nodeKey.layer), nodeKey.layer)); }
    /// @brief solve path search
    bool solvePathSearchAlgo() override { return _solver.solve(); }
    /// @brief return the result as a vector of Node
    const std::vector<NodeKey> & resultAlgo() override; 
    /// @brief reset the path search solver
    void resetAlgo() override {}
    protected:
        IndexType idx(IndexType x, IndexType y, IndexType z) { return x + y * _xSize + z * _xSize *_ySize ;}
        XYZ<IndexType> xyz(IndexType idx) { return XYZ<IndexType>(idx % _xSize, idx / _xSize % _ySize, idx / _xSize / _ySize); }
        IndexType nodeKeyRelation(const NodeKey &node1, const NodeKey &node2);
    protected:
        klib::AStarSolver<CostType> _solver; ///< The boost astar solver
        IndexType _xSize;
        IndexType _ySize;
        IndexType _zSize;
        std::vector<NodeKey> _result;
        std::vector<LocType> _trackWidth = {140, 140, 140, 140, 140, 140, 140}; /// TODO:
};

inanoynmouse IndexType BoostAStar::nodeKeyRelation(const BoostAStar::NodeKey &node1, const BoostAStar::NodeKey &node2)
{
    if (node1.layer != node2.layer)
    {
        Assert(node1.loc == node2.loc);
        return 3;
    }
    if (node1.loc.x() != node2.loc.x())
    {
        Assert(node1.loc.y() == node2.loc.y());
        return 1;
    }
    Assert(node1.loc.y() != node2.loc.y());
    return 2;
}

inanoynmouse const std::vector<BoostAStar::NodeKey> & BoostAStar::resultAlgo() 
{
    _result.clear();
    for (IndexType idx : _solver.resultPath())
    {
        auto xyz = this->xyz(idx);
        XY<LocType> xy = XY<LocType>(xyz.x() * _trackWidth.at(xyz.z()), xyz.y() * _trackWidth.at(xyz.z()));
        _result.emplace_back(NodeKey(xy, xyz.z()));
    }
    if (_result.size() <= 2) { return _result; }
    std::vector<NodeKey> compressed;
    IndexType mode = 0; /// 1 horizontal, 2 vertical, 3, via
    
    compressed.emplace_back(_result.front()); /// The first node must be in
    mode = nodeKeyRelation(_result.at(0), _result.at(1)); /// Start from the relation between the first and the second node

    /// Iterate through the uncompressed result, and save the corner node
    for (IndexType nodeIdx = 1; nodeIdx < _result.size() - 1; ++nodeIdx)
    {
        /// For via, every child node must be added
        if (mode == 3)
        {
            compressed.emplace_back(_result.at(nodeIdx));
        }
        IndexType newMode = nodeKeyRelation(_result.at(nodeIdx), _result.at(nodeIdx + 1));
        if (newMode != mode)
        {
            /// if via, then it has been added. Otherwise, everytime the mode changed, the previous one must be maintained
            if (mode != 3)
            {
                compressed.emplace_back(_result.at(nodeIdx));
            }
            mode = newMode;
        }
    }
    /// The last one must be added
    compressed.emplace_back(_result.back());
    /// Move the compressed to result, so that _result now is the compressed one
    _result.clear(); ///< moveVector<>() insert to the end
    klib::moveVector<NodeKey>(compressed, _result);
    return _result;
}

inanoynmouse BoostAStar::BoostAStar(IndexType xSize, IndexType ySize, IndexType zSize)
    : _xSize(xSize), _ySize(ySize), _zSize(zSize)
{
    for (IndexType z = 0; z < _zSize; ++z)
    {
        for (IndexType y = 0;  y < _ySize; ++y)
        {
            for (IndexType x = 0; x < _xSize; ++x)
            {
                _solver.addVertex(x, y, z);
            }
        }
    }
    /// Undirected 
    for (IndexType z = 0; z < _zSize; ++z)
    {
        for (IndexType y = 0;  y < _ySize; ++y)
        {
            for (IndexType x = 0; x < _xSize - 1; ++x)
            {
                _solver.addEdge(idx(x, y, z), idx(x - 1, y, z), 1);
            }
        }
    }
    for (IndexType z = 0; z < _zSize; ++z)
    {
        for (IndexType x = 0;  x < _xSize; ++x)
        {
            for (IndexType y = 0; y < _ySize - 1; ++y)
            {
                _solver.addEdge(idx(x, y, z), idx(x, y - 1, z), 1);
            }
        }
    }
    for (IndexType x = 0; x < _xSize; ++x)
    {
        for (IndexType y = 0;  y < _ySize; ++y)
        {
            for (IndexType z = 0; z < _zSize - 1; ++z)
            {
                _solver.addEdge(idx(x, y, z), idx(x, y, z - 1), 1);
            }
        }
    }
}

class BoostAStarDR : public BoostAStar, public PathSearchAlgoDR<BoostAStar::NodeKey>
{
    public:
    explicit BoostAStarDR(IndexType xSize, IndexType ySize, IndexType zSize) : BoostAStar(xSize, ySize, zSize) {}
        /// @brief the via at the node. If no via, then it is a wire
        /// @param the NodeKey for the node
        /// @return index of via type. if INDEX_TYPE_MAX, then there is no via
        IndexType viaType(const NodeKey &nodeKey) override { return 0; }
        /// @brief set the net index that being routed
        /// @param the index of the net
        void setNetIdx(IndexType netIdx) override {}
        /// @brief add a source
        void addSourceAlgo(const NodeKey &nodeKey) override { _solver.setSource(idx(nodeKey.loc.x()/ _trackWidth.at(nodeKey.layer), nodeKey.loc.y()/ _trackWidth.at(nodeKey.layer), nodeKey.layer)); }
        /// @brief set the target
        void setGoalAlgo(const NodeKey &nodeKey) override { _solver.setGoal(idx(nodeKey.loc.x() / _trackWidth.at(nodeKey.layer), nodeKey.loc.y() / _trackWidth.at(nodeKey.layer), nodeKey.layer)); }
        /// @brief solve path search
        bool solvePathSearchAlgo() override { return _solver.solve(); }
        // @brief return the result as a vector of Node
        const std::vector<NodeKey> & resultAlgo() override {BoostAStar::resetAlgo(); }
        /// @brief reset the path search solver
        void resetAlgo() override { BoostAStar::resetAlgo(); }
};

PROJECT_NAMESPACE_END
