/**
 * @file nlpOuterOptm.hpp
 * @brief The non-lnear programming outer-problem optimization
 * @author Keren Zhu
 * @date 04/09/2020
 */

#pragma once

#include "global/global.h"
#include "nlpTypes.hpp"
#include "place/different.h"

PROJECT_NAMESPACE_BEGIN

namespace nlp 
{
    namespace outer_stop_condition
    {
        /* Stop outer problem condition */
        template<typename T>
        struct stop_condition_trait 
        {
            // static T construct(NlpType &)
            // static IntType stopPlaceCondition(T&, NlpType &)
        };

        /// @brief stop condition with number of iterations
        template<IntType MaxIter=10>
        struct stop_after_num_outer_iterations
        {
            static constexpr IntType maxIter = MaxIter;
            IntType curIter = 0;
        };
        
        template<IntType MaxIter>
        struct stop_condition_trait<stop_after_num_outer_iterations<MaxIter>>
        {
            template<typename NlpType>
            static stop_after_num_outer_iterations<MaxIter> construct(NlpType &) { return stop_after_num_outer_iterations<MaxIter>(); }

            template<typename NlpType>
            static void init(NlpType &, stop_after_num_outer_iterations<MaxIter> &) {}

            static void clear(stop_after_num_outer_iterations<MaxIter> &) {}
            
            template<typename NlpType>
            static BoolType stopPlaceCondition(NlpType &, stop_after_num_outer_iterations<MaxIter> &stop)
            {
                if (stop.curIter >= stop.maxIter)
                {
                    stop.curIter = 0;
                    return 1;
                }
                ++stop.curIter;
                return 0;
            }
        };

        /// @brief stop after violating is small enough
        struct stop_after_violate_small
        {
            static constexpr RealType overlapRatio = 0.01; ///< with respect to total cell area
            static constexpr RealType outOfBoundaryRatio = 0.05; ///< with respect to boundary
            static constexpr RealType asymRatio = 0.05; ///< with respect to sqrt(total cell area)
        };

        template<>
        struct stop_condition_trait<stop_after_violate_small>
        {
            typedef stop_after_violate_small stop_type;

            template<typename NlpType>
            static stop_type construct(NlpType &) { return stop_type(); }

            template<typename NlpType>
            static void init(NlpType &, stop_type &) {}

            static void clear(stop_type &) {}
            
            template<typename NlpType>
            static BoolType stopPlaceCondition(NlpType &n, stop_type &stop)
            {
                using CoordType = typename NlpType::nlp_coordinate_type;
                // check whether overlapping is small than threshold
                CoordType ovlArea = 0;
                const CoordType ovlThreshold = stop.overlapRatio * n._totalCellArea;
                for (auto &op : n._ovlOps)
                {
                    ovlArea += diff::place_overlap_trait<typename NlpType::nlp_ovl_type>::overlapArea(op);
                    if (ovlArea > ovlThreshold)
                    {
                        return false;
                    }
                }
                // Check whether out of boundary is smaller than threshold
                CoordType oobArea = 0;
                const CoordType oobThreshold = stop.outOfBoundaryRatio * n._boundary.area();
                for (auto &op : n._oobOps)
                {
                    oobArea +=  diff::place_out_of_boundary_trait<typename NlpType::nlp_oob_type>::oobArea(op);
                    if (oobArea > oobThreshold)
                    {
#ifdef DEBUG_GR
                        DBG("fail on oob \n");
#endif
                        return false;
                    }
                }
                // Check whether asymmetry distance is smaller than threshold
                CoordType asymDist = 0;
                const CoordType asymThreshold = stop.asymRatio * std::sqrt(n._totalCellArea);
                for (auto & op : n._asymOps)
                {
                    asymDist += diff::place_asym_trait<typename NlpType::nlp_asym_type>::asymDistanceNormalized(op);
                    if (asymDist > asymThreshold)
                    {
#ifdef DEBUG_GR
                        DBG("fail on asym \n");
#endif
                        return false;
                    }
                }

#ifdef DEBUG_GR
                DBG("ovl area %f target %f \n oob area %f target %f \n asym dist %f target %f \n",  ovlArea, ovlThreshold, oobArea, oobThreshold, asymDist, asymThreshold);
#endif
                return true;
            }
        };
        /// @brief a convenient wrapper for combining different types of stop_condition condition. the list in the template will be check one by one and return converge if any of them say so
        template<typename stop_condition_type, typename... others>
        struct stop_condition_list 
        {
            typedef stop_condition_list<others...> base_type;
            stop_condition_type  _stop;
            stop_condition_list<others...> _list;
        };

        template<typename stop_condition_type>
        struct stop_condition_list<stop_condition_type>
        {
            stop_condition_type _stop;
        };

        template<typename stop_condition_type, typename... others>
        struct stop_condition_trait<stop_condition_list<stop_condition_type, others...>>
        {
            typedef stop_condition_list<stop_condition_type, others...> list_type;
            typedef typename stop_condition_list<stop_condition_type, others...>::base_type base_type;

