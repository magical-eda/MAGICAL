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
#include "wnmem.h"
#include "wnmemb.h"

#include "wncpy.h"



void wn_memacpy(ptr *pout,ptr in,int size)
{
  *pout = wn_alloc(size);
  wn_memcpy(*pout,in,size);
}




