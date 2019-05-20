/**************************************************************************

wn_stack s;

wn_mkstack(&s);
wn_push(s,e)
wn_pop(s)
ptr wn_stacktop(s)
bool wn_stackempty(s)

wn_get_stackmark(&mark,s)
wn_stackjmp(s,mark,paction)

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



wn_mkstack(ps)

wn_stack *ps;

{
  *ps = wn_znew(wn_stack);

  (*ps)->group = wn_curgp();
}



wn_push_routine(s,e)

wn_stack s;
ptr e;

{
  wn_sll el;

  wn_gppush(s->group);

  wn_mksll(&el);

  el->contents = e;
  el->next = s->top;
  s->top = el;

  wn_gppop();
}



wn_pop(s)

wn_stack s;

{
  wn_sll top_el;

  wn_gppush(s->group);

  top_el = s->top;

  wn_assertmsg(top_el != NULL,"wn_pop: attempt to pop empty stack.");

  s->top = top_el->next;
  wn_free((ptr)top_el);

  wn_gppop();
}



ptr wn_stacktop(s)

wn_stack s;

{
  wn_sll top_el;

  top_el = s->top;

  wn_assertmsg(top_el != NULL,
               "wn_stacktop: attempt to access top of empty stack.");

  return(top_el->contents);  
}



bool wn_stackempty(s)

wn_stack s;

{
  return(s->top == NULL);
}



wn_get_stackmark(pmark,s)

wn_stackmark *pmark;
wn_stack s;

{
  *pmark = (wn_stackmark)(s->top);
}



wn_stackjmp(s,mark,paction)

wn_stack s;
wn_stackmark mark;
void (*paction)(/* e */);

{
  wn_stackmark current_mark;

  while(TRUE)
  {
    wn_get_stackmark(&current_mark,s);

    if(current_mark == mark)
    {
      break;
    }

    wn_assertmsg(not(wn_stackempty(s)),
                 "wn_stackjmp: attempt to jmp to non-existant mark.");

    if(paction != NULL)
    {
      (*paction)(wn_stacktop(s));
    }
     
    wn_pop(s);
  }
}



