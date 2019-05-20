#define wnmbtrC

/**************************************************************************

  Note that the software in this module is really not intended for general
use, but is intended only for supporting data structures within the memory
allocator.  Use for any other purpose may cause problems.

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

  Will Naylor, Bill Chapman

****************************************************************************/

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnmax.h"
#include "wnabs.h"
#include "wnmemb.h"

#include "wnmbtr.h"


local void lo_mmkbtree
(
  wn_mbtree tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  tree->handle_tree = NULL;
  tree->pcompare_keys_func = pcompare_keys_func;
}


local int lo_mptrcmp(ptr p1,ptr p2)
{
  if((long unsigned int)p1 > (long unsigned int)p2)
  {
    return(1);
  }
  else if((long unsigned int)p1 == (long unsigned int)p2)
  {
    return(0);
  }
  else /* (long unsigned int)p1 < (long unsigned int)p2 */
  {
    return(-1);
  }
}


void wn_mmkptrbtree(wn_mbtree tree)
{
  lo_mmkbtree(tree, (int (*)(ptr,ptr))(lo_mptrcmp));
}


#define lo_handle_tree_count(_handle_tree2)\
( ((_handle_tree2) == NULL) ? 0 : ((_handle_tree2)->count) )


#define lo_handle_tree_level(_handle_tree1)\
( ((_handle_tree1) == NULL) ? 0 : ((_handle_tree1)->level) )


/* note this just looks at the fields in the IMMEDIATE children, it
** doesn't recurse down.  Doesn't return a value, directly updates
** field in _handle3 */

local void lo_calculate_handle_count_from_children(
  wn_mbhandle _handle3
)
{
  int left_count, right_count;
  wn_mbhandle _left_child, _right_child;

  _left_child  = (_handle3)->left_child;
  _right_child = (_handle3)->right_child;

  left_count  = lo_handle_tree_count(_left_child);
  right_count = lo_handle_tree_count(_right_child);

  (_handle3)->count = left_count + right_count + 1;
}


/* note this just looks at the fields in the IMMEDIATE children, it
** doesn't recurse down.  Doesn't return a value, directly updates
** field in _handle3 */

local void lo_calculate_handle_level_from_children(
  wn_mbhandle _handle2
)
{
  int _left_level,_right_level;
  wn_mbhandle _left_child,_right_child;

  _left_child  = (_handle2)->left_child;
  _right_child = (_handle2)->right_child;

  _left_level  = lo_handle_tree_level(_left_child);
  _right_level = lo_handle_tree_level(_right_child);

  (_handle2)->level = wn_max(_left_level,_right_level) + 1;
}


local void lo_calculate_handle_count_and_level_from_children(
  wn_mbhandle _handle1
)
{
  lo_calculate_handle_count_from_children(_handle1);
  lo_calculate_handle_level_from_children(_handle1);
}


local void lo_get_min_handle
(
  wn_mbhandle *phandle,
  register wn_mbhandle handle_tree
)
{
  register wn_mbhandle next;

  next = handle_tree;

  while(next != NULL)
  {
    handle_tree = next;
    next = handle_tree->left_child;
  }

  *phandle = handle_tree;
}


local void lo_get_max_handle
(
  wn_mbhandle *phandle,
  register wn_mbhandle handle_tree
)
{
  register wn_mbhandle next;

  next = handle_tree;

  while(next != NULL)
  {
    handle_tree = next;
    next = handle_tree->right_child;
  }
    
  *phandle = handle_tree;
}


local void lo_get_lt_handle
(
  wn_mbhandle *phandle,
  wn_mbhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_mbhandle best_so_far;
  int comparison;

  best_so_far = (wn_mbhandle)NULL;

  while(handle_tree != NULL)
  {
    comparison = (*pcompare_keys_func)(key,handle_tree->key);

    if((long unsigned) key <= (long unsigned) handle_tree->key)
    {
      handle_tree = handle_tree->left_child;
    }
    else /* if(comparison > 0) */
    {
      best_so_far = handle_tree;
      handle_tree = handle_tree->right_child;
    }
  }

  *phandle = best_so_far;
}


