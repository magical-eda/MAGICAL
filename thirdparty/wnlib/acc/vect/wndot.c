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



/*****************************************************************

  This code is the inner loop of various n^3 algorithms, so it must
  be fast.  That is why this code is so ugly.

*****************************************************************/
double wn_dot_vects
(
  register double *vect1,
  register double *vect2,
  int len
)
{
  register double result;

  if(len <= 0)
  {
    return(0.0);
  }

  result = ( (*(vect1))*(*(vect2)) );

small:
  switch(len)
  {
    case(16):  result += ( (*(++vect1))*(*(++vect2)) );
    case(15):  result += ( (*(++vect1))*(*(++vect2)) );
    case(14):  result += ( (*(++vect1))*(*(++vect2)) );
    case(13):  result += ( (*(++vect1))*(*(++vect2)) );
    case(12):  result += ( (*(++vect1))*(*(++vect2)) );
    case(11):  result += ( (*(++vect1))*(*(++vect2)) );
    case(10):  result += ( (*(++vect1))*(*(++vect2)) );
    case(9):   result += ( (*(++vect1))*(*(++vect2)) );
    case(8):   result += ( (*(++vect1))*(*(++vect2)) );
    case(7):   result += ( (*(++vect1))*(*(++vect2)) );
    case(6):   result += ( (*(++vect1))*(*(++vect2)) );
    case(5):   result += ( (*(++vect1))*(*(++vect2)) );
    case(4):   result += ( (*(++vect1))*(*(++vect2)) );
    case(3):   result += ( (*(++vect1))*(*(++vect2)) );
    case(2):   result += ( (*(++vect1))*(*(++vect2)) );
    case(1):   break;
    case(0):   result = 0.0;
	       break;

    default:
    {
      do
      {
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );
        result += ( (*(++vect1))*(*(++vect2)) );

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }

  return(result);
}

