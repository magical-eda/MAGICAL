/*************************************************************************

int wn_ilog2(num)
bool wn_is_iexp2(num)

*************************************************************************/
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



int wn_ilog2(num)

unsigned int num;

{
  unsigned int exp,count;

  count = 0;

  for(exp=2;exp > 0;exp=exp<<1)
  {
    if(num < exp)
    {
      return(count);
    }
     
    count++;
  }

  return(count);
}



bool wn_is_iexp2(num)

int num;

{
  switch(num)
  {
    case(0):
      return(FALSE);

    case(1):
    case(1<<1):
    case(1<<2):
    case(1<<3):
    case(1<<4):
    case(1<<5):
    case(1<<6):
    case(1<<7):
    case(1<<8):
    case(1<<9):
    case(1<<10):
    case(1<<11):
    case(1<<12):
    case(1<<13):
    case(1<<14):
    case(1<<15):
    case(1<<16):
    case(1<<17):
    case(1<<18):
    case(1<<19):
    case(1<<20):
    case(1<<21):
    case(1<<22):
    case(1<<23):
    case(1<<24):
    case(1<<25):
    case(1<<26):
    case(1<<27):
    case(1<<28):
    case(1<<29):
    case(1<<30):
      return(TRUE);
  }

  return(FALSE);
}



