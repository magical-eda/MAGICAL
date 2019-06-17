/**
 * @file CktGraph.h
 * @brief A graph for the implementation of circuit at one level of hierarchy
 * @author Keren Zhu
 * @date 06/17/2019
 */

#ifndef MAGICAL_FLOW_CKTGRAPH_H_
#define MAGICAL_FLOW_CKTGRAPH_H_

#include "GraphComponents.h"


PROJECT_NAMESPACE_BEGIN

/// @class MAGICAL_FLOW::CktGraph
/// @brief each CktGraph represent a level of circuit in the hierarchical flow
class CktGraph
{
    public:
        /// @brief default construtor
        explicit CktGraph() = default; 
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the array of circuit nodes
        /// @return the array of circuit nodes
        const std::vector<CktNode> &                                nodeArray() const                                   { return _nodeArray; }
        /// @brief get the array of circuit nodes
        /// @return the array of circuit nodes
        std::vector<CktNode> &                                      nodeArray()                                         { return _nodeArray; }
        /// @brief get the number of nodes
        /// @return the number of nodes this graph has
        IndexType                                                   numNodes() const                                    { return _nodeArray.size(); }
        /// @brief get a circuit node of this graph
        /// @param the index of node
        /// @return the circuit node
        const CktNode &                                             node(IndexType nodeIdx) const                       { return _nodeArray.at(nodeIdx); }
        /// @brief get a circuit node of this graph
        /// @param the index of node
        /// @return the circuit node
        CktNode &                                                   node(IndexType nodeIdx)                             { return _nodeArray.at(nodeIdx); }
        /// @brief get the array of pins
        /// @return the array of pins
        const std::vector<Pin> &                                    pinArray() const                                    { return _pinArray; }
        /// @brief get the array of pins
        /// @return the array of pins
        std::vector<Pin> &                                          pinArray()                                          { return _pinArray; }
        /// @brief get the number of pins
        /// @return the number of pins
        IndexType                                                   numPins() const                                     { return _pinArray.size(); }
        /// @brief get a pin of this graph
        /// @param the index of the pin of this graph
        /// @return the pin object
        const Pin &                                                 pin(IndexType pinIdx) const                         { return _pinArray.at(pinIdx); }
        /// @brief get a pin of this graph
        /// @param the index of the pin of this graph
        /// @return the pin object
        Pin &                                                       pin(IndexType pinIdx)                               { return _pinArray.at(pinIdx); }
        /// @brief get the array of nets of this graph
        /// @return the array of nets of this graph
        const std::vector<Net> &                                    netArray() const                                    { return _netArray; }
        /// @brief get the array of nets of this graph
        /// @return the array of nets of this graph
        std::vector<Net> &                                          netArray()                                          { return _netArray; }
        /// @brief get the number of nets this graph contains
        /// @return the number of nets this graph contains
        IndexType                                                   numNets() const                                     { return _netArray.size(); }
        /// @brief get a net of this graph
        /// @param the index of net in this graph
        /// @return a net
        const Net &                                                 net(IndexType netIdx) const                         { return _netArray.at(netIdx); }
        /// @brief get a net of this graph
        /// @param the index of net in this graph
        /// @return a net
        Net &                                                       net(IndexType netIdx)                               { return _netArray.at(netIdx); }
    private:
        std::vector<CktNode> _nodeArray; ///< The circuit nodes of this graph
        std::vector<Pin> _pinArray; ///< The pins of the circuit
        std::vector<Net> _netArray; ///< The nets of the circuit
};

PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_CKTGRAPH_H_