local void lo_get_gt_handle
(
  wn_mbhandle *phandle,
  wn_mbhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_mbhandle best_so_far;
  int comparison;

  best_so_far = (wn_mbhandle)NULL;

  while(handle_tree != NULL)
  {
    comparison = (*pcompare_keys_func)(key,handle_tree->key);

    if(comparison >= 0)
    {
      handle_tree = handle_tree->right_child;
    }
    else /* if(comparison < 0) */
    {
      best_so_far = handle_tree;
      handle_tree = handle_tree->left_child;
    }
  }

  *phandle = best_so_far;
}


local void lo_get_le_handle
(
  wn_mbhandle *phandle,
  wn_mbhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_mbhandle best_so_far;
  int comparison;

  best_so_far = (wn_mbhandle)NULL;

  while(handle_tree != NULL)
  {
    comparison = (*pcompare_keys_func)(key,handle_tree->key);

    if(comparison < 0)
    {
      handle_tree = handle_tree->left_child;
    }
    else if(comparison > 0) 
    {
      best_so_far = handle_tree;
      handle_tree = handle_tree->right_child;
    }
    else /* == */
    {
      best_so_far = handle_tree;

      break;
    }
  }

  *phandle = best_so_far;
}


local void lo_get_ge_handle
(
  wn_mbhandle *phandle,
  wn_mbhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_mbhandle best_so_far;
  int comparison;

  best_so_far = (wn_mbhandle)NULL;

  while(handle_tree != NULL)
  {
    comparison = (*pcompare_keys_func)(key,handle_tree->key);

    if(comparison > 0)
    {
      handle_tree = handle_tree->right_child;
    }
    else if(comparison < 0) 
    {
      best_so_far = handle_tree;
      handle_tree = handle_tree->left_child;
    }
    else /* == */
    {
      best_so_far = handle_tree;

      break;
    }
  }

  *phandle = best_so_far;
}


local void lo_get_equal_handle
(
  wn_mbhandle *phandle,
  wn_mbhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  int comparison;

  while(handle_tree != NULL)
  {
    comparison = (*pcompare_keys_func)(key,handle_tree->key);

    if(comparison < 0)
    {
      handle_tree = handle_tree->left_child;
    }
    else if(comparison > 0)
    {
      handle_tree = handle_tree->right_child;
    }
    else /* == */
    {
      break;
    }
  }

  *phandle = handle_tree;
}


void wn_mbget(wn_mbhandle *phandle,wn_mbtree tree,ptr key,int compare)
{
  switch(compare)
  {
    case(WN_MB_MIN):
    {
      lo_get_min_handle(phandle,tree->handle_tree);
    } break;
    case(WN_MB_MAX):
    {
      lo_get_max_handle(phandle,tree->handle_tree);
    } break;
    case(WN_MB_LT):
    {
      lo_get_lt_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_MB_GT):
    {
      lo_get_gt_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_MB_LE):
    {
      lo_get_le_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_MB_GE):
    {
      lo_get_ge_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_MB_EQ):
    {
      lo_get_equal_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,
      /**/								key);
    } break;
    default:
      wn_assert_notreached();
  }
}


local void lo_initialize_handle_for_insert
(
  wn_mbhandle handle,
  wn_mbtree tree,
  ptr key
)
{
  handle->free_block_list = NULL;
  handle->key = key;

  handle->level = 1;
  handle->count = 1;

  handle->left_child = handle->right_child = handle->parent =
  /**/						(wn_mbhandle)NULL;
}


local void lo_insert_handle_into_tree
(
  wn_mbhandle handle,
  wn_mbhandle *phandle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  wn_mbhandle parent;
  ptr key;
  int comparison; 

  parent = (wn_mbhandle)NULL;
  key = handle->key;

  for(;;)
  {
    if(*phandle_tree == NULL)
    {
      break;
    }

    parent = *phandle_tree;

    comparison = (*pcompare_keys_func)(key,parent->key);
    if(comparison > 0)
    {
      phandle_tree = &(parent->right_child);
    }
    else if (comparison < 0)
    {
      phandle_tree = &(parent->left_child);
    }
    else /* comparison == 0, shouldn't happen -- bc 5/1/03 */
    {
      wn_assert_notreached();
    }
  }

  handle->parent = parent;
  *phandle_tree = handle;
}


