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
#include "wncmp.h"
#include "wncpy.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wnbtr.h"
#include "wnbtrl.h"



#if 0


local void print_handle(wn_bhandle handle)
{
  printf("handle->key=%d\n",handle->key);
}


main()
{
  wn_btree tree;
  wn_bhandle handle;
  int i,num,count,index,index2;
  
  wn_gpmake("general_free");

  wn_mkintbtree(&tree);
  wn_bverify(tree);

  for(i=0;i<1000;i++)
  {
    /*
    num = wn_random_int_between(0,1000000);
    num = 1;
    */
    num = i;
    wn_bins(&handle,tree,(ptr)num);
    handle->contents = (ptr)(num+1);
  }

  wn_bverify(tree);
  printf("inserts finished\n");

  wn_bact(tree,(print_handle),(ptr)1,WN_B_EQ,(ptr)10,WN_B_GT);
  printf("wnbact finished\n");

  for(i=0;i<1000;i++)
  {
    count = wn_bcount(tree);
    wn_assert(count > 0);

    index = wn_random_int_between(0,count);
    wn_bget_handle_of_index(&handle,tree,index);
    wn_bget_index_of_handle(&index2,tree,handle);
    wn_assert(index == index2);
    wn_bdel(handle,tree);
  }

  wn_bverify(tree);

  count = wn_bcount(tree);
  wn_assert(count == 0);

  wn_bverify(tree);
  printf("deletes finished\n");

  wn_gpfree();
}
#endif



#if 1

#define SIZE  1000000


static int count;

local int count_intcmp(int i1,int i2)
{
  ++count;

  return(wn_intcmp(i1,i2));
}


int main(void)
{
  wn_btree tree;
  wn_bhandle handle;
  int i,num,count_ins,count_del;
  
  wn_gpmake("general_free");

  wn_mkbtree(&tree,
	     (int (*)(ptr,ptr))(count_intcmp),
	     (void (*)(ptr *,ptr))(wn_intcpy),
	     (void (*)(ptr))(wn_do_nothing));

  count = 0;

  wn_seed_random_number_generator(0);

  for(i=0;i<SIZE;++i)
  {
    num = (int)wn_random_int();
    /*
    num = i;
    */
    wn_bins(&handle,tree,(ptr)num);
    handle->contents = (ptr)(num+1);
  }

  printf("inserts finished. compare count = %d\n",count);

#if 0
  count = 0;

  wn_seed_random_number_generator(0);

  for(i=0;i<SIZE;i++)
  {
    num = wn_random_int();
    /*
    num = i;
    */
    wn_bget(&handle,tree,num,WN_B_EQ);
  }

  printf("searches finished. compare count = %d\n",count);
#endif

  count_ins = count_del = 0;

  wn_seed_random_number_generator(1);

  for(i=0;i<SIZE;i++)
  {
    num = (int)wn_random_int();

    count = 0;
    wn_bins(&handle,tree,(ptr)num);
    count_ins += count;

    count = 0;
    wn_bdel(handle,tree);
    count_del += count;
  }

  printf("trials finished. ins count = %d,delete count = %d\n",
	 count_ins,count_del);

  wn_gpfree();

  return(0);
}

#endif

