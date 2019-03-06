/**************************************************************************

wn_sll el,list;

wn_mksll(&ll);
wn_freesll(ll);
wn_freesll_list(ll);

wn_sllins(&list,e);
wn_slleins(&list,el);
bool wn_slldel(&list,e);
wn_slledel(&el);

bool wn_sllget(&e,list,ppredicate)
bool wn_slleget(&el,list,ppredicate)

wn_sllgetmin(&e,list,pcompare_func)

wn_sllindex(&contents,list,index)
wn_slleindex(&el,list,index)
bool wn_sllget_index_routine(pindex,list,e)

wn_sllcpy(&out,in)
wn_sllcat(&out,in)
wn_sllend(&pend,&list)
wn_slllast(&last,list)

bool wn_sllempty(list)
int wn_setcount(list)

bool wn_slleloop(&el)              * iterator *
bool wn_sllloop(&el,&dummy)        * iterator *

wn_sllrev(&list)

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



wn_mksll(pll)

wn_sll *pll;

{
  *pll = wn_new(wn_sll);

  (*pll)->contents = NULL;
  (*pll)->next = NULL;
}



wn_freesll_list(ll)

wn_sll ll;

{
  wn_sll next;

  while(ll != NULL)
  {
    next = ll->next;

    wn_freesll(ll);

    ll = next;
  }
}



wn_freesll(ll)

wn_sll ll;

{
  wn_free((ptr)ll);
}



wn_sllins_routine(plist,e)

wn_sll *plist;
ptr e;

{
  wn_sll new_el;

  wn_mksll(&new_el);
  new_el->contents = e;

  wn_slleins(plist,new_el);
}



wn_slleins(plist,el)

wn_sll *plist,el;

{
  el->next = *plist;
  *plist = el;
}



wn_sllrev(plist)

wn_sll *plist;

{
  register wn_sll el,next,new_list;

  new_list = NULL;

  for(el = *plist;el != NULL;el = next)
  {
    next = el->next;
    el->next = new_list;
    new_list = el;
  }

  *plist = new_list;
}



bool wn_slldel_routine(plist,e)

wn_sll *plist;
register ptr e;

{
  register wn_sll *pel,el;

  pel = plist;

  while(TRUE)
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



wn_slledel(pel)

wn_sll *pel;

{
  wn_sll el;

  el = *pel;

  *pel = el->next;

  wn_freesll(el);
}



wn_slleunlink(pel)

wn_sll *pel;

{
  wn_sll el;

  el = *pel;

  *pel = el->next;

  el->next = NULL;
}



wn_sllcpy(pout,in)

wn_sll *pout,in;

{
  wn_sll el;

  *pout = NULL;

  for(;in != NULL;in=in->next)
  {
    wn_mksll(&el);

    el->contents = in->contents;

    *pout = el;
    pout = &(el->next);
  }
}



wn_sllins_sll(pout,in)

wn_sll *pout,in;

{
  wn_sll *pend;

  wn_sllend(&pend,&in);
  *pend = *pout;

  *pout = in;
}



wn_sllcat(pout,in)

wn_sll *pout,in;

{
  wn_sll *pend;

  wn_sllend(&pend,pout);
  *pend = in;
}



wn_sllend(ppend,plist)

wn_sll **ppend;
register wn_sll *plist;

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



wn_slllast(plast,list)

wn_sll *plast;
register wn_sll list;

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



wn_sllindex_routine(pcontents,list,index)

ptr *pcontents;
wn_sll list;
int index;

{
  wn_sll el;

  wn_slleindex(&el,list,index);

  *pcontents = el->contents;
}


 
wn_slleindex(pel,list,index)

wn_sll *pel;
register wn_sll list;
register int index;

{
  for(;index > 0;--index)
  {
    list = list->next;
  }

  *pel = list;
}



bool wn_sllget_index_routine(pindex,list,e)

int *pindex;
register wn_sll list;
register ptr e;

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



bool wn_sllget_routine(pe,list,ppredicate)

ptr *pe;
wn_sll list;
bool (*ppredicate)(/* entry */);

{
  wn_sll el;

  if(wn_slleget(&el,list,ppredicate))
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
bool wn_slleget(pel,list,ppredicate)

wn_sll *pel,list;
bool (*ppredicate)(/* entry */);

{
  wn_sll el;

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



wn_sllgetmin_routine(pe,list,pcompare_func)

ptr *pe;
wn_sll list;
void (*pcompare_func)(/* e1,e2 */);

{
  ptr best_e;
  wn_sll el;

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



bool wn_sllempty(list)

wn_sll list;

{
  return(list == NULL);
}



int wn_sllcount(list)

register wn_sll list;

{
  register int ret;

  ret = 0;

  for(;list != NULL;list = list->next)
  {
    ++ret;
  }

  return(ret);
}



bool wn_slleloop_routine(pel)      /* iterator */

wn_sll *pel;

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



bool wn_sllloop_routine(pel,pdummy)      /* iterator */

wn_sll *pel;
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



/*
   remains for upward compatibility.  Delete eventually.
   DO NOT DOCUMENT!!!
*/
wn_sllins_set(plist,set)

wn_sll *plist;
wn_set set;

{
  ptr p;

  for(wn_loopinit();wn_setloop(set,&p);)
  {
    wn_sllins(plist,p);
  }
}



