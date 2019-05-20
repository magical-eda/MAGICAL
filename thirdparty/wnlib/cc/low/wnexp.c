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



#ifdef apollo    /* apollo lacks these functions */

double log2(x)   /* log base 2 */

double x;

{
  extern double log();
  static double log_of_2;
  static bool initialized = FALSE;

  if(not(initialized))
  {
    log_of_2 = log(2.0);

    initialized = TRUE;
  }

  return(log(x)/log_of_2);
}

#endif



