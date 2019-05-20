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
#ifndef wnflowH
#define wnflowH



#include "wnspmat.h"



EXTERN void wn_max_flow
(
  double *pflow,
  double *pcost,
  wn_sparse_matrix *presult,
  wn_sparse_matrix capacity_mat,
  wn_sparse_matrix cost_mat,
  int source_i_index,
  int dest_j_index
);


#endif

