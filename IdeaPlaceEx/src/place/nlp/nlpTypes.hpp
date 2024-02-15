/**
 * @file nlpOptmKernel.hpp
 * @brief The non-lnear programming for global plaement: basic type denifitions e
 * @author Keren Zhu
 * @date 04/05/2020
 */

#pragma once

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

template<typename nlp_settings>
class NlpGPlacerBase;

template<typename nlp_settings>
class NlpGPlacerFirstOrder;

template<typename nlp_settings>
class NlpGPlacerSecondOrder;

namespace nlp
{
    template<typename nlp_type>
    struct is_first_order_diff : std::false_type {};
    
    template<typename nlp_settings>
    struct is_first_order_diff<NlpGPlacerFirstOrder<nlp_settings>> : std::true_type {};

    template<typename nlp_settings>
    struct is_first_order_diff<NlpGPlacerSecondOrder<nlp_settings>> : std::true_type {};

    template<typename nlp_type>
    struct is_second_order_diff : std::false_type {};

    template<typename nlp_settings>
    struct is_second_order_diff<NlpGPlacerSecondOrder<nlp_settings>> : std::true_type {};
} //namespace nlp

PROJECT_NAMESPACE_END
