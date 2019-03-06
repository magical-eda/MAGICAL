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
#ifndef wnmapH
#define wnmapH



EXTERN void wn_strcharmap(char *s,char from_c,char to_c);
EXTERN void wn_strmap(char *s,char map_array[256]);
EXTERN void wn_memmap(char *string,int len,char map_array[256]);
EXTERN void wn_load_map_array(char map_array[256],char *from,char *to);



#endif



