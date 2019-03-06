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
#ifndef wni2H
#define wni2H



#define wn_shift(i,amount)\
(\
  IF((amount) >= 0)\
  (\
    (i)<<(amount)\
  )\
  ELSE\
  (\
    (i)>>(amount)\
  )\
)


#define wn_iexp2(exp)    (1<<(exp))
    
  

#endif
