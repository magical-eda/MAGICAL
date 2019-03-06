/**
 * @file IntraCellRouter.h
 * @brief Connection for intra-cell pins
 * @author anoynmous anoynmous
 * @date 12/07/2018
 */

#ifndef AROUTER_INTRA_CELL_ROUTER_H_
#define AROUTER_INTRA_CELL_ROUTER_H_

#include "db/Database.h"
#include "dr/SearchInstruct.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::IntraCellRouter
/// @brief router for intra-cell connection
class IntraCellRouter
{
    public:
        explicit IntraCellRouter(Database &db) : _db(db), _instr(db.searchInstruct()) {  } 
        /*------------------------------*/ 
        /* Specified type connection    */
        /*------------------------------*/ 
        /// @brief perform the intra-cell routing for a standard mosfet
        /// @param gateTermIdx: the terminal index for the gates. drainTermIdx: the terminal index for the drain. sourceTermIdx: the terminal index for the source.
        /// @return if successful
        bool intraCellRouteCMOS(IndexType gateTermIdx, IndexType drainTermIdx, IndexType sourceTermIdx);
        /// @brief perform processing of the default/normal pin shape
        /// @param terminalIdx
        bool intraCellRouteDEFAULT(IndexType terminalIdx);
    private:
        /*------------------------------*/ 
        /* Specified type connection    */
        /*------------------------------*/ 
        /// @brief slice the pin shape and add more search points
        bool slicePinShape(IndexType terminalIdx, IndexType shapeIdx);
    private:
        Database &_db; ///< Database for the design
        SearchInstruct &_instr; ///< wrapper for the design rules
        
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_INTRA_CELL_ROUTER_H_
