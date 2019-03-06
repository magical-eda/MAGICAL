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
#ifndef wnsetH
#define wnsetH

#include "wncons.h"
#include "wnsll.h"
#include "wnmem.h"



typedef struct wn_set_struct
{
  wn_sll list,el;
  wn_memgp group;
} *wn_set;


#define wn_setins(_s,_e)       wn_setins_routine((_s),(ptr)(_e))
#define wn_setdel(_s,_e)       wn_setdel_routine((_s),(ptr)(_e))

#define wn_setgetany(_pe,_s) \
                  wn_setgetany_routine((ptr *)(_pe),(_s))
#define wn_setget(_pe,_s,_ppredicate) \
                  wn_setget_routine((ptr *)(_pe),(_s),(_ppredicate))
#define wn_setgetmin(_pe,_s,_pcompare_func) \
                  wn_setgetmin_routine((ptr *)(_pe),(_s),(_pcompare_func))

#define wn_setloop(_s,_pdummy) wn_setloop_routine((_s),(ptr *)(_pdummy))


#endif

