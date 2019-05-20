/**************************************************************************

wn_bll el,list;

wn_bllrepatch(plist)

wn_mkbll(&ll);
wn_freebll(ll);
wn_freebll_list(ll);

wn_bllins(&list,e);
wn_blleins(&list,el);
bool wn_blldel(list,e);
wn_blledel(el);
wn_blleunlink(el)

bool wn_bllget(&e,list,ppredicate)
bool wn_blleget(&el,list,ppredicate)

wn_bllgetmin(&e,list,pcompare_func)

wn_bllindex(&contents,list,index)
wn_blleindex(&el,list,index)

wn_bllcpy(&out,in)
wn_bllcat(&out,in)
wn_bllend(&pend,&list)

bool wn_bllempty(list)
int wn_setcount(list)

bool wn_blleloop(&el)              * iterator *
bool wn_bllloop(&el,&dummy)        * iterator *

wn_bllrev(&list)

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



wn_mkbll(pll)

wn_bll *pll;

{
  *pll = wn_new(wn_bll);

  (*pll)->contents = NULL;
  (*pll)->next = NULL;
  (*pll)->plist = NULL;
}



wn_freebll_list(ll)

wn_bll ll;

{
  wn_bll next;

  while(ll != NULL)
  {
    next = ll->next;

    wn_freebll(ll);

    ll = next;
  }
}



wn_freebll(ll)

wn_bll ll;

{
  wn_free((ptr)ll);
}



wn_bllins_routine(plist,e)

wn_bll *plist;
ptr e;

{
  wn_bll new_el;

  wn_mkbll(&new_el);
  new_el->contents = e;

  wn_blleins(plist,new_el);
}



wn_blleins(plist,el)

wn_bll *plist,el;

{
  wn_bll next;

  next = *plist;

  el->next = next;
  if(next != NULL)
  {
    next->plist = &(el->next);
  }

  *plist = el;
  el->plist = plist;
}



wn_bllrev(plist)

wn_bll *plist;

{
  wn_sllrev((wn_sll *)plist);
  wn_bllrepatch(plist);
}



wn_bllrepatch(plist)

register wn_bll *plist;

{
  register wn_bll el;

  el = *plist;

  while(el != NULL)
  {
    el->plist = plist;
    plist = &(el->next);
    el = *plist;
  }
}



bool wn_blldel_routine(list,e)

wn_bll list;
ptr e;

{
  register wn_bll el;

  for(el = list;el != NULL;el = el->next)
  {
    if(el->contents == e)
    {
      wn_blledel(el);

      return(TRUE);
    }
  }

  return(FALSE);
}



wn_blledel(el)

wn_bll el;

{
  wn_bll next,*plist;

  next = el->next;
  plist = el->plist;

  *plist = next;
  if(next != NULL)
  {
    next->plist = plist;
  }


  wn_freebll(el);
}



wn_blleunlink(el)

wn_bll el;

{
  wn_bll next,*plist;

  next = el->next;
  plist = el->plist;

  *plist = next;
  if(next != NULL)
  {
    next->plist = plist;
  }

  el->next = NULL;
  el->plist = NULL;
}



wn_bllcpy(pout,in)

wn_bll *pout,in;

{
  wn_bll el;

  *pout = NULL;

  for(;in != NULL;in=in->next)
  {
    wn_mkbll(&el);

    el->contents = in->contents;

    el->plist = pout; 
    *pout = el;
    pout = &(el->next);
  }
}



wn_bllins_bll(pout,in)

wn_bll *pout,in;

{
  wn_bll *pend;

  wn_bllend(&pend,&in);
  *pend = *pout;
  (*pend)->plist = pend;

  *pout = in;
  in->plist = pout;
}



wn_bllcat(pout,in)

wn_bll *pout,in;

{
  wn_bll *pend;

  wn_bllend(&pend,pout);
  *pend = in;
  in->plist = pend;
}



wn_bllend(ppend,plist)

wn_bll **ppend;
register wn_bll *plist;

{
  register wn_bll list;

  list = *plist;

  while(list != NULL)
  {
    plist = &(list->next);
    list = *plist;
  }

  *ppend = plist;
}



wn_bllindex_routine(pcontents,list,index)

ptr *pcontents;
wn_bll list;
int index;

{
  wn_bll el;

  wn_blleindex(&el,list,index);

  *pcontents = el->contents;
}


 
wn_blleindex(pel,list,index)

wn_bll *pel;
register wn_bll list;
register int index;

{
  for(;index > 0;index--)
  {
    list = list->next;
  }

  *pel = list;
}



bool wn_bllget_routine(pe,list,ppredicate)

ptr *pe;
wn_bll list;
bool (*ppredicate)(/* entry */);

{
  wn_bll el;

  if(wn_blleget(&el,list,ppredicate))
  {
    *pe = el->contents;

    return(TRUE);
  }
  else
  {
    *pe = NULL;

    return(FALSE);
  }
}



/*
   wcn -- 11/11/88
   This definition may change.  Don't document yet.
*/
bool wn_blleget(pel,list,ppredicate)

wn_bll *pel,list;
bool (*ppredicate)(/* entry */);

{
  wn_bll el;

  for(el = list;el != NULL;el = el->next)
  {
    if((*ppredicate)(el->contents))
    {
      *pel = el;

      return(TRUE);
    }
  }

  *pel = NULL;

  return(FALSE);
}



wn_bllgetmin_routine(pe,list,pcompare_func)

ptr *pe;
wn_bll list;
void (*pcompare_func)(/* e1,e2 */);

{
  ptr best_e;
  wn_bll el;

  el=list;

  wn_assert(el != NULL);

  best_e = el->contents;

  el=el->next;

  for(;el!=NULL;el=el->next)
  {
    if((*pcompare_func)(best_e,el->contents) > 0)
    {
      best_e = el->contents;
    }
  }

  *pe = best_e;
}



bool wn_bllempty(list)

wn_bll list;

{
  return(list == NULL);
}



int wn_bllcount(list)

wn_bll list;

{
  register int ret;
  register wn_bll el;

  ret = 0;

  for(el = list;el != NULL;el = el->next)
  {
    ret++;
  }

  return(ret);
}



bool wn_blleloop_routine(pel)      /* iterator */

wn_bll *pel;

{
  extern bool wn_loopinit_just_called;

  if(wn_loopinit_just_called)
  {
    wn_loopinit_just_called = FALSE;
  }
  else
  {
    *pel = (*pel)->next;
  }

  return(*pel != NULL);
}



bool wn_bllloop_routine(pel,pdummy)      /* iterator */

wn_bll *pel;
ptr *pdummy;

{
  extern bool wn_loopinit_just_called;

  if(wn_loopinit_just_called)
  {
    wn_loopinit_just_called = FALSE;
  }
  else
  {
    *pel = (*pel)->next;
  }

  if(*pel == NULL)
  {
    *pdummy = NULL;

    return(FALSE);
  }
  else
  {
    *pdummy = (*pel)->contents;

    return(TRUE);
  }
}



