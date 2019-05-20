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
#ifndef wnsllpH
#define wnsllpH

#include "wnlib.h"


typedef struct wn_sllp_struct *wn_sllp;

struct wn_sllp_struct
{
  wn_sllp next;
};


EXTERN void wn_sllpeins(wn_sllp *plist,wn_sllp el);

EXTERN void wn_sllpeunlink(wn_sllp *pel);

EXTERN void wn_sllprev(wn_sllp *plist);

EXTERN void wn_sllpins_sllp(wn_sllp *pout,wn_sllp in);
EXTERN void wn_sllpcat(wn_sllp *pout,wn_sllp in);

EXTERN void wn_sllpend(wn_sllp **ppend,wn_sllp *plist);
EXTERN void wn_sllplast(wn_sllp *plast,wn_sllp list);

EXTERN void wn_sllpeindex(wn_sllp *pel,wn_sllp list,int index);

EXTERN bool wn_sllpempty(wn_sllp list);
EXTERN int wn_sllpcount(wn_sllp list);

EXTERN void wn_randomize_sllp(wn_sllp *plist);


#endif

