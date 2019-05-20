/**************************************************************************

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
#include <string.h>

#include "wnlib.h"
#include "wnnop.h"
#include "wnmem.h"
#include "wncmp.h"
#include "wncpy.h"
#include "wnstr.h"

#include "wnskl.h"
#include "wnskll.h"


void wn_mkstrsklist(wn_sklist *psklist, double threshold)
{
  wn_mksklist(psklist, threshold,
	     (int (*)(ptr,ptr))(strcmp),
	     (void (*)(ptr *,ptr))(wn_stracpy),
	     (void (*)(ptr))(wn_free));
}


void wn_mkintsklist(wn_sklist *psklist, double threshold)
{
  wn_mksklist(psklist, threshold,
	     (int (*)(ptr,ptr))(wn_longcmp),
	     (void (*)(ptr *,ptr))(wn_longcpy),
	     (void (*)(ptr))(wn_do_nothing));
}


void wn_mkdoublesklist(wn_sklist *psklist, double threshold)
{
  wn_mksklist(psklist, threshold,
	     (int (*)(ptr,ptr))(wn_pdoublecmp),
	     (void (*)(ptr *,ptr))(wn_pdoublecpy),
	     (void (*)(ptr))(wn_free));
}
