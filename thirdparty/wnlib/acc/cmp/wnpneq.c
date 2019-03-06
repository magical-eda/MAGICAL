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
#include "wneq.h"
#include "wncmp.h"



bool wn_ptrNULLeq(bool *psuccess,ptr p1,ptr p2)
{
  return(wn_ptrNULLcmp(psuccess,p1,p2) == 0);
}



