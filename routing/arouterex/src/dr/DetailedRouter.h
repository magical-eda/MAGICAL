/**
 * @file DetailedRouter.h
 * @brief Detailed router
 * @author anoynmous anoynmous
 * @date 11/06/2018
 */


#ifndef AROUTER_DETAILEDTOUTER_H_
#define AROUTER_DETAILEDTOUTER_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

class DetailedRouter
{

    public:
        /// @brief constructor
        /// @param database of the router
        explicit DetailedRouter(Database &db);
        /// @brief Solve detailed routing using gridless A* search negotiation-based congestion routing
        /// @return if sucessful
        bool solveGridlessAstarNCR();
    private:
        /// @brief spliting the nets into branches by MST. Also spliting the self sym nets
        /// @return if successful
        bool splitNetsMST();
    private:
        Database &_db;
};


PROJECT_NAMESPACE_END

#endif ///AROUTER_DETAILEDTOUTER_H_
