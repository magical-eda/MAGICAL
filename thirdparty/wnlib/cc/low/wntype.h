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
#ifndef wntypeH
#define wntypeH



#ifdef local
#undef local
#endif
#define local static


#ifdef bool
#undef bool
#endif
#define bool int

#ifdef void
#  undef void
#endif
#define void int

#ifdef TRUE
#undef TRUE
#endif
#define TRUE    (1)

#ifdef FALSE
#undef FALSE
#endif
#define FALSE   (0)



#ifdef ptr
#undef ptr
#endif
#ifdef lint
typedef long unsigned int ptr;   /* shut off alignment problem messages */
#else
typedef char *ptr;               /* use address register */
#endif

#ifdef NULL
#undef NULL
#endif
#define NULL 0



#endif


