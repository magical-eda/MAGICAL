/********************************************************************

double wn_barrier(x)
double wn_dbarrier(x)

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

#include "wnconj.h"


double wn_barrier(double x)
{
  if(x > 0.0)
  {
    return(-log(x));
  }
  else 
  {
    return(WN_FHUGE);
  }
}


double wn_dbarrier(double x)
{
  if(x > 0.0)
  {
    return(-1.0/x);
  }
  else 
  {
    return(-WN_FHUGE);
  }
}

