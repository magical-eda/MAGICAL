/**************************************************************************

int wn_intcmp(i1,i2)
int wn_ptrcmp(p1,p2)
int wn_boolcmp(b1,b2)
int wn_doublecmp(d1,d2)
int wn_pdoublecmp(pd1,pd2)

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



int wn_intcmp(i1,i2)

int i1,i2;

{
  return(i1-i2);
}



int wn_ptrcmp(p1,p2)

ptr p1,p2;

{
  if((long unsigned int)p1 > (long unsigned int)p2)
  {
    return(1);
  }
  else if((long unsigned int)p1 == (long unsigned int)p2)
  {
    return(0);
  }
  else /* (long unsigned int)p1 < (long unsigned int)p2 */
  {
    return(-1);
  }
}



int wn_boolcmp(b1,b2)

bool b1,b2;

{
  return(wn_intcmp((int)b1,(int)b2));
}



int wn_doublecmp(d1,d2)

double d1,d2;

{
  if(d1 > d2)
  {
    return(1);
  }
  else if(d1 == d2)
  {
    return(0);
  }
  else /* d1 < d2 */
  {
    return(-1);
  }
}



int wn_pdoublecmp(pd1,pd2)

double *pd1,*pd2;

{
  return(wn_doublecmp(*pd1,*pd2));
}



