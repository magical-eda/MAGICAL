/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnabs.h"
#include "wnroot.h"


local int count;


local double func(double x)
{
  double ret;
  bool bracketed;
  double x0,f0,x2,f2;

  ret = x*x - 126.0;

  printf("count = %d, x = %lg, ret = %lg\n",count,x,ret);

  wn_findroot_getrange(&bracketed,&x0,&f0,&x2,&f2);
  printf("  bracketed=%d,x0=%lg,f0=%lg,x2=%lg,f2=%lg\n",
         bracketed,x0,f0,x2,f2);
  /*
  if(wn_abs(x2 - x0) < 1.0)
  */
  if(wn_abs(f2 - f0) < 20.0)
  {
    wn_findroot_stop();
  }

  ++count;

  return(ret);
}


int main(int argc,char *argv[])
{
  int code;
  double x0,x1,x2;
  double f0,f1,f2;

  count = 0;

  x0 = 1.0;
  x2 = 2.0;
  f0 = func(x0);
  f2 = func(x2);

  wn_findroot(&code,&x1,&f1,x0,f0,x2,f2,&func,WN_IHUGE);

  /*
  wn_assert(code == WN_SUCCESS);
  */

  printf("answer:  x1 = %lg, f1 = %lg, code = %d\n",x1,f1,code);

  return(0);
}
