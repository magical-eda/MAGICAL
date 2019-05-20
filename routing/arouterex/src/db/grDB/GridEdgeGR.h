/**
 * @file GridEdgeGR.h
 * @brief Grid edge for global routing
 * @author Keren Zhu
 * @date 10/04/2018
 */

#ifndef AROUTER_GRIDEDGEGR_H_
#define AROUTER_GRIDEDGEGR_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::GridEdgeGR
/// @brief include the subnet and capacity information. Don't have the geometric information
class GridEdgeGR
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief default constructor
        explicit GridEdgeGR() = default;
        /// @brief constructor with total capacity
        explicit GridEdgeGR(IntType totalCap) : _totalCap(totalCap) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _subNetIdxVec
        /// @return const reference. The vector of subnets that occupying this edge
        const std::set<IndexType> &     subNets() const  { return _subNetIdxSet; }
        /// @brief get _subNetIdxVec
        /// @return reference. The vector of subnets that occupying this edge
        std::set<IndexType> &           subNets()        { return _subNetIdxSet; }
        /// @brief get _totalCap
        /// @return the capacity this edge can contain
        IntType                         totalCap() const { return _totalCap; }
        /// @brief get _ocpCap
        /// @return the capacity the edge has been occupied
        IntType                         ocpCap() const   { return _ocpCap; }
        /// @brief get _distance
        /// @return the distance of this edge
        //IntType                         distance() const { return _distance; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set _totalCap
        /// @param the total capacity of the edge
        void    setTotalCap(IntType totalCap) { _totalCap = totalCap; }
        /// @brief set _ocpCap
        /// @param the amount of capacity this edge has been used
        void    setOcpCap(IntType ocpCap)     { _ocpCap = ocpCap; }
        /// @brief set _distance
        /// @param the distance of this edge
        //void    setDistance(IntType distance) { _distance = distance; }
        /*------------------------------*/ 
        /* Routing operations           */
        /*------------------------------*/ 
        /// @brief calculate the remainning capacity
        /// @return the remaining capacity this edge still has
        IntType         restCap() const              { return _totalCap - _ocpCap; }
        /// @brief whether this edge is overlowed
        /// @return true: is overflowing, false: is not
        bool            overflow() const             { return _ocpCap > _totalCap; }
        /// @brief calculate the overflowing capacity
        /// @return how much the edge is overflowing
        IntType         overflowCap() const          { return _ocpCap - _totalCap; }
        /// @brief whether there is still enough space for add some more
        /// @param the amount of capacity the addition need to take
        /// @return true: there is enough capacity left. false: not enough capacity left
        bool            enough(IntType useCap) const { return useCap + _ocpCap <= _totalCap; }
        /// @brief increment the used capacity
        /// @param how much of the capacity want to add
        void            addCap(IntType useCap)       { _ocpCap += useCap; }
        /// @brief add subnet and increase the occupied capacity
        /// @param subNetIdx: the index of subnet want to add. useCap: how many capacity it takes
        /// @return true: if no problem false if it already existing
        bool            occupy(IndexType subNetIdx, IntType useCap);
        /// @brief remove a subnet and remove its occupied capacity
        /// @param subNetIdx: the index of subnet want to remove, useCap: how many capacity it takes
        /// @return true: if success false if fails
        bool            remove(IndexType subNetIdx, IntType useCap);
        /// @brief clear the occupation of this edge: include the capacity and the subnet set
        void            clearOcp();
    private:
        std::set<IndexType> _subNetIdxSet; ///< the index of subnets occupying this edge
        IntType _totalCap = 0; ///< the capacity of this edge, could be via, horizontal or vertical capacity
        IntType _ocpCap   = 0; ///< How much of the capacity has been taken
        //IntType _distance = 1; ///< The distance of this edge
};

inline bool GridEdgeGR::occupy(IndexType subNetIdx, IntType useCap)
{
    bool notExistBefore = _subNetIdxSet.insert(subNetIdx).second;
    Assert(notExistBefore);
    if (notExistBefore)
    {
        addCap(useCap); 
    }
    return notExistBefore;
}

inline bool GridEdgeGR::remove(IndexType subNetIdx, IntType useCap)
{
    bool exisitBefore = _subNetIdxSet.erase(subNetIdx);
    if (exisitBefore)
    {
        _ocpCap -= useCap;
    }
    return exisitBefore;
}

inline void GridEdgeGR::clearOcp()
{
    _ocpCap = 0;
    _subNetIdxSet.clear();
}

PROJECT_NAMESPACE_END

#endif ///AROUTER_GRIDEDGEGR_H_
