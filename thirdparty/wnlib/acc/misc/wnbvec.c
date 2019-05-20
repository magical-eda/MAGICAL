/**********************************************************************

FUNCTIONS PROVIDED:

void wn_bitvect_make(wn_bitvect *bvect, length_in_bits)
void wn_bitvect_free(wn_bit_vector bv)
void wn_bitvect_set_all_false(wn_bitvect bvect, int length_in_bits);
void wn_bitvect_set_all_true( wn_bitvect bvect, int length_in_bits);
wn_bit_vector wn_bitvect_and(wn_bit_vector result,
**			wn_bit_vector a, wn_bit_vector b, int length_in_bits)
wn_bit_vector wn_bitvect_or( wn_bit_vector result,
**			wn_bit_vector a, wn_bit_vector b, int length_in_bits)
wn_bit_vector wn_bitvect_xor(wn_bit_vector result,
**			wn_bit_vector a, wn_bit_vector b, int length_in_bits)
wn_bit_vector wn_bitvec_not(wn_bit_vector result,
**					wn_bit_vector a, int length_in_bits)
int wn_bitvect_count_set_bits(wn_bit_vector bv, int length_in_bits)

The following are provided in the .h file as macros:

void wn_bitvect_get_bit(bool *bit, wn_bitvect bvect, int index);
void wn_bitvect_set_bit(bool bit, wn_bitvect bvect, int index);

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

  Bill Chapman

****************************************************************************/

#include <stdio.h>
#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnmemb.h"
#include "wnmemw.h"
#include "wnbvec.h"

void wn_bitvect_make(wn_bitvect *bvect, int length_in_bits)
{
  int size;

  wn_assert(sizeof(unsigned) == 4);	/* wnbvec will have to be
  **					** re-thought if this changes */

  /* number of bits: round up to nearest word */
  size = ((length_in_bits + sizeof(unsigned)*8-1) & ~ 0x1f);

  /* number of bytes */
  size /= 8;

  wn_assert(size*8 <  length_in_bits + 32);
  wn_assert(size*8 >= length_in_bits);

  *bvect = (wn_bitvect) wn_memwrap_alloc(size);
  wn_assert(*bvect);
} /* wn_create_bit_vector */


void wn_bitvect_free(wn_bitvect bv)
{
  wn_memwrap_free((ptr) bv);
} /* wn_bitvect_free */


/* set all bits and clear all bits should be rewritten to use wn_memset */

void wn_bitvect_set_all_true(wn_bitvect bv, int length_in_bits)
{
  /* int i; ** unused - bchapman 041111 */

  wn_assert(length_in_bits > 0);

  wn_memset((ptr) bv, (char) 0xff, wn_bitvect_word_count(length_in_bits)*4);
} /* wn_bitvect_set_all_true */


void wn_bitvect_set_all_false(wn_bitvect bv, int length_in_bits)
{
  /* int i; ** unused - bchapman 041111 */

  wn_assert(length_in_bits > 0);

  wn_memset((ptr) bv, 0x0, wn_bitvect_word_count(length_in_bits)*4);
} /* wn_bitvect_set_all_false */


void wn_bitvect_and(wn_bitvect result,
/**/			wn_bitvect a, wn_bitvect b, int length_in_bits)
{
  int i;

  wn_assert(length_in_bits > 0);

  for (i = wn_bitvect_word_count(length_in_bits) - 1;
  /**/					i >= 0  &&  (i & 0xf) != 0xf; --i)
  {
    result[i] = a[i] & b[i];
  }

  for (  ;  i >= 0;  )
  {
    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;
    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;
    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;
    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;

    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;
    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;
    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;
    result[i] = a[i] & b[i];  --i;    result[i] = a[i] & b[i];  --i;
  }

  wn_assert(-1 == i);
} /* wn_bitvect_and */


