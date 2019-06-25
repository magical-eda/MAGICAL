/**
 * @file GraphComponents.h
 * @brief Abstract components for the graph
 * @author Keren Zhu
 * @date 06/17/2019
 */

#ifndef MAGICAL_FLOW_GRAPH_COMPONENTS_H_
#define MAGICAL_FLOW_GRAPH_COMPONENTS_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class MAGICAL_FLOW::CktNode
/// @brief the abstracted node concepts for representing the physical components circuits
class CktNode 
{
    public:
        /// @brief default constructor
        explicit CktNode() = default; 
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the graph index of the subgraph this node representing
        /// @return the subgraph graph index
        IndexType subgraphIdx() const { return _graphIdx; }
        /// @brief get the array of pin indices this node has (at the current level of graph)
        /// @return the array of pin indices
        std::vector<IndexType> & pinIdxArray() { return _pinIdxArray; }
        /// @brief get the array of pin indices this node has (at the current level of graph)
        /// @return the array of pin indices
        const std::vector<IndexType> & pinIdxArray() const { return _pinIdxArray; }
        /// @brief get the number of pins this CktNode contains
        /// @return the number of pins this CktNode contains
        IndexType numPins() const { return _pinIdxArray.size(); }
        /// @brief get the index n-th pin of this node
        /// @return the index of n-th pin of this node
        IndexType pinIdx(IndexType nth) const { return _pinIdxArray.at(nth); }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the graph index of the subgraph this node representing
        /// @param the subgraph index
        void setSubgraphIdx(IndexType subgraphIdx) { _graphIdx = subgraphIdx; }
        /// @brief set if this node has been physical implemented
        /// @param if this node has been physical implemented
        void setIsImpl(bool isImpl)  {_implPhy = isImpl; }
        /*------------------------------*/ 
        /* Attributes                   */
        /*------------------------------*/ 
        /// @brief get the coordinate offset of this node
        /// @return the offset of this node
        //const XY<LocType> & offset() const { return _offset; }
        /// @brief get the coordinate offset of this node
        /// @return the offset of this node
        XY<LocType> & offset() { return _offset; }
        /// @brief get the orientation of this node
        /// @return the orientation of this node
        //const OriType & orient() const { return _orient; }
        /// @brief get the orientation of this node
        /// @return the orientation of this node
        OriType & orient() { return _orient; }
        /// @brief get whether this node has been physically implemented
        /// @return if this node has been physically implemented
        bool isImpl() const { return _implPhy; }
        /*------------------------------*/ 
        /* Vector operation             */
        /*------------------------------*/ 
        /// @brief append a pinIdx to the pinIdxArray
        /// @param a pinIdx
        void appendPinIdx(IndexType pinIdx) { _pinIdxArray.emplace_back(pinIdx); }
        /*------------------------------*/ 
        /* Graph Properties             */
        /*------------------------------*/ 
        /// @brief if this node is a leaf node
        /// @return whether this node is a leaf node in the graph. If not, it represents a subgraph
        bool isLeaf() const { return _graphIdx == INDEX_TYPE_MAX; }
    private:
        IndexType _graphIdx = INDEX_TYPE_MAX; ///< The index of the sub graph this node corresponding to. If INDEX_TYPE_MAX, then this is a leaf node
        std::vector<IndexType> _pinIdxArray; ///< The pins this node containing
        XY<LocType> _offset; ///< The offset of the location
        OriType _orient; ///< The orientation of this node
        bool _implPhy = false; ///< Whether this node has been implemented physically
};

/// @class MAGICAL_FLOW::Net
/// @brief the abstracted net concepts for representing the connectivity of the circuits
class Net
{
    public:
        /// @brief default constructor
        explicit Net() = default; 
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the array of pin indices that the net connecting
        /// @return the array of pin indices that the net connecting
        const std::vector<IndexType> & pinIdxArray() const { return _pinIdxArray; }
        /// @brief get the array of pin indices that the net connecting
        /// @return the array of pin indices that the net connecting
        std::vector<IndexType> & pinIdxArray() { return _pinIdxArray; }
        /// @brief get the number of pins this net is connecting
        /// @return the number of pins this net is connecting
        IndexType numPins() const { return _pinIdxArray.size(); }
        /// @brief get the n-th pin index of this net
        /// @return the n-th pin index of this net
        IndexType pinIdx(IndexType nth) const { return _pinIdxArray.at(nth); }
        /*------------------------------*/ 
        /* Attributes                   */
        /*------------------------------*/ 
    private:
        std::vector<IndexType> _pinIdxArray; ///< The indices of pins this nets connecting to
};

/// @class MAGICAL_FLOW::Pin
/// @brief the abstracted pin concepts for representing the pin components
class Pin
{
    public:
        /// @brief default constructor
        explicit Pin() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief node index this pin 
        /// @return the node index that this pin belonging to
        IndexType nodeIdx() const { return _nodeIdx; }
        /// @brief internal pin index of this pin
        /// @return the internal pin index of the node this pin corresponding to
        IndexType intPinIdx() const { return _intPinIdx; }
        /// @brief get the net indices array this pin related to 
        /// @return an array of the nets that this pin has relation with
        const std::vector<IndexType> & netIdxArray() const { return _netIdxArray; }
        /// @brief get the net indices array this pin related to 
        /// @return an array of the nets that this pin has relation with
        std::vector<IndexType> & netIdxArray() { return _netIdxArray; }
        /// @brief get how many nets connected to this pin (at the current level)
        /// @return the number of nets connected to this pin (at the current level)
        IndexType numNets() const { return _netIdxArray.size(); }
        /// @brief get one net index that connected to this pin
        /// @param the n-th net of this pin
        /// @return the net index
        IndexType netIdx(IndexType nth) const { return _netIdxArray.at(nth); }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the node index
        /// @param the node index this pin belonging to
        void setNodeIdx(IndexType nodeIdx) { _nodeIdx = nodeIdx; }
        /// @brief set the internal pin index
        /// @param set the internal pin index this pin corresponding to in the node it belonging to
        void setIntPinIdx(IndexType intPinIdx) { _intPinIdx = intPinIdx; }
    private:
        IndexType _nodeIdx = INDEX_TYPE_MAX; ///< The node index of the pin
        IndexType _intPinIdx = INDEX_TYPE_MAX; ///< The corresponding internal pin index in the internal node
        std::vector<IndexType> _netIdxArray; ///< The nets this pin corresponding to
};

PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_GRAPH_COMPONENTS_H_
