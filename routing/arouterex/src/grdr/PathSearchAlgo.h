/**
 * @file PathSearchAlgo.h
 * @brief Base class for Path Search algorithm standard
 * @author Keren Zhu
 * @date 10/29/2018
 */

#ifndef AROUTER_PATH_SEARCH_ALGO_H_
#define AROUTER_PATH_SEARCH_ALGO_H_

#include "global/global.h"
#include "db/NetNode.h"
#include "db/SymNet.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::PathSearchAlgo
/// @brief the base class for path search algorithm
template<typename NodeKey>
class PathSearchAlgo
{
    public:
    /// @brief add a source
    virtual void addSourceAlgo(const NodeKey &nodeKey) = 0;
    /// @brief set the target
    virtual void setGoalAlgo(const NodeKey &nodeKey) = 0;
    /// @brief solve path search
    virtual bool solvePathSearchAlgo() = 0;
    /// @brief return the result as a vector of Node
    virtual const std::vector<NodeKey> & resultAlgo() = 0;
    /// @brief reset the path search solver
    virtual void resetAlgo() = 0;
    /// @brief get the NodeValue
    /// @param the NodeKey
    /// @return NodeValue corresponding to the NodeKey
    //virtual const NodeValue & nodeValueAlgo(const NodeKey &nodeKey) = 0;
};


/// @class AROUTER::PathSearchAlgoDR
/// @brief path search algorithm interface for Detailed routing
template<typename NodeKey>
class PathSearchDetailedRoutingInterface 
{
    public:
        /// @brief add a source
        /// @param first: the node identity for the source node to be added
        /// @param second: the shape identity for the source node to be added
        virtual void addSourceShapeAlgo(const NodeKey &nodeKey, ShapeID shapeID) = 0;
        /// @brief the via at the node. If no via, then it is a wire
        /// @param the NodeKey for the node
        /// @return index of via type. if INDEX_TYPE_MAX, then there is no via
        virtual IndexType viaType(const NodeKey &nodeKey) = 0;
        /// @brief set the net index that being routed
        /// @param the index of the net
        virtual void setNetIdx(IndexType netIdx) = 0;
        /// @brief set wire type
        /// @param the specification for the wire
        virtual void setWireType(const std::vector<NetNodeWire> &wire) = 0;
        /// @brief set effort mode
        /// @param how much effort the path search should do
        virtual void setEffortMode(IntType effort) = 0;
        /// @brief set source shapeID
        void setSourceShapeID(ShapeID shapeID) { _sourceShape = shapeID; }
        /// @brief set target shapeID
        void setTargetShapeID(ShapeID shapeID) { _targetShape = shapeID; }
        /// @brief debug interface
        /// @param the file name
        virtual void debugInterface(const std::string &fileName) = 0;
        /// @brief set whether waive global routing
        /// @param whether waive global rouing
        void setWaiveGr(bool waiveGr) { _waiveGr = waiveGr; }
        /// @brief get whether to waive global routing
        /// @return whether waive global routing
        bool waiveGr() const { return _waiveGr; } 
    protected:
        ShapeID _sourceShape = ShapeID();
        ShapeID _targetShape = ShapeID();
        bool _waiveGr = false; ///< Whether waive global routing
};

/// @class AROUTER::PathSearchAlgoAna
/// @brief path search algorithm for analog routing
class PathSearchAnalogInterface
{
    public:
        /// @brief set the symnet
        /// @param a Symnet object
        void setSymNet(const SymNet &symNet) { _symNet = symNet; }
        /// @brief set the routing mode as normal net no symmetry
        void setNoSym() { _symNet.setInvalid(); }
        /// @brief get the Symnet that is currently being routed
        /// @return the Symnet that is currently being routed
        const SymNet & currentSymNet() const { return _symNet; }
        /// @brief get the Symnet that is currently being routed
        /// @return the Symnet that is currently being routed
        SymNet & currentSymNet() { return _symNet; }
        /// @brief get whether the current mode is symmetry routing
        /// @return whether the current mode is symmetry routing
        bool isSymMode() const { return _symNet.valid(); }
    protected:
        SymNet _symNet; ///< The Symnet is currently being routed 
};

/// @class AROUTER::PathSearchAlgoAnaDR
/// @brief path search algorithm for analog detailed routing
template<typename NodeKey>
class PathSearchAlgoAnaDR :  public PathSearchAlgo<NodeKey>, virtual public PathSearchAnalogInterface, virtual public PathSearchDetailedRoutingInterface<NodeKey>
{
    public:
        /// @brief set the shape ID for the symmetry target
        /// @param the Shape ID for the symmetry target
        void setSymTargetShape(ShapeID shapeID) { _symTargetShape = shapeID; } 
        /// @brief add the source shape ID for the symmetry net. NOTE: the location is for the primary/default/before-flipping net, not the one for after the symmetry
        /// @param first: the node identity for the source node PRIMARY for SYMNET to be added
        /// @param second: the shape identity for the source node to be added SECONDARY FOR SYMNET
        virtual void addSymNodeSourceShape(NodeKey primaryNode, ShapeID secondaryShape) = 0;
        /// @brief return the result as a vector of Node for the seondary symmetry net
        virtual const std::vector<NodeKey> & symResultAlgo() = 0;
        /// @brief set the region for the goal, the algorithm can heurisitically search this region carefully for pin access
        void setGoalRegion(const Box<LocType> &goalRegion) { _goalRegion = goalRegion; }
        /// @brief set the source/target to the shape map with record shape (primary in the symmetry)
        virtual void configureShapeMapID(NodeKey from) = 0;
        /// @brief set the source/target to the shape map with record shape (secondary in symmetry)
        /// @param  the corresponding primary source node for the secondary shape source 
        virtual void configureShapeMapIDSym(NodeKey primFrom) = 0;
    protected:
        ShapeID _symTargetShape = ShapeID(); ///< The shape ID for the symmetry target
        Box<LocType> _goalRegion; ///< The region of target pin
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_PATH_SEARCH_ALGO_H_
