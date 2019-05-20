/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnmlcH
#define wnmlcH


#include "wnlib.h"


EXTERN ptr wn_f_malloc(unsigned int size);
EXTERN ptr wn_f_calloc(unsigned int nelem,unsigned int elsize);
EXTERN void wn_f_free(ptr p);
EXTERN void wn_f_cfree(ptr p);


#define malloc wn_f_malloc
#define calloc wn_f_calloc
#define free wn_f_free
#define cfree wn_f_free


#endif

