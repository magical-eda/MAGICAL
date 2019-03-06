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
#ifndef wnmacH
#define wnmacH


#ifndef not
#define not(_x)    (!(_x))
#endif

#define   IF(_arg)       (_arg)?
#define   ELSE           :  

#define wn_sizeof(_s)    ((int)sizeof(_s))
#define wn_structof(_type)   struct _type/**/_struct
  

#endif
