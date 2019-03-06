/**********************************************************************

double wn_dist2(v1,v2,len)
double wn_dist(v1,v2,len)

double wn_norm2(vect,len)
double wn_norm(vect,len)

wn_unit_vect(vect,len)

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



double wn_dist2(v1,v2,len)

double v1[],v2[];
int len;

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


double wn_dist(v1,v2,len)

double v1[],v2[];
int len;

{
  extern double sqrt();

  return(sqrt(wn_dist2(v1,v2,len)));
}


double wn_norm2(vect,len)

double vect[];
int len;

{
  extern double wn_dot_vects();

  return(wn_dot_vects(vect,vect,len));
}


double wn_norm(vect,len)

double vect[];
int len;

{
  extern double sqrt();

  return(sqrt(wn_norm2(vect,len)));
}


void wn_unit_vect(vect,len)

double vect[];
int len;

{
  double scale;

  scale = wn_norm(vect,len);
  if(scale == 0.0)
  {
    return;
  }

  wn_mult_vect_by_scalar(vect,1.0/scale,len);
}



