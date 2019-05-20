/**************************************************************************

int wn_chash_string_hash(char *key)

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

  Bill Chapman

****************************************************************************/

#include <stdio.h>

#include "wnlib.h"
#include "wnrtab.h"
#include "wnchss.h"


/* ---------------------------------------------------------------- */

/* Hopefully fast yet good hash function for chash.  This is basically
** like memhash, only without expensive recursion and call to strlen */

int wn_chash_string_hash(char *key)
{
  register int ret = 0;
  register unsigned char c;
  register int *random_int_table = wn_random_int_table;

  c = *key;
  while (c)
  {
    ret +=  random_int_table[c];

    c = * ++key;
    if (!c) return ret;

    ret +=  random_int_table[c+0x100];

    c = * ++key;
    if (!c) return ret;

    ret +=  random_int_table[c+0x200];

    c = * ++key;
    if (!c) return ret;

    ret +=  random_int_table[c+(0x300)];

    c = * ++key;
    if (!c) return ret;

    ret +=  random_int_table[c+0x400];

    c = * ++key;
    if (!c) return ret;

    ret +=  random_int_table[c+0x500];

    c = * ++key;
    if (!c) return ret;

    ret +=  random_int_table[c+0x600];

    c = * ++key;
    if (!c) return ret;

    ret +=  random_int_table[c+0x700];

    c = * ++key;
  }

  return ret;
} /* wn_chash_string_hash */
