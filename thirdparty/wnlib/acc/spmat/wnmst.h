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
#ifndef wnmstH
#define wnmstH



#include "wnspmat.h"



EXTERN void wn_min_spanning_tree
(
  int *pcode,
  double *plen,
  wn_sparse_matrix *presult,
  wn_sparse_matrix wn_spmat
);



#endif

