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
#ifndef wncpyH
#define wncpyH


#include "wnlib.h"


EXTERN void wn_ptrcpy(ptr *pp,ptr p);
EXTERN void wn_intcpy(int *pi,int i);
EXTERN void wn_longcpy(long *pl,long l);
EXTERN void wn_pdoublecpy(double **pd,double *d);
EXTERN void wn_memcpy(ptr out,ptr in,int size);
EXTERN void wn_memacpy(ptr *pout,ptr in,int size);


#endif


