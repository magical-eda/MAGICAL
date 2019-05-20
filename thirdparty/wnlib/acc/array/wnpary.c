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

#include "wnpary.h"



void wn_mkptrarray(ptr **pptrarray,int memsize)
{
  *pptrarray = (ptr *)wn_zalloc(memsize*sizeof(ptr));
}


void wn_freeptrarray(ptr *ptrarray)
{
  wn_free((ptr)ptrarray);
}


local void grow_ptrarray(ptr **pptrarray,int *pmemsize)
{
  ptr *new_ptrarray;
  int new_memsize;

  if((*pmemsize) <= 0)
  {
    wn_assert((*pmemsize) == 0);
    *pmemsize = 1;
    wn_mkptrarray(pptrarray,*pmemsize);
  }
  else
  {
    new_memsize = 2*(*pmemsize);
    wn_mkptrarray(&new_ptrarray,new_memsize);
    wn_memcpy((ptr)new_ptrarray,(ptr)(*pptrarray),
	      (*pmemsize)*sizeof(ptr));
    wn_freeptrarray(*pptrarray);

    *pmemsize = new_memsize;
    *pptrarray = new_ptrarray;
  }
}


void wn_ptrarrayins(ptr **pptrarray,int *pusedsize,int *pmemsize,ptr e)
{
  wn_assert(*pusedsize <= *pmemsize);

  if(*pusedsize == *pmemsize)
  {
    grow_ptrarray(pptrarray,pmemsize);
  }

  (*pptrarray)[*pusedsize] = e;
  ++(*pusedsize);
}


void wn_ptrarrayins_into_group
(
  ptr **pptrarray,
  int *pusedsize,
  int *pmemsize,
  ptr e,
  wn_memgp group
)
{
  wn_assert(*pusedsize <= *pmemsize);

  if(*pusedsize == *pmemsize)
  {
    wn_gppush(group);
    grow_ptrarray(pptrarray,pmemsize);
    wn_gppop();
  }

  (*pptrarray)[*pusedsize] = e;
  ++(*pusedsize);
}


bool wn_ptrarraydel(ptr **pptrarray,int *pusedsize,int *pmemsize,ptr e)
{
  int i;

  for(i=0;i<(*pusedsize);++i)
  {
    if((*pptrarray)[i] == e)
    {
      --(*pusedsize);
      if(i < (*pusedsize))
      {
	(*pptrarray)[i] = (*pptrarray)[*pusedsize];
      }
      (*pptrarray)[*pusedsize] = NULL;
      return(TRUE);
    }
  }

  return(FALSE);
}


void wn_ptrarraydelindex(ptr **pptrarray,int *pusedsize,int *pmemsize,int index)
{
  wn_assert(index < *pusedsize);
  wn_assert(index >= 0);

  --(*pusedsize);
  if(index < (*pusedsize))
  {
    (*pptrarray)[index] = (*pptrarray)[*pusedsize];
  }
  (*pptrarray)[*pusedsize] = NULL;
}


int wn_ptrarrayindex(ptr *ptrarray,int usedsize,ptr e)
{
  int i;

  for(i=0;i<usedsize;++i)
  {
    if(ptrarray[i] == e)
    {
      return(i);
    }
  }

  return(-1);
}



