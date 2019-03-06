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
#ifndef wnarylH
#define wnarylH

#include "wnlib.h"


typedef struct wn_int_array_type_struct *wn_int_array_type;
struct wn_int_array_type_struct
{
  int usedsize;
  int memsize;
  int *array;
};

void wn_make_int_array(wn_int_array_type *parray);
void wn_initialize_int_array(wn_int_array_type array);


#endif

