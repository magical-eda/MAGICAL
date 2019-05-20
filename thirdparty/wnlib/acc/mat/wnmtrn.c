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


void wn_transpose_mat(double **to_mat,double **from_mat,int len_i,int len_j)
{
  int i,j;

  for(i=0;i<len_i;++i)  
  for(j=0;j<len_j;++j)
  {
    to_mat[j][i] = from_mat[i][j];
  }
}