void wn_bitvect_or( wn_bitvect result,
/**/			wn_bitvect a, wn_bitvect b, int length_in_bits)
{
  int i;

  wn_assert(length_in_bits > 0);

  for (i = wn_bitvect_word_count(length_in_bits) - 1;
  /**/					i >= 0  &&  (i & 0xf) != 0xf; --i)
  {
    result[i] = a[i] | b[i];
  }

  for (  ;  i >= 0;  )
  {
    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;
    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;
    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;
    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;

    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;
    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;
    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;
    result[i] = a[i] | b[i];  --i;    result[i] = a[i] | b[i];  --i;
  }

  wn_assert(-1 == i);
} /* wn_bitvect_or */


void wn_bitvect_xor(wn_bitvect result,
/**/			wn_bitvect a, wn_bitvect b, int length_in_bits)
{
  int i;

  wn_assert(length_in_bits > 0);

  for (i = wn_bitvect_word_count(length_in_bits) - 1;
  /**/					i >= 0  &&  (i & 0xf) != 0xf; --i)
  {
    result[i] = a[i] ^ b[i];
  }

  for (  ;  i >= 0;  )
  {
    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;
    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;
    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;
    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;

    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;
    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;
    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;
    result[i] = a[i] ^ b[i];  --i;    result[i] = a[i] ^ b[i];  --i;
  }

  wn_assert(-1 == i);
} /* wn_bitvect_xor */


void wn_bitvect_not(wn_bitvect result,
/**/					wn_bitvect a, int length_in_bits)
{
  int i;

  wn_assert(length_in_bits > 0);

  for (i = wn_bitvect_word_count(length_in_bits) - 1;
  /**/					i >= 0  &&  (i & 0xf) != 0xf; --i)
  {
    result[i] = ~ a[i];
  }

  for (  ;  i >= 0;  )
  {
    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;
    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;
    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;
    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;

    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;
    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;
    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;
    result[i] = ~ a[i];  --i;    result[i] = ~ a[i];  --i;
  }

  wn_assert(-1 == i);
} /* wn_bitvect_xor */


int wn_bitvect_count_set_bits(wn_bitvect bv, int length_in_bits)
{
  unsigned u, low_bits;
  int ibool;
  int i, count;
  /* int j; ** unused - bchapman 041111 */

  count = 0;
  for (i = length_in_bits-1;  i >= 0  &&  (i & 0x1f) != 0x1f;  --i)
  {
    wn_bitvect_get_bit(&ibool, bv, i);
    count += ibool;
  }

  i >>= 5;
  for (  ;  i >= 0;  --i)
  {
    u = bv[i];

    while (u)
    {
      low_bits = (u - 1) ^ u;
      u &= ~ low_bits;
      ++ count;
    }
  }

  return count;
} /* wn_bitvect_count_set_bits */


bool wn_bitvect_xor_all_bits(wn_bitvect bvect, int length_in_bits)
{
  /* bool ibool; ** unused - bchapman 041111 */
  unsigned u, m;
  int i, j;

  wn_assert(length_in_bits > 0);

  /* make sure dead space high order bits are 0 */
  i = length_in_bits & 0x1f;
  if (i)
  {
    m = (1 << i) - 1;
    bvect[wn_bitvect_word_count(length_in_bits) - 1] &= m;
  }

  u = 0;
  for (j = wn_bitvect_word_count(length_in_bits) - 1;
  /**/					j >= 0  &&  (j & 0xf) != 0xf;  --j)
  {
    u ^= bvect[j];
  }

  for (  ;  j >= 0;  )
  {
    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;
    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;
    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;
    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;

    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;
    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;
    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;
    u ^= bvect[j];    --j;    u ^= bvect[j];    --j;
  }

  wn_assert(-1 == j);

  u ^= u >> 16;
  u ^= u >> 8;
  u ^= u >> 4;
  u ^= u >> 2;
  u ^= u >> 1;

  return (u & 1);
} /* wn_bitvect_xor_all_bits */
