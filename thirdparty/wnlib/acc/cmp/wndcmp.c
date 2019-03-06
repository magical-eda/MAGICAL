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
#include "wncmp.h"



int wn_doublecmp(double d1,double d2)
{
  if(d1 > d2)
  {
    return(1);
  }
  else if(d1 < d2)
  {
    return(-1);
  }
  else 
  {
    return(0);
  }
}


