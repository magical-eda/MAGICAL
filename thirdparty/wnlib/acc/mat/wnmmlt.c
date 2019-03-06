/**********************************************************************

wn_mult_mat_by_vect(result_vect,mat,vect,len_i,len_j)

wn_mult_mats(result_mat,mat1,mat2,len_i,len_j,len_k)

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
#include "wnmem.h"
#include "wnvect.h"

#include "wnmat.h"



void wn_mult_mat_by_vect(double *result_vect,double **mat,double *vect,
			 int len_i,int len_j)
{
  int i;

  for(i=0;i<len_i;i++)
  {
    result_vect[i] = wn_dot_vects(mat[i],vect,len_j);
  }
}


void wn_mult_mats(double **result_mat,double **mat1,double **mat2,
		  int len_i,int len_j,int len_k)
{
  double *vect2;
  int i,j,k;

  wn_gpmake("no_free");
  wn_gplabel("matrix multiply group");

  wn_make_vect(&vect2,len_j);

  for(k=0;k<len_k;k++)
  {
    for(j=0;j<len_j;j++)
    {
      vect2[j] = mat2[j][k];
    }

    for(i=0;i<len_i;i++)
    {
      result_mat[i][k] = wn_dot_vects(mat1[i],vect2,len_j);
    }
  }

  wn_gpfree();
}




