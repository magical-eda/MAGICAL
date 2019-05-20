/**************************************************************************

wn_randomize_ptrarray(array,size)

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
#include "wnswap.h"



wn_randomize_ptrarray(array,size)

ptr array[];
int size;

{
  int i,count,select1,select2;

  count = size*20;  /* hit each entry 20 times */

  for(i=0;i<count;++i)
  {
    select1 = wn_random_mod_int(size);
    select2 = wn_random_mod_int(size);

    wn_swap(array[select1],array[select2],ptr);
  }
}


