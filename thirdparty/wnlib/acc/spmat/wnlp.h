/**********************************************************************

wn_longest_path(&code,&len,&result,length_mat,start_node,fin_node)

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
#ifndef wnlpH
#define wnlpH



#include "wnspmat.h"



EXTERN void wn_longest_path
(
  int *pcode,
  double *plen,
  wn_sll *presult,         /* list of edges */
  wn_sparse_matrix length_mat,
  int passed_start_node,
  int passed_fin_node
);



#endif

