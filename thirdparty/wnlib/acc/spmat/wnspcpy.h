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
#ifndef wnspcpyH
#define wnspcpyH



#include "wnspmat.h"



EXTERN void wn_copy_sparse_matrix(wn_sparse_matrix *pout,wn_sparse_matrix in);
EXTERN void wn_symmetrize_sparse_matrix(wn_sparse_matrix mat);



#endif