/* replace n1 with loop_handle in the parent */

local void lo_update_parent(
  wn_mbhandle loop_handle,
  wn_mbhandle n1,
  wn_mbhandle parent,
  wn_mbtree tree
)
{
  if(parent == NULL)
  {
    tree->handle_tree = loop_handle;
  }
  else /* parent != NULL */
  {
    if(parent->left_child == n1)
    {
      parent->left_child = loop_handle;
    }
    else  /* parent->right_child == n1 */
    {
      parent->right_child = loop_handle;
    }
  }
}


/************************************************

  wcn: the ugliest part of the code.  If bugs, check
       here first.

  Complicated transforms to rotate the tree to rectify an imbalance of +-2.

************************************************/
void lo_update_level_and_balance(
  wn_mbhandle *ploop_handle,
  wn_mbtree tree
)
{
  int diff;
  wn_mbhandle handle, left_child, right_child, parent, n4, n5, n6, n7;

  handle = *ploop_handle;
  left_child = handle->left_child;
  right_child = handle->right_child;

  diff = lo_handle_tree_level(left_child)-lo_handle_tree_level(right_child);

  switch(diff)
  {
    default:    /* wn_abs(diff) > 2 */
      wn_assert_notreached();  /* should never happen */

    case(-1):   /* not unbalanced enough to remedy */
    case(0):
    case(1):
    {
      lo_calculate_handle_count_and_level_from_children(handle);
    } break;
    case(-2):
    {
      n4 = right_child->right_child;
      n5 = right_child->left_child;
      parent = handle->parent;

      if(lo_handle_tree_level(n4) >= lo_handle_tree_level(n5))
      {
        right_child->left_child = handle;
        handle->parent = right_child;

        *ploop_handle = right_child;
        right_child->parent = parent;

        handle->left_child = left_child;
        if(left_child != NULL)
        {
          left_child->parent = handle;
        }

        right_child->right_child = n4;
        n4->parent = right_child;

        handle->right_child = n5;
        if(n5 != NULL)
        {
          n5->parent = handle;
        }

        lo_calculate_handle_count_and_level_from_children(handle);
        lo_calculate_handle_count_and_level_from_children(right_child);
      }
      else /* lo_handle_tree_level(n4) < lo_handle_tree_level(n5) */
      {
        n6 = n5->right_child;
        n7 = n5->left_child;

        n5->left_child = handle;
        handle->parent = n5;

        n5->right_child = right_child;
        right_child->parent = n5;

        handle->left_child = left_child;
        if(left_child != NULL)
        {
          left_child->parent = handle;
        }

        right_child->right_child = n4;
        if(n4 != NULL)
        {
          n4->parent = right_child;
        }

        *ploop_handle = n5;
        n5->parent = parent;

        right_child->left_child = n6;
        if(n6 != NULL)
        {
          n6->parent = right_child;
        }

        handle->right_child = n7;
        if(n7 != NULL)
        {
          n7->parent = handle;
        }

        lo_calculate_handle_count_and_level_from_children(right_child);
        lo_calculate_handle_count_and_level_from_children(handle);
        lo_calculate_handle_count_and_level_from_children(n5);
      }

      lo_update_parent(*ploop_handle,handle,parent,tree);
    } break;
    case(2):
    {
      n4 = left_child->left_child;
      n5 = left_child->right_child;
      parent = handle->parent;

      if(lo_handle_tree_level(n4) >= lo_handle_tree_level(n5))
      {
        left_child->right_child = handle;
        handle->parent = left_child; 

        *ploop_handle = left_child;
        left_child->parent = parent;

        handle->right_child = right_child;
        if(right_child != NULL)
        {
          right_child->parent = handle; 
        }

        left_child->left_child = n4;
        n4->parent = left_child;  

        handle->left_child = n5;
        if(n5 != NULL)
        {
          n5->parent = handle;
        }

        lo_calculate_handle_count_and_level_from_children(handle);
        lo_calculate_handle_count_and_level_from_children(left_child);
      }
      else /* lo_handle_tree_level(n4) < lo_handle_tree_level(n5) */
      {
        n6 = n5->left_child;
        n7 = n5->right_child;

        n5->right_child = handle;
        handle->parent = n5;

        n5->left_child = left_child;
        left_child->parent = n5;

        handle->right_child = right_child;
        if(right_child != NULL)
        {
          right_child->parent = handle;
        }

        left_child->left_child = n4;
        if(n4 != NULL)
        {
          n4->parent = left_child; 
        }

        *ploop_handle = n5;
        n5->parent = parent;

        left_child->right_child = n6;
        if(n6 != NULL)
        {
          n6->parent = left_child;  
        }

        handle->left_child = n7;
        if(n7 != NULL)
        {
          n7->parent = handle; 
        }

        lo_calculate_handle_count_and_level_from_children(left_child);
        lo_calculate_handle_count_and_level_from_children(handle);
        lo_calculate_handle_count_and_level_from_children(n5);
      }

      lo_update_parent(*ploop_handle,handle,parent,tree);
    } break;
  }
}


