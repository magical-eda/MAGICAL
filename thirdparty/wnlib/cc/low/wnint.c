/**********************************************************************

FUNCTIONS PROVIDED:

  double wn_ceil(x)
  double wn_floor(x)
  double wn_round(x)

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



double wn_floor(x)

double x;

{
  extern double floor();

  return(floor(x));
}



double wn_ceil(x)

double x;

{
  return(-wn_floor(-x));
}



double wn_round(x)

double x;

{
  return(wn_ceil(x-0.5));
}



