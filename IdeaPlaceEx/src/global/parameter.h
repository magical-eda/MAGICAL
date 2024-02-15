/**
 * @file parameter.h
 * @brief Define some hyperparameters
 * @author Keren Zhu
 * @date 09/30/2019
 */

#ifndef IDEAPLACE_PARAMETER_H_
#define IDEAPLACE_PARAMETER_H_

#include "type.h"

PROJECT_NAMESPACE_BEGIN

/* NLP wn conj */
constexpr RealType LAMBDA_1Init = 1; ///< The initial value for x/y overlapping penalty
constexpr RealType LAMBDA_2Init = 1; ///< The initial value for out of boundry penalty
constexpr RealType LAMBDA_3Init = 16; ///< The initial value for wirelength penalty
constexpr RealType LAMBDA_4Init = 1; ///< The initial value for asymmetric penalty
constexpr RealType LAMBDA_MAX_OVERLAP_Init = 0; ///< The initial value for asymmetric penalty
constexpr RealType NLP_WN_CONJ_OVERLAP_THRESHOLD = 0.001; ///< The threshold for whether increase the penalty for overlapping
constexpr RealType NLP_WN_CONJ_OOB_THRESHOLD = 0.05; ///< The threshold for wehther increasing the penalty for out of boundry
constexpr RealType NLP_WN_CONJ_ASYM_THRESHOLD = 2.5; ///< The threshodl for whether increase the penalty for asymmetry
constexpr RealType NLP_WN_CONJ_ALPHA = 1; ///< "alpha" should be a very small value. Used in objective function
constexpr RealType NLP_WN_CONJ_EXP_DECAY_TARGET = 0.05; ///< The exponential decay facor for global placement step size. 
constexpr IndexType NLP_WN_CONJ_DEFAULT_MAX_ITER = 20; ///< The default maxmimum iterations for the global placement
constexpr RealType NLP_WN_CONJ_EPISLON = 200 / NLP_WN_CONJ_DEFAULT_MAX_ITER ;
constexpr RealType NLP_WN_CONJ_DEFAULT_MAX_WHITE_SPACE = 2; ///< The default extra white space for setting the boundry
constexpr RealType NLP_WN_MAX_PENALTY = 1024;
constexpr RealType NLP_WN_REDUCE_PENALTY = 512 / NLP_WN_MAX_PENALTY ;

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PARAMETER_H_