            static void clear(list_type &c)
            {
                stop_condition_trait<stop_condition_type>::clear(c._stop);
                stop_condition_trait<base_type>::clear(c._list);
            }

            template<typename nlp_type>
            static list_type construct(nlp_type &n)
            {
                list_type list;
                list._stop = std::move(stop_condition_trait<stop_condition_type>::construct(n));
                list._list = std::move(stop_condition_trait<base_type>::construct(n));
                return list;
            }

            template<typename nlp_type>
            static BoolType stopPlaceCondition(nlp_type &n,  list_type &c) 
            {
                BoolType stop = false;
                if (stop_condition_trait<stop_condition_type>::stopPlaceCondition(n, c._stop))
                {
                    stop = true;
                }
                if (stop_condition_trait<base_type>::stopPlaceCondition(n, c._list))
                {
                    stop = true;
                }
                if (stop)
                {
                    stop_condition_trait<stop_condition_type>::clear(c._stop);
                }
                return stop;
            }
        };


        template<typename stop_condition_type>
        struct stop_condition_trait<stop_condition_list<stop_condition_type>>
        {
            typedef stop_condition_list<stop_condition_type> list_type;
            static void clear(stop_condition_list<stop_condition_type> &c)
            {
                stop_condition_trait<stop_condition_type>::clear(c._stop);
            }


            template<typename nlp_type>
            static list_type construct(nlp_type &n)
            {
                list_type list;
                list._stop = std::move(stop_condition_trait<stop_condition_type>::construct(n));
                return list;
            }

            template<typename nlp_type>
            static BoolType stopPlaceCondition(nlp_type &n, stop_condition_list<stop_condition_type>&c) 
            {
                if (stop_condition_trait<stop_condition_type>::stopPlaceCondition(n, c._stop))
                {
                    stop_condition_trait<stop_condition_type>::clear(c._stop);
                    return true;
                }
                return false;
            }
        };

    } // namespace outer_stop_condition

    namespace outer_multiplier
    {
        /// @brief the multiplier is categoried by types
        template<typename mult_type>
        struct is_mult_type_dependent_diff : std::false_type {};

        namespace init
        {
            template<typename T>
            struct multiplier_init_trait {};

            struct hard_code_init {};

            template<>
            struct multiplier_init_trait<hard_code_init>
            {
                template<typename nlp_type, typename mult_type>
                static void init(nlp_type &, mult_type& mult)
                {
                    mult._constMults.at(0) = 16; // hpwl
                    mult._constMults.at(1) = 1; // cos
                    mult._constMults.at(2) = 16; // power
                    mult._variedMults.at(0) = 1; // ovl
                    mult._variedMults.at(1) = 1; // oob
                    mult._variedMults.at(2) = 1; // asym
                    mult._variedMults.at(3) = 1; // crf
                }

            };

