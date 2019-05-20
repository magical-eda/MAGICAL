/**
 * @file Design rule checker.h
 * @brief The class for checking design rules
 * @author Keren Zhu
 * @date 01/20/2019
 */

#ifndef AROUTER_SHAPE_CHECKER_H_
#define AROUTER_SHAPE_CHECKER_H_

#include "dr/SearchInstruct.h"

PROJECT_NAMESPACE_BEGIN



enum CheckResult : Byte
{
    ILLEGAL = 0,
    LEGAL = 1,
    PENALTY = 2
};

inline std::string checkResult2Str(CheckResult checkResult)
{
    if (checkResult == CheckResult::ILLEGAL)
    {
        return "ILLEGAL";
    }
    if (checkResult == CheckResult::LEGAL)
    {
        return "LEGAL";
    }
    if (checkResult == CheckResult::PENALTY)
    {
        return "PENALTY";
    }
    return "UKNOWN";
}
class ShapeChecker
{
    public:
        explicit ShapeChecker(SearchInstruct &instr) : _instr(instr) {}

        /*------------------------------*/ 
        /* Individual design rule check */
        /*------------------------------*/ 
        /// @brief check the spacing rules between two box on a layer
        /// @param two boxes representing the shape and the index of the routable layer they are on
        /// @return the result of the checking
        CheckResult metalSpacingChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType layer) const;
        /// @brief check the min metal area rules on a layer
        /// @param the box representing the shape of the wire. layer is the routable layer
        /// @return the result of the checking
        CheckResult metalMinAreaChecking(const Box<LocType> &wireShape, IndexType layer) const;
        /// @brief check the spacing requirement between two cuts from different nets
        /// @param the two boxes represent the shapes, and the routable layer index of the lower metal of the via
        /// @return the result of the checking
        CheckResult cutSpacingDifNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const;
        /// @brief check the spacing requirement between two cuts from Same net
        /// @param the two boxes represent the shapes, and the routable layer index of the lower metal of the via
        /// @return the result of the checking
        CheckResult cutSpacingSameNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const;
        /// @brief check the metal rules for two shapes within the same net
        /// @param two boxes representing the shape and the index of the routable layer they are on
        /// @return the result of the checking
        CheckResult sameNetMetalCheck(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType layer) const;
    private:
        SearchInstruct &_instr; ///< Include the processed design rule information
};


inline CheckResult ShapeChecker::cutSpacingDifNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const 
{
    CheckResult checkResult = CheckResult::LEGAL;
    LocType spacing = klib::boxSpacing(lhs, rhs);
    if (klib::boxAreParallelOverlap(lhs, rhs))
    {
        if (spacing < _instr.cutParallelOverlapSpacings(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    else
    {
        if (spacing < _instr.cutSpacing(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    return checkResult;
}

inline CheckResult ShapeChecker::cutSpacingSameNetChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType lowerMetalLayer) const 
{
    CheckResult checkResult = CheckResult::LEGAL;
    LocType spacing = klib::boxSpacing(lhs, rhs);
    if (klib::boxUnionRectangle(lhs, rhs))
    {
        checkResult = CheckResult::LEGAL;
    }
    else if (klib::boxAreParallelOverlap(lhs, rhs))
    {
        if (spacing < _instr.cutParallelOverlapSpacings(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    else
    {
        if (spacing < _instr.cutSameNetSpacing(lowerMetalLayer))
        {
            checkResult = CheckResult::ILLEGAL;
        }
    }
    return checkResult;
}

inline CheckResult ShapeChecker::metalSpacingChecking(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType layer) const
{
    /// Calculate the geometric information of two rects
    auto bundle = DRUtil::spacingCheckBundle(lhs, rhs);
    LocType spacing = bundle.first.first;
    LocType parallel = bundle.first.second;
    LocType width = std::max(bundle.second.first, bundle.second.second);

    /// Calculate the required spacing between two boxes
    LocType spacingRule = _instr.metalSpacing(width, parallel, layer);


    if (spacing < spacingRule) { return CheckResult::ILLEGAL; }
    return CheckResult::LEGAL;
}

inline CheckResult ShapeChecker::metalMinAreaChecking(const Box<LocType> &wireShape, IndexType layer) const
{
    /// Fetch the required area
    LocType minArea = _instr.metalMinArea(layer);
    
    /// Calculate the rect area
    LocType rectArea = wireShape.area();

    if (rectArea < minArea) { return CheckResult::ILLEGAL; }
    return CheckResult::LEGAL;
}


inline CheckResult ShapeChecker::sameNetMetalCheck(const Box<LocType> &lhs, const Box<LocType> &rhs, IndexType layer) const
{
    /// FIXME
    return CheckResult::LEGAL;
    if (!lhs.overlap(rhs))
    {
        /// If disjoint, then process normal off-track spacing checks
        return metalSpacingChecking(lhs, rhs, layer);
    }
    else
    {
        /// TODO: add rules checking for the on track!
        return CheckResult::LEGAL;
    }
}
PROJECT_NAMESPACE_END

#endif ///AROUTER_SHAPE_CHECKER_H_
