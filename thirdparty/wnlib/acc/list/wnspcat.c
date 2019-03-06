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

#include "wnsllp.h"



void wn_sllpins_sllp(wn_sllp *pout,wn_sllp in)
{
  wn_sllp *pend;

  wn_sllpend(&pend,&in);
  *pend = *pout;

  *pout = in;
}


void wn_sllpcat(wn_sllp *pout,wn_sllp in)
{
  wn_sllp *pend;

  wn_sllpend(&pend,pout);
  *pend = in;
}


