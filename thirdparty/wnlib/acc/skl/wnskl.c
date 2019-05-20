/**************************************************************************

void wn_mksklist
(
  wn_sklist *psklist,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  void (*palloc_copy_key_func)(ptr *pkey,ptr key),
  void (*pfree_key_func)(ptr key)
);
void wn_freesklist(wn_sklist sklist);

void wn_skget(wn_skhandle *phandle, wn_sklist sklist, ptr key, int compare);
void wn_skins(wn_skhandle *phandle, wn_sklist sklist, ptr key);
void wn_skdel(wn_skhandle handle, wn_sklist sklist);
void wn_skmove(wn_skhandle handle, wn_sklist sklist,ptr new_key);

int wn_skcount(wn_sklist sklist);
void wn_skact
(
  wn_sklist sklist,
  void (*paction)(wn_bhandle handle),
  ptr low_key,
  int low_compare,
  ptr high_key,
  int high_compare
);

void wn_skverify(wn_sklist sklist);

**************************************************************************/
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

#include <stdio.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnmax.h"
#include "wnabs.h"
#include "wnrnd.h"

#include "wnskl.h"


#define LO_SKIP_RAND_BITS	6


EXTERN void wn_mksklist
(
  wn_sklist *psklist,
  double threshold,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  void (*palloc_copy_key_func)(ptr *pkey,ptr key),
  void (*pfree_key_func)(ptr key)
)
{
  *psklist = (wn_sklist) wn_alloc(sizeof(struct wn_sklist_struct));

  wn_assert(1.0 > threshold);
  wn_assert(0.0 < threshold);
  (*psklist)->threshold = (int) (threshold * (1<<LO_SKIP_RAND_BITS));
  (*psklist)->threshold = wn_max((*psklist)->threshold, 0);
  (*psklist)->threshold = wn_min((*psklist)->threshold, (1<<LO_SKIP_RAND_BITS)-2);

  (*psklist)->max_levels = 1000; /* for now */

  (*psklist)->pcompare_keys_func = pcompare_keys_func;
  (*psklist)->palloc_copy_key_func = palloc_copy_key_func;
  (*psklist)->pfree_key_func = pfree_key_func;

  (*psklist)->handle = (wn_skhandle) wn_alloc(WN_SKHANDLE_SIZE(1));
  (*psklist)->ptr_count = 1;

  (*psklist)->handle->key = NULL;
  WN_SKIP_NEXTS((*psklist)->handle)[0] = NULL;

  (*psklist)->group = wn_curgp();
} /* wn_mksklist */


void wn_freesklist(wn_sklist sklist)
{
  wn_skhandle handle, condemned;

  wn_gppush(sklist->group);

    for (handle = sklist->handle;  handle;  )
    {
      condemned = handle;
      handle = WN_SKIP_NEXTS(handle)[0];

      wn_free((ptr) condemned);
    }

    wn_free((ptr) sklist);

  wn_gppop();
} /* wn_freesklist */


/* find next handle on the biggest link that is non-null */

local wn_skhandle lo_find_next_big(wn_skhandle node, int *pptr_index)
{
  wn_skhandle next_handle;

  next_handle = WN_SKIP_NEXTS(node)[*pptr_index];
  while (!next_handle)
  {
    if (0 == *pptr_index)
    {
      return NULL;
    }

    -- *pptr_index;
    next_handle = WN_SKIP_NEXTS(node)[*pptr_index];
  }

  wn_assert(next_handle);		wn_assert(0 <= *pptr_index);

  return next_handle;
} /* lo_find_next_big */


local void lo_skget_eq(wn_skhandle *pret_handle, wn_sklist sklist, ptr key)
{
  wn_skhandle handle, next_handle;
  int sts;
  int ptr_index;

  handle = sklist->handle;
  ptr_index = sklist->ptr_count-1;
  next_handle = lo_find_next_big(handle, &ptr_index);

  while (next_handle)
  {
    sts = (*sklist->pcompare_keys_func)(key, next_handle->key);
    if (!sts)
    {
      *pret_handle = next_handle;
      return;
    }
    else if (0 < sts)
    {
      handle = next_handle;

      next_handle = lo_find_next_big(handle, &ptr_index);
    }
    else /* 0 > sts */
    {
      if (0 == ptr_index)
      {
	*pret_handle = NULL;
	return;
      }

      -- ptr_index;
      next_handle = lo_find_next_big(handle, &ptr_index);
    }
  }

  *pret_handle = NULL;
} /* lo_skget_eq */


