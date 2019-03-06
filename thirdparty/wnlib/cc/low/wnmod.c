/**********************************************************************

FUNCTIONS PROVIDED:

  int wn_imod(x,modulus)
  double wn_fmod(x,modulus)

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

  Will Naylor

****************************************************************************/

#include "wnlib.h"



int wn_imod(x,modulus)

int x,modulus;

{
  extern double wn_round();
  double wn_fmod();

  return((int)wn_round(wn_fmod((double)x,(double)modulus)));
}



double wn_fmod(x,modulus)

double x,modulus;

{
  extern double wn_floor();
  double quotient;

  quotient = x/modulus;

  return(((quotient - wn_floor(quotient))*modulus));
}


