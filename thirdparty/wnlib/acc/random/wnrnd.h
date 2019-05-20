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
#ifndef wnrndH
#define wnrndH

#include "wnlib.h"


EXTERN void wn_seed_random_number_generator(int seed);

EXTERN void wn_rand_print_state(void);     /* debugging use only */

EXTERN int wn_random_ints_buf[];           /* internal use only */
EXTERN int wn_random_ints_count;           /* internal use only */  
EXTERN int wn_random_ints_fill_buf(void);  /* internal use only */

/* main user interface */
#define wn_random_int() \
(\
  wn_random_ints_count ? \
    wn_random_ints_buf[--wn_random_ints_count] \
  : \
    wn_random_ints_fill_buf() \
)

/* subroutine version of the macro wn_random_int() */
EXTERN int wn_random_int_func(void);

EXTERN int wn_random_n_bits(int n);
EXTERN int wn_random_bit(void);
EXTERN unsigned int wn_random_mod_int(unsigned mod);

#endif