            /// @brief match by gradient norm
            struct init_by_matching_gradient_norm 
            {
                static constexpr RealType penaltyRatioToObj = 1.0; ///< The ratio of targeting penalty 
                static constexpr RealType small = 0.01;
            };

            
            template<>
            struct multiplier_init_trait<init_by_matching_gradient_norm>
            {
                typedef init_by_matching_gradient_norm init_type;
                template<typename nlp_type, typename mult_type, std::enable_if_t<nlp::is_first_order_diff<nlp_type>::value, void>* = nullptr>
                static void init(nlp_type &nlp, mult_type& mult)
                {
                    RealType totalHpwlWeights = 0.0;
                    RealType totalCosWeights = 0.0;
                    RealType totalPowerWlWeights = 0.0;
                    RealType totalCrfWeights = 0.0;
                    for (const auto & op : nlp._hpwlOps)
                    {
                        totalHpwlWeights += op._weight;
                    }
                    for (const auto & op : nlp._cosOps)
                    {
                        totalCosWeights += op._weight;
                    }
                    for (const auto & op : nlp._powerWlOps)
                    {
                        totalPowerWlWeights += op._weight;
                    }
                    for (const auto & op : nlp._crfOps)
                    {
                        totalCrfWeights += op._weight;
                    }
                    mult._constMults.at(0) = 1.0; // hpwl
                    const auto hpwlMult = mult._constMults.at(0);
                    const auto hpwlNorm = nlp._gradHpwl.norm();
                    const auto hpwlMultNorm = hpwlMult * hpwlNorm;
                    const auto hpwlMultNormPenaltyRatio = hpwlMultNorm * init_type::penaltyRatioToObj;
                    const auto cosNorm = nlp._gradCos.norm();
                    const auto powerWlNorm = nlp._gradPowerWl.norm();
                    const auto ovlNorm = nlp._gradOvl.norm();
                    const auto oobNorm = nlp._gradOob.norm();
                    const auto asymNorm = nlp._gradAsym.norm();
                    const auto crfNorm = nlp._gradCrf.norm();
                    const auto maxPenaltyNorm = ovlNorm;
                    // Make a threshold on by referencing hpwl to determine whether one is small
                    const auto small  = init_type::small * hpwlNorm;

                    // Fix corner case that may happen when the placement is very small
                    if (hpwlNorm < REAL_TYPE_TOL)
                    {
                        mult._constMults.resize(3, 1);
                        mult._variedMults.resize(4, 1);
                        WRN("Ideaplace: NLP global placement: init multipliers: wire length  gradient norm is very small %f!, ", hpwlNorm);
                        return;
                    }
                    // match gradient norm for signal path
                    if (cosNorm > small)
                    {
                        mult._constMults.at(1) = hpwlMultNorm  * totalCosWeights / totalHpwlWeights / cosNorm;
                    }
                    else
                    {
                        mult._constMults.at(1) = hpwlMult * 30;
                    }
                    // match gradient norm for signal path
                    if (powerWlNorm > small)
                    {
                        mult._constMults.at(2) = hpwlMultNorm  * totalCosWeights / totalHpwlWeights / powerWlNorm;
                    }
                    else
                    {
                        mult._constMults.at(2) = hpwlMult * 30;
                    }
                    // overlap
                    if (ovlNorm > small)
                    {
                        mult._variedMults.at(0) = hpwlMultNormPenaltyRatio / ovlNorm;
                    }
                    else
                    {
                        mult._variedMults.at(0) = hpwlMultNormPenaltyRatio / maxPenaltyNorm;
                    }
                    // out of boundary
                    // Since we know oob is small at beginning, and it will take effect after a few iterations. Therefore it would be better to set it to resonable range first
                    //mult._variedMults.at(1) = hpwlMultNormPenaltyRatio;
                    if (oobNorm > small)
                    {
                        mult._variedMults.at(1) = hpwlMultNormPenaltyRatio / oobNorm;
                    }
                    else
                    {
                        mult._variedMults.at(1) = hpwlMultNormPenaltyRatio / maxPenaltyNorm;
                    }
                    // asym
                    if (asymNorm > small)
                    {
                        mult._variedMults.at(2) = hpwlMultNormPenaltyRatio / asymNorm;
                    }
                    else
                    {
                        mult._variedMults.at(2) = hpwlMultNormPenaltyRatio / maxPenaltyNorm;
                    }
                    // crf
                    if (crfNorm > small)
                    {
                        mult._variedMults.at(3) = hpwlMultNormPenaltyRatio * totalCrfWeights / totalHpwlWeights / asymNorm;
                    }
                    else
                    {
                        mult._variedMults.at(3) = hpwlMultNormPenaltyRatio / maxPenaltyNorm;
                    }
#ifdef DEBUG_GR
                    // crf
                    DBG("init mult: hpwl %f cos %f power wl %f \n",
                            mult._constMults[0], mult._constMults[1], mult._constMults[2]);
                    DBG("init mult: ovl %f oob %f asym %f current flow %f\n",
                            mult._variedMults[0], mult._variedMults[1], mult._variedMults[2], mult._variedMults[3]);
#endif
                }
            };
        }; // namespace init
        namespace update
        {
            template<typename T>
            struct multiplier_update_trait {};

            struct no_update {};
            template<>
            struct multiplier_update_trait<no_update>
            {
                typedef no_update update_type;

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static update_type construct(nlp_type &, mult_type&) { return update_type(); }

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void init(nlp_type &, mult_type &, update_type &) 
                { 
                }

                template<typename nlp_type, typename mult_type,  std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void update(nlp_type &, mult_type &, update_type &) {}
            };


            /// @brief increase the total amounts of penalty of by a constant
            struct shared_constant_increase_penalty
            {
                static constexpr RealType penalty  = 20;
            };

            template<>
            struct multiplier_update_trait<shared_constant_increase_penalty>
            {
                typedef shared_constant_increase_penalty update_type;

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static update_type construct(nlp_type &, mult_type&) { return update_type(); }

                template<typename nlp_type, typename mult_type,  std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void update(nlp_type &nlp, mult_type &mult, update_type &update)
                {
                    nlp._wrapObjAllTask.run();
                    const auto rawOvl = nlp._objOvl / mult._variedMults.at(0);
                    const auto rawOob = nlp._objOob / mult._variedMults.at(1);
                    const auto rawAsym = nlp._objAsym / mult._variedMults.at(2);
                    const auto fViolate = rawOvl + rawOob + rawAsym;
#ifdef DEBUG_GR
                    DBG("update mult: raw ovl %f oob %f asym %f total %f \n", rawOvl, rawOob, rawAsym, fViolate);
                    DBG("update mult:  before ovl %f oob %f asym %f \n",
                            mult._variedMults[0], mult._variedMults[1], mult._variedMults[2]);
#endif
                    mult._variedMults.at(0) += update.penalty * (rawOvl / fViolate);
                    mult._variedMults.at(1) += update.penalty * (rawOob / fViolate);
                    mult._variedMults.at(2) += update.penalty * (rawAsym / fViolate);
#ifdef DEBUG_GR
                    DBG("update mult: afterafter  ovl %f oob %f asym %f \n",
                            mult._variedMults[0], mult._variedMults[1], mult._variedMults[2]);
#endif
                }
            };

            /// @brief direct subgradient
            struct direct_subgradient
            {
                static constexpr RealType stepSize = 0.01;
            };

