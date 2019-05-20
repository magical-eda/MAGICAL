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
#ifndef wnskllH
#define wnskllH


#include "wnskl.h"

EXTERN void wn_mkstrsklist(wn_sklist *psklist, double threshold);
EXTERN void wn_mkintsklist(wn_sklist *psklist, double threshold);
EXTERN void wn_mkdoublesklist(wn_sklist *psklist, double threshold);

#endif /* wnskplH */
