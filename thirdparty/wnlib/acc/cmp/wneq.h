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
#ifndef wneqH
#define wneqH



#include "wnlib.h"



EXTERN bool wn_streq(char s1[],char s2[]);
EXTERN bool wn_streqnc(char s1[],char s2[]);
EXTERN bool wn_inteq(int i1,int i2);
EXTERN bool wn_longeq(long l1,long l2);
EXTERN bool wn_ptreq(ptr p1,ptr p2);
EXTERN bool wn_ptrNULLeq(bool *psuccess,ptr p1,ptr p2);
EXTERN bool wn_memeq(ptr m1,ptr m2,int len);



#endif
