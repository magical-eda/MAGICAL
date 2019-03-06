#if 0
*****************************************************************
FUNCTIONS

void wn_mkstrchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
);
void wn_mkptrchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
);
void wn_mkintchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
);
****************************************************************/
#endif /* 0 */
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

#include <stdio.h>
#include <string.h>

#include "wnlib.h"
#include "wnstr.h"
#include "wneq.h"
#include "wncpy.h"
#include "wnnop.h"
#include "wnmem.h"
#include "wnmemw.h"
#include "wnhash.h"
#include "wnchtb.h"
#include "wnchss.h"
#include "wnchtl.h"


local bool lo_fast_streq(char *ps1, char *ps2)
{
  register char c;

  do
  {
    c = *ps1;

    if (!c)
    {
      if (!*ps2)
      {
	return TRUE;
      }
      else
      {
	return FALSE;
      }
    }

    if (c != *ps2)
    {
      return FALSE;
    }

    ++ps1, ++ps2;
  } while (TRUE);

# if defined(linux) || defined(__linux__)
    return FALSE;
# endif
} /* lo_fast_streq */


local void lo_memwrap_stracpy(char *pout[],char in[])
{
  char *out;
  int len;

  len = strlen(in);

  *pout = out = (char *)wn_memwrap_alloc(len+1);

  wn_memcpy(out,in,len);
  out += len;

  *out = '\0';
} /* lo_memwrap_stracpy */


/*   wn_memwrap_free is a #define so we need to make it a routine so we can pass
** a ptr to it. */
local void lo_memwrap_free(ptr p)
{
  wn_memwrap_free(p);
}


void wn_mkstrchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
)
{
  wn_mkchtab(ptable,(int (*)(ptr))(wn_chash_string_hash),
  /**/	    (bool (*)(ptr,ptr))(lo_fast_streq),
  /**/      (void (*)(ptr *,ptr))(lo_memwrap_stracpy),
  /**/	    (void (*)(ptr))(lo_memwrap_free),
  /**/	    start_table_length, fraction_full_before_grow);
} /* wn_mkstrchtab */


void wn_mkptrchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
)
{
  wn_mkchtab(ptable, (wn_ptrhash), (wn_ptreq), (wn_ptrcpy),
  /**/	    (void (*)(ptr))(wn_do_nothing),
  /**/	    start_table_length, fraction_full_before_grow);
} /* wn_mkptrchtab */


void wn_mklongchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
)
{
  wn_mkchtab(ptable,
  /**/	    (int (*)(ptr))(wn_longhash), (bool (*)(ptr,ptr))(wn_longeq),
  /**/	    (void (*)(ptr *,ptr))(wn_longcpy), (void (*)(ptr))(wn_do_nothing),
  /**/	    start_table_length, fraction_full_before_grow);
} /* wn_mkintchtab */


/* this routine is deprecated (see .h file) */
void wn_mkintchtab(
  wn_chtab *ptable,
  int start_table_length,		/* how long should the table start as?
  **					**  Rounded up to 2^1. */
  float fraction_full_before_grow	/* how full should the table get before
  **					** we grow it?  0 < x < 1 */
)
{
  wn_mklongchtab(ptable, start_table_length, fraction_full_before_grow);
} /* wn_mkintchtab */
