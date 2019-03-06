/**************************************************************************

wn_binary_search_int(pcode,presult,low,high,pfind_direction)

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
#include "wnbsr.h"



wn_binary_search_int(pcode,presult,low,high,pfind_direction)

int *pcode;
int *presult,low,high;
int (*pfind_direction)(/*num*/);

{
  int average_ints();
  int direction,middle;

  wn_assertmsg(low <= high,"low > high");

  direction = (*pfind_direction)(low);
  if(direction < 0)
  {
    *presult = low;
    *pcode = WN_OUT_OF_RANGE;

    return;
  }
  else if(direction == 0)
  {
    *presult = low;
    *pcode = WN_EXACT_MATCH;

    return;
  }

  direction = (*pfind_direction)(high);
  if(direction > 0)
  {
    *presult = high;
    *pcode = WN_OUT_OF_RANGE;

    return;
  }
  else if(direction == 0)
  {
    *presult = high;
    *pcode = WN_EXACT_MATCH;

    return;
  }

  while(TRUE)
  {
    if(low+1 >= high)
    {
      *presult = low;
      *pcode = WN_NO_EXACT_MATCH;

      return;
    }

    middle = average_ints(low,high);   /* always round down */

    direction = (*pfind_direction)(middle);

    if(direction > 0)
    {
      low = middle;
    }
    else if(direction < 0)
    {
      high = middle;
    }
    else /* direction == 0 */
    {
      *presult = middle;
      *pcode = WN_EXACT_MATCH;

      return;
    }
  }
}



local int average_ints(i1,i2)  /* always round down */

int i1,i2;

{
  int sum;

  sum = i1+i2;

  if((sum%2) != 0)   /* sum is odd */
  {
    sum -= 1;        /* round down to make even */
  }

  return(sum/2);
}
    


