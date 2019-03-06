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
#include "wnpoly.h"



void wn_differentiate_poly(double out[],double in[],int len)
{
  int i;

  for(i=1;i<len;++i)
  {
    out[i-1] = i*in[i];
  }
}


void wn_integrate_poly(double out[],double in[],int len)
{
  int i;

  out[0] = 0.0;  /* integration constant of 0 */

  for(i=0;i<len;++i)
  {
    out[i+1] = in[i]/(i+1);
  }
}


void wn_mult_polys(double out[],double in1[],int len1,double in2[],int len2)
{
  int i1,i2;

  wn_zero_vect(out,len1+len2-1);

  for(i1=0;i1<len1;++i1)
  for(i2=0;i2<len2;++i2)
  {
    out[i1+i2] += in1[i1]*in2[i2];
  }
}


double wn_eval_poly(double x,double coefs[],int len)
{
  double ret;
  int i;

  if(len >= 2)
  {
    ret = coefs[len-2] + x*coefs[len-1];

    for(i=len-3;i>=0;--i)
    {
      ret = coefs[i] + x*ret;
    }
  }
  else
  {
    if(len <= 0)
    {
      ret = 0.0;
    }
    else  /* len == 1 */
    {
      ret = coefs[0];
    }
  }

  return(ret);
}


