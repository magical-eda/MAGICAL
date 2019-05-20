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



void wn_stracat6
(
  char *pout[],
  char s1[],
  char s2[],
  char s3[],
  char s4[],
  char s5[],
  char s6[]
)
{
  int l1,l2,l3,l4,l5,l6;
  char *out;

  l1 = strlen(s1);
  l2 = strlen(s2);
  l3 = strlen(s3);
  l4 = strlen(s4);
  l5 = strlen(s5);
  l6 = strlen(s6);

  *pout = out = (char *)wn_alloc(l1+l2+l3+l4+l5+l6+1);

  wn_memcpy(out,s1,l1);
  out += l1;
  wn_memcpy(out,s2,l2);
  out += l2;
  wn_memcpy(out,s3,l3);
  out += l3;
  wn_memcpy(out,s4,l4);
  out += l4;
  wn_memcpy(out,s5,l5);
  out += l5;
  wn_memcpy(out,s6,l6);
  out += l6;

  *out = '\0';
}

