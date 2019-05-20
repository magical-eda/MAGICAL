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
#include "wnlib.h"

#include "wnhash.h"



/* 
  This hashing method was invented by Jay Adams in March 1997.

  jay 1-long xor-shift hash 
*/
void wn_cumhasho(int *paccum,int i)
{
  *paccum = i^((*paccum)<<1)^((*paccum)>>1)^((*paccum)&0xd7);
}


void wn_cumhashu(int *paccum,int i)
{
  (*paccum) += i + 0x2fc2ee1e;
}



