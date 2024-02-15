/*! @file db/NetPair.h
    @brief A pair of symmetry nets
    @author Mingjie Liu
    @date 12/06/2018
*/ 
#ifndef __NETPAIR_H__
#define __NETPAIR_H__

#include "global/type.h"

PROJECT_NAMESPACE_BEGIN
/*! @class NetPair
    @brief A pair of Net that are symmetric

    This class stores a pair of Net Id.
    A pair of net are symmetric if all Inst
    connected could be grouped to form 
    MosPair. This also contains self symmetry 
    pairs that are connected to DIFF_SOURCE.
*/
class NetPair
{
public:
/*! @brief Default Constructor. */
    explicit NetPair() = default;
/*! @brief Constructor for NetPair

    Sequence of Ids does not matter.

    @param netId1 Id for Net1
    @param netId2 Id for Net2
*/
    explicit NetPair(IndexType netId1, IndexType netId2)
        :_netId1(netId1), _netId2(netId2)
    {}

    // Getters
/*! @brief Get netId1. */
    IndexType               netId1() const                  { return _netId1; }
/*! @brief Get netId2. */
    IndexType               netId2() const                  { return _netId2; }

private:
    IndexType               _netId1, _netId2;
};

PROJECT_NAMESPACE_END

#endif
