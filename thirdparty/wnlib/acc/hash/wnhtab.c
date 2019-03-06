/**************************************************************************

wn_htab table;

wn_mkhtab(&table,phash_func,pkeys_eq_func,palloc_copy_func,pfree_func)
wn_freehtab(table)

bool wn_hget(&data,table,key)
bool wn_hins(data,table,key)
bool wn_hfins(data,table,key)
bool wn_hdel(table,key)

int wn_hcount(table)

wn_hverify(table)
wn_hdump(table)
wn_hanalyze(table)

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
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"

#include "wnhtab.h"



EXTERN void wn_mkhtab
(
  wn_htab *ptable,
  int (*phash_func)(ptr key),
  bool (*pkeys_eq_func)(ptr key1,ptr key2),
  void (*palloc_copy_func)(ptr *pkey,ptr key),
  void (*pfree_func)(ptr key)
)
{
  *ptable = (wn_htab)wn_alloc(sizeof(struct wn_htab_struct));

  (*ptable)->binary_tree = (wn_binary_tree_data)NULL;

  (*ptable)->group = wn_curgp();

  (*ptable)->phash_func = phash_func;
  (*ptable)->pkeys_eq_func = pkeys_eq_func;
  (*ptable)->palloc_copy_func = palloc_copy_func;
  (*ptable)->pfree_func = pfree_func;
}


local void free_binary_tree
(
  wn_binary_tree_data binary_tree,
  void (*pfree_func)(ptr key)
)
{
  if(binary_tree != NULL)
  {
    free_binary_tree((binary_tree->children)[0],pfree_func);
    free_binary_tree((binary_tree->children)[1],pfree_func);

    (*pfree_func)(binary_tree->key);
    wn_free(binary_tree);
  }
}


void wn_freehtab(wn_htab table)
{
  wn_gppush(table->group);

  free_binary_tree(table->binary_tree,table->pfree_func);
  wn_free(table);

  wn_gppop();
}


bool wn_hins(ptr data,wn_htab table,ptr key)
{
  register wn_binary_tree_data tree,*ptree;
  register int hashed_key,shifted_hashed_key;

  ptree = &(table->binary_tree);
  tree = *ptree;

  hashed_key = (*(table->phash_func))(key);

  if(tree == NULL)  /* table is empty */
  {
    wn_gppush(table->group);

    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_data_struct));

    (tree->children)[0] = (tree->children)[1] = (wn_binary_tree_data)NULL;
    tree->hashed_key = hashed_key;
    tree->data = data;
    (*(table->palloc_copy_func))(&(tree->key),key);

    wn_gppop();

    return(TRUE);
  }
  else
  {
    shifted_hashed_key = hashed_key;

    do
    {
      if(hashed_key == tree->hashed_key)
      {
        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */
        {
	  return(FALSE);
        }
        else
        {
          ptree = &((tree->children)[shifted_hashed_key & 1]);

          shifted_hashed_key >>= 1;
        }
      }
      else
      {
        ptree = &((tree->children)[shifted_hashed_key & 1]);

        shifted_hashed_key >>= 1;
      }

      tree = *ptree;
    }
    while(tree != NULL);

    wn_gppush(table->group);

    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_data_struct));

    (tree->children)[0] = (tree->children)[1] = (wn_binary_tree_data)NULL;
    tree->hashed_key = hashed_key;
    tree->data = data;
    (*(table->palloc_copy_func))(&(tree->key),key);

    wn_gppop();

    return(TRUE);
  } 
}


