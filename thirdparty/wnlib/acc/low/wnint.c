/**********************************************************************

FUNCTIONS PROVIDED:

  double wn_floor(double x)
  double wn_ceil(double x)
  double wn_round(double x)

**********************************************************************/
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
#include "wnint.h"


EXTERN double floor(double);

double wn_floor(double x)
{
  return(floor(x));
}


double wn_ceil(double x)
{
  return(-wn_floor(-x));
}


double wn_round(double x)
{
  return(wn_ceil(x-0.5));
}

