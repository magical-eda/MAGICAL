/**********************************************************************

**********************************************************************/
/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor -- original  2 June 2004

****************************************************************************/

#include <stdio.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnswap.h"

#include "wnrnd.h"

#include "wnrary.h"


void wn_randomize_ptrarray_order(ptr array[],int size)
{
  int i,index;

  for(i = size-1;i>=1;--i)
  {
    index = wn_random_mod_int(i+1);

    if(index != i)
    {
      wn_swap(array[i],array[index],ptr);
    }
  }
}


