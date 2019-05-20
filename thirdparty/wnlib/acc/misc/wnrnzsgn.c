/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include "wnabs.h"
#include "wnrnd.h"

EXTERN int wn_sign_random_nonzero(double x)
{
  if(x == 0.0)
  {
    return(2 * wn_random_bit() - 1);
  }
  else
  {
    return(wn_sign(x));
  }
}
