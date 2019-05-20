/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Bill Chapman

****************************************************************************/
#ifndef wnmemwH
#define wnmemwH

#include <malloc.h>
#include "wnmem.h"

#ifdef __malloc_ptr_t
# define wn__malloc_ptr_t	__malloc_ptr_t
#else
# define wn__malloc_ptr_t	char *
#endif

#ifdef size_t
# define wn_size_t		size_t
#else
# define wn_size_t		unsigned int
#endif

EXTERN bool wn_memwrap_use_malloc;

#define wn_memwrap_alloc(size)	  (wn_memwrap_use_malloc ? \
			(ptr) malloc((wn_size_t) (size)) : wn_alloc(size))
#define wn_memwrap_free(p)	  (wn_memwrap_use_malloc ? \
				free((wn__malloc_ptr_t) (p)) : wn_free(p))
#define wn_memwrap_curgp()	  (wn_memwrap_use_malloc ? \
			(wn_memgp) NULL : wn_curgp())

#endif /* wnmemwH */
