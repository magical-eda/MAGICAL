/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnhashH
#define wnhashH


EXTERN int wn_strhash(char *key);
EXTERN int wn_inthash(int i);
EXTERN int wn_longhash(long i);
EXTERN int wn_ptrhash(ptr p);
EXTERN int wn_memhash(ptr key,int len);

EXTERN void wn_cumhasho(int *paccum,int i);
EXTERN void wn_cumhashu(int *paccum,int i);

EXTERN void wn_cummodhash(int *paccum,int key,int num_bits);


#endif


