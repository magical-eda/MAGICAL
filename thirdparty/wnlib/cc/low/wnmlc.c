/**********************************************************************

FUNCTIONS PROVIDED:

  char *wn_f_malloc(size)
  char *wn_f_calloc(nelem,elsize)

  wn_f_free(p)
  wn_f_cfree(p)

**********************************************************************/
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



local bool initialized = FALSE;
local wn_memgp wn_f_malloc_group;



local initialize_wn_f_malloc_group()
{
  if(not(initialized))
  {
    wn_gppush(wn_defaultgp());
    wn_gpmake("general_free");
    wn_gplabel("wn_f_malloc_group");
    wn_f_malloc_group = wn_curgp();
    wn_gppop();
    wn_gppop();

    initialized = TRUE;
  }
}



char *wn_f_malloc(size)

unsigned int size;

{
  char *ret;

  initialize_wn_f_malloc_group();

  wn_gppush(wn_f_malloc_group);

  ret = (char *)wn_alloc((int)size);
  
  wn_gppop();

  return(ret);
}



char *wn_f_calloc(nelem,elsize)

unsigned int nelem,elsize;

{
  char *ret;
  unsigned int size;

  size = nelem*elsize;

  ret = wn_f_malloc(size);

  wn_blkzero((ptr)ret,(int)size);

  return(ret);
}



wn_f_free(p)

char *p;

{
  wn_gppush(wn_f_malloc_group);

  wn_free((ptr)p);

  wn_gppop();
}



wn_f_cfree(p)

char *p;

{
  wn_f_free(p);
}




