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
#ifndef wnhtblH
#define wnhtblH


#include "wnlib.h"
#include "wnhtab.h"


EXTERN void wn_mkhashhtab(wn_htab *ptable);
EXTERN void wn_mkstrhtab(wn_htab *ptable);
EXTERN void wn_mkptrhtab(wn_htab *ptable);
EXTERN void wn_mklonghtab(wn_htab *ptable);

/*     wn_mkinthtab() is deprecated for 2 reasons: (1) the code was assuming
** that sizeof(int) == sizeof(ptr).  This could be fixed, but (2) many users
** will get the wrong idea and use this table to store ints as data, and
** specifically call "wn_hget(&i, ..." where i is an int.  If ptrs are
** are bigger than ints, which they are sometimes, memory after i will be
** overwritten, causing insidious, subtle bugs.
**     if you want a table indexed by integer or even short or char keys,
** that's fine, just create it with wn_mklonghtab(), and your keys will be
** extended to longs when pass them by value to the hash subroutines. */

void wn_mkinthtab(wn_htab *ptable) WN_DEPRECATED;

#endif
