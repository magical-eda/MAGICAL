/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor

****************************************************************************/
#ifndef wntrnH
#define wntrnH



#include "wnspmat.h"



EXTERN void wn_trans_problem
(
  int *pcode,
  double *pobjective,
  wn_sparse_matrix *presult,
  wn_sparse_matrix cost_mat,
  double i_capacities[],
  double j_capacities[]
);

EXTERN void wn_trans_problem_feasible
(
  int *pcode,
  wn_sparse_matrix *presult,
  wn_sparse_matrix cost_mat,
  double i_capacities[],
  double j_capacities[]
);

EXTERN void wn_trans_problem_simplex_improve
(
  int *pcode,
  double *pobjective,
  double *pdelta,
  wn_sparse_matrix *pnew_result,
  wn_sparse_matrix result,
  wn_sparse_matrix cost_mat,
  int max_time
);

EXTERN void wn_trans_problem_swap_improve
(
  int *pcode,
  double *pobjective,
  double *pdelta,
  wn_sparse_matrix *pnew_result,
  wn_sparse_matrix result,
  wn_sparse_matrix cost_mat,
  int max_time
);


#endif

