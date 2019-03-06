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

#include "wnmem.h"
#include "wnmemb.h"
#include "wnmlc.h"



local bool initialized = FALSE;
local wn_memgp wn_f_malloc_group;



local void initialize_wn_f_malloc_group(void)
{
  if(!(initialized))
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


ptr wn_f_malloc(unsigned int size)
{
  ptr ret;

  initialize_wn_f_malloc_group();

  wn_gppush(wn_f_malloc_group);

  ret = wn_alloc(size);
  
  wn_gppop();

  return(ret);
}


ptr wn_f_calloc(unsigned int nelem,unsigned int elsize)
{
  ptr ret;
  unsigned int size;

  size = nelem*elsize;

  ret = wn_f_malloc(size);

  wn_memzero(ret,size);

  return(ret);
}


void wn_f_free(ptr p)
{
  wn_gppush(wn_f_malloc_group);

  wn_free(p);

  wn_gppop();
}


void wn_f_cfree(ptr p)
{
  wn_f_free(p);
}