bool wn_hfins(ptr data,wn_htab table,ptr key)
{
  register wn_binary_tree_data tree,*ptree;
  register int hashed_key,shifted_hashed_key;

  ptree = &(table->binary_tree);
  tree = *ptree;

  hashed_key = (*(table->phash_func))(key);

  if(tree == NULL)  /* table is empty */
  {
    wn_gppush(table->group);

    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_data_struct));

    (tree->children)[0] = (tree->children)[1] = (wn_binary_tree_data)NULL;
    tree->hashed_key = hashed_key;
    tree->data = data;
    (*(table->palloc_copy_func))(&(tree->key),key);

    wn_gppop();

    return(TRUE);
  }
  else
  {
    shifted_hashed_key = hashed_key;

    do
    {
      if(hashed_key == tree->hashed_key)
      {
        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */
        {
          tree->data = data;   /* overwrite existing data */

	  return(TRUE);
        }
        else
        {
          ptree = &((tree->children)[shifted_hashed_key & 1]);

          shifted_hashed_key >>= 1;
        }
      }
      else
      {
        ptree = &((tree->children)[shifted_hashed_key & 1]);

        shifted_hashed_key >>= 1;
      }

      tree = *ptree;
    }
    while(tree != NULL);

    wn_gppush(table->group);

    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_data_struct));

    (tree->children)[0] = (tree->children)[1] = (wn_binary_tree_data)NULL;
    tree->hashed_key = hashed_key;
    tree->data = data;
    (*(table->palloc_copy_func))(&(tree->key),key);

    wn_gppop();

    return(TRUE);
  } 
}


#if 0
bool wn_hget(ptr *pdata,wn_htab table,ptr key)
{
  wn_binary_tree_data tree;
  int hashed_key,shifted_hashed_key;
  int masked_bit;

  tree = table->binary_tree;
  
  if(tree == NULL)  /* table is empty */
  {
    goto fail;
  }

  hashed_key = (*(table->phash_func))(key);
  shifted_hashed_key = hashed_key;

  for(;;)
  {
    if(hashed_key != tree->hashed_key)
    {
      masked_bit = (shifted_hashed_key&1);
      shifted_hashed_key >>= 1;

      do  /* the main inner loop */
      {
        tree = (tree->children)[masked_bit];
        masked_bit = (shifted_hashed_key&1);
        shifted_hashed_key >>= 1;

        if(tree == NULL)
	{
	  goto fail;
        }
      }
      while(hashed_key != tree->hashed_key);
    }
 
    if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */
    {
      *pdata = tree->data;
 
      return(TRUE);
    }

    tree = (tree->children)[masked_bit];
  }

fail:
  *pdata = NULL;

  return(FALSE);
}
#endif
#if 0
bool wn_hget(ptr *pdata,wn_htab table,ptr key)
{
  register wn_binary_tree_data tree;
  register int hashed_key,shifted_hashed_key;

  tree = table->binary_tree;

  if(tree == NULL)  /* table is empty */
  {
    *pdata = NULL;

    return(FALSE);
  }
  else
  {
    hashed_key = (*(table->phash_func))(key);
    shifted_hashed_key = hashed_key;

    do
    {
      if(hashed_key == tree->hashed_key)
      {
        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */
        {
          *pdata = tree->data;
 
          return(TRUE);
        }
        else
        {
          tree = (tree->children)[shifted_hashed_key & 1];

          shifted_hashed_key >>= 1;
        }
      }
      else
      {
        tree = (tree->children)[shifted_hashed_key & 1];

        shifted_hashed_key >>= 1;
      }
    }
    while(tree != NULL);

    *pdata = NULL;

    return(FALSE);
  } 
}
#endif
#if 1
bool wn_hget(ptr *pdata,wn_htab table,ptr key)
{
  register wn_binary_tree_data tree;
  register int hashed_key,shifted_hashed_key;

  tree = table->binary_tree;

  if(tree == NULL)  /* table is empty */
  {
    *pdata = NULL;

    return(FALSE);
  }
  else
  {
    hashed_key = (*(table->phash_func))(key);
    shifted_hashed_key = hashed_key;

    do
    {
      if(
	  (hashed_key == tree->hashed_key)
	    &&
          (*(table->pkeys_eq_func))(key,tree->key)  /* if == */
	)
      {
        *pdata = tree->data;
 
        return(TRUE);
      }

      tree = (tree->children)[shifted_hashed_key & 1];

      shifted_hashed_key >>= 1;
    }
    while(tree != NULL);

    *pdata = NULL;

    return(FALSE);
  } 
}
#endif


