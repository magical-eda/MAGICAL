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

#include <math.h>

#include "wnlib.h"

#include "wnvect.h"



/*****************************************************************

  This code is the inner loop of various crucial algorithms, so it must
  be fast.  That is why this code is so ugly.

*****************************************************************/
double wn_norm2_vect
(
  register double *vect,
  int len
)
{
  register double result;
  register double num;

  if(len <= 0)
  {
    return(0.0);
  }

  num = *vect;  result = num*num;

small:
  switch(len)
  {
    case(16):  num = *(++vect);  result += num*num;
    case(15):  num = *(++vect);  result += num*num;
    case(14):  num = *(++vect);  result += num*num;
    case(13):  num = *(++vect);  result += num*num;
    case(12):  num = *(++vect);  result += num*num;
    case(11):  num = *(++vect);  result += num*num;
    case(10):  num = *(++vect);  result += num*num;
    case( 9):  num = *(++vect);  result += num*num;
    case( 8):  num = *(++vect);  result += num*num;
    case( 7):  num = *(++vect);  result += num*num;
    case( 6):  num = *(++vect);  result += num*num;
    case( 5):  num = *(++vect);  result += num*num;
    case( 4):  num = *(++vect);  result += num*num;
    case( 3):  num = *(++vect);  result += num*num;
    case( 2):  num = *(++vect);  result += num*num;
    case( 1):   break;
    case( 0):   result = 0.0;
	       break;

    default:
    {
      do
      {
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;
        num = *(++vect);  result += num*num;

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }

  return(result);
}


double wn_norm_vect(double *vect,int len)
{
  return(sqrt(wn_norm2_vect(vect,len)));
}


void wn_unit_vect(double vect[],int len)
{
  double scale;
  int i;

  scale = wn_norm_vect(vect,len);
  if(scale == 0.0)
  {
    return;
  }

  scale = 1.0/scale;

  for(i=0;i<len;++i)
  {
    vect[i] *= scale;
  }
}


