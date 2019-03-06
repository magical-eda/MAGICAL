/**********************************************************************

wn_make_vect(&vect,len_i)
wn_make_mat(&mat,len_i,len_j)

wn_free_vect(vect,len_i)
wn_free_mat(mat,len_i,len_j)

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



void wn_make_vect(pvect,len_i)

double **pvect;
int len_i;

{
  *pvect = (double *)wn_alloc(len_i*wn_sizeof(double));

  wn_zero_vect(*pvect,len_i);
}


void wn_make_mat(pmat,len_i,len_j)

double ***pmat;
int len_i,len_j;

{
  int i;

  *pmat = (double **)wn_alloc(len_i*wn_sizeof(double *));

  for(i=0;i<len_i;i++)
  {
    wn_make_vect(&((*pmat)[i]),len_j);
  }
}


/*ARGSUSED*/ void wn_free_vect(vect,len_i)

double *vect;
int len_i;

{
  wn_free((ptr)vect);
}


void wn_free_mat(mat,len_i,len_j)

double **mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_free_vect(mat[i],len_j);
  }

  wn_free((ptr)mat);
}




