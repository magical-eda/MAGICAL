/**************************************************************************

void wn_ptrcpy(pp,p)
void wn_intcpy(pi,i)
void wn_pdoublecpy(pd,d)

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
#include "wnlib.h"




void wn_ptrcpy(pp,p)

ptr *pp,p;

{
  *pp = p;
}



void wn_intcpy(pi,i)

int *pi,i;

{
  *pi = i;
}



void wn_pdoublecpy(pd,d)

double **pd,*d;

{
  *pd = (double *)wn_alloc(wn_sizeof(double));

  **pd = *d;
}




