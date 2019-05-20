/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include "wnlib.h"
#include "wnasrt.h"

#include "wntol.h"


local double raw_find_tolerance(void)
{
  double ret;
  double one_plus_smallest_delta,old_one_plus_smallest_delta;

  one_plus_smallest_delta = 2.0;

  for(;;)
  {
    old_one_plus_smallest_delta = one_plus_smallest_delta;

    one_plus_smallest_delta = 0.5*(one_plus_smallest_delta + 1.0);
    if((one_plus_smallest_delta - 1.0) == 0.0)
    {
      break;
    }
  }

  ret = (old_one_plus_smallest_delta - 1.0);

  wn_assert(ret > 0.0);

  return(ret);
}


/* 
  calculate minimum number that can be added to
  1.0 to obtain number larger than 1.0
*/
double wn_machine_tolerance(void)
{
  static bool first_time = TRUE;
  static double tolerance;

  if(first_time)
  {
    first_time = FALSE;
    tolerance = raw_find_tolerance();
  }

  return(tolerance);
}
