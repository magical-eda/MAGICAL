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
#include "wnvect.h"

#include "wnconj.h"


local double s;
local int count;


local double function(double v[])
{
  double ret;

  ret = 1.5*v[0]-2.5*v[1];

  ret += wn_penalty(s*v[0]);
  ret += wn_penalty(s*v[1]);
  ret += wn_penalty(s*(1.0-v[0]));
  ret += wn_penalty(s*(1.0-v[1]));

  printf("  ");
  wn_print_vect(v,2);
  printf("    ob = %lf\n",ret);

  return(ret);
}


local void gradient(double grad[],double v[])
{
  printf("gradient. count=%d\n",count);
  ++count;

  grad[0] = 1.5;
  grad[1] = -2.5;

  grad[0] += s*wn_dpenalty(s*v[0]);
  grad[1] += s*wn_dpenalty(s*v[1]);
  grad[0] += -s*wn_dpenalty(s*(1.0-v[0]));
  grad[1] += -s*wn_dpenalty(s*(1.0-v[1]));
}


void main(void)
{
  double *vect;
  int code;
  double val_min;

  wn_gpmake("no_free"); 

  wn_make_vect(&vect,2);

  count = 0;

  vect[0] = 0.3;
  vect[1] = 0.25;

  s = 100.0;
  /*
  for(s=1.0;;s *= 2)
  {
  */
  printf("s = %lf\n",s);

  /*
  wn_conj_direction_method(&code,&val_min,
		           vect,2,(function),WN_IHUGE);
  */
  wn_conj_gradient_method(&code,&val_min,
		          vect,2,(function),(gradient),WN_IHUGE);

  wn_print_vect(vect,2);
  printf("final result: code = %d   ",code);
  printf("count = %d\n",count);
  printf("    ob = %lf\n",val_min);
  /*
  }
  */

  wn_gpfree();
}
