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
#ifndef wnconsH
#define wnconsH


#define WN_PI   (3.141592653589793238462643)
#define WN_E    (2.718281828459045235360287)
#define WN_LN2  (0.69314718055994530942)


#ifdef local
#undef local
#endif
#define local static


/*	bool is a predefined type in C++, that behaves differently from an
**  int.  For historical and possibly temporary reasons, we are defining
**  it to be an int in C++ as well as C.  This may change.
**	If you are using the STL and possibly other C++ packages, redefining
**  bool to an int will screw things up.  To avoid this, compile with
**  "-DWN_LEAVE_BOOL_ALONE" set on your command line.  Wnlib has been
**  tested using natural C++ bools, and will work.
**	bool must be defined for C, because wnlib uses the type 'bool' all
**  over the place, and C doesn't define a bool type. */

#if !defined(__cplusplus) || !defined(WN_LEAVE_BOOL_ALONE)
# ifdef   bool
#   undef bool
# endif
# define  bool unsigned int

# ifdef __STL_CONFIG_H
#   error wnlib: Redefining bool, which can interfere badly with STL
#   error     templates.  See low/wncons.h on how to recover.
# endif
#endif

#ifdef TRUE
#undef TRUE
#endif
#define TRUE    (1)

#ifdef FALSE
#undef FALSE
#endif
#define FALSE   (0)


typedef void *ptr;


#ifndef NULL
#  ifdef __cplusplus
#    define NULL (0)
#  else
#    define NULL ((void *)0)
#  endif
#endif

#endif
