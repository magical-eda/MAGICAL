/**********************************************************************

wn_make_vector(&vect,len_i)
wn_make_raw_vector(&raw_vect,len_i)

wn_make_matrix(&mat,len_i,len_j)
wn_make_raw_matrix(&raw_mat,len_i,len_j)

wn_free_vector(vect)
wn_free_raw_vector(raw_vect,len_i)

wn_free_matrix(mat)
wn_free_raw_matrix(raw_mat,len_i,len_j)

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



wn_make_vector(pvect,len_i)

wn_vector *pvect;
int len_i;

{
  *pvect = wn_znew(wn_vector);

  wn_make_raw_vector(&((*pvect)->numbers),len_i);

  (*pvect)->len_i = len_i;
}



wn_make_raw_vector(praw_vect,len_i)

double **praw_vect;
int len_i;

{
  *praw_vect = (double *)wn_alloc(len_i*wn_sizeof(double));

  wn_zero_raw_vector(*praw_vect,len_i);
}



wn_make_matrix(pmat,len_i,len_j)

wn_matrix *pmat;
int len_i,len_j;

{
  *pmat = wn_znew(wn_matrix);

  wn_make_raw_matrix(&((*pmat)->numbers),len_i,len_j);

  (*pmat)->len_i = len_i;
  (*pmat)->len_j = len_j;
}



wn_make_raw_matrix(praw_mat,len_i,len_j)

double ***praw_mat;
int len_i,len_j;

{
  int i;

  *praw_mat = (double **)wn_alloc(len_i*wn_sizeof(double *));

  for(i=0;i<len_i;i++)
  {
    wn_make_raw_vector(&((*praw_mat)[i]),len_j);
  }
}



wn_free_vector(vect)

wn_vector vect;

{
  wn_free_raw_vector(vect->numbers,vect->len_i);
  wn_free((ptr)vect);
}



/*ARGSUSED*/ wn_free_raw_vector(raw_vect,len_i)

double *raw_vect;
int len_i;

{
  wn_free((ptr)raw_vect);
}



wn_free_matrix(mat)

wn_matrix mat;

{
  wn_free_raw_matrix(mat->numbers,mat->len_i,mat->len_j);
  wn_free((ptr)mat);
}



wn_free_raw_matrix(raw_mat,len_i,len_j)

double **raw_mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_free_raw_vector(raw_mat[i],len_j);
  }

  wn_free((ptr)raw_mat);
}




