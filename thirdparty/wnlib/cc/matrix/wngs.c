/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/****************************************************************

void wn_gramm_schmidt(pcode,mat,len_i,len_j)

****************************************************************/

#include "wnlib.h"
#include "wnmat.h"


void wn_gramm_schmidt(pcode,mat,len_i,len_j)

int *pcode;
double **mat;        /* mat[i][j] */
int len_i,len_j;

{
  extern double wn_norm2(),wn_dot_vects();
  int i1,i2;
  double mult;
  double *vect_to_remove,*vect;
  double norm2_vect_to_remove;

  for(i1=0;i1<len_i;++i1)
  {
    vect_to_remove = mat[i1];
    norm2_vect_to_remove = wn_norm2(vect_to_remove,len_j);
    if(norm2_vect_to_remove == 0.0)
    {
      *pcode = WN_SINGULAR;
      return;
    }

    for(i2=i1+1;i2<len_i;++i2)
    {
      vect = mat[i2];

      mult = -wn_dot_vects(vect_to_remove,vect,len_j)/norm2_vect_to_remove;

      wn_add_scaled_vect(vect,vect_to_remove,mult,len_j);
    }
  }

  *pcode = WN_SUCCESS;
}


