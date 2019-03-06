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
#include "wnvect.h"

#include "wnmat.h"



void wn_random_mat(double **mat,int len_i,int len_j)
{
  int i;

  for(i=0;i<len_i;++i)
  {
    wn_random_vect(mat[i],len_j);
  }
}


