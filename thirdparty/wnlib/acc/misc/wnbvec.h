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
#ifndef wnbvecH
#define wnbvecH

#include "wnport.h"


/* This code will NOT WORK (probably segfault) if length_in_bits == 0 */

typedef unsigned *wn_bitvect;

EXTERN void wn_bitvect_make(wn_bitvect *bvect, int length_in_bits);
EXTERN void wn_bitvect_free(wn_bitvect bvect);

#define wn_bitvect_set_one_bit_false(bv, n)	((bv)[(n) >> 5] &= \
/**/							~(1 << ((n) & 0x1f)))

#define wn_bitvect_set_one_bit_true(bv, n)	((bv)[(n) >> 5] |= \
/**/							(1 << ((n) & 0x1f)))

/* EXTERN void wn_bitvect_get_bit(bool *bit, wn_bitvect bvect, int index); */
#define wn_bitvect_get_bit(pret, bv, n)  (*(pret) = (!!((bv)[(n) >> 5] & \
/**/							(1 << ((n) & 0x1f)))))

/* EXTERN void wn_bitvect_set_bit(bool bit, wn_bitvect bvect, int index); */
#define wn_bitvect_set_bit(bit, bvect, index) \
/**/		((bit) ? wn_bitvect_set_one_bit_true( (bvect), (index)) : \
/**/			 wn_bitvect_set_one_bit_false((bvect), (index)))

EXTERN void wn_bitvect_set_all_false(wn_bitvect bvect, int length_in_bits);
EXTERN void wn_bitvect_set_all_true( wn_bitvect bvect, int length_in_bits);

EXTERN void wn_bitvect_and(wn_bitvect result, wn_bitvect a,
/**/			   wn_bitvect b, int length_in_bits);
EXTERN void wn_bitvect_or( wn_bitvect result, wn_bitvect bvect1,
/**/			   wn_bitvect bvect2, int length_in_bits);
EXTERN void wn_bitvect_xor(wn_bitvect result, wn_bitvect bvect1,
/**/			   wn_bitvect bvect2, int length_in_bits);
EXTERN void wn_bitvect_not(wn_bitvect result, wn_bitvect bvect,
/**/					      int length_in_bits);

EXTERN int wn_bitvect_count_set_bits(wn_bitvect bvect, int length_in_bits);
EXTERN bool wn_bitvect_xor_all_bits(wn_bitvect bvect, int length_in_bits);

#define wn_bitvect_word_count(length_in_bits) \
					(((length_in_bits) + 31) >> 5)

#endif /* wnbvecH */
