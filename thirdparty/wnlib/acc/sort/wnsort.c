/**************************************************************************

wn_sort_sll(&list,pcompare_func)
wn_sll list;
int (*pcompare_func)(p1,p2);   * ptr p1,p2; *

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
#include "wnsll.h"

#include "wnsort.h"



local int (*pcompare_func)(ptr p1,ptr p2);   



local void split_list_into_halves
(
  wn_sll *plist1,
  unsigned int *psize1,
  wn_sll *plist2,
  unsigned int *psize2,
  register wn_sll list,
  unsigned int size
)
{
  register unsigned int stop;
  unsigned int size1;

  *plist1 = list;

  *psize1 = size1 = size >> 1;  /* divide by 2 */
  *psize2 = size - size1;

  for(stop=(size1-1);stop != 0;--stop)  /* do size1-1 times */
  {
    list=list->next;
  }

  *plist2 = list->next;
  list->next = (wn_sll)NULL;
}


local void merge_halves(wn_sll *plist,wn_sll list1,wn_sll list2)
{
  wn_sll *plist_end;

  plist_end = plist;

  for(;;)
  {
    if(list1 == (wn_sll)NULL)
    {
      *plist_end = list2;

      return;
    }
    else if(list2 == (wn_sll)NULL)
    {
      *plist_end = list1;

      return;
    }
    else
    {
      if((*pcompare_func)(list1->contents,list2->contents) <= 0)
      {
        *plist_end = list1;
        list1 = list1->next;
      }
      else
      {
        *plist_end = list2;
        list2 = list2->next;
      }
    }

    plist_end = &((*plist_end)->next);
  }
}


#define merge_sort_sll_big(plist,size)\
{\
  wn_sll list1,list2;\
  unsigned int size1,size2;\
  \
  split_list_into_halves(&list1,&size1,&list2,&size2,*plist,size);\
  \
  merge_sort_sll(&list1,size1);\
  merge_sort_sll(&list2,size2);\
  \
  merge_halves(plist,list1,list2);\
}


#define shuf01     ;        /* 0 assigns */

#define shuf10              /* 3 assigns */\
{\
  *plist = el1; \
  el1->next = el0; \
  el0->next = (wn_sll)NULL; \
}

#define merge_sort_sll_2(plist)\
{\
  wn_sll el0,el1;\
  \
  el0 = *plist;\
  el1 = el0->next;\
\
  if((*pcompare_func)(el0->contents,el1->contents) > 0)\
  {   /* 1 < 0 */\
    shuf10;\
  }\
  else\
  {   /* 0 <= 1 */\
    shuf01;\
  }\
}


#define   shuf012      ;        /* 0 assigns */

#define   shuf021               /* 3 assigns */\
{\
  /* *plist == el0 already */\
  el0->next = el2;\
  el2->next = el1;\
  el1->next = (wn_sll)NULL;\
}

#define   shuf102               /* 3++ assigns */\
{\
  *plist = el1;\
  el1->next = el0;\
  el0->next = el2;\
  /* e2->next == NULL already */\
}

#define   shuf201               /* 3+ assigns */\
{\
  *plist = el2;\
  el2->next = el0;\
  /* el0->next = el1; already */\
  el1->next = (wn_sll)NULL;\
}

#define   shuf120               /* 3+ assigns */\
{\
  *plist = el1;\
  /* el1->next == el2 already */\
  el2->next = el0;\
  el0->next = (wn_sll)NULL;\
}

#define   shuf210               /* 4 assigns */\
{\
  *plist = el2;\
  el2->next = el1;\
  el1->next = el0;\
  el0->next = (wn_sll)NULL;\
}

/*
  average 2.5 compares.  average 2 compares if sorted or reverse sorted.
*/
#define merge_sort_sll_3(plist)\
{\
  wn_sll el0,el1,el2;\
\
  el0 = *plist;\
  el1 = el0->next;\
  el2 = el1->next;\
\
  if((*pcompare_func)(el0->contents,el1->contents) <= 0)\
  {  /* 0 <= 1 */\
    if((*pcompare_func)(el1->contents,el2->contents) <= 0)\
    {  /* 0 <= 1 <= 2 */\
      shuf012;\
    }\
    else\
    {  /* 0 <= 1 && 2 < 1 */\
      if((*pcompare_func)(el0->contents,el2->contents) <= 0)\
      {  /* 0 <= 2 < 1 */\
	shuf021;\
      }\
      else\
      {  /* 2 < 0 <= 1 */\
	shuf201;\
      }\
    }\
  }\
  else\
  {  /* 1 < 0 */ \
    if((*pcompare_func)(el0->contents,el2->contents) <= 0)\
    {  /* 1 < 0 <= 2 */\
      shuf102;\
    }\
    else\
    {  /* 1 < 0 && 2 < 0 */\
      if((*pcompare_func)(el1->contents,el2->contents) <= 0)\
      {  /* 1 <= 2 < 0 */\
	shuf120;\
      }\
      else\
      {  /* 2 < 1 < 0 */\
	shuf210;\
      }\
    }\
  }\
}


local void merge_sort_sll(wn_sll *plist,unsigned int size)
{
  switch(size)
  {
    case(0):  
    case(1):  
    {
      return;
    } /*break;*/
    case(2):  
    {
      merge_sort_sll_2(plist);
    } break;
    case(3):  
    {
      merge_sort_sll_3(plist);
    } break;
    default:
    {
      merge_sort_sll_big(plist,size);
    } break;
  }
}


EXTERN void wn_sort_sll
(
  wn_sll *plist,
  int (*passed_pcompare_func)(ptr p1,ptr p2)
)
{
  unsigned int size;

  pcompare_func = passed_pcompare_func;
  size = wn_sllcount(*plist);

  merge_sort_sll(plist,size);
}



