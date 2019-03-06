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
#include <stdlib.h>
#include <stdio.h>

#include "wnlib.h"

#if !defined(WN_WINDOWS)
# include <sys/times.h>
#else
# define clock_t long
#endif

#include "wnasrt.h"
#include "wnrnd.h"
#include "wnargp.h"
#include "wnnop.h"
#include "wncmp.h"
#include "wncpy.h"
#include "wnmem.h"

#include "wnsll.h"
#include "wnbtr.h"
#include "wnbtrl.h"
#include "wnskl.h"
#include "wnskll.h"

local int lo_printsklist(wn_sklist sklist)
{
  wn_skhandle handle, handle2;
  int i;

  for (handle = WN_SKIP_NEXTS(sklist->handle)[0];  handle;
  /**/					handle = WN_SKIP_NEXTS(handle)[0])
  {
    printf("%2d: ", handle->key);
    for (i = 0;  i < (long) handle->contents;  ++i)
    {
      handle2 = WN_SKIP_NEXTS(handle)[i];
      if (handle2)
      {
	printf("%4d", handle2->key);
      }
      else
      {
	printf(" nil");
      }
    }

    printf("\n");
  }

  printf("\n");

  return 0;
} /* lo_printsklist */


local void lo_test_first(void)
{
  wn_sklist sklist;
  wn_skhandle handle;

  wn_gpmake("general_free");

    wn_mkintsklist(&sklist, .33);

    wn_skins(&handle, sklist, (ptr)  8);
    wn_skins(&handle, sklist, (ptr)  4);
    wn_skins(&handle, sklist, (ptr) 17);
    wn_skins(&handle, sklist, (ptr) 11);
    wn_skins(&handle, sklist, (ptr)  2);

    handle = WN_SKIP_NEXTS(sklist->handle)[0];
    wn_assert(handle->key == (ptr) 2);

    handle = WN_SKIP_NEXTS(handle)[0];
    wn_assert(handle->key == (ptr) 4);

    handle = WN_SKIP_NEXTS(handle)[0];
    wn_assert(handle->key == (ptr) 8);

    handle = WN_SKIP_NEXTS(handle)[0];
    wn_assert(handle->key == (ptr) 11);

    handle = WN_SKIP_NEXTS(handle)[0];
    wn_assert(handle->key == (ptr) 17);

    wn_skins(&handle, sklist, (ptr)  5);
    wn_skins(&handle, sklist, (ptr)  7);
    wn_skins(&handle, sklist, (ptr)  9);
    wn_skins(&handle, sklist, (ptr) 22);
    wn_skins(&handle, sklist, (ptr) 12);
    wn_skins(&handle, sklist, (ptr)  1);

    wn_skget(&handle, sklist, (ptr) 5, WN_SK_LT);
    wn_assert(handle->key == (ptr) 4);

    wn_skget(&handle, sklist, (ptr) 6, WN_SK_LT);
    wn_assert(handle->key == (ptr) 5);

    wn_skget(&handle, sklist, (ptr) 4, WN_SK_LT);
    wn_assert(handle->key == (ptr) 2);

    wn_skget(&handle, sklist, (ptr) 15, WN_SK_LT);
    wn_assert(handle->key == (ptr) 12);

    wn_skget(&handle, sklist, (ptr) 5, WN_SK_LE);
    wn_assert(handle->key == (ptr) 5);

    wn_skget(&handle, sklist, (ptr) 6, WN_SK_LE);
    wn_assert(handle->key == (ptr) 5);

    wn_skget(&handle, sklist, (ptr) 4, WN_SK_LE);
    wn_assert(handle->key == (ptr) 4);

    wn_skget(&handle, sklist, (ptr) 15, WN_SK_LE);
    wn_assert(handle->key == (ptr) 12);

    wn_skget(&handle, sklist, (ptr) 5, WN_SK_GT);
    wn_assert(handle->key == (ptr) 7);

    wn_skget(&handle, sklist, (ptr) 6, WN_SK_GT);
    wn_assert(handle->key == (ptr) 7);

    wn_skget(&handle, sklist, (ptr) 4, WN_SK_GT);
    wn_assert(handle->key == (ptr) 5);

    wn_skget(&handle, sklist, (ptr) 15, WN_SK_GT);
    wn_assert(handle->key == (ptr) 17);

    wn_skget(&handle, sklist, (ptr) 5, WN_SK_GE);
    wn_assert(handle->key == (ptr) 5);

    wn_skget(&handle, sklist, (ptr) 6, WN_SK_GE);
    wn_assert(handle->key == (ptr) 7);

    wn_skget(&handle, sklist, (ptr) 4, WN_SK_GE);
    wn_assert(handle->key == (ptr) 4);

    wn_skget(&handle, sklist, (ptr) 15, WN_SK_GE);
    wn_assert(handle->key == (ptr) 17);

    lo_printsklist(sklist);

    wn_skget(&handle, sklist, (ptr) 6, WN_SK_EQ);
    wn_assert(!handle);

    wn_skget(&handle, sklist, (ptr) 17, WN_SK_EQ);
    wn_assert((ptr) 17 == handle->key);

    wn_skget(&handle, sklist, (ptr) 9, WN_SK_EQ);
    wn_assert((ptr) 9 == handle->key);


    wn_skget(&handle, sklist, (ptr) 17, WN_SK_EQ);
    wn_skdel(handle, sklist);

    lo_printsklist(sklist);

    wn_skget(&handle, sklist, (ptr) 9, WN_SK_EQ);
    wn_skdel(handle, sklist);

    wn_skget(&handle, sklist, (ptr) 12, WN_SK_EQ);
    wn_skdel(handle, sklist);

    lo_printsklist(sklist);

  wn_gpfree();
} /* lo_test_first */


