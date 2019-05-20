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
#include <string.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnmemb.h"

#include "wnstr.h"



void wn_stracat(char *pout[],char s1[],char s2[])
{
  int l1,l2;
  char *out;

  l1 = strlen(s1);
  l2 = strlen(s2);

  *pout = out = (char *)wn_alloc(l1+l2+1);

  wn_memcpy(out,s1,l1);
  out += l1;
  wn_memcpy(out,s2,l2);
  out += l2;

  *out = '\0';
}

