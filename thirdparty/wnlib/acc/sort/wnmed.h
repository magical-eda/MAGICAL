/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnmedH
#define wnmedH


#include "wnlib.h"
#include "wnsll.h"


void wn_split_sll_by_pivot
(
  wn_sll *plt_list,int *plt_count,
  wn_sll *peq_list,int *peq_count,
  wn_sll *pgt_list,int *pgt_count,
  wn_sll list,
  ptr pivot,
  int (*pcompare_func)(ptr p1,ptr p2)
);

void wn_find_nth_sll
(
  ptr *presult,
  wn_sll *plist,
  int n,
  int (*pcompare_func)(ptr p1,ptr p2)
);


#endif
