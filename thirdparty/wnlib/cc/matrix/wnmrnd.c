/**********************************************************************

wn_random_vect(vect,len_i)

wn_random_mat(mat,len_i,len_j)

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



void wn_random_vect(vect,len_i)

double vect[];
int len_i;

{
  extern double wn_normal_distribution();
  int i;

  for(i=0;i<len_i;++i)
  {
    vect[i] = wn_normal_distribution();
  }
}


void wn_random_mat(mat,len_i,len_j)

double **mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;++i)
  {
    wn_random_vect(mat[i],len_j);
  }
}


