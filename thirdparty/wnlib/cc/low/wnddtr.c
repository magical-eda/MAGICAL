/**************************************************************************

wn_ddtree tree;
wn_ddhandle handle;

handle->key
handle->contents

wn_mkddtree(&tree,num_dimensions,
	    compare_keys_func,alloc_copy_key_func,free_key_func)
wn_freeddtree(tree)

wn_ddins(&handle,tree,key)
wn_dddel(handle,tree)
wn_ddmove(handle,tree,new_key);

wn_ddact(tree,paction,low_key,low_compare,high_key,high_compare) 
        void (*paction)(handle);

int wn_ddcount(tree)

wn_ddverify(tree)

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
#include "wnary.h"
#include "wnddtr.h"



wn_mkddtree(ptree,num_dimensions,
	    compare_keys_func,alloc_copy_key_func,free_key_func)

wn_ddtree *ptree;
int num_dimensions;
int (*(compare_keys_func[]))(/*key1,key2*/);
void (*(alloc_copy_key_func[]))(/*&key,key*/);
void (*(free_key_func[]))(/*key*/);

{
  *ptree = wn_znew(wn_ddtree);

  (*ptree)->num_dimensions = num_dimensions;

  wn_blkacpy((ptr *)&((*ptree)->compare_keys_func),
	     (ptr)compare_keys_func,
	     num_dimensions*wn_sizeof(int (*(*))()));
  wn_blkacpy((ptr *)&((*ptree)->alloc_copy_key_func),
	     (ptr)alloc_copy_key_func,
	     num_dimensions*wn_sizeof(void (*(*))()));
  wn_blkacpy((ptr *)&((*ptree)->free_key_func),
	     (ptr)free_key_func,
	     num_dimensions*wn_sizeof(void (*(*))()));

  (*ptree)->group = wn_curgp();
}



wn_freeddtree(tree)

wn_ddtree tree;

{
  wn_gppush(tree->group);

  free_handle_tree(tree->handle_tree,tree->num_dimensions,tree->free_key_func);

  wn_free((ptr)(tree->compare_keys_func));
  wn_free((ptr)(tree->alloc_copy_key_func));
  wn_free((ptr)(tree->free_key_func));

  wn_free((ptr)tree);

  wn_gppop();
}



local free_handle_tree(handle_tree,num_dimensions,free_key_func)

wn_ddhandle handle_tree;
int num_dimensions;
void (*(free_key_func[]))(/*key*/);

{
  if(handle_tree != NULL)
  {
    int i;
    ptr *key;

    free_handle_tree(handle_tree->left_child,num_dimensions,free_key_func);
    free_handle_tree(handle_tree->right_child,num_dimensions,free_key_func);

    key = handle_tree->key;

    for(i=0;i<num_dimensions;++i)
    {
      (*(free_key_func[i]))(key[i]);
    }

    wn_free((ptr)key);

    wn_free((ptr)handle_tree);
  }
}



wn_ddins(phandle,tree,key)

wn_ddhandle *phandle;
wn_ddtree tree;
ptr key[];

{
  wn_gppush(tree->group);

  *phandle = wn_znew(wn_ddhandle);
  insert_handle_into_ddtree(*phandle,tree,key);

  wn_gppop();
}



local insert_handle_into_ddtree(handle,tree,key)

wn_ddhandle handle;
wn_ddtree tree;
ptr key[];

{
  initialize_handle_for_insert(handle,tree,key);
  insert_handle_into_tree(handle,&(tree->handle_tree),
			  tree->compare_keys_func,tree->num_dimensions);
}



local initialize_handle_for_insert(handle,tree,key)

wn_ddhandle handle;
wn_ddtree tree;
ptr key[];

{
  int i,num_dimensions;
  ptr *handle_key;

  num_dimensions = tree->num_dimensions;

  handle_key = (ptr *)wn_zalloc(num_dimensions*wn_sizeof(ptr));

  for(i=0;i<num_dimensions;++i)
  {
    (*((tree->alloc_copy_key_func)[i]))(&(handle_key[i]),key[i]);
  }

  handle->key = handle_key;
}



local insert_handle_into_tree(handle,phandle_tree,
			      compare_keys_func,num_dimensions)

wn_ddhandle handle,*phandle_tree;
int (*(compare_keys_func[]))(/* key1,key2 */);
int num_dimensions;

{
  wn_ddhandle parent;
  ptr *key;
  int comparison,split_dimension; 

  parent = NULL;
  split_dimension = 0;
  key = handle->key;

  while(TRUE)
  {
    if(*phandle_tree == NULL)
    {
      break;
    }

    parent = *phandle_tree;

    split_dimension = parent->split_dimension;

    comparison = (*(compare_keys_func[split_dimension]))
		       (key[split_dimension],(parent->key)[split_dimension]);
    if(comparison > 0)
    {
      phandle_tree = &(parent->right_child);
    }
    else /* comparsion <= 0 */
    {
      phandle_tree = &(parent->left_child);
    }
  }

  ++split_dimension;
  if(split_dimension >= num_dimensions)
  {
    split_dimension = 0;
  }

  handle->parent = parent;
  handle->split_dimension = split_dimension;
  *phandle_tree = handle;
}