local void find_leaf_tree_node_pointer
(
  register wn_binary_tree_data **ppleaf_node,
  register wn_binary_tree_data tree_node,
  register int shifted_hashed_key
)
{
  register wn_binary_tree_data *pchild_node,child_node;
  register int index;

  pchild_node = (wn_binary_tree_data *)NULL;

loop:
    *ppleaf_node = pchild_node;

    index = (shifted_hashed_key & 1);
    pchild_node = &((tree_node->children)[index]);
    child_node = *pchild_node;

    shifted_hashed_key >>= 1;

    if(child_node != NULL)
    {
      tree_node = child_node;

      goto loop;
    }

    index ^= 1;
    pchild_node = &((tree_node->children)[index]);
    child_node = *pchild_node;

    if(child_node != NULL)
    {
      tree_node = child_node;

      goto loop;
    }
}


local void remove_tree_node
(
  wn_binary_tree_data *ptree_node,
  int shifted_hashed_key
)
{
  wn_binary_tree_data tree_node;
  wn_binary_tree_data *pleaf_node,leaf_node;

  tree_node = *ptree_node;

  find_leaf_tree_node_pointer(&pleaf_node,tree_node,shifted_hashed_key);

  if(pleaf_node == NULL)
  {
    *ptree_node = (wn_binary_tree_data)NULL;
  }
  else
  {
    leaf_node = *pleaf_node;
    *pleaf_node = (wn_binary_tree_data)NULL;

    (leaf_node->children)[0] = (tree_node->children)[0];
    (leaf_node->children)[1] = (tree_node->children)[1];

    *ptree_node = leaf_node;
  }
}


bool wn_hdel(wn_htab table,ptr key)
{
  register wn_binary_tree_data tree,*ptree;
  register int hashed_key,shifted_hashed_key;

  ptree = &(table->binary_tree);
  tree = *ptree;

  if(tree == NULL)  /* table is empty */
  {
    return(FALSE);
  }
  else
  {
    hashed_key = (*(table->phash_func))(key);
    shifted_hashed_key = hashed_key;

    do
    {
      if(hashed_key == tree->hashed_key)
      {
        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */
        {
          wn_gppush(table->group);  /* only push group if found */

          (*(table->pfree_func))(tree->key);
          remove_tree_node(ptree,shifted_hashed_key); 
          wn_free(tree);

          wn_gppop();

	  return(TRUE);
        }
        else
        {
          ptree = &((tree->children)[shifted_hashed_key & 1]);

          shifted_hashed_key >>= 1;
        }
      }
      else
      {
        ptree = &((tree->children)[shifted_hashed_key & 1]);

        shifted_hashed_key >>= 1;
      }

      tree = *ptree;
    }
    while(tree != NULL);

    return(FALSE);
  } 
}


local int count;

local /*ARGSUSED*/ void count_entries(ptr data,ptr key)
{
  ++count;
}


int wn_hcount(wn_htab table)
{
  count = 0;

  wn_hact(table,(count_entries));

  return(count);
}


local void act_on_binary_tree
(
  wn_binary_tree_data binary_tree,
  void (*paction)(ptr data,ptr key)
)
{
  if(binary_tree != NULL)
  {
    (*paction)(binary_tree->data,binary_tree->key);

    act_on_binary_tree((binary_tree->children)[0],paction);
    act_on_binary_tree((binary_tree->children)[1],paction);
  }
}


EXTERN void wn_hact(wn_htab table,void (*paction)(ptr data,ptr key))
{
  act_on_binary_tree(table->binary_tree,paction);
}


