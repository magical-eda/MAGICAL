/**************************************************************************

wn_btree tree;
wn_bhandle handle;

handle->key
handle->contents

wn_mkbtree(&tree,pcompare_keys_func,palloc_copy_key_func,pfree_key_func)
wn_freebtree(tree)

wn_bget(&handle,tree,key,compare)
wn_bins(&handle,tree,key)
wn_bdel(handle,tree)
wn_bmove(handle,tree,new_key);

wn_bget_index_of_handle(&index,tree,handle)
wn_bget_handle_of_index(&handle,tree,index)

wn_bact(tree,paction,low_key,low_compare,high_key,high_compare) 
        void (*paction)(handle);

int wn_bcount(tree)

wn_bverify(tree)

** tree traversal routines **
wn_bhandle _wn_for_btree_get_first(wn_btree   tree);
void       _wn_for_btree_get_next( wn_bhandle *phandle);
wn_bhandle _wn_for_btree_get_last( wn_btree   tree);
void       _wn_for_btree_get_prev( wn_bhandle *phandle);


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

  Will Naylor

****************************************************************************/
#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnmax.h"
#include "wnabs.h"

#include "wnbtr.h"



EXTERN void wn_mkbtree
(
  wn_btree *ptree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  void (*palloc_copy_key_func)(ptr *pkey,ptr key),
  void (*pfree_key_func)(ptr key)
)
{
  *ptree = (wn_btree)wn_zalloc(sizeof(struct wn_btree_struct));

  (*ptree)->pcompare_keys_func = pcompare_keys_func;
  (*ptree)->palloc_copy_key_func = palloc_copy_key_func;
  (*ptree)->pfree_key_func = pfree_key_func;

  (*ptree)->group = wn_curgp();
}


local void free_handle_tree
(
  wn_bhandle handle_tree,
  void (*pfree_key_func)(ptr key)
)
{
  if(handle_tree != NULL)
  {
    free_handle_tree(handle_tree->left_child,pfree_key_func);
    free_handle_tree(handle_tree->right_child,pfree_key_func);

    (*pfree_key_func)(handle_tree->key);
    wn_free(handle_tree);
  }
}


void wn_freebtree(wn_btree tree)
{
  wn_gppush(tree->group);

  free_handle_tree(tree->handle_tree,tree->pfree_key_func);
  wn_free(tree);

  wn_gppop();
}


#define handle_tree_count(_handle_tree2)\
( ((_handle_tree2) == NULL) ? 0 : ((_handle_tree2)->count) )


#define handle_tree_level(_handle_tree1)\
( ((_handle_tree1) == NULL) ? 0 : ((_handle_tree1)->level) )


#define calculate_handle_count_from_children(_handle3) \
{\
  int left_count,right_count;\
  wn_bhandle _left_child,_right_child;\
  \
  _left_child = (_handle3)->left_child;\
  _right_child = (_handle3)->right_child;\
  \
  left_count = handle_tree_count(_left_child);\
  right_count = handle_tree_count(_right_child);\
  \
  (_handle3)->count = left_count + right_count + 1;\
}


#define calculate_handle_level_from_children(_handle2)\
{\
  int _left_level,_right_level;\
  wn_bhandle _left_child,_right_child;\
  \
  _left_child = (_handle2)->left_child;\
  _right_child = (_handle2)->right_child;\
  \
  _left_level = handle_tree_level(_left_child);\
  _right_level = handle_tree_level(_right_child);\
  \
  (_handle2)->level = wn_max(_left_level,_right_level) + 1;\
}


#define calculate_handle_count_and_level_from_children(_handle1) \
{\
  calculate_handle_count_from_children(_handle1);\
  calculate_handle_level_from_children(_handle1);\
}


local void get_min_handle
(
  wn_bhandle *phandle,
  register wn_bhandle handle_tree
)
{
  register wn_bhandle next;

  next = handle_tree;

  while(next != NULL)
  {
    handle_tree = next;
    next = handle_tree->left_child;
  }
    
  *phandle = handle_tree;
}


local void get_max_handle
(
  wn_bhandle *phandle,
  register wn_bhandle handle_tree
)
{
  register wn_bhandle next;

  next = handle_tree;

  while(next != NULL)
  {
    handle_tree = next;
    next = handle_tree->right_child;
  }
    
  *phandle = handle_tree;
}


