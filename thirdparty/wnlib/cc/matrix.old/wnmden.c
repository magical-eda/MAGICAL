/**********************************************************************

wn_matrix_density(&density,mat)
wn_raw_matrix_density(&density,raw_mat)

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



wn_matrix_density(pdensity,mat)

double *pdensity;
wn_matrix mat;

{
  wn_raw_matrix_density(pdensity,mat->numbers,mat->len_i,mat->len_j);
}



wn_raw_matrix_density(pdensity,raw_mat,len_i,len_j)

double *pdensity,**raw_mat;
int len_i,len_j;

{
  int total,total_non_zero;
  int i,j;

  total = 0;
  total_non_zero = 0;

  for(i=0;i<len_i;i++)  for(j=0;j<len_j;j++)
  {
    if(raw_mat[i][j] != 0.0)
    {
      total_non_zero++;
    }

    total++;
  }

  *pdensity = ((double)total_non_zero)/((double)total);
}




