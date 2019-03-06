// 
// Created by anoynmous on 04/06/2018
//
//

#ifndef AROUTER_CONSTANT_H_
#define AROUTER_CONSTANT_H_

#include "type.h"

PROJECT_NAMESPACE_BEGIN

constexpr char OUTPUT_DIRECTORY[] = "./output/"; ///< The directory for the output files
constexpr char GNUPLOT_FILENAME[] = "GNUPLOT_COMMAND"; ///< gnu output file name
constexpr IndexType PERCENT_ACC = 100; ///< avoid using float, and use PERCENT_ACC * ratio, not neccessary be 100
constexpr IntType DUMMY_BLOCKAGE_LAYER = 155; ///< The layer for dummy blockage
constexpr IntType DUMMY_BLOCKAGE_DATATYPE = 100; ///< The data type for the dummt blockage
constexpr IntType POBLK_LAYER = 150; ///< POBLK layer
constexpr IntType POBLK_DATATYPE = 20; ///< POBLK datatype
PROJECT_NAMESPACE_END

#endif // AROUTER_CONSTANT_H_


