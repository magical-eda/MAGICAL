/**************************************************************************

wn_mkstrbtree(&tree)
wn_mkintbtree(&tree)
wn_mkdoublebtree(&tree)

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
#include "wnbtr.h"



wn_mkstrbtree(ptree)

wn_btree *ptree;

{
  extern int wn_strcmp();
  extern void wn_stracpy();
  extern void wn_free();

  wn_mkbtree(ptree,(wn_strcmp),(wn_stracpy),(wn_free));
}



wn_mkintbtree(ptree)

wn_btree *ptree;

{
  extern int wn_intcmp();
  extern void wn_intcpy();
  extern void wn_do_nothing();

  wn_mkbtree(ptree,(wn_intcmp),(wn_intcpy),(wn_do_nothing));
}



wn_mkdoublebtree(ptree)

wn_btree *ptree;

{
  extern int wn_pdoublecmp();
  extern void wn_pdoublecpy();
  extern void wn_free();

  wn_mkbtree(ptree,(wn_pdoublecmp),(wn_pdoublecpy),(wn_free));
}



