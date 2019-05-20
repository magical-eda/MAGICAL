/**************************************************************************

wn_mkptrhtab(&table)

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
#include "wnmem.h"
#include "wnstr.h"
#include "wneq.h"
#include "wncpy.h"
#include "wnnop.h"
#include "wnhash.h"
#include "wnhtab.h"

#include "wnhtbl.h"



void wn_mkptrhtab(wn_htab *ptable)
{
  wn_mkhtab(ptable,(wn_ptrhash),(wn_ptreq),(wn_ptrcpy),
	    (void (*)(ptr))(wn_do_nothing));
}




