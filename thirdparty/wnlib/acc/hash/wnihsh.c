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
#include "wnrtab.h"

#include "wnhash.h"



/*
int wn_inthash(int i)
{
  return(i);
}
*/

int wn_inthash(int i)
{
  return(wn_memhash((ptr)&i,sizeof(int)));
}


int wn_longhash(long l)
{
  return(wn_memhash((ptr)&l,sizeof(long)));
}

