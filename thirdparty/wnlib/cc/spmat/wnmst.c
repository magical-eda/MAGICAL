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
#include "wnswap.h"
#include "wnsmat.h"
#include "wnary.h"



wn_min_spanning_tree(pcode,plen,presult,mat)

int *pcode;
double *plen;
wn_sparse_matrix *presult,mat;

{
  int wn_compare_entry_values();
  wn_memgp top_group;
  wn_sll list;
  wn_set tree_set;
  wn_sparse_matrix_entry entry;

  top_group = wn_curgp();
  wn_gpmake("no_free");

  wn_assert(mat->len_i == mat->len_j);

  make_all_entry_list(&list,mat);
  wn_sort_sll(&list,(wn_compare_entry_values));

  compute_tree_set(pcode,&tree_set,list,mat->len_i);

  /* wcn: 11/30/89 do always 
  if(*pcode == WN_SUCCESS)
  */
  {
    wn_gppush(top_group);

    *plen = 0.0;
    wn_make_sparse_matrix(presult,mat->len_i,mat->len_j);

    for(wn_loopinit();wn_setloop(tree_set,&entry);)
    {
      *plen += entry->value;

      wn_insert_sparse_matrix_value(*presult,entry->value,entry->i,entry->j);
      wn_insert_sparse_matrix_value(*presult,entry->value,entry->j,entry->i);
    }

    wn_sort_sparse_matrix(*presult);

    wn_gppop();
  }

  wn_gpfree();
}



local make_all_entry_list(plist,mat)

wn_sll *plist;
wn_sparse_matrix mat;

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



int wn_compare_entry_values(e1,e2)

wn_sparse_matrix_entry e1,e2;

{
  return(wn_doublecmp(e1->value,e2->value));
}



local compute_tree_set(pcode,ptree_set,list,len)

int *pcode;
wn_set *ptree_set;
wn_sll list;
int len;

{
  int *labels,*nexts,*counts;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  int count;

  *pcode = WN_INFEASIBLE;

  wn_mkset(ptree_set);

  labels = (int *)wn_alloc(len*wn_sizeof(int));
  nexts = (int *)wn_alloc(len*wn_sizeof(int));
  counts = (int *)wn_alloc(len*wn_sizeof(int));

  wn_identity_permutation(labels,len);
  wn_arrayfill(nexts,len,-1,int);
  wn_arrayfill(counts,len,1,int);

  count = 1;

  for(wn_loopinit(),el=list;wn_sllloop(&el,&entry);)
  {
    if(labels[entry->i] != labels[entry->j])
    {
      wn_setins(*ptree_set,entry);

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



local merge_groups(i,j,labels,nexts,counts)

int i,j;
register int labels[],nexts[];
int counts[];

{
  int larger_label,smaller_label;
  register int index,last_index;

  larger_label = labels[i];
  smaller_label = labels[j];

  if(counts[smaller_label] > counts[larger_label])
  {
    wn_swap(larger_label,smaller_label,int);
  }

  counts[larger_label] += counts[smaller_label];

  for(index=smaller_label;index != -1;index = nexts[index])
  {
    labels[index] = larger_label;
    last_index = index;
  }

  nexts[last_index] = nexts[larger_label];
  nexts[larger_label] = smaller_label;
}



