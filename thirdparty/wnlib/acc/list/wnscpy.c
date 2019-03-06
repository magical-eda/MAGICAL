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



void wn_sllcpy(wn_sll *pout,wn_sll in)
{
  wn_sll el;

  *pout = (wn_sll)NULL;

  for(;in != NULL;in=in->next)
  {
    wn_mksll(&el);

    el->contents = in->contents;

    *pout = el;
    pout = &(el->next);
  }
}

