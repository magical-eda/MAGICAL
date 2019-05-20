/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/**********************************************************************

wn_trans_problem(&code,&objective,&result,
                 cost_mat,
                 i_capacities,j_capacities)

**********************************************************************/

#include "wnlib.h"
#include "wnasrt.h"
#include "wnspmat.h"
#include "wntrn.h"



void wn_trans_problem
(
  int *pcode,
  double *pobjective,
  wn_sparse_matrix *presult,
  wn_sparse_matrix cost_mat,
  double i_capacities[],
  double j_capacities[]
)
{
  wn_sparse_matrix feasible_result;
  wn_memgp top_group;
  double delta;

  top_group = wn_curgp();
  wn_gpmake("no_free");

  wn_trans_problem_feasible(pcode,&feasible_result,
                            cost_mat,
                            i_capacities,j_capacities);

/*
  wn_print_sparse_matrix(feasible_result);
*/

  if(*pcode == WN_SUBOPTIMAL)
  {
     wn_gppush(top_group);

     wn_trans_problem_simplex_improve(pcode,pobjective,&delta,
                                      presult,feasible_result,
                                      cost_mat,
                                      WN_IHUGE);

     wn_assert(*pcode == WN_SUCCESS);
                        
     wn_gppop();
  }
  else
  {
    *pobjective = WN_FHUGE;
  }

  wn_gpfree();
}



