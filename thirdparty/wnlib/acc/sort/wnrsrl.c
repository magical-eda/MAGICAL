/**************************************************************************

char wn_radix_sort_unsigned_index_char(unsigned int u, int index)
char wn_radix_sort_int_index_char(int i, int index)
char wn_radix_sort_string_index_char(char *string, int index)

int wn_radix_sort_int_length(ptr p)		** p is ignored **
int wn_radix_sort_unsigned_length(ptr p)	** p is ignored **
    wn_radix_sort_string_length	defined in .h file

**************************************************************************/
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
#include "wnlib.h"
#include "wnmemb.h"
#include "wnsll.h"

#include "wnsort.h"
#include "wnrsrl.h"



/* The wn_radix_sort_*_index_char routines return a char value.
** Note that the result of this call
** will be masked to the lower 8 bits immediately after these
** results are returned, so we don't need to worry about what
** the upper 24 bits may be doing. */

char wn_radix_sort_unsigned_index_char(unsigned int u, int index)
{
  return (u  >>  (((sizeof(unsigned int)-1)-index) << 3));
}


char wn_radix_sort_int_index_char(int i, int index)
{
  if (0 == index)
  {
    /* the value is a signed char, we must elevate it by 128.  That way
    ** (-1 maps to 127) < (1 maps to 129) and it sorts like ints */

    return wn_radix_sort_unsigned_index_char((unsigned int) i, index) ^
    /**/							(1 << 7);
  }
  else
  {
    return wn_radix_sort_unsigned_index_char((unsigned int) i, index);
  }
} /* wn_radix_sort_int_index_char */



char wn_radix_sort_string_index_char(const char *string, int index)
{
  return string[index];
}


/*ARGUSED*/ int wn_radix_sort_unsigned_length(ptr p)	/* p is ignored */
{
  return sizeof(unsigned int);
}


/*ARGUSED*/ int wn_radix_sort_int_length(ptr p)		/* p is ignored */
{
  return sizeof(int);
}
