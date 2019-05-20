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



void wn_freesll(wn_sll el)
{
  wn_free(el);
}


void wn_freesll_list(wn_sll el)
{
  wn_sll next;

  while(el != NULL)
  {
    next = el->next;

    wn_freesll(el);

    el = next;
  }
}


bool wn_slldel(wn_sll *plist,register ptr e)
{
  register wn_sll *pel,el;

  pel = plist;

  for(;;)
  {
    el = *pel;

    if(el == NULL)
    {
      return(FALSE);
    }
    else if(el->contents == e)
    {
      wn_slledel(pel);

      return(TRUE);
    }

    pel = &(el->next);
  }
}


void wn_slledel(wn_sll *pel)
{
  wn_sll el;

  el = *pel;

  *pel = el->next;

  wn_freesll(el);
}


void wn_slleunlink(wn_sll *pel)
{
  wn_sll el;

  el = *pel;

  *pel = el->next;

  el->next = (wn_sll)NULL;
}

