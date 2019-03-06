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
#ifndef wnfftH
#define wnfftH


#include "wncplx.h"


EXTERN void wn_inverse_fft_vect(wn_cplx vector[],int len_i);
EXTERN void wn_fft_vect(wn_cplx vector[],int len_i);


#endif

