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
#ifndef wnsrndH
#define wnsrndH



#include "wnspmat.h"



EXTERN void wn_random_sparse_matrix
(
  wn_sparse_matrix *pmat,
  int len_i,
  int len_j,
  double low,
  double high,
  double density
);
EXTERN void wn_random_symmetric_sparse_matrix
(
  wn_sparse_matrix *pmat,
  int len_i,
  double low,
  double high,
  double density
);



#endif

