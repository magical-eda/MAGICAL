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



void wn_stracpy(char *pout[],char in[])
{
  char *out;
  int len;

  len = strlen(in);

  *pout = out = (char *)wn_alloc(len+1);

  wn_memcpy(out,in,len);
  out += len;

  *out = '\0';
}


