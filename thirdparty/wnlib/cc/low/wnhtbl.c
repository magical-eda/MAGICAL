/**************************************************************************

wn_mkstrhtab(&table)
wn_mkptrhtab(&table)
wn_mkinthtab(&table)

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



wn_mkhashhtab(ptable)

wn_htab *ptable;

{
  int pass_int();
  bool wn_inteq();
  extern void wn_intcpy();
  extern void wn_do_nothing();

  wn_mkhtab(ptable,(pass_int),(wn_inteq),(wn_intcpy),(wn_do_nothing));
}



local int pass_int(i)

int i;

{
  return(i);
}



wn_mkstrhtab(ptable)

wn_htab *ptable;

{
  extern int wn_strhash();
  extern bool wn_streq();
  extern wn_stracpy();
  extern wn_free();

  wn_mkhtab(ptable,(wn_strhash),(wn_streq),
            (void (*)())(wn_stracpy),(void (*)())(wn_free));
}



wn_mkptrhtab(ptable)

wn_htab *ptable;

{
  extern int wn_ptrhash();
  bool wn_ptreq();
  extern void wn_ptrcpy();
  extern void wn_do_nothing();

  wn_mkhtab(ptable,(wn_ptrhash),(wn_ptreq),(wn_ptrcpy),(wn_do_nothing));
}



bool wn_ptreq(p1,p2)

ptr p1,p2;

{
  return(p1 == p2);
}



wn_mkinthtab(ptable)

wn_htab *ptable;

{
  extern int wn_inthash();
  bool wn_inteq();
  extern void wn_intcpy();
  extern void wn_do_nothing();

  wn_mkhtab(ptable,(wn_inthash),(wn_inteq),(wn_intcpy),(wn_do_nothing));
}



bool wn_inteq(i1,i2)

int i1,i2;

{
  return(i1 == i2);
}




