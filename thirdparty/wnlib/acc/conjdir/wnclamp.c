/********************************************************************

double wn_clamp(x)
double wn_dclamp(x)

********************************************************************/
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

#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"

#include "wnconj.h"


double wn_clamp(double x)
{
  if(x >= 1.0)
  {
    return(x);
  }
  else if(x <= 0.0)
  {
    return(0.5);
  }
  else  /* 0.0 < x < 1.0 */
  {
    return(0.5*x*x+0.5);
  }
}


double wn_dclamp(double x)
{
  if(x >= 1.0)
  {
    return(1.0);
  }
  else if(x <= 0.0)
  {
    return(0.0);
  }
  else  /* 0.0 < x < 1.0 */
  {
    return(x);
  }
}

