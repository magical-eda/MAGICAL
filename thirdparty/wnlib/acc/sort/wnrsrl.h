#ifndef wnrsrlH
#define wnrsrlH

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

/* The wn_radix_sort_*_index_char routines return a char value.
** Note that the result of this call
** will be masked to the lower 8 bits immediately after these
** results are returned, so we don't need to worry about what
** the upper 24 bits may be doing. */

#if !defined(sparc64)
  EXTERN char wn_radix_sort_unsigned_index_char(unsigned int u, int index);
  EXTERN char wn_radix_sort_int_index_char(int i, int index);
  EXTERN char wn_radix_sort_string_index_char(const char *string, int index);

  EXTERN int wn_radix_sort_unsigned_length(ptr p);	/* p is ignored */
  EXTERN int wn_radix_sort_int_length(ptr p);		/* p is ignored */
  #define    wn_radix_sort_string_length    ((int (*)(ptr)) strlen)
#else
  /* gets around a weird bug in 64 bit sparc */
	 char wn_radix_sort_unsigned_index_char(unsigned int u, int index);
	 char wn_radix_sort_int_index_char(int i, int index);
	 char wn_radix_sort_string_index_char(const char *string, int index);

	 int wn_radix_sort_unsigned_length(ptr p);	/* p is ignored */
	 int wn_radix_sort_int_length(ptr p);		/* p is ignored */
  #define    wn_radix_sort_string_length    ((int (*)(ptr)) strlen)
#endif

#endif /* wnrsrlH */
