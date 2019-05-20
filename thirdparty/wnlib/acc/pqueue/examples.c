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
#include <stdio.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnasrt.h"
#include "wnnop.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wnpq.h"



int main(void)
{
  wn_pqueue queue;
  wn_pqhandle handle,handle_save;

  wn_gpmake("no_free");

  wn_mkpqueue(&queue);

  wn_pqins(&handle,queue,1.0);
  wn_pqins(&handle,queue,-1.0);
  wn_pqins(&handle,queue,0.0);
  wn_pqins(&handle,queue,2.0);
  wn_pqins(&handle,queue,-2.0);
  handle_save = handle;

  wn_pqmove(handle_save,queue,-15.0);

  for(;;)
  {
    wn_pqgetmin(&handle,queue);
    if(handle == NULL)
    {
      break;
    }
    printf("handle->key=%lg\n",handle->key);
    wn_pqdel(handle,queue);
  }

  return(0);
}
