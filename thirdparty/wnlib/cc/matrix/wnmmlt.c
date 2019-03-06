/**********************************************************************

wn_mult_vect_by_scalar(vect,scalar,len_i)
wn_mult_mat_by_scalar(mat,scalar,len_i,len_j)

double wn_dot_vects(vect1,vect2,len_i)

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
#include "wnmat.h"



void wn_mult_vect_by_scalar(vect,scalar,len_i)

register double vect[];
register double scalar;
register int len_i;

{
  register int i;

  for(i=0;i<len_i;i++)
  {
    vect[i] *= scalar;
  }
}


void wn_mult_mat_by_scalar(mat,scalar,len_i,len_j)

double **mat;
double scalar;
int len_i;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_mult_vect_by_scalar(mat[i],scalar,len_j);
  }
}


/*****************************************************************

  This code is the inner loop of an n^3 algorithm, so it must
  be fast.  That is why this code is so ugly.

*****************************************************************/
double wn_dot_vects(vect1,vect2,len_i)

register double *vect1,*vect2;
int len_i;

{
  register double result;

  if(len_i <= 0)
  {
    return(0.0);
  }

  result = ( (*(vect1))*(*(vect2)) );

small:
  switch(len_i)
  {
    case(16):  result += ( (*(++vect1))*(*(++vect2)) );
    case(15):  result += ( (*(++vect1))*(*(++vect2)) );
    case(14):  result += ( (*(++vect1))*(*(++vect2)) );
    case(13):  result += ( (*(++vect1))*(*(++vect2)) );
    case(12):  result += ( (*(++vect1))*(*(++vect2)) );
    case(11):  result += ( (*(++vect1))*(*(++vect2)) );
    case(10):  result += ( (*(++vect1))*(*(++vect2)) );
    case(9):   result += ( (*(++vect1))*(*(++vect2)) );
    case(8):   result += ( (*(++vect1))*(*(++vect2)) );
    case(7):   result += ( (*(++vect1))*(*(++vect2)) );
    case(6):   result += ( (*(++vect1))*(*(++vect2)) );
    case(5):   result += ( (*(++vect1))*(*(++vect2)) );
    case(4):   result += ( (*(++vect1))*(*(++vect2)) );
    case(3):   result += ( (*(++vect1))*(*(++vect2)) );
    case(2):   result += ( (*(++vect1))*(*(++vect2)) );
    case(1):   break;
    case(0):   result = 0.0;
	       break;

    default:
    {
      wn_assert(len_i > 16);

      do
      {
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );

	len_i -= 16;
      }
      while(len_i > 16);

      goto small;
    }
  }

  return(result);
}


void wn_mult_mat_by_vect(result_vect,mat,vect,len_i,len_j)

double *result_vect,**mat,*vect;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    result_vect[i] = wn_dot_vects(mat[i],vect,len_j);
  }
}


void wn_mult_mats(result_mat,mat1,mat2,len_i,len_j,len_k)

double **result_mat,**mat1,**mat2;
int len_i,len_j,len_k;

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




