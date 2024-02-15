/**
 * @file   lefViaRule.hpp
 * @brief  LEF ViaRule configuration
 * @author Keren Zhu
 * @date   02/07/2020
 *
 **/

#ifndef _DB_LEF_VIA_RULE_HPP_
#define _DB_LEF_VIA_RULE_HPP_


#include "src/global/global.hpp"
#include "src/geo/box.hpp"

namespace LefDefParser
{
    class lefiViaRule;
}

PROJECT_NAMESPACE_START

/// Standard ViaRule
class LefViaRule
{
    friend class LefReader;
    public:
        explicit LefViaRule() = default;
    public:
        std::string name = ""; ///< The name of this LefViaRule
};


/// @brief ViaRule template 1.  No special Via property. enclosure and width in metals. rect and spacing in via Based on tsmc40 lef.
class LefViaRuleTemplate1 : public LefViaRule
{
    public:
        explicit LefViaRuleTemplate1() = default;
        /// @brief construct the object with lefiViaRule. 
        /// @param a lefiViaRule class from LEF parser
        /// @return true: if fitted into the template. false: not fitted into the template
        bool constructFromLefParser(const LefDefParser::lefiViaRule &v,  std::function<Int_t(Float_t)> lefToDbUnitCnvter);
        /// @brief swap two metal layers
        void swapMetalLayers()
        {
            std::swap(metalLayerNames[0], metalLayerNames[1]);
            std::swap(enclosureOverhang1[0], enclosureOverhang1[1]);
            std::swap(enclosureOverhang2[0], enclosureOverhang2[1]);
            std::swap(widthLo[0], widthLo[1]);
            std::swap(widthHi[0], widthHi[1]);
        }
        std::string str() const;
    public:
        std::array<std::string, 2> metalLayerNames;
        std::array<Int_t, 2> enclosureOverhang1;
        std::array<Int_t, 2> enclosureOverhang2;
        std::array<Int_t, 2> widthLo;
        std::array<Int_t, 2> widthHi;
        std::string viaLayerName = "";
        Int_t rectXLo = 0.0;
        Int_t rectYLo = 0.0;
        Int_t rectXHi = 0.0;
        Int_t rectYHi = 0.0;
        Int_t spacingStepX = 0.0;
        Int_t spacingStepY = 0.0;
};

PROJECT_NAMESPACE_END

#endif //DB_LEF_VIA_RULE_HPP_
