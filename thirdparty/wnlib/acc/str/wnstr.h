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
#ifndef wnstrH
#define wnstrH


#include "wnlib.h"


EXTERN void wn_stracpy(char *pout[],char in[]);
EXTERN void wn_strncpy(char *out,char *in,int n);
EXTERN void wn_stracat(char *pout[],char s1[],char s2[]);
EXTERN void wn_stracat3
(
  char *pout[],
  char s1[],
  char s2[],
  char s3[]
);
EXTERN void wn_stracat4
(
  char *pout[],
  char s1[],
  char s2[],
  char s3[],
  char s4[]
);
EXTERN void wn_stracat5
(
  char *pout[],
  char s1[],
  char s2[],
  char s3[],
  char s4[],
  char s5[]
);
EXTERN void wn_stracat6
(
  char *pout[],
  char s1[],
  char s2[],
  char s3[],
  char s4[],
  char s5[],
  char s6[]
);
EXTERN void wn_strncat(char out[],char in[],int n);
EXTERN bool wn_char_in_string(char c,char *s);


#endif
