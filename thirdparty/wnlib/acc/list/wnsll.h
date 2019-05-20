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
#ifndef wnsllH
#define wnsllH

#include "wnlib.h"


typedef struct wn_sll_struct *wn_sll;

struct wn_sll_struct
{
  ptr contents;
  wn_sll next;
};


EXTERN void wn_mksll(wn_sll *pel);
EXTERN void wn_sllins(wn_sll *plist,ptr e);
EXTERN void wn_slleins(wn_sll *plist,wn_sll el);

EXTERN void wn_freesll(wn_sll el);
EXTERN void wn_freesll_list(wn_sll el);
EXTERN bool wn_slldel(wn_sll *plist,ptr e);
EXTERN void wn_slledel(wn_sll *pel);
EXTERN void wn_slleunlink(wn_sll *pel);

EXTERN void wn_sllrev(wn_sll *plist);

EXTERN void wn_sllcpy(wn_sll *pout,wn_sll in);

EXTERN void wn_sllins_sll(wn_sll *pout,wn_sll in);
EXTERN void wn_sllcat(wn_sll *pout,wn_sll in);

EXTERN void wn_sllend(wn_sll **ppend,wn_sll *plist);
EXTERN void wn_slllast(wn_sll *plast,wn_sll list);

EXTERN void wn_sllindex(ptr *pcontents,wn_sll list,int index);
EXTERN void wn_slleindex(wn_sll *pel,wn_sll list,int index);
EXTERN bool wn_sllget_index(int *pindex,wn_sll list,ptr e);

EXTERN bool wn_sllempty(wn_sll list);
EXTERN int wn_sllcount(wn_sll list);

EXTERN void wn_randomize_sll(wn_sll *plist);

#define WN_FOR_LIST( _vtype, _val, _list) {				\
    wn_sll _wn_for_list_el;						\
    _vtype _val;							\
									\
    wn_fast_assert(sizeof(_val) == sizeof(ptr));			\
									\
    for (_wn_for_list_el = _list;  _wn_for_list_el;  _wn_for_list_el =	\
    /**/					  _wn_for_list_el->next) {\
      _val = (_vtype) _wn_for_list_el->contents;			\
      {

# define WN_END_FOR_LIST() 						\
      }									\
    }									\
									\
    /* just have the compiler check we're matched with a WN_FOR_LIST */	\
    wn_fast_assert(TRUE  ||  (wn_sll) NULL == _wn_for_list_el);		\
  }

#endif

