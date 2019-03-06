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
#include "wnrtab.h"
#include "wnasrt.h"

#include "wnhash.h"



#if 0
#define PRIME ((unsigned)(2147483647))   /* largest prime less than 1<<31 */
#define PRIME ((unsigned)(1610614771))   
#define PRIME ((unsigned)(2147483629))  /* 2nd largest prime less than 1<<31 */
#endif

#define PRIME ((unsigned)(1125763841))  /* random prime less than 1<<31 */




void wn_cummodhash(int *paccum,int key,int num_bits)
{
  int i;

  for(i=0;i<num_bits;++i)
  {
    *paccum <<= 1;
    *paccum |= (1&key);
    key >>= 1;

    if(((unsigned int)*paccum) >= (unsigned int)PRIME)
    {
      if(((unsigned int)*paccum) >= (unsigned int)2*PRIME)
      {
        *paccum -= 2*PRIME;
      }
      else
      {
        *paccum -= PRIME;
      }
    }
  }
}

