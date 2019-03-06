/**********************************************************************

wn_enter_sparse_matrix(pmat)

wn_print_sparse_matrix(mat)
wn_print_symmetric_sparse_matrix(mat)

**********************************************************************/
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
#ifndef wnspioH
#define wnspioH



#include "wnspmat.h"



EXTERN void wn_enter_sparse_matrix(wn_sparse_matrix *pmat);
EXTERN void wn_print_sparse_matrix(wn_sparse_matrix mat);
EXTERN void wn_print_sparse_matrix_entry(wn_sparse_matrix_entry entry);
EXTERN void wn_print_symmetric_sparse_matrix(wn_sparse_matrix mat);



#endif


