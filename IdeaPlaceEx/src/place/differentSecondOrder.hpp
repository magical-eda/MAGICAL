/**
 * @file differentSecondOrder.h
 * @brief The numerical differentiable concepts and implementations for second-order derivatives.
 * @author Keren Zhu
 * @date 04/21/2020
 */

#pragma once

#include "different.h"

PROJECT_NAMESPACE_BEGIN

namespace diff
{

    template<typename hessian_type>
    struct is_diagnol_matrix : std::false_type {};


    template<typename operator_type>
    struct jacobi_hessian_approx_trait {};

    template<typename operator_type>
    struct is_diagnol_matrix<jacobi_hessian_approx_trait<operator_type>> : std::true_type {};


    template<typename NumType, typename CoordType>
    struct jacobi_hessian_approx_trait<LseHpwlDifferentiable<NumType, CoordType>>
    {
        typedef LseHpwlDifferentiable<NumType, CoordType> operator_type;
        static void accumulateHessian(const operator_type & op, const std::function<void(NumType, IndexType, IndexType, Orient2DType, Orient2DType)> &accumulateHessianFunc)
        {
            const NumType alpha = op._getAlphaFunc();
            const NumType lambda = op._getLambdaFunc();

            // first compute the exp sums
            NumType exp_xi_sum  = 0.0; // sum (exp(xi / alpha))
            NumType exp_neg_xi_sum = 0.0; // sum(exp(-x / alpha))
            NumType exp_yi_sum  = 0.0; // sum (exp(yi / alpha))
            NumType exp_neg_yi_sum = 0.0; // sum(exp(-y / alpha))
            for (IndexType pinIdx = 0; pinIdx < op._cells.size(); ++pinIdx)
            {
                const NumType x = op::conv<NumType>(
                        op._getVarFunc(op._cells[pinIdx], Orient2DType::HORIZONTAL) + op._offsetX[pinIdx]
                        );
                const NumType y = op::conv<NumType>(
                        op._getVarFunc(op._cells[pinIdx], Orient2DType::VERTICAL) + op._offsetY[pinIdx]
                        );
                exp_xi_sum += std::exp(x / alpha);
                exp_neg_xi_sum += std::exp(-x / alpha);
                exp_yi_sum += std::exp(y / alpha);
                exp_neg_yi_sum += std::exp(-y / alpha);
            }
            if (op._validVirtualPin == 1)
            {
                exp_xi_sum += exp(op._virtualPinX / alpha);
                exp_neg_xi_sum += exp(- op._virtualPinX / alpha);
                exp_yi_sum += exp(op._virtualPinY / alpha);
                exp_neg_yi_sum += exp(- op._virtualPinY / alpha);
            }
            // calculate and update
            for (IndexType pinIdx = 0; pinIdx < op._cells.size(); ++pinIdx)
            {
                NumType x = op::conv<NumType>(
                        op._getVarFunc(op._cells[pinIdx], Orient2DType::HORIZONTAL) + op._offsetX[pinIdx]
                        );
                NumType y = op::conv<NumType>(
                        op._getVarFunc(op._cells[pinIdx], Orient2DType::VERTICAL) + op._offsetY[pinIdx]
                        );
                const NumType xPartial = (exp_xi_sum - std::exp(x / alpha)) * std::exp(x / alpha) / (alpha * std::pow(exp_xi_sum, 2.0))
                                       +  (exp_neg_xi_sum - std::exp(-x / alpha)) * std::exp(-x / alpha) / (alpha * std::pow(exp_neg_xi_sum, 2.0));
                const NumType yPartial = (exp_yi_sum - std::exp(y / alpha)) * std::exp(y / alpha) / (alpha * std::pow(exp_yi_sum, 2.0))
                                       +  (exp_neg_yi_sum - std::exp(-y / alpha)) * std::exp(-y / alpha) / (alpha * std::pow(exp_neg_yi_sum, 2.0));
                accumulateHessianFunc(lambda * xPartial, op._cells[pinIdx], op._cells[pinIdx], Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
                accumulateHessianFunc(lambda * yPartial, op._cells[pinIdx], op._cells[pinIdx], Orient2DType::VERTICAL, Orient2DType::VERTICAL);
            }
        }
    };


