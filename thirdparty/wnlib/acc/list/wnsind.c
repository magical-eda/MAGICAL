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



void wn_sllindex(ptr *pcontents,wn_sll list,int index)
{
  wn_sll el;

  wn_slleindex(&el,list,index);

  *pcontents = el->contents;
}

 
void wn_slleindex(wn_sll *pel,register wn_sll list,register int index)
{
  for(;index > 0;--index)
  {
    list = list->next;
  }

  *pel = list;
}


bool wn_sllget_index(int *pindex,register wn_sll list,register ptr e)
{
  register int index;

  index = 0;

  for(;list != NULL;list=list->next)
  {
    if(list->contents == e)
    {
      *pindex = index;

      return(TRUE);
    }
    
    ++index;
  }

  *pindex = -1;

  return(FALSE);
}


