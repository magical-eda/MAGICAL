/**************************************************************************

wn_set s;

wn_mkset(&s);
wn_freeset(s);

wn_setins(s,e)
wn_setdel(s,e)

wn_set_to_sll(pl,s)
wn_sll_to_set(ps,l)

wn_setcpy(pout,in)
wn_setcat(out,in)

bool wn_setempty(s)
int wn_setcount(s)

bool wn_setloop(s,&dummy)       * iterator *

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



wn_mkset(ps)

wn_set *ps;

{
  *ps = wn_new(wn_set);

  (*ps)->list = NULL;
  (*ps)->el = NULL;

  (*ps)->group = wn_curgp();
}



wn_freeset(s)

wn_set s;

{
  wn_gppush(s->group);

  wn_freesll_list(s->list);
  wn_free((ptr)s);

  wn_gppop();
}



wn_setins_routine(s,e)

wn_set s;
ptr e;

{
  wn_gppush(s->group);

  wn_sllins(&(s->list),e);

  wn_gppop();
}



bool wn_setdel_routine(s,e)

wn_set s;
ptr e;

{
  bool success;

  wn_gppush(s->group);

  success = wn_slldel(&(s->list),e);

  wn_gppop();

  return(success);
}



wn_set_to_sll(pl,s)

wn_sll *pl;
wn_set s;

{
  *pl = s->list;
}



wn_sll_to_set(ps,l)

wn_set *ps;
wn_sll l;

{
  wn_mkset(ps);

  (*ps)->list = l;
}



wn_setcpy(pout,in)

wn_set *pout,in;

{
  wn_mkset(pout);

  wn_sllcpy(&((*pout)->list),in->list);
}



wn_setcat(out,in)

wn_set out,in;

{
  wn_sllcat(&(in->list),out->list);   /* cat in this order because out->list
                                         usually much longer */
  out->list = in->list;
}



bool wn_setempty(s)

wn_set s;

{
  return(wn_sllempty(s->list));
}



int wn_setcount(s)

wn_set s;

{
  return(wn_sllcount(s->list));
}



bool wn_setloop_routine(s,pdummy)   

register wn_set s;
ptr *pdummy;

{
  extern bool wn_loopinit_just_called;

  if(wn_loopinit_just_called)
  {
    wn_loopinit_just_called = FALSE;

    s->el = s->list;
  }
  else
  {
    s->el = s->el->next;
  }

  if(s->el == NULL)
  {
    *pdummy = NULL;

    return(FALSE);
  }
  else
  {
    *pdummy = s->el->contents;

    return(TRUE);
  }
}




