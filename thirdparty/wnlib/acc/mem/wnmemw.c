/**********************************************************************

FUNCTIONS PROVIDED:

  none

GLOBALS PROVIDED:

** determines whether wn_memwrap calls use malloc or wnmem **
bool wn_memwrap_use_malloc;

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

  Bill Chapman

****************************************************************************/

#define wnmemwC

#include <stdio.h>
#include "wnlib.h"
#include "wnmemw.h"

bool wn_memwrap_use_malloc = FALSE;
