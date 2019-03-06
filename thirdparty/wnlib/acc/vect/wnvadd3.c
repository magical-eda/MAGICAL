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

  This must be as fast as possible, because it is the inner loop for 
  conjugate gradient.  That is why the code is so ugly.

***************************************************************************/
void wn_add_vect_and_scaled_vect
(
  register double *out,
  register double *v1,
  register double *v2,
  register double s2,
  int len
)
{
small:
  switch(len)
  {
    case(16):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(15):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(14):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(13):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(12):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(11):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(10):   *(out++) = *(v1++) + (*(v2++))*s2;
    case(9):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(8):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(7):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(6):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(5):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(4):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(3):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(2):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(1):    *(out++) = *(v1++) + (*(v2++))*s2;
    case(0):    return;
    default:
    {
      do
      {
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;
        *(out++) = *(v1++) + (*(v2++))*s2;

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }
}


/***************************************************************************

  This must be as fast as possible, because it is the inner loop for 
  conjugate gradient.  That is why the code is so ugly.

***************************************************************************/
void wn_add_scaled_vects
(
  register double *out,
  register double *v1,
  register double s1,
  register double *v2,
  register double s2,
  int len
)
{
small:
  switch(len)
  {
    case(16):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(15):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(14):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(13):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(12):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(11):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(10):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(9):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(8):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(7):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(6):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(5):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(4):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(3):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(2):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(1):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
    case(0):    return;
    default:
    {
      do
      {
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2;

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }
}


/***************************************************************************

  This must be as fast as possible, because it is the inner loop for 
  conjugate gradient.  That is why the code is so ugly.

***************************************************************************/
void wn_add_3_scaled_vects
(
  register double *out,
  register double *v1,
  register double s1,
  register double *v2,
  register double s2,
  register double *v3,
  register double s3,
  int len
)
{
small:
  switch(len)
  {
    case(16):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(15):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(14):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(13):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(12):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(11):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(10):   *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(9):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(8):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(7):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(6):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(5):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(4):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(3):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(2):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(1):    *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
    case(0):    return;
    default:
    {
      do
      {
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;
        *(out++) = (*(v1++))*s1 + (*(v2++))*s2 + (*(v3++))*s3;

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }
}
