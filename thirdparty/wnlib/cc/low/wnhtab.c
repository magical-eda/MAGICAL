/**************************************************************************

wn_htab table;

wn_mkhtab(&table,phash_func,pkeys_eq_func,palloc_copy_func,pfree_func)
wn_freehtab(table)

bool wn_hget(&data,table,key)
bool wn_hins(data,table,key)
bool wn_hfins(data,table,key)
bool wn_hdel(table,key)

int wn_hcount(table)
wn_hact(table,paction)      void (*paction)(data,key);

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
#include "wnlib.h"



wn_mkhtab(ptable,phash_func,pkeys_eq_func,palloc_copy_func,pfree_func)

wn_htab *ptable;
int (*phash_func)(/*key*/);
bool (*pkeys_eq_func)(/*key1,key2*/);
void (*palloc_copy_func)(/*pkey,key*/);
void (*pfree_func)(/*key*/);

{
  *ptable = wn_new(wn_htab);

  (*ptable)->binary_tree = NULL;

  (*ptable)->group = wn_curgp();

  (*ptable)->phash_func = phash_func;
  (*ptable)->pkeys_eq_func = pkeys_eq_func;
  (*ptable)->palloc_copy_func = palloc_copy_func;
  (*ptable)->pfree_func = pfree_func;
}



wn_freehtab(table)

wn_htab table;

{
  wn_gppush(table->group);

  free_binary_tree(table->binary_tree,table->pfree_func);
  wn_free((ptr)table);

  wn_gppop();
}



local free_binary_tree(binary_tree,pfree_func)

wn_binary_tree_data binary_tree;
void (*pfree_func)(/*key*/);

{
  if(binary_tree != NULL)
  {
    free_binary_tree((binary_tree->children)[0],pfree_func);
    free_binary_tree((binary_tree->children)[1],pfree_func);

    (*pfree_func)(binary_tree->key);
    wn_free((ptr)binary_tree);
  }
}



bool wn_hins_routine(data,table,key)

ptr data;
wn_htab table;
ptr key;

{
  register wn_binary_tree_data tree,*ptree;
  register int hashed_key,shifted_hashed_key;

  ptree = &(table->binary_tree);
  tree = *ptree;

  hashed_key = (*(table->phash_func))(key);

  if(tree == NULL)  /* table is empty */
  {
    wn_gppush(table->group);

    *ptree = tree = wn_new(wn_binary_tree_data);

    (tree->children)[0] = (tree->children)[1] = NULL;
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

    *ptree = tree = wn_new(wn_binary_tree_data);

    (tree->children)[0] = (tree->children)[1] = NULL;
    tree->hashed_key = hashed_key;
    tree->data = data;
    (*(table->palloc_copy_func))(&(tree->key),key);

    wn_gppop();

    return(TRUE);
  } 
}



bool wn_hfins_routine(data,table,key)

ptr data;
wn_htab table;
ptr key;

{
  register wn_binary_tree_data tree,*ptree;
  register int hashed_key,shifted_hashed_key;

  ptree = &(table->binary_tree);
  tree = *ptree;

  hashed_key = (*(table->phash_func))(key);

  if(tree == NULL)  /* table is empty */
  {
    wn_gppush(table->group);

    *ptree = tree = wn_new(wn_binary_tree_data);

    (tree->children)[0] = (tree->children)[1] = NULL;
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

    *ptree = tree = wn_new(wn_binary_tree_data);

    (tree->children)[0] = (tree->children)[1] = NULL;
    tree->hashed_key = hashed_key;
    tree->data = data;
    (*(table->palloc_copy_func))(&(tree->key),key);

    wn_gppop();

    return(TRUE);
  } 
}



bool wn_hget_routine(pdata,table,key)

ptr *pdata;
register wn_htab table;
ptr key;

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



bool wn_hdel_routine(table,key)

wn_htab table;
ptr key;

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
          wn_free((ptr)tree);

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



