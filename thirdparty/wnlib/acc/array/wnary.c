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

#include "wnary.h"



void wn_grow_array(ptr *parray,int *pmemsize,int blocksize)
{
  ptr new_array;
  int new_memsize;

  if((*pmemsize) <= 0)
  {
    wn_assert((*pmemsize) == 0);
    *pmemsize = 1;
    *parray = (ptr)wn_alloc(blocksize);
  }
  else
  {
    new_memsize = 2*(*pmemsize);
    new_array = (ptr)wn_alloc(new_memsize*blocksize);
    wn_memcpy(new_array,*parray,(*pmemsize)*blocksize);
    wn_free(*parray);

    *pmemsize = new_memsize;
    *parray = new_array;
  }
}


void wn_grow_2array
(
  ptr *parray1,ptr *parray2,
  int *pmemsize,
  int blocksize1,int blocksize2
)
{
  ptr new_array;
  int new_memsize;

  if((*pmemsize) <= 0)
  {
    wn_assert((*pmemsize) == 0);
    *pmemsize = 1;
    *parray1 = (ptr)wn_alloc(blocksize1);
    *parray2 = (ptr)wn_alloc(blocksize2);
  }
  else
  {
    new_memsize = 2*(*pmemsize);

    new_array = (ptr)wn_alloc(new_memsize*blocksize1);
    wn_memcpy(new_array,*parray1,(*pmemsize)*blocksize1);
    wn_free(*parray1);
    *parray1 = new_array;

    new_array = (ptr)wn_alloc(new_memsize*blocksize2);
    wn_memcpy(new_array,*parray2,(*pmemsize)*blocksize2);
    wn_free(*parray2);
    *parray2 = new_array;

    *pmemsize = new_memsize;
  }
}


