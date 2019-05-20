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
#ifndef wnparyH
#define wnparyH

#include "wnlib.h"


void wn_mkptrarray(ptr **pptrarray,int memsize);

void wn_freeptrarray(ptr *ptrarray);

void wn_ptrarrayins(ptr **pptrarray,int *pusedsize,int *pmemsize,ptr e);

bool wn_ptrarraydel(ptr **pptrarray,int *pusedsize,int *pmemsize,ptr e);

void wn_ptrarraydelindex(ptr **pptrarray,int *pusedsize,int *pmemsize,
			 int index);

int wn_ptrarrayindex(ptr *ptrarray,int usedsize,ptr e);

void wn_ptrarrayins_into_group
(
  ptr **pptrarray,
  int *pusedsize,
  int *pmemsize,
  ptr e,
  wn_memgp group
);

#endif