local void lo_skget_lt(wn_skhandle *pret_handle, wn_sklist sklist, ptr key)
{
  wn_skhandle handle, next_handle;
  int sts;
  int ptr_index;

  handle = sklist->handle;
  ptr_index = sklist->ptr_count-1;
  next_handle = lo_find_next_big(handle, &ptr_index);

  while (next_handle)
  {
    sts = (*sklist->pcompare_keys_func)(key, next_handle->key);
    if (0 >= sts)
    {
      if (0 == ptr_index)
      {
	break;
      }

      -- ptr_index;
      next_handle = lo_find_next_big(handle, &ptr_index);
    }
    else if (0 < sts)
    {
      handle = next_handle;

      next_handle = lo_find_next_big(handle, &ptr_index);
    }
  }

  wn_assert(handle);
  if (handle == sklist->handle)
  {
    handle = NULL;
  }
  *pret_handle = handle;
} /* lo_skget_lt */


local void lo_skget_le(wn_skhandle *pret_handle, wn_sklist sklist, ptr key)
{
  wn_skhandle handle, next_handle;
  int sts;
  int ptr_index;

  handle = sklist->handle;
  ptr_index = sklist->ptr_count-1;
  next_handle = lo_find_next_big(handle, &ptr_index);

  while (next_handle)
  {
    sts = (*sklist->pcompare_keys_func)(key, next_handle->key);
    if (0 == sts)
    {
      *pret_handle = next_handle;
      return;
    }
    else if (0 > sts)
    {
      if (0 == ptr_index)
      {
	break;
      }

      -- ptr_index;
      next_handle = lo_find_next_big(handle, &ptr_index);
    }
    else if (0 < sts)
    {
      handle = next_handle;

      next_handle = lo_find_next_big(handle, &ptr_index);
    }
  }

  wn_assert(handle);
  if (handle == sklist->handle)
  {
    handle = NULL;
  }
  *pret_handle = handle;
} /* lo_skget_le */


local void lo_skget_gt(wn_skhandle *pret_handle, wn_sklist sklist, ptr key)
{
  wn_skhandle handle, next_handle;
  int sts;
  int ptr_index;

  handle = sklist->handle;
  ptr_index = sklist->ptr_count-1;
  next_handle = lo_find_next_big(handle, &ptr_index);

  while (next_handle)
  {
    sts = (*sklist->pcompare_keys_func)(key, next_handle->key);
    if (0 > sts)
    {
      if (0 == ptr_index)
      {
	break;
      }

      -- ptr_index;
      next_handle = lo_find_next_big(handle, &ptr_index);
    }
    else /* if (0 <= sts) */
    {
      handle = next_handle;

      next_handle = lo_find_next_big(handle, &ptr_index);
    }
  }

  *pret_handle = next_handle;
} /* lo_skget_gt */


local void lo_skget_ge(wn_skhandle *pret_handle, wn_sklist sklist, ptr key)
{
  wn_skhandle handle, next_handle;
  int sts;
  int ptr_index;

  handle = sklist->handle;
  ptr_index = sklist->ptr_count-1;
  next_handle = lo_find_next_big(handle, &ptr_index);

  while (next_handle)
  {
    sts = (*sklist->pcompare_keys_func)(key, next_handle->key);
    if (0 == sts)
    {
      break;
    }
    else if (0 > sts)
    {
      if (0 == ptr_index)
      {
	break;
      }

      -- ptr_index;
      next_handle = lo_find_next_big(handle, &ptr_index);
    }
    else /* if (0 < sts) */
    {
      handle = next_handle;

      next_handle = lo_find_next_big(handle, &ptr_index);
    }
  }

  *pret_handle = next_handle;
} /* lo_skget_ge */


local void lo_skget_min(wn_skhandle *pret_handle, wn_sklist sklist)
{

  *pret_handle = WN_SKIP_NEXTS(sklist->handle)[0];
}


local void lo_skget_max(wn_skhandle *pret_handle, wn_sklist sklist)
{
  wn_skhandle handle, next_handle;
  /* int sts; ** unused - bchapman 041111 */
  int ptr_index;

  next_handle = sklist->handle;
  ptr_index = sklist->ptr_count-1;
  do
  {
    handle = next_handle;
    next_handle = lo_find_next_big(handle, &ptr_index);
  } while (next_handle);

  *pret_handle = handle;
} /* lo_skget_max */


