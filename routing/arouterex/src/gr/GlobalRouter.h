/**
 * @file GlobalRouter.h
 * @brief wrapper of global router
 * @author anoynmous anoynmous
 * @date 10/08/2018
 */

#ifndef AROUTER_GLOBALROUET_H_
#define AROUTER_GLOBALROUET_H_
#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::GlobalRouter
/// @brief wrapper of global routing
class GlobalRouter
{
    public:
        /// @brief constructor
        /// @param reference to the Database
        explicit GlobalRouter(Database &db) : _db(db) {}
        /// @brief core function for global routing
        /// @return if successful
        bool solve();
        /// @brief does not solve, but does init
        /// @return if successful
        bool skipSolve();

    private:
        /// @brief init the grDB 
        /// @return if successful
        bool init();
        /// @brief split the nets in to 2-pin subNets
        /// @return if successful
        bool splitSubNets();

        /*------------------------------*/ 
        /* Misc. init.                  */
        /*------------------------------*/ 
    public:
        /// @brief Initialize a SymNet, determine its symmetry type and symmetry axis/point
        /// @param Reference to a SymNet
        /// @return if initialization is successful
        bool initOneSymNet(SymNet &symNet);
    private:
        /*------------------------------*/ 
        /* Output print draw etc.       */
        /*------------------------------*/ 
        /// @brief write the usage of each grid cell to a file
        void writeGridCellUsage() const;
        
    private:
        Database &_db; /// reference to the database
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_GLOBALROUET_H_
