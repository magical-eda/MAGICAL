/**************************************************************************

int wn_ptrNULLcmp(&success,p1,p2)
bool wn_ptrNULLeq(&success,p1,p2)

**************************************************************************/
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



int wn_ptrNULLcmp(psuccess,p1,p2)

bool *psuccess;
ptr p1,p2;

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
    else
    {
      *psuccess = FALSE;

      return(0);
    }
  }
}



bool wn_ptrNULLeq(psuccess,p1,p2)

bool *psuccess;
ptr p1,p2;

{
  if(p1 == NULL)
  {
    *psuccess = TRUE;

    if(p2 == NULL)
    {
      return(TRUE);
    }
    else
    {
      return(FALSE);
    }
  }
  else /* p1 != NULL */
  {
    if(p2 == NULL)
    {
      *psuccess = TRUE;

      return(FALSE);
    }
    else
    {
      *psuccess = FALSE;

      return(TRUE);
    }
  }
}



