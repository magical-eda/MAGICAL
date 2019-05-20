/**
 * @file GlobalRoutingAlgo.h
 * @brief Base class for GLobal Routing algorithm
 * @author Keren Zhu
 * @date 10/12/2018
 */

#ifndef AROUTER_GLOBAL_ROUTINGALGO_H_
#define AROUTER_GLOBAL_ROUTINGALGO_H_

#include "grdr/RoutingAlgo.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::RoutingAlgo
/// @brief Base class for all routing algorithm
class GlobalRoutingAlgo : public RoutingAlgo
{
    public:
        /// @brief constructor
        /// @param the reference to the database
        explicit GlobalRoutingAlgo(Database &db) : RoutingAlgo(db) {}
        virtual ~GlobalRoutingAlgo() {}
        /// @brief run the kernal
        /// @return if successful
        virtual bool operator()() { return runKernel(); }
    protected:
        /// @brief kernal function for routing
        /// @return if successful
        virtual bool runKernel() = 0;
        /// @brief retrieve/search the path from one node to another node in the existing edges of a net
        /// @param first: the index of net. 
        /// @param second: the starting point of the search. 
        /// @param third: the target terminal
        /// @param fourth: a vector that should the results saved in
        /// @return a vector of nodes representing the path
        void retrieveNetPath(IndexType netIdx, const XYZ<IndexType> &from, IndexType termIdx, std::vector<XYZ<IndexType>> &results);
    protected:
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_GLOBAL_ROUTINGALGO_H_ 
