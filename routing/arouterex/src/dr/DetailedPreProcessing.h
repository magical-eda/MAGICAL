/**
 * @file DetailedPreProcessing.h
 * @brief Some data preprocessing for the detailed routing
 * @author anoynmous anoynmous
 * @date 12/04/2018
 */

#ifndef AROUTER_DETAILED_PREPROCESSING_H_
#define AROUTER_DETAILED_PREPROCESSING_H_

#include "DRBase.h"
#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::DetailedPreProcessing
/// @brief Proprocessing for some data preparation
class DetailedPreProcessing : public DRBase
{
    public:
        explicit DetailedPreProcessing(Database &db) : DRBase(db), _instr() { _instr.init(db); }

        /*------------------------------*/ 
        /* Processing Pins              */
        /*------------------------------*/ 
    public:
        /// @brief process pins
        /// @return if successful
        bool processPins();
        /// @brief add pins to the global routing solution
        /// @return if successful
        bool addPinToGlobalResult();
    private:
        /// @brief slice the pin shapes into several slicing 
        /// @return if sucessful
        bool sliceSearchPoint(IndexType pinIdx);
    private:
        SearchInstruct _instr; ///< Some wrappered technology information
};
PROJECT_NAMESPACE_END
#endif ///AROUTER_DETAILED_PREPROCESSING_H_