/*   This routine appears to assume that loop_handle->count is OK, and that
** will propagate up thru the parents in the first loop.  After that, we
** expect to change by a fixed count, "count_change", applied to each level
** after that.  (Would lo_calculate_handle_count_from_children at each level
** accomplish the same thing (albiet slower)?).  Note this routine is used
** both when inserting and deleting a node. */

local void lo_update_tree_from_handle
(
  wn_mbhandle loop_handle,
  wn_mbtree tree,
  int count_change
)
{
  int old_level;

  for(;loop_handle != NULL;loop_handle=loop_handle->parent)
  {
    old_level = loop_handle->level;

    lo_update_level_and_balance(&loop_handle,tree);

    if(loop_handle->level == old_level)
    {
      loop_handle = loop_handle->parent;

      break;
    }
  }

  for(;loop_handle != NULL;loop_handle=loop_handle->parent)
                                       /* update count only */
  {
    loop_handle->count += count_change;
  }
}


/* called twice: once in in bins, once in bmove */

local void lo_insert_handle_into_btree(wn_mbhandle handle,wn_mbtree tree,ptr key)
{
  lo_initialize_handle_for_insert(handle,tree,key);
  lo_insert_handle_into_tree(handle, &(tree->handle_tree),
  /**/						tree->pcompare_keys_func);
  lo_update_tree_from_handle(handle->parent,tree,1);
}


void wn_mbins(wn_mbhandle handle, wn_mbtree tree, ptr key)
{
  wn_memzero(handle, sizeof(struct wn_mbhandle_struct));

  lo_insert_handle_into_btree(handle,tree,key);
}


/*     given a handle, return a ptr to the ptr from the handle's parent to
**  that handle */

local void lo_find_handle_pointer
(
  wn_mbhandle **pphandle,
  wn_mbtree tree,
  wn_mbhandle handle
)
{
  wn_mbhandle parent;

  parent = handle->parent;

  if(parent != NULL)
  {
    if(parent->left_child == handle)
    {
      *pphandle = &(parent->left_child);
    }
    else /* parent->right_child == handle */
    {
      *pphandle = &(parent->right_child);
    }
  }
  else /* parent == NULL */
  {
    *pphandle = &(tree->handle_tree);
  }

  wn_assert(**pphandle == handle);  /* cannot find handle in tree */
}


/*     Given a handle ("handle" here), find a child one to the left
** and then as far as possible to the right after that, and return a ptr to
** the ptr from its parent to it.  Note that the child will be the node "one
** to the left" of handle. */