            template<>
            struct multiplier_update_trait<direct_subgradient>
            {
                typedef direct_subgradient update_type;

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static update_type construct(nlp_type &, mult_type&) { return update_type(); }

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void init(nlp_type &, mult_type&, update_type &) 
                { 
                }

                template<typename nlp_type, typename mult_type,  std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void update(nlp_type &nlp, mult_type &mult, update_type &update)
                {
                    nlp._wrapObjAllTask.run();
                    const auto rawOvl = nlp._objOvl / mult._variedMults.at(0);
                    const auto rawOob = nlp._objOob / mult._variedMults.at(1);
                    const auto rawAsym = nlp._objAsym / mult._variedMults.at(2);
                    const auto rawCos = nlp._objCos / mult._constMults.at(1);
                    const auto rawCrf = nlp._objCrf / mult._variedMults.at(3);
#ifdef DEBUG_GR
                    DBG("update mult: raw ovl %f oob %f asym %f cos %f powerWl %f current flow\n", rawOvl, rawOob, rawAsym, rawCos, nlp._objPowerWl, rawCrf);
                    DBG("update mult:  before ovl %f oob %f asym %f current flow %f \n",
                            mult._variedMults[0], mult._variedMults[1], mult._variedMults[2], mult._variedMults[3]);
#endif
                    mult._variedMults.at(0) += update.stepSize * (rawOvl );
                    mult._variedMults.at(1) += update.stepSize * (rawOob );
                    mult._variedMults.at(2) += update.stepSize * (rawAsym );
                    mult._variedMults.at(3) += update.stepSize * (rawCrf );
                    mult._constMults.at(1) += update.stepSize * (rawCos);
#ifdef DEBUG_GR
                    DBG("update mult: afterafter  ovl %f oob %f asym %f cos %f current flow %f \n",
                            mult._variedMults[0], mult._variedMults[1], mult._variedMults[2], mult._constMults[1], mult._variedMults[3]);
#endif
                }
            };

            template<typename nlp_numerical_type>
            struct match_grad_const_multipliers
            {
                nlp_numerical_type totalHpwlWeights = 0.0;
                nlp_numerical_type totalCosWeights = 0.0;
                nlp_numerical_type totalPowerWlWeights = 0.0;
                static constexpr nlp_numerical_type maxMult = 500;
                bool _recordedInit = false; ///< Whether the init multipliers have been recorded
                nlp_numerical_type ratio = 1.0; ///< The ratio of matched part vs constant part
                static constexpr nlp_numerical_type ratioDecayRate = 0.98; ///< The decay factor of "ratio"
                nlp_numerical_type hpwlInitMult = 1.0;
                nlp_numerical_type cosInitMult = 1.0;
                nlp_numerical_type powerWlInitMult = 1.0;
                void decay()
                {
                    ratio *= ratioDecayRate;
                }
            };

            template<typename nlp_numerical_type>
            struct multiplier_update_trait<match_grad_const_multipliers<nlp_numerical_type>>
            {
                typedef match_grad_const_multipliers<nlp_numerical_type> update_type;
                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static update_type construct(nlp_type &, mult_type&) { return update_type(); }

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void init(nlp_type & n, mult_type &, update_type & u) 
                { 
                    for (const auto & op : n._hpwlOps)
                    {
                        u.totalHpwlWeights += op._weight;
                    }
                    for (const auto & op : n._cosOps)
                    {
                        u.totalCosWeights += op._weight;
                    }
                    for (const auto & op : n._powerWlOps)
                    {
                        u.totalPowerWlWeights += op._weight;
                    }
                }

                template<typename nlp_type, typename mult_type,  std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void update(nlp_type &nlp, mult_type &mult, update_type &u)
                {
                    nlp._wrapObjAllTask.run();
                    // Record the init multipliers
                    if (not u._recordedInit)
                    {
                        u._recordedInit = true;
                        u.hpwlInitMult = mult._constMults.at(0);
                        u.cosInitMult = mult._constMults.at(0) * u.totalCosWeights / u.totalHpwlWeights;
                        u.powerWlInitMult = mult._constMults.at(0) * u.totalPowerWlWeights / u.totalHpwlWeights;
                    }
                    const auto hpwlMult = mult._constMults.at(0);
                    const auto hpwlNorm = nlp._gradHpwl.norm();
                    const auto hpwlMultNorm = hpwlMult * hpwlNorm / u.totalHpwlWeights;
                    const auto cosNorm = nlp._gradCos.norm() / mult._constMults.at(1);
                    const auto powerWlNorm = nlp._gradPowerWl.norm() / mult._constMults.at(2);
                    // Make a threshold on by referencing hpwl to determine whether one is small
                    const auto small  = 0.001 * hpwlNorm;

                    // Fix corner case that may happen when the placement is very small
                    if (hpwlNorm < REAL_TYPE_TOL)
                    {
                        return;
                    }
                    nlp_numerical_type cosMatch;
                    // match gradient norm for signal path
                    if (cosNorm > small)
                    {
                        cosMatch = std::min(hpwlMultNorm * u.totalCosWeights  / cosNorm, update_type::maxMult);
                    }
                    else
                    {
                        cosMatch = hpwlMult ;
                    }
                    // match gradient norm for signal path
                    nlp_numerical_type powerWlMatch;
                    if (powerWlNorm > small)
                    {
                        powerWlMatch = std::min(hpwlMultNorm * u.totalPowerWlWeights  / powerWlNorm, update_type::maxMult);
                    }
                    else
                    {
                        powerWlMatch = hpwlMult;
                    }
                    mult._constMults.at(1) = u.ratio * cosMatch + (1 - u.ratio) * u.cosInitMult; 
                    mult._constMults.at(2) = u.ratio * powerWlMatch + (1 - u.ratio) * u.powerWlInitMult; 
                    u.decay();
#ifdef DEBUG_GR
                    DBG("match_grad_const_multipliers: multipliers hpwl %f cos %f power wl %f \n", mult._constMults.at(0), mult._constMults.at(1), mult._constMults.at(2));
#endif
                }
            };

