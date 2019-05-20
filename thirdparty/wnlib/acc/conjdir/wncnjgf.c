/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/********************************************************************

********************************************************************/
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnasrt.h"
#include "wnswap.h"
#include "wnabs.h"
#include "wnrndd.h"

#include "wnvect.h"

#include "wnconj.h"


local int save_len;
local double *save_delta_vect;
local double (*psave_function)(double vect[]);


local void gradient(double grad[],double vect[])
{
  int i;
  double val_0,val_m,val_p;
  double delta;

  for(i=0;i<save_len;++i)
  {
    delta = save_delta_vect[i];

    val_0 = vect[i];

    vect[i] = val_0-delta;
    val_m = (*psave_function)(vect);

    vect[i] = val_0+delta;
    val_p = (*psave_function)(vect);

    vect[i] = val_0;

    grad[i] = (val_p-val_m)/(2.0*delta);
  }
}


EXTERN void wn_conj_gradient_diff_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  double delta_vect[],
  int len,
  double (*pfunction)(double vect[]),
  int max_iterations
)
{
  wn_memgp old_group;

  old_group = wn_curgp();
  wn_gpmake("no_free");

  save_len = len;
  save_delta_vect = delta_vect;
  psave_function = pfunction;

  wn_gppush(old_group);

  wn_conj_gradient_method(pcode,pval_min,vect,len,pfunction,&gradient,max_iterations);

  wn_gppop();
  wn_gpfree();
}

