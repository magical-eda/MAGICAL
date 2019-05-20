/**********************************************************************

wn_zero_raw_vector(raw_vect,len_i)

wn_zero_raw_matrix(raw_mat,len_i,len_j)

wn_identity_matrix(&mat,len_i)
wn_identity_raw_matrix(raw_mat,len_i)

wn_hilbert_matrix(&mat,len_i)
wn_hilbert_raw_matrix(raw_mat,len_i)

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



wn_zero_raw_vector(raw_vect,len_i)

double raw_vect[];
int len_i;

{
  wn_arrayfill(raw_vect,len_i,0.0,double);
}



wn_zero_raw_matrix(raw_mat,len_i,len_j)

double **raw_mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_zero_raw_vector(raw_mat[i],len_j);
  }
}



wn_identity_matrix(pmat,len_i)

wn_matrix *pmat;
int len_i;

{
  wn_make_matrix(pmat,len_i,len_i);

  wn_identity_raw_matrix((*pmat)->numbers,len_i);
}



wn_identity_raw_matrix(raw_mat,len_i)

double **raw_mat;
int len_i;

{
  int i;

  wn_zero_raw_matrix(raw_mat,len_i,len_i);

  for(i=0;i<len_i;i++)
  {
    raw_mat[i][i] = 1.0;
  }
}



wn_hilbert_matrix(pmat,len_i)

wn_matrix *pmat;
int len_i;

{
  wn_make_matrix(pmat,len_i,len_i);

  wn_hilbert_raw_matrix((*pmat)->numbers,len_i);
}



wn_hilbert_raw_matrix(raw_mat,len_i)

double **raw_mat;
int len_i;

{
  int i,j;

  for(i=0;i<len_i;i++)  for(j=0;j<len_i;i++)
  {
    raw_mat[i][j] = 1.0/((double)(i+j+1));
  }
}



