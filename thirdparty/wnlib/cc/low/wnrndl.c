/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/**************************************************************************

wn_randomize_sll(&list)
wn_sll list;

**************************************************************************/
#include "wnlib.h"
#include "wnswap.h"



#define split_list_into_halves(list1,list2,el)\
{\
  wn_sll next;\
  \
  list1 = list2 = NULL;\
  \
  for(;el != NULL;el=next)\
  {\
    next = el->next;\
    \
    if((bool)wn_random_bit())\
    {\
      el->next = list2;\
      list2 = el;\
    }\
    else\
    {\
      el->next = list1;\
      list1 = el;\
    }\
  }\
}



#define split_randomize_sll_big(plist,list)\
{\
  wn_sll list1,list2;\
  \
  split_list_into_halves(list1,list2,list);\
  \
  wn_randomize_sll(&list1);\
  wn_randomize_sll(&list2);\
  \
  *plist = list1;\
  wn_sllcat(plist,list2);\
}



#define split_randomize_sll_2(plist,e0,e1)\
{\
  if((bool)wn_random_bit())\
  { \
    e1->next = e0;\
    e0->next = NULL;\
    *plist = e1;\
  }\
}



wn_randomize_sll(plist)

wn_sll *plist;

{
  wn_sll e0,e1;

  e0 = *plist;

  if(e0 == NULL)  /* length == 0 */
  {
    return;
  }
  else 
  {
    e1 = e0->next;

    if(e1 == NULL)  /* length == 1 */
    {
      return;
    }
    else if(e1->next == NULL)  /* length == 2 */
    {
      split_randomize_sll_2(plist,e0,e1);
    }
    else  /* length > 2 */
    {
      split_randomize_sll_big(plist,e0);
    }
  }
}



