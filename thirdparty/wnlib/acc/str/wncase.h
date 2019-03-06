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
#ifndef wncaseH
#define wncaseH


EXTERN void wn_memupcase(char *string,int len);
EXTERN void wn_memlowcase(char *string,int len);

EXTERN void wn_strupcase(char *string);
EXTERN void wn_strlowcase(char *string);

EXTERN void wn_charupcase(char *pc);
EXTERN void wn_charlowcase(char *pc);


#endif

