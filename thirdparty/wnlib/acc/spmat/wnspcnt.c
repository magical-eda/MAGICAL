/**********************************************************************

int wn_sparse_matrix_entry_count(mat)

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
#include "wnsll.h"

#include "wnspmat.h"
#include "wnspcnt.h"



int wn_sparse_matrix_entry_count(wn_sparse_matrix mat)
{
  int i;
  int count;

  count = 0;

  for(i=0;i<(mat->len_i);++i)
  {
    count += wn_sllcount((mat->i_lists)[i]);
  }

  return(count);
}



