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
#ifndef wnbllpH
#define wnbllpH

#include "wnlib.h"
#include "wnsllp.h"


typedef struct wn_bllp_struct *wn_bllp;

struct wn_bllp_struct
{
  wn_bllp next,*plist;
};


EXTERN void wn_bllpeins(wn_bllp *plist,wn_bllp el);

EXTERN void wn_bllprev(wn_bllp *plist);

EXTERN void wn_bllprepatch(wn_bllp *plist);

EXTERN void wn_bllpeunlink(wn_bllp el);

EXTERN void wn_bllpins_bllp(wn_bllp *pout,wn_bllp in);

EXTERN void wn_bllpcat(wn_bllp *pout,wn_bllp in);

EXTERN void wn_bllpend(wn_bllp **ppend,wn_bllp *plist);
 
EXTERN void wn_bllpeindex(wn_bllp *pel,wn_bllp list,int index);

EXTERN bool wn_bllpempty(wn_bllp list);

EXTERN int wn_bllpcount(wn_bllp list);



#endif

