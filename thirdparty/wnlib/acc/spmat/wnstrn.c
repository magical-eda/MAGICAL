/**********************************************************************

wn_transpose_sparse_matrix(mat)

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

#include "wnlib.h"
#include "wnswap.h"
#include "wnspmat.h"



void wn_transpose_sparse_matrix(wn_sparse_matrix mat)
{
  int i;
  register wn_sll el;
  register wn_sparse_matrix_entry entry;

  wn_swap(mat->len_i,mat->len_j,int);
  wn_swap(mat->i_lists,mat->j_lists,wn_sll *);

  for(i=0;i<(mat->len_i);++i)
  {
    for(el=((mat->i_lists)[i]);el != NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);

      wn_swap(entry->i,entry->j,int);
    }
  }
}



