/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include "wnlib.h"
#include "wnmem.h"
#include "wnsll.h"
#include "wncpy.h"
#include "wnrnd.h"



void main(void)
{
  bool advanced;
  unsigned long int start_search;
  unsigned long int test,prime;
  int i;
  wn_sll prime_list,*pend,el;

  start_search = 1;
  for(i=0;i<30;++i)
  {
    start_search <<= 1;
    start_search |= wn_random_bit();
  }
  printf("start_search=%d %x\n",start_search,start_search);

  prime_list = NULL;
  pend = &prime_list;

  wn_sllins(pend,(ptr)2);
  pend = &((*pend)->next);

  advanced = FALSE;

  for(test=3;test<(unsigned)(1<<31);test+=2)
  {
    for(el=prime_list;el!=NULL;el=el->next)
    {
      prime = (unsigned long int)(el->contents);

      if(prime*prime > test)
      {
	printf("new prime = %lu\n",test);
        wn_sllins(pend,(ptr)test);
        pend = &((*pend)->next);
	if((test > 100000)&&(!advanced))
	{
	  /*
	  test = (unsigned)(1<<31)-1001;
	  */
	  test = start_search;
	  advanced = TRUE;
        }
	break;
      }
      else if(test%prime == 0)
      {
	break;  /* not prime */
      }
    }
  }
}

