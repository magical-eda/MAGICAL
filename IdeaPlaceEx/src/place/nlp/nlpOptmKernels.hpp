/**
 * @file nlpOptmKernel.hpp
 * @brief The non-lnear programming basic optimization kernel for global plaement
 * @author Keren Zhu
 * @date 04/05/2020
 */

#pragma once

#include "global/global.h"
#include "nlpTypes.hpp"

PROJECT_NAMESPACE_BEGIN

namespace nlp
{
    namespace converge
    {
        template<typename converge_type>
        struct converge_criteria_trait {};

        /// @brief used for place holder
        struct converge_criteria_empty {};

        template<>
        struct converge_criteria_trait<converge_criteria_empty>
        {
            typedef converge_criteria_empty converge_type;
            template<typename nlp_type, typename optm_type>
            static constexpr BoolType stopCriteria(nlp_type &, optm_type &, converge_type&) { return false;}
            template<typename converge_type>
            static constexpr void clear(converge_type&) {}
        };


        template<IntType max_iter=20>
        struct converge_criteria_max_iter
        {
            IntType _iter = 0;
        };

        template<IntType max_iter>
        struct converge_criteria_trait<converge_criteria_max_iter<max_iter>>
        {
            typedef converge_criteria_max_iter<max_iter> converge_type;
            static void clear(converge_type &c)
            {
                c._iter = 0;
            }
            template<typename nlp_type, typename optm_type>
            static BoolType stopCriteria(nlp_type &, optm_type &, converge_type &c)
            {
                if (c._iter >= max_iter)
                {
                    clear(c);
                    return true;
                }
                else
                {
                    ++c._iter;
                    return false;
                }
            }
        };

        template<typename nlp_numerical_type>
        struct converge_grad_norm_by_init
        {
            nlp_numerical_type confidenceRatio = 0.1; ///< stop if the norm reduce to this ratio of the init
            nlp_numerical_type initNorm = -1.0;
        };

        template<typename nlp_numerical_type>
        struct converge_criteria_trait<converge_grad_norm_by_init<nlp_numerical_type>>
        {
            typedef converge_grad_norm_by_init<nlp_numerical_type> converge_type;
            static void clear(converge_type &c)
            {
                c.initNorm = -1.0;
            }
            template<typename nlp_type, typename optm_type, std::enable_if_t<is_first_order_diff<nlp_type>::value, void>* = nullptr>
            static BoolType stopCriteria(nlp_type &n, optm_type &, converge_type &c)
            {
                if (c.initNorm < 0)
                {
                    c.initNorm = n._grad.norm();
                    return false;
                }
                if (n._grad.norm() < c.confidenceRatio * c.initNorm)
                {
                    clear(c);
                    return true;
                }
                return false;
            }
        };

        template<typename nlp_numerical_type=RealType>
        struct converge_criteria_stop_improve_less_in_last_two_step
        {
            static constexpr RealType _stopThreshold = 0.0001;
            nlp_numerical_type _fLastStep = - 1.0;
            nlp_numerical_type _fLastLastStep = - 1.0;
        };

        template<typename nlp_numerical_type>
        struct converge_criteria_trait<converge_criteria_stop_improve_less_in_last_two_step<nlp_numerical_type>>
        {
            typedef converge_criteria_stop_improve_less_in_last_two_step<nlp_numerical_type> converge_type;
            static void clear(converge_type &c)
            {
                c._fLastStep = - 1.0;
                c._fLastLastStep = - 1.0;
            }
            template<typename nlp_type, typename optm_type>
            static BoolType stopCriteria(nlp_type &n, optm_type &, converge_type &c)
            {
                if (c._fLastLastStep >= 0.0)
                {
                    if ((c._fLastLastStep - n._obj) / c._fLastLastStep < c._stopThreshold)
                    {
                        clear(c);
                        return true;
                    }
                }
                c._fLastLastStep = c._fLastStep;
                c._fLastStep = n._obj;
                return false;
            }
        };

        /// @brief a convenient wrapper for combining different types of converge condition. the list in the template will be check one by one and return converge if any of them say so
        template<typename converge_type, typename... others>
        struct converge_list 
        {
            typedef converge_list<others...> base_type;
            converge_type  _converge;
            converge_list<others...> _list;
        };

        template<typename converge_type>
        struct converge_list<converge_type>
        {
            converge_type _converge;
        };

        template<typename converge_type, typename... others>
        struct converge_criteria_trait<converge_list<converge_type, others...>>
        {
            typedef typename converge_list<converge_type, others...>::base_type base_type;
            static void clear(converge_list<converge_type, others...> &c)
            {
                converge_criteria_trait<converge_type>::clear(c._converge);
                converge_criteria_trait<base_type>::clear(c._list);
            }
            template<typename nlp_type, typename optm_type>
            static BoolType stopCriteria(nlp_type &n, optm_type &o, converge_list<converge_type, others...>&c) 
            {
                BoolType stop = false;
                if (converge_criteria_trait<converge_type>::stopCriteria(n, o, c._converge))
                {
                    stop = true;
                }
                if (converge_criteria_trait<base_type>::stopCriteria(n, o, c._list))
                {
                    stop = true;
                }
                if (stop)
                {
                    converge_criteria_trait<converge_type>::clear(c._converge);
                }
                return stop;
            }
        };


        template<typename converge_type>
        struct converge_criteria_trait<converge_list<converge_type>>
        {
            static void clear(converge_list<converge_type> &c)
            {
                converge_criteria_trait<converge_type>::clear(c._converge);
            }
            template<typename nlp_type, typename optm_type>
            static BoolType stopCriteria(nlp_type &n, optm_type &o, converge_list<converge_type>&c) 
            {
                if (converge_criteria_trait<converge_type>::stopCriteria(n, o, c._converge))
                {
                    converge_criteria_trait<converge_type>::clear(c._converge);
                    return true;
                }
                return false;
            }
        };
    } // namespace converge
} // namespace nlp

PROJECT_NAMESPACE_END
