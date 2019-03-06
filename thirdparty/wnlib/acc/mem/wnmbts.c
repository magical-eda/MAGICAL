#define wnmbtsC

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

*****************************************************************************

  Self test code for custom mbtr datastructures used within freeable
  memory groups.

****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnasrt.h"
#include "wnmbtr.h"


#define SIZE  100000


local int lo_old_mbtr_test(void)
{
  struct wn_mbtree_struct stree, *tree;
  wn_mbhandle handle;
  int i, j, num;
  /* int count,index,index2; ** unused - bchapman 041111 */

  tree = &stree;
  wn_mmkptrbtree(tree);
  wn_mbverify(tree);

  for(i=0;i<SIZE;i++)
  {
    j = (i * 17) % SIZE;

    /*
    num = wn_random_int_between(0,1000000);
    num = 1;
    */
    num = j;
    handle = (wn_mbhandle) malloc(sizeof(*handle));
    wn_mbins(handle,tree,(ptr)num);
    handle->free_block_list = (wn_free_block)(num+1);
  }

  wn_assert(0 == wn_mem_used());
  wn_mbverify(tree);

  for(i=0;i<SIZE;i++)
  {
    num = i;
    wn_mbget(&handle,tree,(ptr)num,WN_MB_EQ);
    wn_assert((long int)(handle->free_block_list) == num+1);
  }

  for(i=0;i<SIZE;i++)
  {
    num = i;
    wn_mbget(&handle,tree,(ptr)num,WN_MB_EQ);
    wn_mbdel(handle,tree);
    free(handle);
  }

  wn_mbverify(tree);

  wn_assert(tree->handle_tree == NULL);

  wn_mbverify(tree);

  return(0);
} /* lo_old_mbtr_test */


#define LO_MIN_KEY 123
#define LO_MID_KEY 1234
#define LO_MAX_KEY 12345

local int lo_scores[LO_MAX_KEY+1];

local void lo_test_act(wn_mbhandle handle)
{
  ++ lo_scores[(long int) handle->key];
}


local void lo_mbtr_test_main(void)
{
  struct wn_mbtree_struct stree, *tree;
  wn_mbhandle handle;
  int i, j;

  tree = &stree;
  wn_mmkptrbtree(tree);

  for (i = 0;  i <= LO_MAX_KEY;  ++i)
  {
    j = (i * 7) % (LO_MAX_KEY + 1);
    if (j >= LO_MIN_KEY  &&  j <= LO_MAX_KEY)
    {
      handle = (wn_mbhandle) malloc(sizeof(*handle));
      wn_mbins(handle, tree, (ptr) j);
      handle->free_block_list = (wn_free_block) (j + 2);
    }
  }

  wn_mbverify(tree);

  wn_mbget(&handle, tree, (ptr) 0, WN_MB_MIN);
  wn_assert(handle->key == (ptr) LO_MIN_KEY);

  wn_mbget(&handle, tree, (ptr) 0, WN_MB_MAX);
  wn_assert(handle->key == (ptr) LO_MAX_KEY);

  wn_mbget(&handle, tree, (ptr) (LO_MIN_KEY - 5), WN_MB_LT);
  wn_assert(!handle);

  wn_mbget(&handle, tree, (ptr) (LO_MIN_KEY - 5), WN_MB_LE);
  wn_assert(!handle);

  wn_mbget(&handle, tree, (ptr) (LO_MIN_KEY - 5), WN_MB_EQ);
  wn_assert(!handle);

  wn_mbget(&handle, tree, (ptr) (LO_MIN_KEY - 5), WN_MB_GE);
  wn_assert(LO_MIN_KEY == (long int) handle->key);

  wn_mbget(&handle, tree, (ptr) (LO_MIN_KEY - 5), WN_MB_GT);
  wn_assert(LO_MIN_KEY == (long int) handle->key);

  wn_mbget(&handle, tree, (ptr) LO_MIN_KEY, WN_MB_LT);
  wn_assert(!handle);

  wn_mbget(&handle, tree, (ptr) LO_MAX_KEY, WN_MB_GT);
  wn_assert(!handle);

  wn_mbget(&handle, tree, (ptr) (LO_MAX_KEY + 5), WN_MB_LT);
  wn_assert(LO_MAX_KEY == (long int) handle->key);

  wn_mbget(&handle, tree, (ptr) (LO_MAX_KEY + 5), WN_MB_LE);
  wn_assert(LO_MAX_KEY == (long int) handle->key);

  wn_mbget(&handle, tree, (ptr) (LO_MAX_KEY + 5), WN_MB_GE);
  wn_assert(!handle);

  wn_mbget(&handle, tree, (ptr) (LO_MAX_KEY + 5), WN_MB_GT);
  wn_assert(!handle);

  for (i = LO_MIN_KEY;  i <= LO_MAX_KEY;  ++i)
  {
    wn_mbget(&handle, tree, (ptr) i, WN_MB_EQ);
    wn_assert((ptr) i == handle->key);

    wn_mbget(&handle, tree, (ptr) i, WN_MB_LE);
    wn_assert((ptr) i == handle->key);

    wn_mbget(&handle, tree, (ptr) i, WN_MB_GE);
    wn_assert((ptr) i == handle->key);
  }

  for (i = LO_MIN_KEY+1;  i <= LO_MAX_KEY;  ++i)
  {
    wn_mbget(&handle, tree, (ptr) i, WN_MB_LT);
    wn_assert((ptr) (i-1) == handle->key);
  }

  for (i = LO_MIN_KEY;  i <= LO_MAX_KEY-1;  ++i)
  {
    wn_mbget(&handle, tree, (ptr) i, WN_MB_GT);
    wn_assert((ptr) (i+1) == handle->key);
  }

  for (i = 0;  i <= LO_MAX_KEY;  ++i)
  {
    lo_scores[i] = 0;
  }
  wn_mbact(tree, lo_test_act, (ptr) LO_MIN_KEY, WN_MB_GE,
  /**/			      (ptr) LO_MAX_KEY, WN_MB_LE);

  for (i = 0;  i < LO_MIN_KEY;  ++i)
  {
    wn_assert(0 == lo_scores[0]);
  }
  for (i = LO_MIN_KEY;  i <= LO_MAX_KEY;  ++i)
  {
    wn_assert(1 == lo_scores[i]);
  }

  for (i = LO_MIN_KEY;  i < LO_MID_KEY;  ++i)
  {
    wn_mbget(&handle, tree, (ptr) i, WN_MB_EQ);

    wn_mbdel(handle, tree);
    free(handle);
  }

  for (i = LO_MIN_KEY;  i < LO_MID_KEY;  ++i)
  {
    wn_mbget(&handle, tree, (ptr) i, WN_MB_EQ);

    wn_assert(!handle);
  }
  for (i = LO_MID_KEY;  i <= LO_MAX_KEY;  ++i)
  {
    wn_mbget(&handle, tree, (ptr) i, WN_MB_EQ);

    wn_assert((ptr) i == handle->key);
  }

  wn_assert(LO_MAX_KEY + 1 - LO_MID_KEY == wn_mbcount(tree));

  wn_mbverify(tree);
} /* lo_mbtr_test_main */


void wn_mbtree_test(void)
{

  printf("testing wnmbtr, memory btrees...\n");

  lo_old_mbtr_test();

  lo_mbtr_test_main();

  printf("  ok!!!!!!\n");
} /* wn_mbtree_test */
