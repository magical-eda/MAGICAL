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
#include "wnasrt.h"
#include "wnmem.h"
#include "wnsll.h"
#include "wnbll.h"



void wn_mkbll(wn_bll *pll)
{
  *pll = (wn_bll)wn_zalloc(sizeof(struct wn_bll_struct));

  (*pll)->contents = NULL;
  (*pll)->next = (wn_bll)NULL;
  (*pll)->plist = (wn_bll *)NULL;
}


void wn_freebll_list(wn_bll ll)
{
  wn_bll next;

  while(ll != NULL)
  {
    next = ll->next;

    wn_freebll(ll);

    ll = next;
  }
}


void wn_freebll(wn_bll ll)
{
  wn_free((ptr)ll);
}


void wn_bllins(wn_bll *plist,ptr e)
{
  wn_bll new_el;

  wn_mkbll(&new_el);
  new_el->contents = e;

  wn_blleins(plist,new_el);
}


void wn_blleins(wn_bll *plist,wn_bll el)
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


void wn_bllrev(wn_bll *plist)
{
  wn_sllrev((wn_sll *)plist);
  wn_bllrepatch(plist);
}


void wn_bllerepatch(wn_bll *pel)
{
  if(*pel != NULL)
  {
    (*pel)->plist = pel;
  }
}


void wn_bllrepatch(register wn_bll *plist)
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


bool wn_blldel(wn_bll list,ptr e)
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


void wn_blledel(wn_bll el)
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


void wn_blleunlink(wn_bll el)
{
  wn_bll next,*plist;

  next = el->next;
  plist = el->plist;

  *plist = next;
  if(next != NULL)
  {
    next->plist = plist;
  }

  el->next = (wn_bll)NULL;
  el->plist = (wn_bll *)NULL;
}


void wn_bllcpy(wn_bll *pout,wn_bll in)
{
  wn_bll el;

  *pout = (wn_bll)NULL;

  for(;in != NULL;in=in->next)
  {
    wn_mkbll(&el);

    el->contents = in->contents;

    el->plist = pout; 
    *pout = el;
    pout = &(el->next);
  }
}


void wn_bllins_bll(wn_bll *pout,wn_bll in)
{
  wn_bll *pend;

  wn_bllend(&pend,&in);
  *pend = *pout;
  (*pend)->plist = pend;

  *pout = in;
  in->plist = pout;
}


void wn_bllcat(wn_bll *pout,wn_bll in)
{
  wn_bll *pend;

  if(in == NULL)
  {
    return;
  }

  wn_bllend(&pend,pout);
  *pend = in;
  in->plist = pend;
}


void wn_bllend(wn_bll **ppend,register wn_bll *plist)
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


void wn_bllindex
(
  ptr *pcontents,
  wn_bll list,
  int index
)
{
  wn_bll el;

  wn_blleindex(&el,list,index);

  *pcontents = el->contents;
}

 
void wn_blleindex(wn_bll *pel,register wn_bll list,register int index)
{
  for(;index > 0;index--)
  {
    list = list->next;
  }

  *pel = list;
}


EXTERN bool wn_bllget
(
  ptr *pe,
  wn_bll list,
  bool (*ppredicate)(ptr entry)
)
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
EXTERN bool wn_blleget
(
  wn_bll *pel,
  wn_bll list,
  bool (*ppredicate)(ptr entry)
)
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

  *pel = (wn_bll)NULL;

  return(FALSE);
}


EXTERN void wn_bllgetmin
(
  ptr *pe,
  wn_bll list,
  int (*pcompare_func)(ptr e1,ptr e2)
)
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


bool wn_bllempty(wn_bll list)
{
  return(list == NULL);
}


int wn_bllcount(wn_bll list)
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


local void wn_blleverify(wn_bll *pel)
{
  wn_assert(pel == (*pel)->plist);
}


void wn_bllverify(wn_bll *plist)
{
  wn_bll *pel;

  for(pel = plist;*pel != NULL;pel = &((*pel)->next))
  {
    wn_blleverify(pel);
  }
}

