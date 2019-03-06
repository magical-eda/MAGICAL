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
#include "wnasrt.h"
#include "wnmem.h"
#include "wnmemb.h"

#include "wnaryp.h"



void wn_grow_arrayp
(
  ptr *parray,
  ptr *pnextptr,
  ptr *pfinptr,
  int blocksize
)
{
  ptr new_array;
  int memsize,new_memsize,index;

  memsize = (char *)(*pfinptr) - (char *)(*parray);
  index = (char *)(*pnextptr) - (char *)(*parray);

  if(memsize <= 0)
  {
    wn_assert(memsize == 0);
    new_memsize = blocksize;
    new_array = (ptr)wn_alloc(new_memsize);
  }
  else
  {
    new_memsize = 2*memsize;
    new_array = (ptr)wn_alloc(new_memsize);
    wn_memcpy(new_array,*parray,memsize);
    wn_free(*parray);
  }

  *parray = new_array;
  *pfinptr = (ptr)(((char *)new_array) + new_memsize);
  *pnextptr = (ptr)(((char *)new_array) + index);
}


