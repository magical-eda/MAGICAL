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

#include "wncmp.h"


int wn_ptrcmp(ptr p1,ptr p2)
{
  if((long unsigned int)p1 > (long unsigned int)p2)
  {
    return(1);
  }
  else if((long unsigned int)p1 == (long unsigned int)p2)
  {
    return(0);
  }
  else /* (long unsigned int)p1 < (long unsigned int)p2 */
  {
    return(-1);
  }
}


