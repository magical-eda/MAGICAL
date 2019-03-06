/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnsll.h"
#include "wnasrt.h"
#include "wnhash.h"
#include "wnrndd.h"
#include "wnswap.h"
#include "wnsqr.h"
#include "wnvect.h"

#include "wnconj.h"


local double function(double x)
{
  double ret;

  ret = 1.0/x + x;

  printf("x = %20.20lf,f = %20.20lf\n",x,ret);

  return(ret);
}


/*
local double function(double x)
{
  double ret;

  ret = x-1.6666;

  if(ret > 0)
  {
    ret *= 20.0;
  }
  else
  {
    ret *= -1.0;
  }

  ret += 1.5555;

  printf("x = %20.20lf,f = %20.20lf\n",x,ret);

  return(ret);
}
*/


void main(void)
{
  double f0,f1,f2,x0,x1,x2;
  int code;

  wn_gpmake("no_free");

  x0 = 0.001;
  f0 = function(x0);

  x1 = 1.5;
  f1 = function(x1);

  x2 = 2.0;
  f2 = function(x2);

  wn_minimize_1d_raw(&code,
		     &f0,&f1,&f2,&x0,&x1,&x2,
		     &function);

  printf("code = %d,f0=%lf,f1=%lf,f2=%lf,x0=%lf,x1=%lf,x2=%lf\n",
	 code,f0,f1,f2,x0,x1,x2);
}


