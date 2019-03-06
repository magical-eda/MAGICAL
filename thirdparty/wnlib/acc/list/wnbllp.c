/**************************************************************************

wn_bllp el,list;

wn_bllprepatch(plist)

wn_bllpeins(&list,el);
wn_bllpeunlink(el)

wn_bllpeindex(&el,list,index)

wn_bllpcat(&out,in)
wn_bllpend(&pend,&list)

bool wn_bllpempty(list)

wn_bllprev(&list)

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
#include "wnasrt.h"
#include "wnmem.h"
#include "wnsllp.h"
#include "wnbllp.h"



void wn_bllpeins(wn_bllp *plist,wn_bllp el)
{
  wn_bllp next;

  next = *plist;

  el->next = next;
  if(next != NULL)
  {
    next->plist = &(el->next);
  }

  *plist = el;
  el->plist = plist;
}


void wn_bllprev(wn_bllp *plist)
{
  wn_sllprev((wn_sllp *)plist);
  wn_bllprepatch(plist);
}


void wn_bllprepatch(register wn_bllp *plist)
{
  register wn_bllp el;

  el = *plist;

  while(el != NULL)
  {
    el->plist = plist;
    plist = &(el->next);
    el = *plist;
  }
}


void wn_bllpeunlink(wn_bllp el)
{
  wn_bllp next,*plist;

  next = el->next;
  plist = el->plist;

  *plist = next;
  if(next != NULL)
  {
    next->plist = plist;
  }

  el->next = (wn_bllp)NULL;
  el->plist = (wn_bllp *)NULL;
}


void wn_bllpins_bllp(wn_bllp *pout,wn_bllp in)
{
  wn_bllp *pend;

  wn_bllpend(&pend,&in);
  *pend = *pout;
  (*pend)->plist = pend;

  *pout = in;
  in->plist = pout;
}


void wn_bllpcat(wn_bllp *pout,wn_bllp in)
{
  wn_bllp *pend;

  if(in == NULL)
  {
    return;
  }

  wn_bllpend(&pend,pout);
  *pend = in;
  in->plist = pend;
}


void wn_bllpend(wn_bllp **ppend,register wn_bllp *plist)
{
  register wn_bllp list;

  list = *plist;

  while(list != NULL)
  {
    plist = &(list->next);
    list = *plist;
  }

  *ppend = plist;
}

 
void wn_bllpeindex(wn_bllp *pel,register wn_bllp list,register int index)
{
  for(;index > 0;index--)
  {
    list = list->next;
  }

  *pel = list;
}


bool wn_bllpempty(wn_bllp list)
{
  return(list == NULL);
}


int wn_bllpcount(wn_bllp list)
{
  register int ret;
  register wn_bllp el;

  ret = 0;

  for(el = list;el != NULL;el = el->next)
  {
    ret++;
  }

  return(ret);
}



