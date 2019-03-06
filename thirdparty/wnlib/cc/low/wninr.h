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
#ifndef wninrH
#define wninrH


#define wn_inrange(_x1,_c2,_x3,_c4,_x5) \
( \
  ((_x1)_c2(_x3)) \
    && \
  ((_x3)_c4(_x5)) \
)


#endif
