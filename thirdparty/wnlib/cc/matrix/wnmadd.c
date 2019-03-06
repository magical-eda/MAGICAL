/**********************************************************************

wn_add_vects(result_vect,vect1,vect2,len_i)
wn_add_mats(result_mat,mat1,mat2,len_i,len_j)

wn_add_vect(to_vect,from_vect,len_i)
wn_add_mat(result_mat,from_mat,len_i,len_j)

wn_add_scaled_vect(to_vect,from_vect,scalar,len)
wn_add_vect_and_scaled_vect(out,v1,v2,s2,len)

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



void wn_add_vects(result_vect,vect1,vect2,len_i)

register double result_vect[],vect1[],vect2[];
register int len_i;

{
  register int i;

  for(i=0;i<len_i;i++)
  {
    result_vect[i] = vect1[i] + vect2[i];
  }
}


void wn_add_mats(result_mat,mat1,mat2,len_i,len_j)

double **result_mat,**mat1,**mat2;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;++i)
  {
    wn_add_vects(result_mat[i],mat1[i],mat2[i],len_j);
  }
}


void wn_add_vect(to_vect,from_vect,len_i)

register double to_vect[],from_vect[];
register int len_i;

{
  register int i;

  for(i=0;i<len_i;i++)
  {
    to_vect[i] += from_vect[i];
  }
}


void wn_add_mat(result_mat,from_mat,len_i,len_j)

double **result_mat,**from_mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;++i)
  {
    wn_add_vect(result_mat[i],from_mat[i],len_j);
  }
}


/***************************************************************************

  This must be as fast as possible, because it is the inner loop for most
  n^3 matrix algebra algorithms.  That is why the code is so ugly.

***************************************************************************/
void wn_add_scaled_vect(to_vect,from_vect,scalar,len)

register double *to_vect,*from_vect;
register double scalar;
int len;

{
small:
  switch(len)
  {
    case(16):   *(to_vect++) += (scalar*(*(from_vect++)));
    case(15):   *(to_vect++) += (scalar*(*(from_vect++)));
    case(14):   *(to_vect++) += (scalar*(*(from_vect++)));
    case(13):   *(to_vect++) += (scalar*(*(from_vect++)));
    case(12):   *(to_vect++) += (scalar*(*(from_vect++)));
    case(11):   *(to_vect++) += (scalar*(*(from_vect++)));
    case(10):   *(to_vect++) += (scalar*(*(from_vect++)));
    case(9):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(8):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(7):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(6):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(5):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(4):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(3):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(2):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(1):    *(to_vect++) += (scalar*(*(from_vect++)));
    case(0):    return;
    default:
    {
      wn_assert(len > 16);

      do
      {
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));
        *(to_vect++) += (scalar*(*(from_vect++)));

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }
}


/***************************************************************************

  This must be as fast as possible, because it is the inner loop for 
  conjugate gradient.  That is why the code is so ugly.

***************************************************************************/
void wn_add_vect_and_scaled_vect(out,v1,v2,s2,len)

register double *out,*v1,*v2;
register double s2;
int len;

{
small:
  switch(len)
  {
    case(16):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(15):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(14):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(13):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(12):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(11):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(10):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(9):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(8):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(7):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(6):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(5):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(4):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(3):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(2):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(1):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(0):    return;
    default:
    {
      wn_assert(len > 16);

      do
      {
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }
}



