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

wn_randomize_sllp(&list)
wn_sllp list;

**************************************************************************/
#include "wnlib.h"
#include "wnswap.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wnsllp.h"



#define split_list_into_halves(list1,list2,el)\
{\
  wn_sllp next;\
  \
  list1 = list2 = (wn_sllp)NULL;\
  \
  for(;el != (wn_sllp)NULL;el=next)\
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


#define split_randomize_sllp_big(plist,list)\
{\
  wn_sllp list1,list2;\
  \
  split_list_into_halves(list1,list2,list);\
  \
  wn_randomize_sllp(&list1);\
  wn_randomize_sllp(&list2);\
  \
  *plist = list1;\
  wn_sllpcat(plist,list2);\
}


#define split_randomize_sllp_2(plist,e0,e1)\
{\
  if((bool)wn_random_bit())\
  { \
    e1->next = e0;\
    e0->next = (wn_sllp)NULL;\
    *plist = e1;\
  }\
}


void wn_randomize_sllp(wn_sllp *plist)
{
  wn_sllp e0,e1;

  e0 = *plist;

  if(e0 == (wn_sllp)NULL)  /* length == 0 */
  {
    return;
  }
  else 
  {
    e1 = e0->next;

    if(e1 == (wn_sllp)NULL)  /* length == 1 */
    {
      return;
    }
    else if(e1->next == (wn_sllp)NULL)  /* length == 2 */
    {
      split_randomize_sllp_2(plist,e0,e1);
    }
    else  /* length > 2 */
    {
      split_randomize_sllp_big(plist,e0);
    }
  }
}



