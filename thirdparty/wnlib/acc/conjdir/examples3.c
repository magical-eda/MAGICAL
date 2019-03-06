/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/


#include "wnlib.h"
#include "wnmem.h"
#include "wnabs.h"
#include "wnrndd.h"
#include "wnvect.h"
#include "wnmat.h"

#include "wnconj.h"


#define  SIZE   50000


local int total_count,count;

local double *p;


local double function(double v[])
{
  double ret,fnminus1;
  int i;

  for(i=0;i<SIZE-1;++i)
  {
    if(v[i] <= 0.0)
    {
      return(WN_FHUGE);
    }
  }

  fnminus1 = 1.0;

  for(i=0;i<SIZE-1;++i)
  {
    fnminus1 -= v[i];
  }

  if(fnminus1 <= 0.0)
  {
    return(WN_FHUGE);
  } 

  ret = p[SIZE-1]/fnminus1;

  for(i=0;i<SIZE-1;++i)
  {
    ret += p[i]/v[i];
  } 

  /*
  wn_print_vect(v,SIZE-1);
  printf("ob = %lf\n",ret);
  */

  return(ret);
}


local void gradient(double grad[],double v[])
{
  double termnminus1,fnminus1;
  int i;

  /*
  printf("gradient, total_count=%d,count=%d.\n",total_count,count);
  */

  ++count;
  ++total_count;

  fnminus1 = 1.0;

  for(i=0;i<SIZE-1;++i)
  {
    fnminus1 -= v[i];
  }

  termnminus1 = p[SIZE-1]/(fnminus1*fnminus1);

  for(i=0;i<SIZE-1;++i)
  {
    grad[i] = -p[i]/(v[i]*v[i]) + termnminus1;
  }
}


void main(void)
{
  int code,i;
  double val_min,val;
  double *vect;

  wn_gpmake("no_free");

  wn_make_vect(&p,SIZE);
  wn_make_vect(&vect,SIZE);

  for(i=0;i<SIZE;++i)
  {
    val = wn_normal_distribution();
    p[i] = wn_abs(val);
    /*
    p[i] = 1.0;
    */
  }

  for(i=0;i<SIZE-1;++i)
  {
    vect[i] = 1.0/SIZE*(1.0+0.01*wn_normal_distribution());
    /*
    vect[i] = 1.0/SIZE;
    */
  }

  wn_conj_gradient_method(&code,&val_min,
		          vect,SIZE-1,(function),(gradient),
			  WN_IHUGE);
  /*
  wn_conj_direction_method(&code,&val_min,
		          vect,SIZE-1,(function),
			  WN_IHUGE);
  */

  printf("final result: code = %d   ",code);
  printf("    ob = %lf\n",val_min);
  printf("total_count=%d,count=%d.\n",total_count,count);
  /*
  wn_print_vect(p,SIZE);
  {
    int i;
    double val;

    val = 1.0;

    for(i=0;i<SIZE-1;++i)
    {
      val -= vect[i];
    }

    vect[SIZE-1] = val;
  }
  wn_print_vect(vect,SIZE);
  {
    int i;

    for(i=0;i<SIZE-1;++i)
    {
      printf("%lf ",p[i]/(vect[i]*vect[i]));
    }
  }
  */

  wn_gpfree();
}
