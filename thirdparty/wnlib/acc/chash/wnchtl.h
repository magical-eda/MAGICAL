/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Bill Chapman

****************************************************************************/
#ifndef wnchtlH
#define wnchtlH


#include "wnlib.h"
#include "wnhtab.h"


/*   note declaring these EXTERN confuses C++ in both sparc 64 and linux for
** some reason. */

void wn_mkhashchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
);
void wn_mkstrchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
);
void wn_mkptrchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
);

/*     wn_mkintchtab() is deprecated for 2 reasons: (1) the code was assuming
** that sizeof(int) == sizeof(ptr).  This could be fixed, but (2) many users
** will get the wrong idea and use this table to store ints as data, and
** specifically call "wn_chget(&i, ..." where i is an int.  If ptrs are
** are bigger than ints, which they are sometimes, memory after i will be
** overwritten, causing insidious, subtle bugs.
**     if you want a table indexed by integer or even short or char keys,
** that's fine, just create it with wn_mklonghtab(), and your keys will be
** extended to longs when pass them by value to the chash subroutines. */

void wn_mkintchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
) WN_DEPRECATED;
void wn_mklongchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
);

#endif /* wnchtlH */