void wn_skget(wn_skhandle *phandle, wn_sklist sklist, ptr key, int compare)
{
  switch (compare)
  {
    case (WN_SK_MIN):
    {
      lo_skget_min(phandle, sklist);
    } break;
    case (WN_SK_MAX):
    {
      lo_skget_max(phandle, sklist);
    } break;
    case (WN_SK_LT):
    {
      lo_skget_lt(phandle, sklist, key);
    } break;
    case (WN_SK_LE):
    {
      lo_skget_le(phandle, sklist, key);
    } break;
    case (WN_SK_EQ):
    {
      lo_skget_eq(phandle, sklist, key);
    } break;
    case (WN_SK_GT):
    {
      lo_skget_gt(phandle, sklist, key);
    } break;
    case (WN_SK_GE):
    {
      lo_skget_ge(phandle, sklist, key);
    } break;
    default:
    {
      wn_assert_notreached();
    }
  }
} /* wn_skget */


local wn_skhandle lo_find_prev(wn_skhandle handle, wn_sklist sklist,
/**/							ptr key, int level)
{
  int compare;
  wn_skhandle next_handle;

  next_handle = WN_SKIP_NEXTS(handle)[level];

  while (next_handle)
  {
    compare = (*sklist->pcompare_keys_func)(key, next_handle->key);
    if (compare > 0)
    {
      handle = next_handle;
      next_handle = WN_SKIP_NEXTS(next_handle)[level];
    }
    else /* compare >= 0 */
    {
      return handle;
    }
  }

  return handle;
} /* lo_find_prev */


void wn_skins(wn_skhandle *phandle, wn_sklist sklist, ptr key)
{
  wn_skhandle prev_handle;
  /* wn_skhandle new_handle, new_head_handle; ** unused - bchapman 041111 */
  int new_ptr_count;
  int i;

  for (new_ptr_count = 1;  new_ptr_count < sklist->max_levels;  ++new_ptr_count)
  {
    if (wn_random_n_bits(LO_SKIP_RAND_BITS) > sklist->threshold)
    {
      break;
    }
  }

  wn_gppush(sklist->group);

    *phandle = (wn_skhandle) wn_alloc(WN_SKHANDLE_SIZE(new_ptr_count));
    (*(sklist->palloc_copy_key_func))(&(*phandle)->key, key);

    if (new_ptr_count > sklist->ptr_count)
    {
      /* we need a new headnode */
      wn_realloc((ptr *) &sklist->handle, WN_SKHANDLE_SIZE(sklist->ptr_count),
      /**/			    WN_SKHANDLE_SIZE(  new_ptr_count));

      for (i = sklist->ptr_count;  i < new_ptr_count;  ++i)
      {
	WN_SKIP_NEXTS(sklist->handle)[i] = NULL;
      }

      sklist->ptr_count =            new_ptr_count;
    }

    prev_handle = sklist->handle;
    for (i = sklist->ptr_count - 1;  i >= new_ptr_count;  --i)
    {
      prev_handle = lo_find_prev(prev_handle, sklist, key, i);
      wn_assert(prev_handle);
    }
    for (  ;  i >= 0;  --i)
    {
      prev_handle = lo_find_prev(prev_handle, sklist, key, i);
      wn_assert(prev_handle);

      WN_SKIP_NEXTS( *phandle)[i] = WN_SKIP_NEXTS(prev_handle)[i];
      WN_SKIP_NEXTS(prev_handle)[i] = *phandle;
    }

    /* this is useful for some debugging, but may be overridden by user */
    (*phandle)->contents = (ptr) new_ptr_count;

  wn_gppop();
} /* wn_skins */


/* returns a node closer to or equal to start_node, exactly preceeding
** target_node, on level level */

local wn_skhandle lo_del_find_prev(
  wn_skhandle start_handle,	/* this node starts out somewhere before
  **				** the target node */
  wn_sklist sklist,
  wn_skhandle target_handle,	/* we want to be before this node */
  int level)
{
  int compare;
  wn_skhandle next_handle, ret_handle;

  ret_handle = start_handle;
  next_handle = WN_SKIP_NEXTS(start_handle)[level];

  while (next_handle)
  {
    compare = (*sklist->pcompare_keys_func)(target_handle->key,next_handle->key);
    if (compare > 0  ||  0 == compare && next_handle != target_handle)
    {
      ret_handle = next_handle;
      next_handle = WN_SKIP_NEXTS(next_handle)[level];
    }
    else /* compare >= 0 */
    {
      break;
    }
  }

  return ret_handle;
} /* lo_del_find_prev */


