/**
 * @file nlpSecondOrderKernel.hpp
 * @brief The non-lnear programming second order optimization kernel for global plaement
 * @author Keren Zhu
 * @date 04/24/2020
 */

#pragma once

#include "global/global.h"
#include "nlpOptmKernels.hpp"
#include "nlpFirstOrderKernel.hpp"

PROJECT_NAMESPACE_BEGIN

namespace nlp
{
    namespace optm
    {
        namespace second_order
        {
            /// @brief Naive gradient descent. It will stop if maxIter reaches or the improvement 
            template<typename converge_criteria_type>
            struct naive_gradient_descent
            {
                typedef converge_criteria_type converge_type;
                static constexpr RealType _stepSize = 0.001;
                converge_criteria_type _converge;
            };
            /// @brief adam
            template<typename converge_criteria_type, typename nlp_numerical_type>
            struct adam
            {
                typedef converge_criteria_type converge_type;
                converge_criteria_type _converge;
                static constexpr nlp_numerical_type alpha = 0.005;
                static constexpr nlp_numerical_type beta1 = 0.9;
                static constexpr nlp_numerical_type beta2 = 0.999;
                static constexpr nlp_numerical_type epsilon = 1e-8;

            };
            /// @brief nesterov accelerated gradient
            template<typename converge_criteria_type, typename nlp_numerical_type>
            struct nesterov
            {
                typedef converge_criteria_type converge_type;
                converge_criteria_type _converge;
                static constexpr nlp_numerical_type eta = 0.003;

            };
        } // namspace second_order
        template<typename converge_criteria_type>
        struct optm_trait<second_order::naive_gradient_descent<converge_criteria_type>>
        {
            typedef second_order::naive_gradient_descent<converge_criteria_type> optm_type;
            typedef typename optm_type::converge_type converge_type;
            typedef nlp::converge::converge_criteria_trait<converge_type> converge_trait;
            template<typename nlp_type, std::enable_if_t<nlp::is_second_order_diff<nlp_type>::value, void>* = nullptr>
            static void optimize(nlp_type &n, optm_type &o)
            {
                converge_trait::clear(o._converge);
                IndexType iter = 0;
                do 
                {
                    n.calcGrad();
                    n.calcHessian();;
                    n._pl -= optm_type::_stepSize * n.inverseHessian() * n._grad;
                    ++iter;
                } while (!converge_trait::stopCriteria(n, o, o._converge) );
                n.calcObj();
                DBG("converge at iter %d \n", iter);
                DBG("norm %f \n", n._gradOvl.norm());
                DBG("naive gradiend descent: %f hpwl %f cos %f ovl %f oob %f asym %f \n", n._obj, n._objHpwl, n._objCos, n._objOvl, n._objOob, n._objAsym);
            }
        };
        template<typename converge_criteria_type, typename nlp_numerical_type>
        struct optm_trait<second_order::adam<converge_criteria_type, nlp_numerical_type>>
        {
            typedef second_order::adam<converge_criteria_type, nlp_numerical_type> optm_type;
            typedef typename optm_type::converge_type converge_type;
            typedef nlp::converge::converge_criteria_trait<converge_type> converge_trait;
            template<typename nlp_type, std::enable_if_t<nlp::is_second_order_diff<nlp_type>::value, void>* = nullptr>
            static void optimize(nlp_type &n, optm_type &o)
            {
                converge_trait::clear(o._converge);
                const IndexType numVars = n._numVariables;
                typename nlp_type::EigenVector m, v;
                m.resize(numVars); m.setZero();
                v.resize(numVars); v.setZero();
                IndexType iter = 0;
                do 
                {
                    ++iter;
                    n.calcGrad();
                    n.calcHessian();
                    auto grad = n.inverseHessian() * n._grad;
                    m = o.beta1 * m + (1 - o.beta1) * grad;
                    v = o.beta2 * v + (1 - o.beta2) * grad.cwiseProduct(grad);
                    auto mt = m / (1 - pow(o.beta1, iter));
                    auto vt = v / (1 - pow(o.beta2, iter));
                    auto bot = vt.array().sqrt() + o.epsilon;
                    n._pl = n._pl - o.alpha * ( mt.array() / bot).matrix();
                    //n.calcObj();
                    //DBG("norm %f \n", n._grad.norm());
                    //DBG("adam: %f hpwl %f cos %f ovl %f oob %f asym %f \n", n._obj, n._objHpwl, n._objCos, n._objOvl, n._objOob, n._objAsym);
                } while (!converge_trait::stopCriteria(n, o, o._converge) );
                n.calcObj();
                DBG("adam: %f hpwl %f cos %f ovl %f oob %f asym %f \n", n._obj, n._objHpwl, n._objCos, n._objOvl, n._objOob, n._objAsym);
                DBG("gradient norm %f \n", n._grad.norm());
                DBG("converge at iter %d \n", iter);
            }
        };


        template<typename converge_criteria_type, typename nlp_numerical_type>
        struct optm_trait<second_order::nesterov<converge_criteria_type, nlp_numerical_type>>
        {
            typedef second_order::nesterov<converge_criteria_type, nlp_numerical_type> optm_type;
            typedef typename optm_type::converge_type converge_type;
            typedef nlp::converge::converge_criteria_trait<converge_type> converge_trait;
            template<typename nlp_type, std::enable_if_t<nlp::is_second_order_diff<nlp_type>::value, void>* = nullptr>
            static void optimize(nlp_type &n, optm_type &o)
            {
                converge_trait::clear(o._converge);
                const IndexType numVars = n._numVariables;
                IndexType iter = 0;
                nlp_numerical_type lambdaPrev = 0;
                nlp_numerical_type lambdaCurr = 1;
                nlp_numerical_type gamma = 1;
                typename nlp_type::EigenVector yCurr, yPrev;
                yCurr.resize(numVars); yCurr.setZero();
                yPrev.resize(numVars); yPrev = n._pl;
                do 
                {
                    ++iter;
                    n.calcGrad();
                    n.calcHessian();
                    yCurr = n._pl - o.eta * n.inverseHessian() * n._grad;
                    n._pl = (1 - gamma) * yCurr + gamma * yPrev;

                    yPrev = yCurr;

                    const auto lambdaTemp = lambdaCurr;
                    lambdaCurr = (1 + std::sqrt(1 + 4 * lambdaPrev * lambdaPrev)) * 0.5;
                    lambdaPrev = lambdaTemp;

                    gamma = (1 - lambdaPrev) / lambdaCurr;
                    gamma = std::min(gamma, 0.999999999);
                    gamma = std::max(gamma, 1e-8);
                } while (!converge_trait::stopCriteria(n, o, o._converge) );
                n.calcObj();
                DBG("nesterov: %f hpwl %f cos %f ovl %f oob %f asym %f \n", n._obj, n._objHpwl, n._objCos, n._objOvl, n._objOob, n._objAsym);
                DBG("gradient norm %f \n", n._grad.norm());
                DBG("converge at iter %d \n", iter);
            }
        };
    } // namespace optm
} // namespace nlp

PROJECT_NAMESPACE_END
