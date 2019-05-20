/**********************************************************************

double wn_dist2_vect(v1,v2,len)
double wn_dist_vect(v1,v2,len)

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
#include <math.h>

#include "wnlib.h"

#include "wnvect.h"



double wn_dist2_vect(double v1[],double v2[],int len)
{
  int i;
  double sum,diff;

  sum = 0.0;

  for(i=0;i<len;++i)
  {
    diff = v1[i]-v2[i];
    sum += diff*diff;
  }

  return(sum);
}


double wn_dist_vect(double v1[],double v2[],int len)
{
  return(sqrt(wn_dist2_vect(v1,v2,len)));
}


