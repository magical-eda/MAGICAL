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
#include "wnmem.h"

#include "wnsll.h"



void wn_mksll(wn_sll *pel)
{
  *pel = (wn_sll)wn_alloc(sizeof(struct wn_sll_struct));

  (*pel)->contents = NULL;
  (*pel)->next = (wn_sll)NULL;
}


void wn_slleins(wn_sll *plist,wn_sll el)
{
  el->next = *plist;
  *plist = el;
}


void wn_sllins(wn_sll *plist,ptr e)
{
  wn_sll new_el;

  wn_mksll(&new_el);
  new_el->contents = e;

  wn_slleins(plist,new_el);
}


