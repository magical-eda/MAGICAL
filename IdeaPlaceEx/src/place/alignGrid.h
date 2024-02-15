/**
 * @file alignGrid.h
 * @brief The post processing step for aligning the placement to grid
 * @author Keren Zhu
 * @date 02/09/2020
 */

#ifndef IDEAPLACE_ALIGN_GRID_H_
#define IDEAPLACE_ALIGN_GRID_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @brief simple post processing to align the cells to some grid
class GridAligner
{
    public:
        /// @param the database for the placer
        explicit GridAligner(Database &db) 
            : _db(db) {}
        /// @brief solve the alignment problem
        /// @param the grid step. Assume uniform in both x and y
        void align(LocType stepSize);
        LocType findCurrentSymAxis();
    private:
        void naiveAlign();
        void bettherThanNaiveAlign();
        void adjustOffset(const XY<LocType> &offset);
        void adjustSymPair(const SymPair &symPair, LocType symAxis);
        void adjustSelfSym(IndexType cellIdx, LocType symAxis);
    private:
        Database &_db; ///< The placement database
        LocType _stepSize = 1;  ///< The grid step size
        XY<LocType> _offset; ///< The grid offset
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_ALIGN_GRID_H_
