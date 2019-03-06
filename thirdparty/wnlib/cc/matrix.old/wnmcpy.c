/**********************************************************************

wn_copy_vector(&to_vect,from_vect)
wn_copy_raw_vector(to_raw_vect,from_raw_vect,len_i)

wn_copy_matrix(&out_mat,in_mat)
wn_copy_raw_matrix(out_raw_mat,in_raw_mat,len_i,len_j)

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



wn_copy_vector(pto_vect,from_vect)

wn_vector *pto_vect,from_vect;

{
  int len_i;

  len_i = from_vect->len_i;

  wn_make_vector(pto_vect,len_i);

  wn_copy_raw_vector((*pto_vect)->numbers,from_vect->numbers,len_i);
}



wn_copy_raw_vector(to_raw_vect,from_raw_vect,len_i)

double to_raw_vect[],from_raw_vect[];
int len_i;

{
  wn_arraycpy(to_raw_vect,from_raw_vect,len_i,double);
}



wn_copy_matrix(pout_mat,in_mat)

wn_matrix *pout_mat,in_mat;

{
  int len_i,len_j;

  len_i = in_mat->len_i;
  len_j = in_mat->len_j;

  wn_make_matrix(pout_mat,len_i,len_j);

  wn_copy_raw_matrix((*pout_mat)->numbers,in_mat->numbers,len_i,len_j);
}



wn_copy_raw_matrix(out_raw_mat,in_raw_mat,len_i,len_j)

double **out_raw_mat,**in_raw_mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_copy_raw_vector(out_raw_mat[i],in_raw_mat[i],len_j);
  }
}