void wn_skdel(wn_skhandle handle, wn_sklist sklist)
{
  wn_skhandle prev_handle;
  /* wn_skhandle new_handle, new_head_handle; ** unused - bchapman 041111 */
  /* int new_ptr_count; ** unused - bchapman 041111 */
  int i, j;

  prev_handle = sklist->handle;
  for (j = sklist->ptr_count - 1;  j >= 0;  --j)
  {
    prev_handle = lo_del_find_prev(prev_handle, sklist, handle, j);
    wn_assert(prev_handle);

    if (WN_SKIP_NEXTS(prev_handle)[j] == handle)
    {
      break;
    }
  }

  for (i = j;  i >= 0;  --i)
  {
    prev_handle = lo_del_find_prev(prev_handle, sklist, handle, i);
    wn_assert(prev_handle);

    WN_SKIP_NEXTS(prev_handle)[i] = WN_SKIP_NEXTS(handle)[i];
  }

  wn_free(handle);
} /* wn_skdel */


void wn_skmove(wn_skhandle handle, wn_sklist sklist, ptr new_key)
{
  wn_skhandle new_handle;

  wn_skins(&new_handle, sklist, new_key);
  new_handle->contents = handle->contents;

  wn_skdel(handle, sklist);
} /* wn_skmove */


#if 0  /* not included because not possible to implement efficiently */
void wn_skget_index_of_handle(int *pindex, wn_sklist sklist, wn_skhandle handle)
{
  wn_skhandle handle2;
  int i;

  for (i = -1, handle2 = sklist->handle;  handle2 && handle2 != handle;
  /**/				handle2 = WN_SKIP_NEXTS(handle2)[0], ++i)
  {
    ;
  }

  wn_assert(handle2);

  *pindex = i;
} /* wn_skget_index_of_handle */


void wn_skget_handle_of_index(wn_skhandle *phandle, wn_sklist sklist, int index)
{
  wn_skhandle handle2;
  int i;

  for (handle2 = sklist->handle;  handle2  &&  index >= 0;
  /**/				handle2 = WN_SKIP_NEXTS(handle2)[0], --index)
  {
    ;
  }

  *phandle = handle2;
} /* wn_skget_handle_of_index */
#endif


int wn_skcount(wn_sklist sklist)
{
  wn_skhandle handle2;
  int i;

  for (i = -1, handle2 = sklist->handle;  handle2;
  /**/				handle2 = WN_SKIP_NEXTS(handle2)[0], ++i)
  {
    ;
  }

  return i;
} /* wn_skcount */


EXTERN void wn_skact
(
  wn_sklist sklist,
  void (*paction)(wn_skhandle handle),
  ptr low_key,
  int low_compare,
  ptr high_key,
  int high_compare
)
{
  wn_skhandle bottom_handle, top_handle, handle;

  wn_skget(&bottom_handle, sklist,  low_key,  low_compare);
  wn_skget(&top_handle,    sklist, high_key, high_compare);

  if (!bottom_handle)
  {
    bottom_handle = WN_SKIP_NEXTS(sklist->handle)[0];
  }

  for (handle = bottom_handle;  TRUE;  handle = WN_SKIP_NEXTS(handle)[0])
  {
    if (!handle)
    {
      return;
    }

    (*paction)(handle);

    if (handle == top_handle)
    {
      return;
    }
  }
} /* wn_skact */


void wn_skverify(wn_sklist sklist)
{
  wn_skhandle handle, handle1, handle2;
  int i, j;

  for (i = sklist->ptr_count-1;  i >= 0;  --i)
  {
    for (handle = sklist->handle;  handle;  handle = WN_SKIP_NEXTS(handle)[i])
    {
      for (j = i;  j >= 0  &&  !WN_SKIP_NEXTS(handle)[j];  --j)
      {
	;
      }
      for (  ;  j >= 0;  --j)
      {
	wn_assert(WN_SKIP_NEXTS(handle)[j]);
      }

      wn_assert(-1 == j);

      if (handle != sklist->handle)
      {
	for (j = i;  j >= 0;  --j)
	{
	  handle2 = WN_SKIP_NEXTS(handle)[j];

	  if (handle2)
	  {
	    wn_assert((*sklist->pcompare_keys_func)(handle->key,
	    /**/					handle2->key) <= 0);

	    if (j > 0)
	    {
	      handle1 = WN_SKIP_NEXTS(handle)[j-1];
	      wn_assert(handle1);

	      wn_assert((*sklist->pcompare_keys_func)(handle1->key,
	      /**/					handle2->key) <= 0);
	    }
	  }
	}
      }
    }
  }
} /* wn_skverify */
