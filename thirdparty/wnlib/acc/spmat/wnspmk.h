/**********************************************************************

wn_make_sparse_matrix(&mat,len_i,len_j)

wn_free_sparse_matrix(mat)

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
#ifndef wnsmkH
#define wnsmkH


#include "wnlib.h"
#include "wnspmat.h"



EXTERN void wn_make_sparse_matrix
(
  wn_sparse_matrix *pmat,
  int len_i,
  int len_j
);
EXTERN void wn_free_sparse_matrix(wn_sparse_matrix mat);

EXTERN void wn_make_sparse_matrix_entry
(
  wn_sparse_matrix_entry *pentry,
  double value,
  int i,
  int j
);



#endif


