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

#include "wncmp.h"



int wn_ptrNULLcmp(bool *psuccess,ptr p1,ptr p2)
{
  if(p1 == NULL)
  {
    *psuccess = TRUE;

    if(p2 == NULL)
    {
      return(0);
    }
    else
    {
      return(-1);
    }
  }
  else /* p1 != NULL */
  {
    if(p2 == NULL)
    {
      *psuccess = TRUE;

      return(1);
    }
    else  /* p2 != NULL */
    {
      *psuccess = (p1 == p2);

      return(0);
    }
  }
}



