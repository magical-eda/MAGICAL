/**********************************************************************

int wn_random_mod_int(mod)

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

#include <stdio.h>
#include <string.h>

#include "wnlib.h"
#include "wnrnd.h"



#if 0  /* slow version */
local int get_num_bits(register unsigned int mod)
{
  register int ret;

  for(ret=0;ret<32;++ret)
  {
    if(mod == 0)
    {
      break;
    }

    mod >>= 1;
  }

  return(ret);
}
#endif


local int get_num_bits(register unsigned int x)
{
  register int ret,shift;
  register unsigned int cmp;

  ret = 0;

  shift = 32;
  cmp = (((unsigned int)1) << (shift-1));

  if(x >= cmp)
  {
    return(32);
  }

  for(shift = 16;shift > 0;shift >>= 1)
  {
    cmp = (((unsigned int)1) << (shift-1));

    if(x >= cmp)
    {
      ret += shift;

      x >>= shift;
    }
  }

  return(ret);
}


/* slow version */
unsigned int wn_random_mod_int_old(unsigned int mod)
{
  int num_bits;
  unsigned int ret;

  if(mod < 2)
  {
    return(0);
  }

  num_bits = get_num_bits(mod-1);

  do
  {
    ret = wn_random_n_bits(num_bits);
  }
  while(ret >= mod);

  return(ret);
}

#define B0      0x0u
#define B1      0x1u
#define B2      0x3u
#define B3      0x7u
#define B4      0xfu
#define B5      0x1fu
#define B6      0x3fu
#define B7      0x7fu
#define B8      0xffu
#define B9      0x1ffu
#define B10     0x3ffu
#define B11     0x7ffu
#define B12     0xfffu
#define B13     0x1fffu
#define B14     0x3fffu
#define B15     0x7fffu
#define B16     0xffffu
#define B17     0x1ffffu
#define B18     0x3ffffu
#define B19     0x7ffffu
#define B20     0xfffffu
#define B21     0x1fffffu
#define B22     0x3fffffu
#define B23     0x7fffffu
#define B24     0xffffffu
#define B25     0x1ffffffu
#define B26     0x3ffffffu
#define B27     0x7ffffffu
#define B28     0xfffffffu
#define B29     0x1fffffffu
#define B30     0x3fffffffu
#define B31     0x7fffffffu
#define B32     0xffffffffu

static unsigned lut0[256] = {
  B0, B1, B2, B2, B3, B3, B3, B3, B4, B4, B4, B4, B4, B4, B4, B4, 
  B5, B5, B5, B5, B5, B5, B5, B5, B5, B5, B5, B5, B5, B5, B5, B5, 
  B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, 
  B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, B6, 
  B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, 
  B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, 
  B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, 
  B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, B7, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
  B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, B8, 
};
static unsigned lut1[256] = {
  B8 ,B9 ,B10,B10,B11,B11,B11,B11,B12,B12,B12,B12,B12,B12,B12,B12,
  B13,B13,B13,B13,B13,B13,B13,B13,B13,B13,B13,B13,B13,B13,B13,B13,
  B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,
  B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,B14,
  B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,
  B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,
  B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,
  B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,B15,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
  B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,B16,
};
static unsigned lut2[256] = {
  B16,B17,B18,B18,B19,B19,B19,B19,B20,B20,B20,B20,B20,B20,B20,B20,
  B21,B21,B21,B21,B21,B21,B21,B21,B21,B21,B21,B21,B21,B21,B21,B21,
  B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,
  B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,B22,
  B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,
  B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,
  B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,
  B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,B23,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
  B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,B24,
};
static unsigned lut3[256] = {
  B24,B25,B26,B26,B27,B27,B27,B27,B28,B28,B28,B28,B28,B28,B28,B28,
  B29,B29,B29,B29,B29,B29,B29,B29,B29,B29,B29,B29,B29,B29,B29,B29,
  B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,
  B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,B30,
  B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,
  B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,
  B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,
  B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,B31,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
  B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,B32,
};


unsigned int wn_random_mod_int(unsigned int mod)
{
  unsigned ret, mask;

  --mod;

  /* an alternative (to avoid the lut's) -- a bit slower
  mask = mod | (mod >> 16);
  mask = mask | (mask >> 8);
  mask = mask | (mask >> 4);
  mask = mask | (mask >> 2);
  mask = mask | (mask >> 1);
  */
  if (mod >= 0x10000u)
  {
    if (mod >= 0x1000000u)
    {
      if (mod == 0xffffffffu)
        mask = 0; /* mod was zero! return zero to preserve old behaviour */
      else
        mask = lut3[mod >> 24];
    }
    else
    {
      mask = lut2[mod >> 16];
    }
  }
  else
  {
    if (mod >= 0x100u)
      mask = lut1[mod >> 8];
    else
      mask = lut0[mod];
  }

  do
  {
    ret = wn_random_int() & mask;
  }
  while (ret > mod);

  return ret;
}
