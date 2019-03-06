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

  This must be as fast as possible.
  That is why the code is so ugly.

***************************************************************************/
void wn_scale_vect
(
  register double *vect,
  register double scalar,
  int len
)
{
small:
  switch(len)
  {
    case(16):   *(vect++) *= scalar;
    case(15):   *(vect++) *= scalar;
    case(14):   *(vect++) *= scalar;
    case(13):   *(vect++) *= scalar;
    case(12):   *(vect++) *= scalar;
    case(11):   *(vect++) *= scalar;
    case(10):   *(vect++) *= scalar;
    case(9):    *(vect++) *= scalar;
    case(8):    *(vect++) *= scalar;
    case(7):    *(vect++) *= scalar;
    case(6):    *(vect++) *= scalar;
    case(5):    *(vect++) *= scalar;
    case(4):    *(vect++) *= scalar;
    case(3):    *(vect++) *= scalar;
    case(2):    *(vect++) *= scalar;
    case(1):    *(vect++) *= scalar;
    case(0):    return;
    default:
    {
      do
      {
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;
        *(vect++) *= scalar;

	len -= 16;
      }
      while(len > 16);

      goto small;
    }
  }
}

void
wn_multiply_vect_by_vect(double *v1, double *v2, int len)
{
  int i;

  for(i = 0; i < len; i++)
    v1[i] *= v2[i];
}

void
wn_divide_vect_by_vect(double *v1, double *v2, int len)
{
  int i;

  for(i = 0; i < len; i++)
    v1[i] /= v2[i];
}
