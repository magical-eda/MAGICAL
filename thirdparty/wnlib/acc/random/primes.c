/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHORS:

  Will Naylor

*****************************************************************************

Generate primes for use in random number code.

****************************************************************************/
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnsll.h"



local bool num_is_prime(int num,wn_sll prime_list)
{
  int prime;
  wn_sll el;

  for(el=prime_list;el!=NULL;el=el->next)
  {
    prime = (int)(el->contents);

    if(prime > (int)sqrt((double)num))
    {
      break;
    }

    if(num%prime == 0)
    {
      return(FALSE);
    }
  }

  return(TRUE);
}


int main(int argc,char *argv[])
{
  int i,prime,new_prime;
  wn_sll prime_list,*pprime_list_end,el;

  wn_gpmake("no_free");

  prime_list = NULL;
  pprime_list_end = &prime_list;

  for(i=2;i<9000;++i)
  {
    if(num_is_prime(i,prime_list))
    {
      new_prime = i;

      /* insert new prime at end of list */
      wn_mksll(&el);
      el->contents = (ptr)new_prime;
      *pprime_list_end = el;
      pprime_list_end = &(el->next);
    }
  }

  for(el=prime_list;el!=NULL;el=el->next)
  {
    prime = (int)(el->contents);

    printf("prime = %d\n",prime);
  }

  wn_gpfree();
}
