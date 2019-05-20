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
#ifndef wnbllH
#define wnbllH

#include "wnlib.h"
#include "wnsll.h"


typedef struct wn_bll_struct *wn_bll;

struct wn_bll_struct
{
  ptr contents;
  wn_bll next,*plist;
};


EXTERN void wn_mkbll(wn_bll *pll);
EXTERN void wn_freebll_list(wn_bll ll);
EXTERN void wn_freebll(wn_bll ll);

EXTERN void wn_bllins(wn_bll *plist,ptr e);
EXTERN void wn_blleins(wn_bll *plist,wn_bll el);

EXTERN void wn_bllrev(wn_bll *plist);

EXTERN void wn_bllerepatch(wn_bll *pel);
EXTERN void wn_bllrepatch(wn_bll *plist);

EXTERN bool wn_blldel(wn_bll list,ptr e);
EXTERN void wn_blledel(wn_bll el);
EXTERN void wn_blleunlink(wn_bll el);

EXTERN void wn_bllcpy(wn_bll *pout,wn_bll in);

EXTERN void wn_bllins_bll(wn_bll *pout,wn_bll in);

EXTERN void wn_bllcat(wn_bll *pout,wn_bll in);

EXTERN void wn_bllend(wn_bll **ppend,wn_bll *plist);

EXTERN void wn_bllindex
(
  ptr *pcontents,
  wn_bll list,
  int index
);
 
EXTERN void wn_blleindex(wn_bll *pel,wn_bll list,int index);

EXTERN bool wn_bllget
(
  ptr *pe,
  wn_bll list,
  bool (*ppredicate)(ptr entry)
);
EXTERN bool wn_blleget
(
  wn_bll *pel,
  wn_bll list,
  bool (*ppredicate)(ptr entry)
);

EXTERN void wn_bllgetmin
(
  ptr *pe,
  wn_bll list,
  int (*pcompare_func)(ptr e1,ptr e2)
);

EXTERN bool wn_bllempty(wn_bll list);

EXTERN int wn_bllcount(wn_bll list);

EXTERN void wn_bllverify(wn_bll *plist);



#endif