local void verify_child_polarity
(
  wn_binary_tree_data child,
  int polarity,
  int shift_count
)
{
  wn_assert(((child->hashed_key>>shift_count) & 1) == polarity);
}


local void verify_binary_tree
(
  wn_htab table,
  wn_binary_tree_data binary_tree,
  int shift_count
)
{
  wn_binary_tree_data child0,child1;
  int new_shift_count;

  wn_assert(binary_tree->hashed_key == 
	    ((*(table->phash_func))(binary_tree->key)));

  new_shift_count = shift_count+1;

  child0 = (binary_tree->children)[0];
  child1 = (binary_tree->children)[1];

  if(child0 != NULL)
  {
    verify_child_polarity(child0,0,shift_count);
    verify_binary_tree(table,child0,new_shift_count);
  }
  if(child1 != NULL)
  {
    verify_child_polarity(child1,1,shift_count);
    verify_binary_tree(table,child1,new_shift_count);
  }
}


void wn_hverify(wn_htab table)
{
  wn_binary_tree_data binary_tree;

  wn_assert((ptr)(table->phash_func) != NULL);
  wn_assert((ptr)(table->pkeys_eq_func) != NULL);
  wn_assert((ptr)(table->palloc_copy_func) != NULL);
  wn_assert((ptr)(table->pfree_func) != NULL);
  wn_assert(table->group != NULL);

  binary_tree = table->binary_tree;

  if(binary_tree != NULL)
  {
    verify_binary_tree(table,binary_tree,0);
  }
}



#if 0
wn_hdump(table)

wn_htab table;

{
  dump_binary_tree(table->binary_tree,0);
}



local dump_binary_tree(binary_tree,level)

wn_binary_tree_data binary_tree;
int level;

{
  if(binary_tree != NULL)
  {
    print_spaces(2*level);
    printf("* hashed_key = %d\n",binary_tree->hashed_key);

    print_spaces(2*level);
    printf("left_children:\n");
    dump_binary_tree(binary_tree->left_child,level+1);

    print_spaces(2*level);
    printf("right_children:\n");
    dump_binary_tree(binary_tree->right_child,level+1);
  }
}



local print_spaces(n)

int n;

{
  int i;

  for(i=0;i<n;i++)
  {
    printf(" ");
  }
}
#endif


#if 1
#include <math.h>


local void analyze_binary_tree
(
  wn_binary_tree_data binary_tree,
  wn_binary_tree_data parent,
  int *pnum_entries,
  int *pdepth_sum,
  int *pnum_collisions,
  int level
)
{
  if(binary_tree != NULL)
  {
    (*pnum_entries)++;
    (*pdepth_sum) += level;

    if((parent != NULL)&&(parent->hashed_key == binary_tree->hashed_key))
    {
      ++(*pnum_collisions);
    }

    analyze_binary_tree((binary_tree->children)[0],binary_tree,
                        pnum_entries,pdepth_sum,pnum_collisions,level+1);
    analyze_binary_tree((binary_tree->children)[1],binary_tree,
                        pnum_entries,pdepth_sum,pnum_collisions,level+1);
  }
}


void wn_hanalyze(wn_htab table)
{
  int num_entries,depth_sum,num_collisions;
  double avg_depth,merit;

  num_entries = depth_sum = num_collisions = 0;

  analyze_binary_tree(table->binary_tree,(wn_binary_tree_data)NULL,
		      &num_entries,&depth_sum,&num_collisions,1);

  if(num_entries > 0)
  {
    avg_depth = ((double)depth_sum)/((double)num_entries);
    merit = avg_depth/(log((double)num_entries)/log(2.0));

    printf("num_entries = %d,avg depth=%lf,merit=%lf,#collisions=%d\n",
           num_entries,avg_depth,merit,num_collisions);
  }
  else
  {
    printf("num_entries = %d,avg depth=--,merit=--,#collisions=--\n",
           num_entries);
  }
}
#endif


