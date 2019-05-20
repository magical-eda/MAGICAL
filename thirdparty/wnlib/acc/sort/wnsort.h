/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnsortH
#define wnsortH


#include "wnlib.h"
#include "wnsll.h"


EXTERN void wn_sort_sll
(
  wn_sll *plist,
  int (*pcompare_func)(ptr p1,ptr p2)
);

EXTERN void wn_radix_sort_sll
(
  wn_sll *plist,
  char (*pkeyindex_func)(ptr key,int index),
  int (*pkeylen_func)(ptr key)
);

EXTERN void wn_sort_ptrarray(ptr array[],int size,int (*pcompare_func)(ptr p1,ptr p2));

#endif
