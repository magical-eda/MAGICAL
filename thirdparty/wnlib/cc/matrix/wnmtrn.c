/**********************************************************************

wn_transpose_mat(mat,len_i,len_j)

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
#include "wnmat.h"

#include "wnswap.h"


void wn_transpose_mat(mat,len_i,len_j)

register double **mat;
int len_i;
int len_j;

{
  double tmp;
  register int i,j;

  for(i=0;i<len_i;++i)  
  for(j=0;j<i;++j)
  {
    tmp = mat[i][j];
    mat[i][j] = mat[j][i];
    mat[j][i] = tmp;
  }
}




