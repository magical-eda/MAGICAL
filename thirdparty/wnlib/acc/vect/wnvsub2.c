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

#include "wnvect.h"



/***************************************************************************

  This must be as fast as possible, because it is the inner loop for most
  n^3 matrix algebra algorithms.  That is why the code is so ugly.

***************************************************************************/
void wn_subtract_vect
(
  register double *to_vect,
  register double *from_vect,
  int len
)
{
small:
  switch(len)
  {
    case(16):   *(to_vect++) -= (*(from_vect++));
    case(15):   *(to_vect++) -= (*(from_vect++));
    case(14):   *(to_vect++) -= (*(from_vect++));
    case(13):   *(to_vect++) -= (*(from_vect++));
    case(12):   *(to_vect++) -= (*(from_vect++));
    case(11):   *(to_vect++) -= (*(from_vect++));
    case(10):   *(to_vect++) -= (*(from_vect++));
    case(9):    *(to_vect++) -= (*(from_vect++));
    case(8):    *(to_vect++) -= (*(from_vect++));
    case(7):    *(to_vect++) -= (*(from_vect++));
    case(6):    *(to_vect++) -= (*(from_vect++));
    case(5):    *(to_vect++) -= (*(from_vect++));
    case(4):    *(to_vect++) -= (*(from_vect++));
    case(3):    *(to_vect++) -= (*(from_vect++));
    case(2):    *(to_vect++) -= (*(from_vect++));
    case(1):    *(to_vect++) -= (*(from_vect++));
    case(0):    return;
    default:
    {
      do
      {
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));
        *(to_vect++) -= (*(from_vect++));

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }
}

