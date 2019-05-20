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
#include "wnmemb.h"

#include "wnvect.h"



void wn_copy_vect(double to_vect[],double from_vect[],int len)
{
  wn_memcpy((ptr)to_vect,(ptr)from_vect,len*sizeof(double));
}