#if 0
wn_dddel(handle,tree)

wn_ddhandle handle;
wn_ddtree tree;

{
  wn_crashmsg("wn_dddel: not yet implemented.");
}



wn_ddmove(handle,tree,new_key)

wn_ddhandle handle;
wn_ddtree tree;
ptr new_key[];

{
  wn_crashmsg("wn_ddmove: not yet implemented.");
}
#endif



static int count;

int wn_ddcount(tree)

wn_ddtree tree;

{
  void count_dd_entry();
  int num_dimensions;
  ptr *key;
  int *compare;

  wn_gpmake("no_free");

  count = 0;

  num_dimensions = tree->num_dimensions;
    
  key = (ptr *)wn_zalloc(num_dimensions*wn_sizeof(ptr));
  compare = (int *)wn_zalloc(num_dimensions*wn_sizeof(int));

  wn_arrayfill(key,num_dimensions,NULL,ptr);
  wn_arrayfill(compare,num_dimensions,WN_DD_UNBOUNDED,int);

  wn_ddact(tree,(count_dd_entry),key,compare,key,compare);

  wn_gpfree();

  return(count);
}



local /*ARGSUSED*/ void count_dd_entry(handle,key)

wn_ddhandle handle;
ptr key[];

{
  ++count;
}



wn_ddact(tree,paction,low_key,low_compare,high_key,high_compare) 

wn_ddtree tree;
void (*paction)(/* handle */);
ptr low_key[],high_key[];
int low_compare[],high_compare[];

{
  act_on_handle_tree(tree->handle_tree,paction,
		     low_key,low_compare,high_key,high_compare,
                     tree->compare_keys_func,
		     tree->num_dimensions);
}



local act_on_handle_tree(handle_tree,paction,
                         low_key,low_compare,high_key,high_compare,
                         compare_keys_func,
			 num_dimensions)

wn_ddhandle handle_tree;
void (*paction)(/* handle */);
ptr low_key[],high_key[];
int low_compare[],high_compare[];
int (*(compare_keys_func[]))(/* key1,key2 */);
int num_dimensions;

{
  int split_dimension,range_comparison;

  if(handle_tree == NULL)
  {
    return;
  }

  split_dimension = handle_tree->split_dimension;

  range_comparison = compare_with_range((handle_tree->key)[split_dimension],
					low_key[split_dimension],
					low_compare[split_dimension],
					high_key[split_dimension],
					high_compare[split_dimension],
                                        compare_keys_func[split_dimension]);

  if(range_comparison >= 0)
  {
    act_on_handle_tree(handle_tree->left_child,paction,
                       low_key,low_compare,high_key,high_compare,
                       compare_keys_func,num_dimensions);
  }

  if(range_comparison == 0)
  {
    if(key_is_in_rectangle(handle_tree->key,
			   low_key,low_compare,high_key,high_compare,
			   compare_keys_func,num_dimensions))
    {
      (*paction)(handle_tree);
    }
  }

  if(range_comparison <= 0)
  {
    act_on_handle_tree(handle_tree->right_child,paction,
                       low_key,low_compare,high_key,high_compare,
                       compare_keys_func,num_dimensions);
  }
}



local bool key_is_in_rectangle(key,
			       low_key,low_compare,high_key,high_compare,
			       compare_keys_func,
			       num_dimensions)

ptr key[],low_key[],high_key[];
int low_compare[],high_compare[];
int (*(compare_keys_func[]))(/* key1,key2 */);
int num_dimensions;

{
  int dimension,range_comparison;

  for(dimension=0;dimension<num_dimensions;++dimension)
  {
    range_comparison = compare_with_range(key[dimension],
					  low_key[dimension],
					  low_compare[dimension],
					  high_key[dimension],
					  high_compare[dimension],
                                          compare_keys_func[dimension]);

    if(range_comparison != 0)
    {
      return(FALSE);
    }
  }

  return(TRUE);
}



local int compare_with_range(key,
			     low_key,low_compare,high_key,high_compare,
			     pcompare_keys_func)

ptr key,low_key,high_key;
int low_compare,high_compare;
int (*pcompare_keys_func)(/* key1,key2 */);

{
  switch(low_compare)
  {
    case(WN_DD_UNBOUNDED):
      break;
    case(WN_DD_INCLUSIVE):
      if((*pcompare_keys_func)(key,low_key) < 0)
      {
	return(-1);
      }
      break;
    case(WN_DD_EXCLUSIVE):
      if((*pcompare_keys_func)(key,low_key) <= 0)
      {
	return(-1);
      }
      break;
    default:
      wn_crash();
  }

  switch(high_compare)
  {
    case(WN_DD_UNBOUNDED):
      break;
    case(WN_DD_INCLUSIVE):
      if((*pcompare_keys_func)(key,high_key) > 0)
      {
	return(1);
      }
      break;
    case(WN_DD_EXCLUSIVE):
      if((*pcompare_keys_func)(key,high_key) >= 0)
      {
	return(1);
      }
      break;
    default:
      wn_crash();
  }

  return(0);
}



#if 0
wn_ddverify(tree)

wn_ddtree tree;

{
}
#endif


