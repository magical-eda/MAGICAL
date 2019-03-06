/**********************************************************************

wn_multiply_into_vector_scalar(vect,scalar)
wn_multiply_into_raw_vector_scalar(raw_vect,scalar,len_i)

wn_multiply_into_matrix_scalar(mat,scalar)
wn_multiply_into_raw_matrix_scalar(raw_mat,scalar,len_i,len_j)

wn_multiply_into_vector_vector(to_vect,vect)
wn_multiply_into_raw_vector_raw_vector(to_raw_vect,raw_vect,len_i)

wn_vector_dot_vector(&result,vect1,vect2)
wn_raw_vector_dot_raw_vector(&result,raw_vect1,raw_vect2,len_i)

wn_matrix_multiply_vector(&result_vect,mat,vect)
wn_raw_matrix_multiply_raw_vector(result_raw_vect,raw_mat,raw_vect,
                                  len_i,len_j)

wn_matrix_multiply_matrix(&result_mat,mat1,mat2)
wn_raw_matrix_multiply_raw_matrix(result_raw_mat,raw_mat1,raw_mat2,
                                  len_i,len_j,len_k)

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



wn_multiply_into_vector_scalar(vect,scalar)

wn_vector vect;
double scalar;

{
  wn_multiply_into_raw_vector_scalar(vect->numbers,scalar,vect->len_i);
}



wn_multiply_into_raw_vector_scalar(raw_vect,scalar,len_i)

register double raw_vect[];
register double scalar;
register int len_i;

{
  register int i;

  for(i=0;i<len_i;i++)
  {
    raw_vect[i] *= scalar;
  }
}



wn_multiply_into_matrix_scalar(mat,scalar)

wn_matrix mat;
double scalar;

{
  wn_multiply_into_raw_matrix_scalar(mat->numbers,scalar,
                                     mat->len_i,mat->len_j);
}



wn_multiply_into_raw_matrix_scalar(raw_mat,scalar,len_i,len_j)

double **raw_mat;
double scalar;
int len_i;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_multiply_into_raw_vector_scalar(raw_mat[i],scalar,len_j);
  }
}



wn_multiply_into_vector_vector(to_vect,vect)

wn_vector to_vect,vect;

{
  wn_assertmsg(to_vect->len_i == vect->len_i,"bad lens");

  wn_multiply_into_raw_vector_raw_vector(to_vect->numbers,vect->numbers,
                                         vect->len_i);
}



wn_multiply_into_raw_vector_raw_vector(to_raw_vect,raw_vect,len_i)

register double to_raw_vect[],raw_vect[];
register int len_i;

{
  register int i;

  for(i=0;i<len_i;i++)
  {
    to_raw_vect[i] *= raw_vect[i];
  }
}



wn_vector_dot_vector(presult,vect1,vect2)

double *presult;
wn_vector vect1,vect2;

{
  wn_assertmsg(vect1->len_i == vect2->len_i,"bad lens");

  wn_raw_vector_dot_raw_vector(presult,
                               vect1->numbers,vect2->numbers,
                               vect1->len_i);
}



#if 0   /* wcn: deleted 11/6/89 */
/*****************************************************************

  This code is the inner loop of an n^3 algorithm, so it must
  be fast.  That is why this code is so ugly.

*****************************************************************/
wn_raw_vector_dot_raw_vector(presult,raw_vect1,raw_vect2,len_i)

double *presult;
register double *raw_vect1,*raw_vect2;
int len_i;

{
  register double *fin;
  register double result;

  result = 0.0;

  fin = raw_vect2 + len_i;

  while(raw_vect2 != fin)
  {
    result += ( (*(raw_vect1++))*(*(raw_vect2++)) );
  }

  *presult = result;
}
#endif
  


/*****************************************************************

  This code is the inner loop of an n^3 algorithm, so it must
  be fast.  That is why this code is so ugly.

*****************************************************************/
wn_raw_vector_dot_raw_vector(presult,raw_vect1,raw_vect2,len_i)

double *presult;
register double *raw_vect1,*raw_vect2;
int len_i;

{
  register double result;

  result = ( (*(raw_vect1))*(*(raw_vect2)) );

small:
  switch(len_i)
  {
    case(16):  result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(15):  result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(14):  result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(13):  result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(12):  result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(11):  result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(10):  result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(9):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(8):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(7):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(6):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(5):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(4):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(3):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(2):   result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
    case(1):   break;
    case(0):   result = 0.0;
	       break;

    default:
    {
      wn_assert(len_i > 16);

      do
      {
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );
        result += ( (*(++raw_vect1))*(*(++raw_vect2)) );

	len_i -= 16;
      }
      while(len_i > 16);

      goto small;
    }
  }

  *presult = result;
}



wn_matrix_multiply_vector(presult_vect,mat,vect)

wn_vector *presult_vect,vect;
wn_matrix mat;

{
  int len_i,len_j;

  wn_assertmsg(mat->len_j == vect->len_i,"lens wrong.");

  len_i = mat->len_i;
  len_j = mat->len_j;

  wn_make_vector(presult_vect,len_i);

  wn_raw_matrix_multiply_raw_vector((*presult_vect)->numbers,
                                    mat->numbers,vect->numbers,
                                    len_i,len_j); 
}
      


wn_raw_matrix_multiply_raw_vector(result_raw_vect,raw_mat,raw_vect,
                                  len_i,len_j)

double *result_raw_vect,**raw_mat,*raw_vect;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_raw_vector_dot_raw_vector(&(result_raw_vect[i]),
                                 raw_mat[i],raw_vect,len_j);
  }
}



wn_matrix_multiply_matrix(presult_mat,mat1,mat2)

wn_matrix *presult_mat,mat1,mat2;

{
  int len_i,len_j,len_k;

  wn_assertmsg(mat1->len_j == mat2->len_i,"lens wrong.");

  len_i = mat1->len_i;
  len_j = mat1->len_j;
  len_k = mat2->len_j;

  wn_make_matrix(presult_mat,len_i,len_k);

  wn_raw_matrix_multiply_raw_matrix((*presult_mat)->numbers,
                                    mat1->numbers,mat2->numbers,
                                    len_i,len_j,len_k);
}
      


wn_raw_matrix_multiply_raw_matrix(result_raw_mat,raw_mat1,raw_mat2,
                                  len_i,len_j,len_k)

double **result_raw_mat,**raw_mat1,**raw_mat2;
int len_i,len_j,len_k;

{
  double *vect2;
  int i,j,k;

  wn_gpmake("no_free");
  wn_gplabel("matrix multiply group");

  wn_make_raw_vector(&vect2,len_j);

  for(k=0;k<len_k;k++)
  {
    for(j=0;j<len_j;j++)
    {
      vect2[j] = raw_mat2[j][k];
    }

    for(i=0;i<len_i;i++)
    {
      wn_raw_vector_dot_raw_vector(&(result_raw_mat[i][k]),
                                   raw_mat1[i],vect2,len_j);
    }
  }

  wn_gpfree();
}




