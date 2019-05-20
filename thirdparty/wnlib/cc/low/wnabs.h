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
#ifndef wnabsH
#define wnabsH


#define wn_abs(_x)         (  ((_x)<0) ? (-(_x)) : (_x)  ) 

#define wn_sign(_x)        (  ((_x)>0) ? 1 : ( ((_x)==0) ? 0:(-1) )  ) 


#endif