local remove_tree_node(ptree_node,shifted_hashed_key)

wn_binary_tree_data *ptree_node;
int shifted_hashed_key;

{
  wn_binary_tree_data tree_node;
  wn_binary_tree_data *pleaf_node,leaf_node;

  tree_node = *ptree_node;

  find_leaf_tree_node_pointer(&pleaf_node,tree_node,shifted_hashed_key);

  if(pleaf_node == NULL)
  {
    *ptree_node = NULL;
  }
  else
  {
    leaf_node = *pleaf_node;
    *pleaf_node = NULL;

    (leaf_node->children)[0] = (tree_node->children)[0];
    (leaf_node->children)[1] = (tree_node->children)[1];

    *ptree_node = leaf_node;
  }
}



local find_leaf_tree_node_pointer(ppleaf_node,tree_node,shifted_hashed_key)

register wn_binary_tree_data **ppleaf_node,tree_node;
register int shifted_hashed_key;

{
  register wn_binary_tree_data *pchild_node,child_node;
  register int index;

  pchild_node = NULL;

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



local int count;

int wn_hcount(table)

wn_htab table;

{
  void count_entries();

  count = 0;

  wn_hact(table,(count_entries));

  return(count);
}



local /*ARGSUSED*/ void count_entries(data,key)

ptr data,key;

{
  ++count;
}



wn_hact(table,paction)

wn_htab table;
void (*paction)(/*data,key*/);

{
  act_on_binary_tree(table->binary_tree,paction);
}



local act_on_binary_tree(binary_tree,paction)

wn_binary_tree_data binary_tree;
void (*paction)(/*data,key*/);

{
  if(binary_tree != NULL)
  {
    (*paction)(binary_tree->data,binary_tree->key);

    act_on_binary_tree((binary_tree->children)[0],paction);
    act_on_binary_tree((binary_tree->children)[1],paction);
  }
}



wn_hverify(table)

wn_htab table;

{
  wn_binary_tree_data binary_tree;

  wn_assert(table->phash_func != NULL);
  wn_assert(table->pkeys_eq_func != NULL);
  wn_assert(table->palloc_copy_func != NULL);
  wn_assert(table->pfree_func != NULL);
  wn_assert(table->group != NULL);

  binary_tree = table->binary_tree;

  if(binary_tree != NULL)
  {
    verify_binary_tree(table,binary_tree,0);
  }
}



local verify_binary_tree(table,binary_tree,shift_count)

wn_htab table;
wn_binary_tree_data binary_tree;
int shift_count;

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



local verify_child_polarity(child,polarity,shift_count)

wn_binary_tree_data child;
int polarity,shift_count;

{
  wn_assert(((child->hashed_key>>shift_count) & 1) == polarity);
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



#if 0
wn_hanalyze(table)

wn_htab table;

{
  extern double log();
  int num_entries,depth_sum;
  double avg_depth,merit;

  num_entries = depth_sum = 0;

  analyze_binary_tree(table->binary_tree,&num_entries,&depth_sum,1);

  if(num_entries > 0)
  {
    avg_depth = ((double)depth_sum)/((double)num_entries);
    merit = avg_depth/(log((double)num_entries)/log(2.0));

    printf("num_entries = %d,avg depth=%f,merit=%f\n",
           num_entries,avg_depth,merit);
  }
  else
  {
    printf("num_entries = %d,avg depth=--,merit=--\n",
           num_entries);
  }
}



local analyze_binary_tree(binary_tree,pnum_entries,pdepth_sum,level)

wn_binary_tree_data binary_tree;
int *pnum_entries,*pdepth_sum,level;

{
  if(binary_tree != NULL)
  {
    (*pnum_entries)++;
    (*pdepth_sum) += level;

    analyze_binary_tree((binary_tree->children)[0],
                        pnum_entries,pdepth_sum,level+1);
    analyze_binary_tree((binary_tree->children)[1],
                        pnum_entries,pdepth_sum,level+1);
  }
}
#endif