local void lo_find_middle_handle_pointer
(
  wn_mbhandle **ppmiddle_handle,
  wn_mbhandle handle
)
{
  register wn_mbhandle *phandle;

  for(phandle = &(handle->left_child);
      (*phandle)->right_child != NULL;
      phandle = &((*phandle)->right_child))
    ;

  *ppmiddle_handle = phandle;
} /* lo_find_middle_handle_pointer */


local void lo_move_up_middle_handle
(
  wn_mbhandle *pupdate_from_handle,
  wn_mbhandle *phandle
)
{
  wn_mbhandle *pmiddle_handle, middle_handle, left_child;

  wn_assert(phandle[0]->left_child);
  wn_assert(phandle[0]->right_child);

  lo_find_middle_handle_pointer(&pmiddle_handle, *phandle);
  middle_handle = *pmiddle_handle;

  wn_assert(!middle_handle->right_child);

  /*   middle_handle can be promoted up to *phandle (at the end of this routine).
  ** 1st we must point middle_handle's parent's right child ptr
  ** (== *pmiddle_handle) to middle_handle's left child. */

  left_child = middle_handle->left_child;
  *pmiddle_handle = left_child;
  if(left_child != NULL)
  {
    left_child->parent = middle_handle->parent;
  }

  /* set up pupdate_from_handle.  Special case if middle_handle is direct
  ** descendant of *phandle */

  if(middle_handle->parent == *phandle)
  {
    *pupdate_from_handle = middle_handle;
  }
  else
  {
    *pupdate_from_handle = middle_handle->parent;
  }

  /* now link in new left and right children of middle handle */

  if((*phandle)->left_child != middle_handle)
  {
    middle_handle->left_child = (*phandle)->left_child;
  }
  if(middle_handle->left_child != NULL)
  {
    middle_handle->left_child->parent = middle_handle;
  }

  middle_handle->right_child = (*phandle)->right_child;
  if(middle_handle->right_child != NULL)
  {
    middle_handle->right_child->parent = middle_handle;
  }

  middle_handle->count = (*phandle)->count;
  middle_handle->level = (*phandle)->level;

  /* finally, finish promoting middle_handle */

  middle_handle->parent = (*phandle)->parent;
  *phandle = middle_handle;
}


/* assume it is not this routine's job to free the memory of the handle, the
** caller will keep a ptr to it to free it.  Get the handle out of the tree,
** and point *pupdate_from_handle to its new (== old) parent. */

local void lo_delete_handle_from_tree
(
  wn_mbhandle *pupdate_from_handle,
  wn_mbhandle *phandle
)
{
  wn_mbhandle parent;

  parent = (*phandle)->parent;

  if((*phandle)->left_child == NULL)
  {
    *phandle = (*phandle)->right_child;
    *pupdate_from_handle = parent;
  }
  else if((*phandle)->right_child == NULL)
  {
    *phandle = (*phandle)->left_child;
    *pupdate_from_handle = parent;
  }
  else  /* both not null */
  {
    lo_move_up_middle_handle(pupdate_from_handle,phandle);
  }

  if(*phandle != NULL)
  {
    (*phandle)->parent = parent;
  }
}


local void lo_delete_handle_from_btree(wn_mbhandle handle,wn_mbtree tree)
{
  wn_mbhandle update_from_handle,*phandle;

  lo_find_handle_pointer(&phandle,tree,handle); 
  lo_delete_handle_from_tree(&update_from_handle,phandle);
  lo_update_tree_from_handle(update_from_handle,tree,-1);
}


void wn_mbdel(wn_mbhandle handle, wn_mbtree tree)
{
  lo_delete_handle_from_btree(handle, tree);
}


int wn_mbcount(wn_mbtree tree)
{
  wn_mbhandle handle_tree;

  handle_tree = tree->handle_tree;

  return(lo_handle_tree_count(handle_tree));
}


local bool lo_range_is_reasonable
(
  ptr low_key,
  ptr high_key,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  int comparison;

  comparison = (*pcompare_keys_func)(low_key,high_key);

  return(comparison <= 0);
}


