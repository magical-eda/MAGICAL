/**
 * @file linear_programming_trait.h
 * @brief The wrappers for linear programming interface
 * @author Keren Zhu
 * @date 03/19/2020
 */

#ifndef KLIB_LINEAR_PROGRAMMING_H_
#define KLIB_LINEAR_PROGRAMMING_H_

#include "global/namespace.h"
#include "lp_limbo.h"

namespace klib {
// Select LP solver
namespace _lp {

    template<int rank>
    struct lp_rank
    {
        static const bool if_enable = true;
        typedef _undefined LpModel;
        typedef linear_programming_trait<_undefined> LpTrait;
    };

    template<>
    struct lp_rank<0>
    {
#ifndef LP_NOT_USE_GUROBI
        static const bool if_enable = true;
        typedef LimboLpGurobi LpModel;
        typedef LimboLpGurobiTrait LpTrait;
#else
        static const bool if_enable = false;
#endif
    };

    template<>
    struct lp_rank<1>
    {
#ifndef LP_NOT_USE_LPSOLVE
        static const bool if_enable = true;
        typedef LimboLpsolve LpModel;
        typedef LimboLpsolveTrait LpTrait;
#else
        static const bool if_enable = false;
#endif
    };

    template<int rank, bool>
    struct select_lp {};
    template<int rank>
    struct select_lp<rank, true>
    {
        typedef typename lp_rank<rank>::LpModel LpModel;
        typedef typename lp_rank<rank>::LpTrait LpTrait;
    };
    template<int rank>
    struct select_lp<rank, false>
    {
        typedef lp_rank<rank+1> rank_type;
        typedef typename select_lp<rank+1, rank_type::if_enable>::LpModel LpModel;
        typedef typename select_lp<rank+1, rank_type::if_enable>::LpTrait LpTrait;
    };
}; //namspace _lp

namespace lp
{
    typedef typename _lp::select_lp<0, _lp::lp_rank<0>::if_enable>::LpModel LpModel;
    typedef typename _lp::select_lp<0, _lp::lp_rank<0>::if_enable>::LpTrait LpTrait;
};

} //namespace klib

#endif //KLIB_LINEAR_PROGRAMMING_H_
