/**
 * @file RoutingAlgo.h
 * @brief Base class for Routing algorithm standard
 * @author anoynmous anoynmous
 * @date 10/12/2018
 */

#ifndef AROUTER_ROUTINGALGO_H_
#define AROUTER_ROUTINGALGO_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::RoutingAlgo
/// @brief Base class for all routing algorithm
class RoutingAlgo
{
    public:
        /// @brief constructor
        /// @param the reference to the database
        explicit RoutingAlgo(Database &db) : _db(db) {}
        virtual ~RoutingAlgo() {}
        /// @brief run the kernal
        /// @return if successful
        virtual bool operator()() { return runKernel(); }
    protected:
        /// @brief kernal function for routing
        /// @return if successful
        virtual bool runKernel() = 0;
    protected:
        Database &_db; ///< The database
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_ROUTINGALGO_H_ 
