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

#include "wnsll.h"



void wn_sllend(wn_sll **ppend,register wn_sll *plist)
{
  register wn_sll list;

  list = *plist;

  while(list != NULL)
  {
    plist = &(list->next);
    list = *plist;
  }

  *ppend = plist;
}


void wn_slllast(wn_sll *plast,register wn_sll list)
{
  register wn_sll next;

  next = list->next;

  while(next != NULL)
  {
    list = next;
    next = list->next;
  }

  *plast = list;
}


