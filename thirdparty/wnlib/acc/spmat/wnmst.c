/**********************************************************************

wn_min_spanning_tree(&code,&len,&result,mat)

**********************************************************************/
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
#include "wnswap.h"
#include "wncmp.h"
#include "wnsort.h"
#include "wnprm.h"

#include "wnmst.h"



local void make_all_entry_list(wn_sll *plist,wn_sparse_matrix mat)
{
  int i,len_i;
  wn_sll listcopy,*i_lists;

  *plist = NULL;

  len_i = mat->len_i;
  i_lists = mat->i_lists;

  for(i=0;i<len_i;i++)
  {
    wn_sllcpy(&listcopy,i_lists[i]);

    wn_sllcat(&listcopy,*plist);
    *plist = listcopy;
  }
}


local int compare_entry_values
(
  wn_sparse_matrix_entry e1,
  wn_sparse_matrix_entry e2
)
{
  return(wn_doublecmp(e1->value,e2->value));
}


local void merge_groups
(
  int i,
  int j,
  int labels[],
  int nexts[],
  int counts[]
)
{
  int larger_label,smaller_label;
  int index,last_index;

  larger_label = labels[i];
  smaller_label = labels[j];

  if(counts[smaller_label] > counts[larger_label])
  {
    wn_swap(larger_label,smaller_label,int);
  }

  counts[larger_label] += counts[smaller_label];

  last_index = -1;

  for(index=smaller_label;index != -1;index = nexts[index])
  {
    labels[index] = larger_label;
    last_index = index;
  }

  wn_assert(last_index != -1);  /* smaller group has at least 1 item */

  nexts[last_index] = nexts[larger_label];
  nexts[larger_label] = smaller_label;
}


local void compute_tree_list(int *pcode,wn_sll *ptree_list,wn_sll list,int len)
{
  int *labels,*nexts,*counts;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  int i,count;

  *pcode = WN_INFEASIBLE;

  *ptree_list = NULL;

  labels = (int *)wn_alloc(len*sizeof(int));
  nexts = (int *)wn_alloc(len*sizeof(int));
  counts = (int *)wn_alloc(len*sizeof(int));

  wn_identity_permutation(labels,len);
  for(i=0;i<len;++i)
  {
    nexts[i] = -1;
    counts[i] = 1;
  }

  count = 1;

  for(el=list;el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    if(labels[entry->i] != labels[entry->j])
    {
      wn_sllins(ptree_list,entry);

      merge_groups(entry->i,entry->j,labels,nexts,counts);      

      count++;

      if(count == len)
      {
        *pcode = WN_SUCCESS;

        break;
      }
    }
  }
}


void wn_min_spanning_tree
(
  int *pcode,
  double *plen,
  wn_sparse_matrix *presult,
  wn_sparse_matrix mat
)
{
  wn_memgp top_group;
  wn_sll list;
  wn_sll tree_list,tree_el;
  wn_sparse_matrix_entry entry;

  top_group = wn_curgp();
  wn_gpmake("no_free");

  wn_assert(mat->len_i == mat->len_j);

  make_all_entry_list(&list,mat);
  wn_sort_sll(&list,(int (*)(ptr,ptr))&compare_entry_values);

  compute_tree_list(pcode,&tree_list,list,mat->len_i);

  /* wcn: 11/30/89 do always 
  if(*pcode == WN_SUCCESS)
  */
  {
    wn_gppush(top_group);

    *plen = 0.0;
    wn_make_sparse_matrix(presult,mat->len_i,mat->len_j);

    for(tree_el=tree_list;tree_el!=NULL;tree_el=tree_el->next)
    {
      entry = (wn_sparse_matrix_entry)(tree_el->contents);

      *plen += entry->value;

      wn_insert_sparse_matrix_value(*presult,entry->value,entry->i,entry->j);
      wn_insert_sparse_matrix_value(*presult,entry->value,entry->j,entry->i);
    }

    wn_sort_sparse_matrix(*presult);

    wn_gppop();
  }

  wn_gpfree();
}
