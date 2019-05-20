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
#include "wnmem.h"

#include "wnary.h"
#include "wnaryl.h"



void wn_make_int_array(wn_int_array_type *parray)
{
  *parray = (wn_int_array_type)wn_alloc(
               sizeof(struct wn_int_array_type_struct));

  (*parray)->usedsize = 0;
  (*parray)->memsize = 0;
}


void wn_initialize_int_array(wn_int_array_type array)
{
  array->usedsize = 0;
  array->memsize = 0;
}




