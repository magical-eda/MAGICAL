/**********************************************************************

wn_copy_vect(to_vect,from_vect,len_i)

wn_copy_mat(out_mat,in_mat,len_i,len_j)

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
#include "wnary.h"



void wn_copy_vect(to_vect,from_vect,len_i)

double to_vect[],from_vect[];
int len_i;

{
  wn_blkcpy((ptr)to_vect,(ptr)from_vect,len_i*wn_sizeof(double));
}


void wn_copy_mat(out_mat,in_mat,len_i,len_j)

double **out_mat,**in_mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;++i)
  {
    wn_copy_vect(out_mat[i],in_mat[i],len_j);
  }
}


