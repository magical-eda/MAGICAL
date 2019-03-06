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

#include <string.h>
#include "wnlib.h"

#include "wnvect.h"



void wn_zero_vect(register double vect[],register int len)
{
  double test_val;

  /* make sure that memset produces floating-point zeros before using it */
  test_val = 12.345;
  memset(&test_val, 0, sizeof(double));
  if (test_val == 0)
  {
    memset(vect, 0, sizeof(double) * len);
  }
  else
  {
    register int i;

    for(i=0;i<len;++i)
    {
      vect[i] = 0.0;
    }
  }
}

