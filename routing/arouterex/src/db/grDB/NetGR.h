/**
 * @file NetGR.h
 * @brief Net class for global router
 * @author Keren Zhu
 * @date 10/07/2018
 */

#ifndef AROUTER_NETGR_H_
#define AROUTER_NETGR_H_

#include "global/global.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::NetGR
/// @brief Net class for the global router
class NetGR
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit NetGR() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _name
        /// @return the name for the net
        const std::string &                 name() const            { return _name; }
        /// @brief get _benchmarkIdx
        /// @return the index of the net read from the raw benchmark
        IndexType                           benchNetIdx() const     { return _benchNetIdx; }
        /// @brief get _gridBBox
        /// @return const reference to the bounding box of the net in the unit of grid cell index
        const Box<IndexType> &              gridBBox() const        { return _gridBBox; }
        /// @brief get _gridBBox
        /// @return reference to the bounding box of the net in the unit of grid cell index
        Box<IndexType> &                    gridBBox()              { return _gridBBox; }
        /// @brief get _subNetVec
        /// @return const reference to the vector of subNet-2 pin nets indices
        const std::vector<IndexType> &      subNets() const         { return _subNetVec; }
        /// @brief get _subNetVec
        /// @return reference to the vector of subNet-2 pin nets indices
        std::vector<IndexType> &            subNets()               { return _subNetVec; }
        /// @brief get _physPinArray
        /// @return const reference the vector of physical pins. For most cases, use the wrapper pins() instead of this one
        const std::vector<IndexType> &      physPinArray() const    { return _physPinArray; }
        /// @brief get _physPinArray
        /// @return reference the vector of physical pins. For most cases, use the wrapper pins() instead of this one
        std::vector<IndexType> &            physPinArray()          { return _physPinArray; }
        /// @brief get _virtualPinVec
        /// @return const reference the vector of steiner pins. For most cases, use the wrapper pins() instead of this one
        const std::vector<IndexType> &      virtualPinArray() const { return _virtualPinVec; }
        /// @brief get _physPinVec
        /// @return reference the vector of steiner pins. For most cases, use the wrapper pins() instead of this one
        std::vector<IndexType> &            virtualPinArray()       { return _virtualPinVec; }
        /// @brief get _terminalArray
        /// @return the array for the terminal indices
        const std::vector<IndexType> &      terminalArray() const   { return _terminalArray; }
        /// @brief get _terminalArray
        /// @return the array for the terminal indices
        std::vector<IndexType> &            terminalArray()         { return _terminalArray; }
        /// @brief get the number of terminal
        /// @return the number of terminals
        IndexType numTerminals() const { return _terminalArray.size(); }
        /// @brief get _isSym
        /// @return bool: whether this net in sym net pair
        bool                                isSym() const           { return _symNetIdx != INDEX_TYPE_MAX; }
        /// @brief whether self symmetry
        bool                                isSelfSym() const       { return _selfSymNetIdx != INDEX_TYPE_MAX; }
        /// @brief get symNet index
        /// @return the index of SymNet
        IndexType                           symNetIdx() const       { return _symNetIdx; }
        /// @brief get the self symmetry net index
        /// @return the index of the self symmetry net
        IndexType                           selfSymNetIdx() const { return _selfSymNetIdx; }
        /// @brief get _capUsage
        /// @return the amount of edge capacity this net takes
        IntType                             capUse() const          { return _capUsage; }
        /// @brief get _isRouted
        /// @return whether this net has been routed
        bool isRouted() const { return _isRouted; }
        /// @brief get net weight
        /// @return the weight of the net
        IntType weight() const { return _weight; }
        /// @brief get whether waive the global routing for this net
        /// @return whether waive the global routing for this net
        bool grWaived() const { return _grWaived; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set _name
        /// @param the name of this net
        void setName(const std::string &name)  { _name = name; }
        /// @brief set weight of the net
        /// @param the weight of the net
        void setWeight(IntType weight) { _weight = weight; }
        /// @brief set _symNetIdx
        /// @param the index of the net this one is symmetric to
        void setSymNetIdx(IndexType symNetIdx) { _symNetIdx = symNetIdx; }
        /// @brief remove symmetry constraint from this net
        void invalidateSym() { _symNetIdx = INDEX_TYPE_MAX; }
        /// @brief set self sym net index
        void setSelfSymIdx(IndexType idx) { _selfSymNetIdx  = idx; }
        /// @brief remove the self symmetry
        void invalidSelfSym() { _selfSymNetIdx = INDEX_TYPE_MAX; }
        /// @brief set the index of this net read from the benchmark
        /// @param the index of this net read from the benchmark
        void setBenchmarkNetIdx(IndexType benchNetIdx) { _benchNetIdx = benchNetIdx; }
        /// @brief set _capUsage
        /// @param the amount of edge capacity this net takes
        void setCapUse(IntType capUse) { _capUsage = capUse; }
        /// @brief set _isRouted
        /// @param whether this net has been routed
        void setIsRouted(bool isRouted) { _isRouted = isRouted; }
        /// @brief add a terminal
        /// @param the index of the terminal
        void addTerminal(IndexType terminalIdx) { _terminalArray.emplace_back(terminalIdx); }
        /// @brief set whether waive this net from global routing
        /// @param whether waive this net from global routing
        void setWaiveGR(bool grWaived) { _grWaived = grWaived; }
        
        /*------------------------------*/ 
        /* Sizes                        */
        /*------------------------------*/ 
        /// @brief the number of subnets
        /// @return the number of subnets
        IndexType numSubNets() const     { return _subNetVec.size(); }
        /// @brief the number of physical pins
        /// @return the number of physical pins
        IndexType numPhyPins() const     { return _physPinArray.size(); }
        /// @brief the number of steiner points
        /// @return the number of the steiner points
        IndexType numVirtualPins() const { return _virtualPinVec.size(); }
        /// @brief the number of "pins"
        /// @return the number of "pins"
        IndexType numPins() const        { return this->numPhyPins() + this->numVirtualPins(); }

        
    private:
        std::string _name = ""; ///< the name of the net
        IndexType _benchNetIdx = INDEX_TYPE_MAX; ///< The index of the net read from the benchmark
        Box<IndexType> _gridBBox; ///< the bounding box of this net in the unit of grid cell index
        std::vector<IndexType> _subNetVec; ///< the vector of subnets. This is used in 2-pin net-based routing
        std::vector<IndexType> _physPinArray; ///< the vector for the pins. Note that this include only the physical pins. Size should be fixed
        std::vector<IndexType> _virtualPinVec; ///< the vector for the steiner points
        IndexType _symNetIdx = INDEX_TYPE_MAX; ///< The index of the symNet
        IndexType _selfSymNetIdx = INDEX_TYPE_MAX; ///< The index of the self sym net
        IntType _capUsage = 1; ///< how many capacity this net use 
        bool _isRouted = false; ///< if this net has been routed
        std::vector<IndexType> _terminalArray; ///< The vector for the terminals
        IntType _weight = 0; ///< The weight of this net
        bool _grWaived = false; ///< Whether this net is waived from global routing solution (basically no feasible solution found by GR)
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_NETGR_H_