            /// @breif subgradient normalize by values in iter 0
            template<typename nlp_numerical_type>
            struct subgradient_normalized_by_init
            {
                static constexpr nlp_numerical_type stepSize = 10;
                std::vector<nlp_numerical_type> normalizeFactor;
            };

            template<typename nlp_numerical_type>
            struct multiplier_update_trait<subgradient_normalized_by_init<nlp_numerical_type>>
            {
                typedef subgradient_normalized_by_init<nlp_numerical_type> update_type;

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static update_type construct(nlp_type &, mult_type&) { return update_type(); }

                template<typename nlp_type, typename mult_type, std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void init(nlp_type &nlp, mult_type &mult, update_type &update) 
                { 
                    update.normalizeFactor.resize(3);
                    update.normalizeFactor.at(0) = mult._variedMults.at(0) / nlp._objOvl;
                    update.normalizeFactor.at(1) = 1;// mult._variedMults.at(1) / nlp._objOob;
                    update.normalizeFactor.at(2) = mult._variedMults.at(2) / nlp._objAsym;
                    //update.normalizeFactor.at(0) = 1 / nlp._objOvl;
                    //update.normalizeFactor.at(1) = 1;// / nlp._objOob;
                    //update.normalizeFactor.at(2) = 1 / nlp._objAsym;
                }

                template<typename nlp_type, typename mult_type,  std::enable_if_t<is_mult_type_dependent_diff<mult_type>::value, void>* = nullptr>
                static void update(nlp_type &nlp, mult_type &mult, update_type &update)
                {
                    nlp._wrapObjAllTask.run();
                    const auto rawOvl = nlp._objOvl / mult._variedMults.at(0);
                    const auto rawOob = nlp._objOob / mult._variedMults.at(1);
                    const auto rawAsym = nlp._objAsym / mult._variedMults.at(2);
                    const auto normalizedOvl = rawOvl * update.normalizeFactor.at(0);
                    const auto normalizedOob = rawOob * update.normalizeFactor.at(1);
                    const auto normalizedAsym = rawAsym  * update.normalizeFactor.at(2);
#ifdef DEBUG_GR
                    DBG("update mult: raw ovl %f oob %f asym %f total %f \n", normalizedOvl, normalizedOob, normalizedAsym);
                    DBG("update mult:  before ovl %f oob %f asym %f \n",
                            mult._variedMults[0], mult._variedMults[1], mult._variedMults[2]);
#endif
                    mult._variedMults.at(0) += update.stepSize * (normalizedOvl );
                    mult._variedMults.at(1) += update.stepSize * (normalizedOob );
                    mult._variedMults.at(2) += update.stepSize * (normalizedAsym );
#ifdef DEBUG_GR
                    DBG("update mult: afterafter  ovl %f oob %f asym %f \n",
                            mult._variedMults[0], mult._variedMults[1], mult._variedMults[2]);
#endif
                }
            };


        } // namespace update
        template<typename T> 
        struct multiplier_trait {};

        template<typename nlp_numerical_type, typename init_type, typename update_type>
        struct mult_const_hpwl_cos_and_penalty_by_type
        {
            typedef init_type mult_init_type;
            typedef update_type mult_update_type;
            std::vector<nlp_numerical_type> _constMults; ///< constant mults
            std::vector<nlp_numerical_type> _variedMults; ///< varied penalty multipliers
            update_type update;
        };

        template<typename nlp_numerical_type, typename init_type, typename update_type>
        struct is_mult_type_dependent_diff<mult_const_hpwl_cos_and_penalty_by_type<nlp_numerical_type, init_type, update_type>> : std::true_type {};

        template<typename nlp_numerical_type, typename init_type, typename update_type>
        struct multiplier_trait<mult_const_hpwl_cos_and_penalty_by_type<nlp_numerical_type, init_type, update_type>>
        {
            typedef mult_const_hpwl_cos_and_penalty_by_type<nlp_numerical_type, init_type, update_type> mult_type;

