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
#include <stdio.h>

#include "wnlib.h"

#include "wnvect.h"



void wn_enter_vect(double vect[],int len)
{
  int i;

  for(i=0;i<len;++i)
  {
    printf("enter element[%d]: ",i);
    scanf("%lf",&(vect[i]));
  }
}


