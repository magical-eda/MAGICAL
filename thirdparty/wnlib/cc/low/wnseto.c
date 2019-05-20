/**************************************************************************

wn_setdel_set(out,in)
wn_setfilter(pout,in,ppredicate)
bool wn_setget(&e,s,ppredicate)
bool wn_setgetany(&e,s)
wn_setgetmin(&e,s,(pcompare_func))

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



wn_setdel_set(out,in)

wn_set out,in;

{
  extern int wn_ptrcmp();
  register wn_sll *pout_el,in_el;
  register long unsigned int out_contents,in_contents;
  wn_sll in_list;
  wn_memgp temp_group;

  if(
      wn_setempty(out)
        ||
      wn_setempty(in)
    )
  {
    return;
  }

  wn_gpmake("no_free");
  temp_group = wn_curgp();

  wn_sllcpy(&in_list,in->list);

  wn_gppop();

  wn_gppush(out->group);

  wn_sort_sll(&(out->list),(wn_ptrcmp));
  wn_sort_sll(&in_list,(wn_ptrcmp));

  pout_el = &(out->list);
  in_el = in_list;

  while(((*pout_el) != NULL)&&(in_el != NULL))
  {
    out_contents = (long unsigned int)((*pout_el)->contents);
    in_contents = (long unsigned int)(in_el->contents);

    if(out_contents == in_contents)
    {
      wn_slledel(pout_el);
    }
    else if(out_contents < in_contents)
    {
      pout_el = &((*pout_el)->next);
    }
    else /* out_contents > in_contents */
    {
      in_el = in_el->next;
    }
  }

  wn_gppop();

  wn_gppush(temp_group);
  wn_gpfree();
}



wn_setfilter(pout,in,ppredicate)

wn_set *pout,in;
bool (*ppredicate)(/* entry */);

{
  ptr entry;

  wn_mkset(pout);

  for(wn_loopinit();wn_setloop(in,&entry);)
  {
    if((*ppredicate)(entry))
    {
      wn_setins(*pout,entry);
    }
  }
}



bool wn_setget_routine(pe,s,ppredicate)

ptr *pe;
wn_set s;
bool (*ppredicate)(/* entry */);

{
  return(wn_sllget_routine(pe,s->list,ppredicate));
}



bool wn_setgetany_routine(pe,s)

ptr *pe;
wn_set s;

{
  wn_sll list;

  list = s->list;

  if(list != NULL)
  {
    *pe = list->contents;
  
    return(TRUE);
  }
  else
  {
    *pe = NULL;

    return(FALSE);
  }
}



wn_setgetmin_routine(pe,s,pcompare_func)

ptr *pe;
wn_set s;
void (*pcompare_func)(/* e1,e2 */);

{
  wn_sllgetmin(pe,s->list,pcompare_func);
}