            template<typename nlp_type>
            static mult_type construct(nlp_type &nlp)
            {
                mult_type mult;
                mult._constMults.resize(3, 0.0);
                mult._variedMults.resize(4, 0.0);
                mult.update = update::multiplier_update_trait<update_type>::construct(nlp, mult);
                return mult;
            }

            template<typename nlp_type>
            static void init(nlp_type &nlp, mult_type &mult)
            {
                init::multiplier_init_trait<init_type>::init(nlp, mult);
                update::multiplier_update_trait<update_type>::init(nlp, mult, mult.update);
                for (auto &op : nlp._hpwlOps) { op._getLambdaFunc = [&](){ return mult._constMults[0]; }; }
                for (auto &op : nlp._cosOps) { op._getLambdaFunc = [&](){ return mult._constMults[1]; }; }
                for (auto &op : nlp._ovlOps) { op._getLambdaFunc = [&](){ return mult._variedMults[0]; }; }
                for (auto &op : nlp._oobOps) { op._getLambdaFunc = [&](){ return mult._variedMults[1]; }; }
                for (auto &op : nlp._asymOps) { op._getLambdaFunc = [&](){ return mult._variedMults[2]; }; }
                for (auto &op : nlp._powerWlOps) { op._getLambdaFunc = [&](){ return mult._constMults[2]; }; }
                for (auto &op : nlp._crfOps) { op._getLambdaFunc = [&](){ return mult._variedMults[3]; }; }
            }

            template<typename nlp_type>
            static void update(nlp_type &nlp, mult_type &mult)
            {
                update::multiplier_update_trait<update_type>::update(nlp, mult, mult.update);
            }

            template<typename nlp_type>
            static void recordRaw(nlp_type &nlp, mult_type &mult)
            {
                nlp._objHpwlRaw = nlp._objHpwl / mult._constMults[0];
                nlp._objCosRaw = nlp._objCos / mult._constMults[1];
                nlp._objOvlRaw = nlp._objOvl / mult._variedMults[0];
                nlp._objOobRaw = nlp._objOob / mult._variedMults[1];
                nlp._objAsymRaw = nlp._objAsym / mult._variedMults[2];
                nlp._objCrfRaw = nlp._objCrf / mult._variedMults[3];
            }

        };
    } // namespace outer_multiplier

    /// @brief alpha 
    namespace alpha
    {
        namespace update
        {
            template<typename T>
            struct alpha_update_trait {};

        } //namespace update

        template<typename T>
        struct alpha_trait {};

        template<typename nlp_numerical_type>
        struct alpha_hpwl_ovl_oob
        {
            std::vector<nlp_numerical_type> _alpha;
        };

        template<typename nlp_numerical_type>
        struct alpha_trait<alpha_hpwl_ovl_oob<nlp_numerical_type>>
        {
            typedef alpha_hpwl_ovl_oob<nlp_numerical_type> alpha_type;

            template<typename nlp_type>
            static alpha_type construct(nlp_type &)
            {
                alpha_type alpha;
                alpha._alpha.resize(4, 1.0);
                return alpha;
            }

            template<typename nlp_type>
            static void init(nlp_type &nlp, alpha_type &alpha)
            {
                for (auto & op : nlp._hpwlOps)
                {
                    op.setGetAlphaFunc([&](){ return alpha._alpha[0]; });
                }
                for (auto & op : nlp._ovlOps)
                {
                    op.setGetAlphaFunc([&](){ return alpha._alpha[1]; });
                }
                for (auto & op : nlp._oobOps)
                {
                    op.setGetAlphaFunc([&](){ return alpha._alpha[2]; });
                }
                for (auto & op : nlp._crfOps)
                {
                    op.setGetAlphaFunc([&](){ return alpha._alpha[3]; });
                }
            }

        };

        namespace update
        {
            /// @breif update the alpha that mapping objective function to alpha, from [0, init_obj] -> [min, max]
            /// @tparam the index of which alpha to update
            template<typename nlp_numerical_type, IndexType alphaIdx>
            struct exponential_by_obj
            {
                static constexpr nlp_numerical_type alphaMax = 1.5;
                static constexpr nlp_numerical_type alphaMin = 0.4;
                static constexpr nlp_numerical_type alphaMin_minus_one = alphaMin - 1;
                static constexpr nlp_numerical_type log_alphaMax_minus_alphaMin_plus_1 = std::log(alphaMax - alphaMin_minus_one);
                nlp_numerical_type theConstant = 0.0; ///< log(alpha_max - alpha_min + 1) / init
            };

            template<typename nlp_numerical_type, IndexType alphaIdx>
            struct alpha_update_trait<exponential_by_obj<nlp_numerical_type, alphaIdx>> 
            {
                typedef exponential_by_obj<nlp_numerical_type, alphaIdx> update_type;

                template<typename nlp_type>
                static constexpr typename nlp_type::nlp_numerical_type obj(nlp_type &nlp) 
                { 
                    switch(alphaIdx)
                    {
                        case 0: return nlp._objHpwlRaw; break;
                        case 1: return nlp._objOvlRaw; break;
                        case 2: return nlp._objOobRaw; break;
                        default: return nlp._objCrfRaw; break;
                    }
                }

