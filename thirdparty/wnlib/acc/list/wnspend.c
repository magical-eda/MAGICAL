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

#include "wnsllp.h"



void wn_sllpend(wn_sllp **ppend,register wn_sllp *plist)
{
  register wn_sllp list;

  list = *plist;

  while(list != NULL)
  {
    plist = &(list->next);
    list = *plist;
  }

  *ppend = plist;
}


void wn_sllplast(wn_sllp *plast,register wn_sllp list)
{
  register wn_sllp next;

  next = list->next;

  while(next != NULL)
  {
    list = next;
    next = list->next;
  }

  *plast = list;
}


