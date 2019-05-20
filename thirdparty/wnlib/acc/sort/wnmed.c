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
#include "wnsll.h"
#include "wnrnd.h"
#include "wnasrt.h"

#include "wnsort.h"
#include "wnmed.h"



local void pick_random_pivot(ptr *ppivot,wn_sll list,int size)
{
  int rnum;

  rnum = wn_random_mod_int(size);

  wn_sllindex(ppivot,list,rnum);
}


void wn_split_sll_by_pivot
(
  wn_sll *plt_list,int *plt_count,
  wn_sll *peq_list,int *peq_count,
  wn_sll *pgt_list,int *pgt_count,
  wn_sll list,
  ptr pivot,
  int (*pcompare_func)(ptr p1,ptr p2)
)
{
  wn_sll el,next_el;
  ptr e;
  int cmp;

  *plt_list = NULL;
  *plt_count = 0;

  *peq_list = NULL;
  *peq_count = 0;

  *pgt_list = NULL;
  *pgt_count = 0;

  next_el = list;

  for(;;)
  {
    el = next_el;
    if(el == NULL)
    {
      break;
    }
    next_el = el->next;
    el->next = NULL;

    e = el->contents;

    cmp = (*pcompare_func)(e,pivot);

    if(cmp < 0)
    {
      wn_slleins(plt_list,el);
      ++(*plt_count);
    }
    else if(cmp > 0)
    {
      wn_slleins(pgt_list,el);
      ++(*pgt_count);
    }
    else
    {
      wn_assert(cmp == 0);

      wn_slleins(peq_list,el);
      ++(*peq_count);
    }
  }

  wn_sllrev(plt_list);
  wn_sllrev(peq_list);
  wn_sllrev(pgt_list);
}


local void find_nth_sll(ptr *presult,wn_sll *plist,
                        int size,int n,
                        int (*pcompare_func)(ptr p1,ptr p2))
{
  ptr pivot;
  wn_sll lt_list,eq_list,gt_list;
  int lt_count,eq_count,gt_count;

  pick_random_pivot(&pivot,*plist,size);

  wn_split_sll_by_pivot(&lt_list,&lt_count,
			&eq_list,&eq_count,
			&gt_list,&gt_count,
			*plist,pivot,
			pcompare_func);

  wn_assert((lt_count+eq_count+gt_count)==size);

  if(n < lt_count)
  {
    find_nth_sll(presult,&lt_list,lt_count,n,
		 pcompare_func);
  }
  else if((lt_count+eq_count) <= n)
  {
    find_nth_sll(presult,&gt_list,gt_count,n-(lt_count+eq_count),
		 pcompare_func);
  }
  else 
  {
    *presult = pivot;
  }

  *plist = lt_list;
  wn_sllcat(plist,eq_list);
  wn_sllcat(plist,gt_list);
}


void wn_find_nth_sll
(
  ptr *presult,
  wn_sll *plist,
  int n,
  int (*pcompare_func)(ptr p1,ptr p2)
)
{
  int size;

  size = (int)wn_sllcount(*plist);

  /* Make sure the list is large enough to contain a n-th element */
  wn_assert(size > n);

  find_nth_sll(presult,plist,size,n,
	       pcompare_func);
}



