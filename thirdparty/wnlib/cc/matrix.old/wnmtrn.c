/**********************************************************************

wn_transpose_matrix(&result_mat,mat)
wn_transpose_raw_matrix(result_raw_mat,raw_mat,len_i,len_j)

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



wn_transpose_matrix(presult_mat,mat)

wn_matrix *presult_mat,mat;

{
  int len_i,len_j;

  len_i = mat->len_i;
  len_j = mat->len_j;

  wn_make_matrix(presult_mat,len_j,len_i);

  wn_transpose_raw_matrix((*presult_mat)->numbers,mat->numbers,len_i,len_j);
}



wn_transpose_raw_matrix(result_raw_mat,raw_mat,len_i,len_j)

register double **result_raw_mat,**raw_mat;
int len_i;
register int len_j;

{
  int i;
  register int j;

  for(i=0;i<len_i;i++)  for(j=0;j<len_j;j++)
  {
    result_raw_mat[j][i] = raw_mat[i][j];
  }
}




