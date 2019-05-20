/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Bill Chapman, Will Naylor

****************************************************************************/
#include "wncmp.h"


/* This fails for abs(i1 - i2) >= 2^31

{
  return(i1-i2);
}
*/


int wn_intcmp(int i1,int i2)
{
  if (i1 > i2)
  {
    return 1;
  }
  else if (i1 == i2)
  {
    return 0;
  }
  else
  {
    /* i1 < i2 */
    return -1;
  }
}


int wn_longcmp(long i1,long i2)
{
  if (i1 > i2)
  {
    return 1;
  }
  else if (i1 == i2)
  {
    return 0;
  }
  else
  {
    /* i1 < i2 */
    return -1;
  }
}
