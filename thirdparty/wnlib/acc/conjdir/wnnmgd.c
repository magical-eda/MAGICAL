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

void wn_numerical_gradient
(
  double grad[],
  double vect[],
  double delta_vect[],
  int len,
  double (*pfunction)(double vect[])
);

********************************************************************/
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnasrt.h"

#include "wnvect.h"

#include "wnconj.h"


EXTERN void wn_numerical_gradient
(
  double grad[],
  double vect[],
  double delta_vect[],
  int len,
  double (*pfunction)(double vect[])
)
{
  double f,base_f,delta_f;
  double save_vect_el,delta_vect_el;
  int i;

  base_f = (*pfunction)(vect);

  for(i=0;i<len;++i)
  {
    save_vect_el = vect[i];

    delta_vect_el = delta_vect[i];
    wn_assert(delta_vect_el != 0.0);
    vect[i] += delta_vect_el;

    f = (*pfunction)(vect);

    delta_f = f-base_f;

    grad[i] = delta_f/delta_vect_el;

    vect[i] = save_vect_el;
  }
}