/* total memory in blocks of this group and its children. */

local int lo_total_block_group_memory_size(wn_memgp group)
{
  wn_mem_block block;
  wn_memgp child;
  int ret;

  ret = 0;
  for (block = group->current_block;  block;  block = block->next)
  {
    ret += block->size;
  }

  for (child = group->children;  child;  child->sub_next)
  {
    ret += lo_total_block_group_memory_size(child);
  }

  return ret;
} /* lo_total_block_group_memory_size */


local clock_t lo_get_time(void)
{
# ifndef WN_WINDOWS
    struct tms time_buf;
    int sts;

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    return time_buf.tms_utime;
# else
    static bool first_time = TRUE;

    if (first_time) {
      first_time = FALSE;
      fprintf(stderr, "Time function not working on windows - clock\n");
      fprintf(stderr,				"    is just returning 0\n");
    }

    return 0;
# endif
} /* lo_get_time */


local void lo_test_sklist_access(int trials, double threshold)
{
  wn_sklist sklist;
  wn_btree btr;
  wn_sll rand_list, el;
  int r;
  wn_skhandle handle;
  wn_bhandle bhandle;
  wn_memgp sklist_group, rand_list_group, btr_group;
  int start_time, end_time;
  int i;

  wn_gpmake("no_free");
    wn_mkintsklist(&sklist, threshold);
    sklist_group = wn_curgp();
  wn_gppop();

  wn_gpmake("no_free");
    rand_list_group = wn_curgp();
  wn_gppop();

  wn_gpmake("no_free");
    wn_mkintbtree(&btr);
    btr_group = wn_curgp();
  wn_gppop();

  wn_gppush(rand_list_group);

    /* note the sklist stuff goes in the sklist group, the list stuff in the
    ** list memory group */
    for (i = 0;  i < trials;  ++i)
    {
      r = wn_random_int();
      wn_skins(&handle, sklist, (ptr) r);
      wn_bins(&bhandle, btr, (ptr) r);
      wn_sllins(&rand_list, (ptr) r);
    }

  wn_gppop();

  wn_sllrev(&rand_list);
  /* paw through the cache */
  for (el = rand_list;  el;  el = el->next)
  {
    ;
  }

  printf("For %d trials, thresh %.2f MB:: list: %.2f, btr: %.2f, sklist: %.2f\n",
  /**/							trials, threshold, 
  /**/	lo_total_block_group_memory_size(rand_list_group) / 1024 / 1024.0,
  /**/	lo_total_block_group_memory_size(btr_group) / 1024 / 1024.0,
  /**/	lo_total_block_group_memory_size(sklist_group) / 1024 / 1024.0);

  start_time = lo_get_time();

    for (el = rand_list;  el;  el = el->next)
    {
      wn_skget(&handle, sklist, el->contents, WN_SK_EQ);
      wn_assert(handle);
    }

  end_time = lo_get_time();

  printf("%d successful skgets took %.2f seconds\n", trials,
  /**/					(end_time - start_time) / 100.0);

  start_time = lo_get_time();

    for (el = rand_list;  el;  el = el->next)
    {
      wn_bget(&bhandle, btr, el->contents, WN_B_EQ);
      wn_assert(bhandle);
    }

  end_time = lo_get_time();

  printf("%d successful  bgets took %.2f seconds\n", trials,
  /**/					(end_time - start_time) / 100.0);
} /* lo_test_sklist_access */


int main(int argc, char *argv[])
{
  static bool first_test, access_test;
  static int access_trials;
  static double access_threshold;
  static wn_arg_format arg_format_array[] =
  {
    WN_KEY_BOOL(&first_test, "f"),
    WN_KEY_BOOL(&access_test, "a"),
    WN_KEY_INT(&access_trials, "trials", "access_trials", WN_OPTIONAL),
    WN_KEY_DOUBLE(&access_threshold, "threshold", "access_threshold",
    /**/							WN_OPTIONAL),
    WN_ARG_TERMINATE
  };

  access_trials = 1000000;
  access_threshold = 0.33;

  wn_parse_args(argc, argv, arg_format_array);

  setbuf(stdout, NULL);

  if (first_test)
  {
    lo_test_first();
  }

  if (access_test)
  {
    lo_test_sklist_access(access_trials, access_threshold);
  }

  return 0;
} /* main */