    template<typename NumType, typename CoordType>
    struct jacobi_hessian_approx_trait<CellPairOverlapPenaltyDifferentiable<NumType, CoordType>>
    {
        typedef CellPairOverlapPenaltyDifferentiable<NumType, CoordType> operator_type;
        static void accumulateHessian(const operator_type & op, const std::function<void(NumType, IndexType, IndexType, Orient2DType, Orient2DType)> &accumulateHessianFunc)
        {
            const NumType alpha = op._getAlphaFunc();
            const NumType lambda = op._getLambdaFunc();

            const NumType xi = op::conv<NumType>(op._getVarFunc(op._cellIdxI, Orient2DType::HORIZONTAL));
            const NumType yi = op::conv<NumType>(op._getVarFunc(op._cellIdxI, Orient2DType::VERTICAL));
            const NumType xj = op::conv<NumType>(op._getVarFunc(op._cellIdxJ, Orient2DType::HORIZONTAL));
            const NumType yj = op::conv<NumType>(op._getVarFunc(op._cellIdxJ, Orient2DType::VERTICAL));
            const NumType wi = op::conv<NumType>(op._cellWidthI);
            const NumType hi = op::conv<NumType>(op._cellHeightI);
            const NumType wj = op::conv<NumType>(op._cellWidthJ);
            const NumType hj = op::conv<NumType>(op._cellHeightJ);

            const NumType dxi2 = (2*std::pow(alpha, 2.0)*log(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha)/alpha - exp(-(wj - xi + xj)/alpha)/alpha, 2.0))/((1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha), 3.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha)/alpha - exp(-(wj - xi + xj)/alpha)/alpha, 2.0))/(std::pow(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1, 2.0)*std::pow(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha), 4.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*(exp(-(wi + xi - xj)/alpha)/std::pow(alpha, 2.0) + exp(-(wj - xi + xj)/alpha)/std::pow(alpha, 2.0)))/((1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha), 2.0));
 
 
            const NumType dxj2 = (2*std::pow(alpha, 2.0)*log(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha)/alpha - exp(-(wj - xi + xj)/alpha)/alpha, 2.0))/((1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha), 3.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha)/alpha - exp(-(wj - xi + xj)/alpha)/alpha, 2.0))/(std::pow(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1, 2.0)*std::pow(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha), 4.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*(exp(-(wi + xi - xj)/alpha)/std::pow(alpha, 2.0) + exp(-(wj - xi + xj)/alpha)/std::pow(alpha, 2.0)))/((1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha), 2.0));
 
 
            const NumType dyi2 = (2*std::pow(alpha, 2.0)*log(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha)/alpha - exp(-(hj - yi + yj)/alpha)/alpha, 2.0))/((1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha), 3.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha)/alpha - exp(-(hj - yi + yj)/alpha)/alpha, 2.0))/(std::pow(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1, 2.0)*std::pow(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha), 4.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*(exp(-(hi + yi - yj)/alpha)/std::pow(alpha, 2.0) + exp(-(hj - yi + yj)/alpha)/std::pow(alpha, 2.0)))/((1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha), 2.0));
 
 
            const NumType dyj2 = (2*std::pow(alpha, 2.0)*log(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha)/alpha - exp(-(hj - yi + yj)/alpha)/alpha, 2.0))/((1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha), 3.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha)/alpha - exp(-(hj - yi + yj)/alpha)/alpha, 2.0))/(std::pow(1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1, 2.0)*std::pow(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha), 4.0)) - (std::pow(alpha, 2.0)*log(1/(exp(-(wi + xi - xj)/alpha) + exp(-(wj - xi + xj)/alpha)) + 1)*(exp(-(hi + yi - yj)/alpha)/std::pow(alpha, 2.0) + exp(-(hj - yi + yj)/alpha)/std::pow(alpha, 2.0)))/((1/(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha)) + 1)*std::pow(exp(-(hi + yi - yj)/alpha) + exp(-(hj - yi + yj)/alpha), 2.0));

            accumulateHessianFunc(lambda * dxi2, op._cellIdxI, op._cellIdxI, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
            accumulateHessianFunc(lambda * dyi2, op._cellIdxI, op._cellIdxI, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
            accumulateHessianFunc(lambda * dxj2, op._cellIdxJ, op._cellIdxJ, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
            accumulateHessianFunc(lambda * dyj2, op._cellIdxJ, op._cellIdxJ, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
 
        }
    };


    template<typename NumType, typename CoordType>
    struct jacobi_hessian_approx_trait<CellOutOfBoundaryPenaltyDifferentiable<NumType, CoordType>>
    {
        typedef CellOutOfBoundaryPenaltyDifferentiable<NumType, CoordType> operator_type;
        static void accumulateHessian(const operator_type & op, const std::function<void(NumType, IndexType, IndexType, Orient2DType, Orient2DType)> &accumulateHessianFunc)
        {
            const NumType alpha = op._getAlphaFunc();
            const NumType lambda = op._getLambdaFunc();
            const NumType xi = op::conv<NumType>(op._getVarFunc(op._cellIdx, Orient2DType::HORIZONTAL));
            const NumType yi = op::conv<NumType>(op._getVarFunc(op._cellIdx, Orient2DType::VERTICAL));
            const NumType wi = op::conv<NumType>(op._cellWidth);
            const NumType hi = op::conv<NumType>(op._cellHeight);
            const NumType bbXLo = op::conv<NumType>(op._boundary->xLo());
            const NumType bbXHi = op::conv<NumType>(op._boundary->xHi());
            const NumType bbYLo = op::conv<NumType>(op._boundary->yLo());
            const NumType bbYHi = op::conv<NumType>(op._boundary->yHi());

            const NumType dxi2 = exp((wi - bbXHi + xi)/alpha)/(exp((wi - bbXHi + xi)/alpha) + 1) - exp((bbXLo - xi)/alpha)/(exp((bbXLo - xi)/alpha) + 1);
            const NumType dyi2 = exp((hi - bbYHi + yi)/alpha)/(exp((hi - bbYHi + yi)/alpha) + 1) - exp((bbYLo - yi)/alpha)/(exp((bbYLo - yi)/alpha) + 1);

            accumulateHessianFunc(lambda * dxi2, op._cellIdx, op._cellIdx, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
            accumulateHessianFunc(lambda * dyi2, op._cellIdx, op._cellIdx, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
 
        }
    };


    template<typename NumType, typename CoordType>
    struct jacobi_hessian_approx_trait<CosineDatapathDifferentiable<NumType, CoordType>>
    {
        typedef CosineDatapathDifferentiable<NumType, CoordType> operator_type;
        static void accumulateHessian(const operator_type & op, const std::function<void(NumType, IndexType, IndexType, Orient2DType, Orient2DType)> &accumulateHessianFunc)
        {
            const NumType lambda = op._getLambdaFunc();
            const NumType x1 = op::conv<NumType>(op._getVarFunc(op._sCellIdx, Orient2DType::HORIZONTAL));
            const NumType y1 = op::conv<NumType>(op._getVarFunc(op._sCellIdx, Orient2DType::VERTICAL));
            const NumType x2 = op::conv<NumType>(op._getVarFunc(op._midCellIdx, Orient2DType::HORIZONTAL));
            const NumType y2 = op::conv<NumType>(op._getVarFunc(op._midCellIdx, Orient2DType::VERTICAL));
            const NumType x3 = op::conv<NumType>(op._getVarFunc(op._tCellIdx, Orient2DType::HORIZONTAL));
            const NumType y3 = op::conv<NumType>(op._getVarFunc(op._tCellIdx, Orient2DType::VERTICAL));
            const NumType ox1 = op._sOffset.x();
            const NumType oy1 = op._sOffset.y();
            const NumType ox2a = op._midOffsetA.x();
            const NumType oy2a = op._midOffsetA.y();
            const NumType ox2b = op._midOffsetB.x();
            const NumType oy2b = op._midOffsetB.y();
            const NumType ox3 = op._tOffset.x();
            const NumType oy3 = op._tOffset.y();

            const NumType dx1_2 = (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*ox1 - 2*ox2a + 2*x1 - 2*x2, 2.0))/(4*std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 2.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((2*ox1 - 2*ox2a + 2*x1 - 2*x2)*(ox3 - ox2b - x2 + x3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0)));
 
 
            const NumType dy1_2 = (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*oy1 - 2*oy2a + 2*y1 - 2*y2, 2.0))/(4*std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 2.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((2*oy1 - 2*oy2a + 2*y1 - 2*y2)*(oy3 - oy2b - y2 + y3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0)));
 
 
            const NumType dx2_2 = 2/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5)) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((2*ox1 - 2*ox2a + 2*x1 - 2*x2)*(ox1 + ox3 - ox2a - ox2b + x1 - 2*x2 + x3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((2*ox3 - 2*ox2b - 2*x2 + 2*x3)*(ox1 + ox3 - ox2a - ox2b + x1 - 2*x2 + x3))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5)) + (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*ox1 - 2*ox2a + 2*x1 - 2*x2, 2.0))/(4*std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 2.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) + (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*ox3 - 2*ox2b - 2*x2 + 2*x3, 2.0))/(4*std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 2.5)) + (((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*(2*ox1 - 2*ox2a + 2*x1 - 2*x2)*(2*ox3 - 2*ox2b - 2*x2 + 2*x3))/(2*std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5));
 
 
            const NumType dy2_2 = 2/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5)) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((2*oy1 - 2*oy2a + 2*y1 - 2*y2)*(oy1 + oy3 - oy2a - oy2b + y1 - 2*y2 + y3))/(std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) - ((2*oy3 - 2*oy2b - 2*y2 + 2*y3)*(oy1 + oy3 - oy2a - oy2b + y1 - 2*y2 + y3))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5)) + (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*oy1 - 2*oy2a + 2*y1 - 2*y2, 2.0))/(4*std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 2.5)*std::sqrt(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0))) + (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*oy3 - 2*oy2b - 2*y2 + 2*y3, 2.0))/(4*std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 2.5)) + (((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*(2*oy1 - 2*oy2a + 2*y1 - 2*y2)*(2*oy3 - 2*oy2b - 2*y2 + 2*y3))/(2*std::pow(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0), 1.5)*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5));
 
 
            const NumType dx3_2 = (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*ox3 - 2*ox2b - 2*x2 + 2*x3, 2.0))/(4*std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 2.5)) - ((2*ox3 - 2*ox2b - 2*x2 + 2*x3)*(ox1 - ox2a + x1 - x2))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5)) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5));
 
 
            const NumType dy3_2 = (3*((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))*std::pow(2*oy3 - 2*oy2b - 2*y2 + 2*y3, 2.0))/(4*std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 2.5)) - ((2*oy3 - 2*oy2b - 2*y2 + 2*y3)*(oy1 - oy2a + y1 - y2))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5)) - ((ox1 - ox2a + x1 - x2)*(ox3 - ox2b - x2 + x3) + (oy1 - oy2a + y1 - y2)*(oy3 - oy2b - y2 + y3))/(std::sqrt(std::pow(ox1 - ox2a + x1 - x2, 2.0) + std::pow(oy1 - oy2a + y1 - y2, 2.0))*std::pow(std::pow(ox3 - ox2b - x2 + x3, 2.0) + std::pow(oy3 - oy2b - y2 + y3, 2.0), 1.5));

            accumulateHessianFunc(dx1_2 * lambda, op._sCellIdx, op._sCellIdx, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
            accumulateHessianFunc(dy1_2 * lambda, op._sCellIdx, op._sCellIdx, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
            accumulateHessianFunc(dx2_2 * lambda, op._midCellIdx, op._midCellIdx, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
            accumulateHessianFunc(dy2_2 * lambda, op._midCellIdx, op._midCellIdx, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
            accumulateHessianFunc(dx3_2 * lambda, op._tCellIdx, op._tCellIdx, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
            accumulateHessianFunc(dy3_2 * lambda, op._tCellIdx, op._tCellIdx, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
        }
    };


    template<typename NumType, typename CoordType>
    struct jacobi_hessian_approx_trait<AsymmetryDifferentiable<NumType, CoordType>>
    {
        typedef AsymmetryDifferentiable<NumType, CoordType> operator_type;
        static void accumulateHessian(const operator_type & op, const std::function<void(NumType, IndexType, IndexType, Orient2DType, Orient2DType)> &accumulateHessianFunc)
        {
            return;
            const NumType lambda = op._getLambdaFunc();

            for (IndexType symPairIdx = 0; symPairIdx < op._pairCells.size(); ++symPairIdx)
            {
                const IndexType cellI = op._pairCells[symPairIdx][0];
                const IndexType cellJ = op._pairCells[symPairIdx][1];
                accumulateHessianFunc(2 * lambda, cellI, cellI, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
                accumulateHessianFunc(2 * lambda, cellJ, cellJ, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
                accumulateHessianFunc(2 * lambda, cellI, cellI, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
                accumulateHessianFunc(2 * lambda, cellJ, cellJ, Orient2DType::VERTICAL, Orient2DType::VERTICAL);
                accumulateHessianFunc(8 * lambda, op._symGrpIdx, op._symGrpIdx, Orient2DType::NONE, Orient2DType::NONE);
            }
            for (IndexType ssIdx : op._selfSymCells)
            {
                accumulateHessianFunc(2 * lambda, ssIdx, ssIdx, Orient2DType::HORIZONTAL, Orient2DType::HORIZONTAL);
                accumulateHessianFunc(2 * lambda, op._symGrpIdx, op._symGrpIdx, Orient2DType::NONE, Orient2DType::NONE);
            }
        }
    };


    template<typename NumType, typename CoordType>
    struct jacobi_hessian_approx_trait<PowerVerQuadraticWireLengthDifferentiable<NumType, CoordType>>
    {
        typedef PowerVerQuadraticWireLengthDifferentiable<NumType, CoordType> operator_type;
        static void accumulateHessian(const operator_type & op, const std::function<void(NumType, IndexType, IndexType, Orient2DType, Orient2DType)> &accumulateHessianFunc)
        {

            if (! op.validHpwl())
            {
                return;
            }
            if (op._validVirtualPin != 1)
            {
                return;
            }
            const NumType lambda = op._getLambdaFunc();
            for (IndexType pinIdx = 0; pinIdx < op._cells.size(); ++pinIdx)
            {
                accumulateHessianFunc(2 * op._weight * lambda,op. _cells[pinIdx], op._cells[pinIdx], Orient2DType::VERTICAL, Orient2DType::VERTICAL);
            }
        }
    };

};

PROJECT_NAMESPACE_END