                template<typename nlp_type>
                static constexpr update_type construct(nlp_type &, alpha_hpwl_ovl_oob<nlp_numerical_type> &) { return update_type(); }

                template<typename nlp_type>
                static constexpr void init(nlp_type &nlp, alpha_hpwl_ovl_oob<nlp_numerical_type> &alpha, update_type &update) 
                { 
                    if (obj(nlp) < REAL_TYPE_TOL)
                    {
                        update.theConstant = -1;
                        alpha._alpha[alphaIdx] = update.alphaMax;
                        return;
                    }
                    update.theConstant = update.log_alphaMax_minus_alphaMin_plus_1 / obj(nlp);
                    alpha._alpha[alphaIdx] = update.alphaMax;
                }

                template<typename nlp_type>
                static constexpr void update(nlp_type &nlp, alpha_hpwl_ovl_oob<nlp_numerical_type> &alpha, update_type &update) 
                { 
                    if (update.theConstant < REAL_TYPE_TOL)
                    {
                        return;
                    }
                    if (obj(nlp) < REAL_TYPE_TOL)
                    {
                        alpha._alpha[alphaIdx] = update.alphaMin;
                        return;
                    }
                    alpha._alpha[alphaIdx] = std::exp(update.theConstant * obj(nlp)) + update.alphaMin - 1;
#ifdef DEBUG_GR
                    DBG("new alpha idx %d %f \n", alphaIdx, alpha._alpha[alphaIdx]);
                    DBG("obj %f , the const %f \n", obj(nlp), update.theConstant);
#endif 
                }

            };

            /// @breif update the alpha that mapping objective function to alpha, from [0, init_obj] -> [min, max]
            /// @tparam the index of which alpha to update
            template<typename nlp_numerical_type, IndexType alphaIdx>
            struct reciprocal_by_obj
            {
                // alpha = a / (x - k * obj_init) + b
                static constexpr nlp_numerical_type alphaMax = 2.0;
                static constexpr nlp_numerical_type alphaMin = 0.4;
                static constexpr nlp_numerical_type k = 100; ///< k > 1.0
                nlp_numerical_type a = -1.0; ///< (k ^2 - k) * obj_init * (alphaMax - alphaMin), should > 0
                nlp_numerical_type b = 1.0; // -k * alphaMax + k * alphaMin + alphaMax; < 0 for most k
                nlp_numerical_type kObjInit = -1.0;
            };

            template<typename nlp_numerical_type, IndexType alphaIdx>
            struct alpha_update_trait<reciprocal_by_obj<nlp_numerical_type, alphaIdx>> 
            {
                typedef reciprocal_by_obj<nlp_numerical_type, alphaIdx> update_type;

                template<typename nlp_type>
                static constexpr typename nlp_type::nlp_numerical_type obj(nlp_type &nlp) 
                { 
                    switch(alphaIdx)
                    {
                        case 0: return nlp._objHpwlRaw; break;
                        case 1: return nlp._objOvlRaw; break;
                        case 2: return nlp._objOobRaw; break;
                        default: return nlp._objCrfRaw; break;
                    }
                }

                template<typename nlp_type>
                static constexpr update_type construct(nlp_type &, alpha_hpwl_ovl_oob<nlp_numerical_type> &) { return update_type(); }

                template<typename nlp_type>
                static constexpr void init(nlp_type &nlp, alpha_hpwl_ovl_oob<nlp_numerical_type> &alpha, update_type &update) 
                { 
                    if (obj(nlp) < REAL_TYPE_TOL)
                    {
                        alpha._alpha[alphaIdx] = update.alphaMax;
                        return;
                    }
                    auto objInit = obj(nlp);
                    alpha._alpha[alphaIdx] = update.alphaMax;
                    update.a = (update.k * update.k - update.k) * objInit * (update.alphaMax - update.alphaMin);
                    update.b = - update.k * update.alphaMax + update.k * update.alphaMin + update.alphaMax;
                    update.kObjInit = update.k * objInit;
                }

                template<typename nlp_type>
                static constexpr void update(nlp_type &nlp, alpha_hpwl_ovl_oob<nlp_numerical_type> &alpha, update_type &update) 
                { 
                    if (update.kObjInit < REAL_TYPE_TOL)
                    {
                        init(nlp, alpha, update);
                        return;
                    }
                    if (obj(nlp) < REAL_TYPE_TOL)
                    {
                        alpha._alpha[alphaIdx] = update.alphaMin;
                        return;
                    }
                    alpha._alpha[alphaIdx] = - update.a / (obj(nlp) - update.kObjInit) + update.b;
#ifdef DEBUG_GR
                    DBG("update alpha:: new alpha idx %d %f \n", alphaIdx, alpha._alpha[alphaIdx]);
#endif
                }

            };


            /// @breif update the alpha that mapping objective function to alpha, from [0, init_obj] -> [min, max]
            /// @tparam the index of which alpha to update
            template<typename nlp_numerical_type, IndexType alphaIdx>
            struct linear_by_obj
            {
                // alpha = a / (x - k * obj_init) + b
                static constexpr nlp_numerical_type alphaMax = 2.0;
                static constexpr nlp_numerical_type alphaMin = 0.4;
                nlp_numerical_type objInit = -1.0;
            };

