/**********************************************************************

wn_insert_sparse_matrix_value(mat,value,i,j)
wn_insert_sparse_matrix_entry(mat,entry)
wn_sort_sparse_matrix(mat)

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
#ifndef wnspconH
#define wnspconH



#include "wnspmat.h"



EXTERN void wn_insert_sparse_matrix_entry
(
  wn_sparse_matrix mat,
  wn_sparse_matrix_entry entry
);
EXTERN void wn_insert_sparse_matrix_value
(
  wn_sparse_matrix mat,
  double value,
  int i,
  int j
);
EXTERN void wn_sort_sparse_matrix(wn_sparse_matrix mat);



#endif