local void get_lt_handle
(
  wn_bhandle *phandle,
  wn_bhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_bhandle best_so_far;
  int comparison;

  best_so_far = (wn_bhandle)NULL;

  while(handle_tree != NULL)
  {
    comparison = (*pcompare_keys_func)(key,handle_tree->key);

    if(comparison <= 0)
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


local void get_gt_handle
(
  wn_bhandle *phandle,
  wn_bhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_bhandle best_so_far;
  int comparison;

  best_so_far = (wn_bhandle)NULL;

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


local void get_le_handle
(
  wn_bhandle *phandle,
  wn_bhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_bhandle best_so_far;
  int comparison;

  best_so_far = (wn_bhandle)NULL;

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


local void get_ge_handle
(
  wn_bhandle *phandle,
  wn_bhandle handle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  ptr key
)
{
  wn_bhandle best_so_far;
  int comparison;

  best_so_far = (wn_bhandle)NULL;

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


local void get_equal_handle
(
  wn_bhandle *phandle,
  wn_bhandle handle_tree,
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


void wn_bget(wn_bhandle *phandle,wn_btree tree,ptr key,int compare)
{
  switch(compare)
  {
    case(WN_B_MIN):
    {
      get_min_handle(phandle,tree->handle_tree);
    } break;
    case(WN_B_MAX):
    {
      get_max_handle(phandle,tree->handle_tree);
    } break;
    case(WN_B_LT):
    {
      get_lt_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_B_GT):
    {
      get_gt_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_B_LE):
    {
      get_le_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_B_GE):
    {
      get_ge_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    case(WN_B_EQ):
    {
      get_equal_handle(phandle,tree->handle_tree,tree->pcompare_keys_func,key);
    } break;
    default:
      wn_assert_notreached();
  }
}


local void initialize_handle_for_insert
(
  wn_bhandle handle,
  wn_btree tree,
  ptr key
)
{
  handle->contents = NULL;
  (*(tree->palloc_copy_key_func))(&(handle->key),key);

  handle->level = 1;
  handle->count = 1;

  handle->left_child = handle->right_child = handle->parent = 
	  (wn_bhandle)NULL;
}


local void insert_handle_into_tree
(
  wn_bhandle handle,
  wn_bhandle *phandle_tree,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  wn_bhandle parent;
  ptr key;
  int comparison; 

  parent = (wn_bhandle)NULL;
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
    else /* comparsion <= 0 */
    {
      phandle_tree = &(parent->left_child);
    }
  }

  handle->parent = parent;
  *phandle_tree = handle;
}


#define update_parent(loop_handle,n1,parent,tree)\
{\
  if(parent == NULL)\
  {\
    tree->handle_tree = loop_handle;\
  }\
  else /* parent != NULL */\
  {\
    if(parent->left_child == n1)\
    {\
      parent->left_child = loop_handle;\
    }\
    else  /* parent->right_child == n1 */\
    {\
      parent->right_child = loop_handle;\
    }\
  }\
}


/************************************************

  wcn: the ugliest part of the code.  If bugs, check
       here first.

************************************************/
#define update_level_and_balance(loop_handle,tree) \
{\
  int diff;\
  wn_bhandle handle,left_child,right_child,parent,n4,n5,n6,n7;\
  \
  handle = loop_handle;\
  left_child = handle->left_child;\
  right_child = handle->right_child;\
\
  diff = handle_tree_level(left_child)-handle_tree_level(right_child);\
\
  switch(diff)\
  {\
    default:    /* wn_abs(diff) > 2 */\
      wn_assert_notreached();  /* should never happen */\
\
    case(-1):   /* not unbalanced enough to remedy */\
    case(0):\
    case(1):\
    {\
      calculate_handle_count_and_level_from_children(handle);\
    } break;\
    case(-2):\
    {\
      n4 = right_child->right_child;\
      n5 = right_child->left_child;\
      parent = handle->parent;\
\
      if(handle_tree_level(n4) >= handle_tree_level(n5))\
      {\
        right_child->left_child = handle;\
        handle->parent = right_child; \
\
        loop_handle = right_child;\
        right_child->parent = parent;\
\
        handle->left_child = left_child;\
        if(left_child != NULL)\
        {\
          left_child->parent = handle;\
        }\
\
        right_child->right_child = n4;\
        n4->parent = right_child;\
\
        handle->right_child = n5;\
        if(n5 != NULL)\
        {\
          n5->parent = handle; \
        }\
\
        calculate_handle_count_and_level_from_children(handle);\
        calculate_handle_count_and_level_from_children(right_child);\
      }\
      else /* handle_tree_level(n4) < handle_tree_level(n5) */\
      {\
        n6 = n5->right_child;\
        n7 = n5->left_child;\
\
        n5->left_child = handle;\
        handle->parent = n5; \
\
        n5->right_child = right_child;\
        right_child->parent = n5;\
\
        handle->left_child = left_child;\
        if(left_child != NULL)\
        {\
          left_child->parent = handle;      \
        }\
\
        right_child->right_child = n4;\
        if(n4 != NULL)\
        {\
          n4->parent = right_child;\
        }\
\
        loop_handle = n5;\
        n5->parent = parent;\
\
        right_child->left_child = n6;\
        if(n6 != NULL)\
        {\
          n6->parent = right_child;\
        }\
\
        handle->right_child = n7;\
        if(n7 != NULL)\
        {\
          n7->parent = handle; \
        }\
\
        calculate_handle_count_and_level_from_children(right_child);\
        calculate_handle_count_and_level_from_children(handle);\
        calculate_handle_count_and_level_from_children(n5);\
      }\
\
      update_parent(loop_handle,handle,parent,tree);\
    } break;\
    case(2):\
    {\
      n4 = left_child->left_child;\
      n5 = left_child->right_child;\
      parent = handle->parent;\
\
      if(handle_tree_level(n4) >= handle_tree_level(n5))\
      {\
        left_child->right_child = handle;\
        handle->parent = left_child; \
\
        loop_handle = left_child;\
        left_child->parent = parent;\
\
        handle->right_child = right_child;\
        if(right_child != NULL)\
        {\
          right_child->parent = handle; \
        }\
\
        left_child->left_child = n4;\
        n4->parent = left_child;  \
\
        handle->left_child = n5;\
        if(n5 != NULL)\
        {\
          n5->parent = handle;\
        }\
\
        calculate_handle_count_and_level_from_children(handle);\
        calculate_handle_count_and_level_from_children(left_child);\
      }\
      else /* handle_tree_level(n4) < handle_tree_level(n5) */\
      {\
        n6 = n5->left_child;\
        n7 = n5->right_child;\
\
        n5->right_child = handle;\
        handle->parent = n5;\
\
        n5->left_child = left_child;\
        left_child->parent = n5;\
\
        handle->right_child = right_child;\
        if(right_child != NULL)\
        {\
          right_child->parent = handle;\
        }\
\
        left_child->left_child = n4;\
        if(n4 != NULL)\
        {\
          n4->parent = left_child; \
        }\
\
        loop_handle = n5;\
        n5->parent = parent;\
\
        left_child->right_child = n6;\
        if(n6 != NULL)\
        {\
          n6->parent = left_child;  \
        }\
\
        handle->left_child = n7;\
        if(n7 != NULL)\
        {\
          n7->parent = handle; \
        }\
\
        calculate_handle_count_and_level_from_children(left_child);\
        calculate_handle_count_and_level_from_children(handle);\
        calculate_handle_count_and_level_from_children(n5);\
      }\
\
      update_parent(loop_handle,handle,parent,tree);\
    } break;\
  }\
}


local void update_tree_from_handle
(
  register wn_bhandle loop_handle,
  wn_btree tree,
  int count_change
)
{
  int old_level;

  for(;loop_handle != NULL;loop_handle=loop_handle->parent)
  {
    old_level = loop_handle->level;

    update_level_and_balance(loop_handle,tree);

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


local void insert_handle_into_btree(wn_bhandle handle,wn_btree tree,ptr key)
{
  initialize_handle_for_insert(handle,tree,key);
  insert_handle_into_tree(handle,&(tree->handle_tree),tree->pcompare_keys_func);
  update_tree_from_handle(handle->parent,tree,1);
}


void wn_bins(wn_bhandle *phandle,wn_btree tree,ptr key)
{
  wn_gppush(tree->group);

  *phandle = (wn_bhandle)wn_zalloc(sizeof(struct wn_bhandle_struct));
  insert_handle_into_btree(*phandle,tree,key);

  wn_gppop();
}


local void find_handle_pointer
(
  wn_bhandle **pphandle,
  wn_btree tree,
  wn_bhandle handle
)
{
  wn_bhandle parent;

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


local void find_middle_handle_pointer
(
  wn_bhandle **ppmiddle_handle,
  wn_bhandle handle
)
{
  register wn_bhandle *phandle;

  for(phandle = &(handle->left_child);
      (*phandle)->right_child != NULL;
      phandle = &((*phandle)->right_child))
    ;

  *ppmiddle_handle = phandle;
}


local void move_up_middle_handle
(
  wn_bhandle *pupdate_from_handle,
  wn_bhandle *phandle
)
{
  wn_bhandle *pmiddle_handle,middle_handle,left_child;

  find_middle_handle_pointer(&pmiddle_handle,*phandle);
  middle_handle = *pmiddle_handle;

  left_child = middle_handle->left_child;
  *pmiddle_handle = left_child;
  if(left_child != NULL)
  {
    left_child->parent = middle_handle->parent;
  }
  if(middle_handle->parent == *phandle)
  {
    *pupdate_from_handle = middle_handle;
  }
  else
  {
    *pupdate_from_handle = middle_handle->parent;
  }

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

  middle_handle->parent = (*phandle)->parent;
  *phandle = middle_handle;
}


local void delete_handle_from_tree
(
  wn_bhandle *pupdate_from_handle,
  wn_bhandle *phandle
)
{
  wn_bhandle parent;

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
    move_up_middle_handle(pupdate_from_handle,phandle);
  }

  if(*phandle != NULL)
  {
    (*phandle)->parent = parent;
  }
}


local void delete_handle_from_btree(wn_bhandle handle,wn_btree tree)
{
  wn_bhandle update_from_handle,*phandle;

  find_handle_pointer(&phandle,tree,handle); 
  delete_handle_from_tree(&update_from_handle,phandle);
  update_tree_from_handle(update_from_handle,tree,-1);
}


void wn_bdel(wn_bhandle handle,wn_btree tree)
{
  wn_gppush(tree->group);

  delete_handle_from_btree(handle,tree);

  (*(tree->pfree_key_func))(handle->key);
  wn_free(handle);

  wn_gppop();
}


void wn_bmove(wn_bhandle handle,wn_btree tree,ptr new_key)
{
  ptr contents;

  contents = handle->contents;

  delete_handle_from_btree(handle,tree);

  (*(tree->pfree_key_func))(handle->key);

  insert_handle_into_btree(handle,tree,new_key);

  handle->contents = contents;
}


/*ARGSUSED*/ void wn_bget_index_of_handle
(
  int *pindex,
  wn_btree tree,
  wn_bhandle handle
)
{
  int sum;
  wn_bhandle parent,left_child;

  left_child = handle->left_child;
  sum = handle_tree_count(left_child);

  for(;;)
  {
    parent = handle->parent;

    if(parent == NULL)    /* no parent */
    {
      break;
    }
    if(parent->right_child == handle)  /* parent is less */
    {
      left_child = parent->left_child;
      sum += (handle_tree_count(left_child) + 1);
    }

    handle = parent;  
  }

  *pindex = sum;
}


void wn_bget_handle_of_index(wn_bhandle *phandle,wn_btree tree,int index)
{
  wn_bhandle handle_tree,left_child;
  int parent_sum,left_count,total;

  handle_tree = tree->handle_tree;
  parent_sum = 0;

  while(handle_tree != NULL)
  {
    left_child = handle_tree->left_child;
    left_count = handle_tree_count(left_child);

    total = parent_sum+left_count;

    if(total > index)
    {
      handle_tree = left_child;
    }
    else if(total < index)
    {
      handle_tree = handle_tree->right_child;

      parent_sum += (left_count+1);
    }
    else /* total == index */
    {
      break;
    }
  }

  *phandle = handle_tree;
}


int wn_bcount(wn_btree tree)
{
  wn_bhandle handle_tree;

  handle_tree = tree->handle_tree;

  return(handle_tree_count(handle_tree));
}


local bool range_is_reasonable
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


local int compare_with_range
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


local void act_on_handle_tree
(
  wn_bhandle handle_tree,
  void (*paction)(wn_bhandle handle),
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

  range_comparison = compare_with_range(handle_tree->key,low_key,high_key,
                                        pcompare_keys_func);

  if(range_comparison >= 0)
  {
    act_on_handle_tree(handle_tree->left_child,paction,
                       low_key,high_key,
                       pcompare_keys_func);
  }

  if(range_comparison == 0)
  {
    (*paction)(handle_tree);
  }

  if(range_comparison <= 0)
  {
    act_on_handle_tree(handle_tree->right_child,paction,
                       low_key,high_key,
                       pcompare_keys_func);
  }
}


EXTERN void wn_bact
(
  wn_btree tree,
  void (*paction)(wn_bhandle handle),
  ptr low_key,
  int low_compare,
  ptr high_key,
  int high_compare
)
{
  wn_bhandle low_handle,high_handle;

  wn_bget(&low_handle,tree,low_key,low_compare);
  wn_bget(&high_handle,tree,high_key,high_compare);

  if(
      (low_handle == NULL)||(high_handle == NULL)
        ||
      !(range_is_reasonable(low_handle->key,high_handle->key,
                                tree->pcompare_keys_func))
    )
  {
    return;  /* do nothing */
  }

  act_on_handle_tree(tree->handle_tree,paction,
                     low_handle->key,high_handle->key,
                     tree->pcompare_keys_func);
}



local void verify_this_handle_tree
(
  wn_bhandle handle_tree,
  wn_bhandle parent,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  int left_count,right_count,left_level,right_level;

  wn_assert(handle_tree->parent == parent);
  wn_assert(handle_tree->level >= 1);
  wn_assert(handle_tree->count >= 1);

  left_count = handle_tree_count(handle_tree->left_child);
  right_count = handle_tree_count(handle_tree->right_child);
  left_level = handle_tree_level(handle_tree->left_child);
  right_level = handle_tree_level(handle_tree->right_child);

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
  }
  if(handle_tree->right_child != NULL)
  {
    wn_assert(
                    (*pcompare_keys_func)(handle_tree->key,
                                          handle_tree->right_child->key)
                      <= 0
             );
  }
}


local void verify_handle_tree
(
  wn_bhandle handle_tree,
  wn_bhandle parent,
  int (*pcompare_keys_func)(ptr key1,ptr key2)
)
{
  if(handle_tree == NULL)
  {
    return;
  }

  verify_this_handle_tree(handle_tree,parent,pcompare_keys_func);

  verify_handle_tree(handle_tree->left_child,handle_tree,pcompare_keys_func);
  verify_handle_tree(handle_tree->right_child,handle_tree,pcompare_keys_func);
}


void wn_bverify(wn_btree tree)
{
  wn_assert(tree->pcompare_keys_func != NULL);
  wn_assert(tree->palloc_copy_key_func != NULL);
  wn_assert(tree->pfree_key_func != NULL);
  wn_assert(tree->group != NULL);

  verify_handle_tree(tree->handle_tree,(wn_bhandle)NULL,
                     tree->pcompare_keys_func);
}


/* ---------------------------------------------------------------- */
/* tree traversal, forward */

wn_bhandle _wn_for_btree_get_first(wn_btree   tree) {
  wn_bhandle node;

  node = tree->handle_tree;
  if (!node) return NULL;

  while (node->left_child)
  /**/			   node = node->left_child;

  return node;
} /* _wn_for_btree_get_first */


void _wn_for_btree_get_next(wn_bhandle *phandle) {
  wn_bhandle node = *phandle;

  if (node->right_child) {
    for (node = node->right_child;  node->left_child;  ) {
      node = node->left_child;
    }
    *phandle = node;
    return;
  }

  while (node->parent  &&  node->parent->right_child == node) {
    node = node->parent;
  }
  if (!(node->parent)) {
    *phandle = NULL;
    return;
  }

  wn_assert(node->parent->left_child == node);

  *phandle = node->parent;
  return;
} /* _wn_for_btree_get_next */


/* ---------------------------------------------------------------- */
/* tree traversal, backward */

wn_bhandle _wn_for_btree_get_last(wn_btree   tree) {
  wn_bhandle node;

  node = tree->handle_tree;
  if (!node) return NULL;

  while (node->right_child)
  /**/			   node = node->right_child;

  return node;
} /* _wn_for_btree_get_first */


void _wn_for_btree_get_prev(wn_bhandle *phandle) {
  wn_bhandle node = *phandle;

  if (node->left_child) {
    for (node = node->left_child;  node->right_child;  ) {
      node = node->right_child;
    }
    *phandle = node;
    return;
  }

  while (node->parent  &&  node->parent->left_child == node) {
    node = node->parent;
  }
  if (!(node->parent)) {
    *phandle = NULL;
    return;
  }

  wn_assert(node->parent->right_child == node);

  *phandle = node->parent;
  return;
} /* _wn_for_btree_get_prev */