            template<typename nlp_numerical_type, IndexType alphaIdx>
            struct alpha_update_trait<linear_by_obj<nlp_numerical_type, alphaIdx>> 
            {
                typedef linear_by_obj<nlp_numerical_type, alphaIdx> update_type;

                template<typename nlp_type>
                static constexpr typename nlp_type::nlp_numerical_type obj(nlp_type &nlp) 
                { 
                    switch(alphaIdx)
                    {
                        case 0: return nlp._objHpwlRaw; break;
                        case 1: return nlp._objOvlRaw; break;
                        case 2: return nlp._objOobRaw; break;
                        default: return nlp._objCrfRaw; break;
                    }
                }

                template<typename nlp_type>
                static constexpr update_type construct(nlp_type &, alpha_hpwl_ovl_oob<nlp_numerical_type> &) { return update_type(); }

                template<typename nlp_type>
                static constexpr void init(nlp_type &nlp, alpha_hpwl_ovl_oob<nlp_numerical_type> &alpha, update_type &update) 
                { 
                    if (obj(nlp) < REAL_TYPE_TOL)
                    {
                        alpha._alpha[alphaIdx] = update.alphaMax;
                        return;
                    }
                    auto objInit = obj(nlp);
                    alpha._alpha[alphaIdx] = update.alphaMax;
                    update.objInit = objInit;
                }

                template<typename nlp_type>
                static constexpr void update(nlp_type &nlp, alpha_hpwl_ovl_oob<nlp_numerical_type> &alpha, update_type &update) 
                { 
                    auto objCurr = obj(nlp);
                    if (update.objInit < objCurr)
                    {
                        init(nlp, alpha, update);
                        return;
                    }
                    if (update.objInit < REAL_TYPE_TOL)
                    {
                        init(nlp, alpha, update);
                        return;
                    }
                    if (objCurr < REAL_TYPE_TOL)
                    {
                        alpha._alpha[alphaIdx] = update.alphaMin;
                        return;
                    }
                    alpha._alpha[alphaIdx] = ((update.alphaMax - update.alphaMin) / update.objInit) * objCurr + update.alphaMin ;
#ifdef DEBUG_GR
                    DBG("update alpha:: new alpha idx %d %f \n", alphaIdx, alpha._alpha[alphaIdx]);
#endif
                }

            };


        /// @brief a convenient wrapper for combining different types of stop_condition condition. the list in the template will be check one by one and return converge if any of them say so
        template<typename alpha_update_type, typename... others>
        struct alpha_update_list 
        {
            typedef alpha_update_list<others...> base_type;
            alpha_update_type  _update;
            alpha_update_list<others...> _list;
        };

        template<typename alpha_update_type>
        struct alpha_update_list<alpha_update_type>
        {
            alpha_update_type _update;
        };

        template<typename alpha_update_type, typename... others>
        struct alpha_update_trait<alpha_update_list<alpha_update_type, others...>>
        {
            typedef alpha_update_list<alpha_update_type, others...> list_type;
            typedef typename alpha_update_list<alpha_update_type, others...>::base_type base_type;

            template<typename nlp_type, typename alpha_type>
            static list_type construct(nlp_type &n, alpha_type & a)
            {
                list_type list;
                list._update = std::move(alpha_update_trait<alpha_update_type>::construct(n, a));
                list._list = std::move(alpha_update_trait<base_type>::construct(n, a));
                return list;
            }

            template<typename nlp_type, typename alpha_type>
            static void init(nlp_type &n, alpha_type & a, list_type &c)
            {
                alpha_update_trait<alpha_update_type>::init(n, a, c._update);
                alpha_update_trait<base_type>::init(n, a, c._list);
            }

            template<typename nlp_type, typename alpha_type>
            static void update(nlp_type &n, alpha_type &alpha,  list_type &c) 
            {
                alpha_update_trait<alpha_update_type>::update(n, alpha, c._update);
                alpha_update_trait<base_type>::update(n, alpha, c._list);
            }
        };


        template<typename alpha_update_type>
        struct alpha_update_trait<alpha_update_list<alpha_update_type>>
        {
            typedef alpha_update_list<alpha_update_type> list_type;


            template<typename nlp_type, typename alpha_type>
            static void init(nlp_type &n, alpha_type & a, list_type &c)
            {
                alpha_update_trait<alpha_update_type>::init(n, a, c._update);
            }

            template<typename nlp_type, typename alpha_type>
            static list_type construct(nlp_type &n, alpha_type &alpha)
            {
                list_type list;
                list._update = std::move(alpha_update_trait<alpha_update_type>::construct(n, alpha));
                return list;
            }

            template<typename nlp_type, typename alpha_type>
            static void update(nlp_type &n, alpha_type &alpha,  list_type &c) 
            {
                alpha_update_trait<alpha_update_type>::update(n, alpha, c._update);
            }
        };

        } // namespace update
    } // namespace alpha
} //namespace nlp
PROJECT_NAMESPACE_END