local int lo_compare_with_range
(
  ptr key,
  ptr low_key,
  ptr high_key,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  if((*pcompare_keys_func)(key,low_key) < 0)
  {
    return(-1);
  }
  else if((*pcompare_keys_func)(key,high_key) > 0)
  {
    return(1);
  }
  else
  {
    return(0);
  }
}


local void lo_act_on_handle_tree
(
  wn_mbhandle handle_tree,
  void (*paction)(wn_mbhandle handle),
  ptr low_key,
  ptr high_key,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  int range_comparison;

  if(handle_tree == NULL)
  {
    return;
  }

  range_comparison = lo_compare_with_range(handle_tree->key, low_key, high_key,
  /**/							pcompare_keys_func);

  if(range_comparison >= 0)
  {
    lo_act_on_handle_tree(handle_tree->left_child,paction,
    /**/				low_key, high_key, pcompare_keys_func);
  }

  if(range_comparison == 0)
  {
    (*paction)(handle_tree);
  }

  if(range_comparison <= 0)
  {
    lo_act_on_handle_tree(handle_tree->right_child,paction,
    /**/				low_key, high_key, pcompare_keys_func);
  }
}


void wn_mbact
(
  wn_mbtree tree,
  void (*paction)(wn_mbhandle handle),
  ptr low_key,
  int low_compare,
  ptr high_key,
  int high_compare
)
{
  wn_mbhandle low_handle, high_handle;

  wn_mbget(&low_handle,  tree, low_key,  low_compare);
  wn_mbget(&high_handle, tree, high_key, high_compare);

  if(
      (low_handle == NULL)||(high_handle == NULL)
        ||
      !(lo_range_is_reasonable(low_handle->key,high_handle->key,
                                tree->pcompare_keys_func))
    )
  {
    return;  /* do nothing */
  }

  lo_act_on_handle_tree(tree->handle_tree,paction,
                     low_handle->key,high_handle->key,
                     tree->pcompare_keys_func);
} /* wn_mbact */


local void lo_verify_this_handle_tree
(
  wn_mbhandle handle_tree,
  wn_mbhandle parent,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  int left_count,right_count,left_level,right_level;

  wn_assert(handle_tree->parent == parent);
  wn_assert(handle_tree->level >= 1);
  wn_assert(handle_tree->count >= 1);

  left_count = lo_handle_tree_count(handle_tree->left_child);
  right_count = lo_handle_tree_count(handle_tree->right_child);
  left_level = lo_handle_tree_level(handle_tree->left_child);
  right_level = lo_handle_tree_level(handle_tree->right_child);

  wn_assert(handle_tree->count == left_count+right_count+1);
  wn_assert(handle_tree->level == wn_max(left_level,right_level)+1);
  wn_assert(wn_abs(left_level-right_level) <= 2);

  if(handle_tree->left_child != NULL)
  {
    wn_assert(
                    (*pcompare_keys_func)(handle_tree->left_child->key,
                                          handle_tree->key)
                      <= 0
             );
    wn_assert(handle_tree->left_child->parent == handle_tree);
  }
  if(handle_tree->right_child != NULL)
  {
    wn_assert(
                    (*pcompare_keys_func)(handle_tree->key,
                                          handle_tree->right_child->key)
                      <= 0
             );
    wn_assert(handle_tree->right_child->parent == handle_tree);
  }
} /* lo_verify_this_handle_tree */


local void lo_verify_handle_tree
(
  wn_mbhandle handle_tree,
  wn_mbhandle parent,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  if(handle_tree == NULL)
  {
    return;
  }

  lo_verify_this_handle_tree(handle_tree,parent,pcompare_keys_func);

  lo_verify_handle_tree(handle_tree->left_child,handle_tree,pcompare_keys_func);
  lo_verify_handle_tree(handle_tree->right_child,handle_tree,pcompare_keys_func);
} /* lo_verify_handle_tree */


void wn_mbverify(wn_mbtree tree)
{
  wn_assert(tree->pcompare_keys_func != (int (*)(ptr,ptr))NULL);

  lo_verify_handle_tree(tree->handle_tree,(wn_mbhandle)NULL,
  /**/						tree->pcompare_keys_func);
}
