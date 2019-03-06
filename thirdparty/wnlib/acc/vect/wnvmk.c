/**********************************************************************

wn_make_vect(&vect,len)
wn_free_vect(vect,len)

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
#include "wnmem.h"
#include "wnvect.h"


void wn_make_vect(double **pvect,int len)
{
  *pvect = (double *)wn_alloc(len*sizeof(double));

  wn_zero_vect(*pvect,len);
}


/*ARGSUSED*/ void wn_free_vect(double *vect,int len)
{
  wn_free((ptr)vect);
}



