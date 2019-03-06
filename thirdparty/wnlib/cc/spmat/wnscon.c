/**********************************************************************

wn_insert_sparse_matrix_value(mat,value,i,j)
wn_insert_sparse_matrix_entry(mat,entry)
wn_sort_sparse_matrix(mat)

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
#include "wnary.h"
#include "wnsmat.h"



wn_insert_sparse_matrix_value(mat,value,i,j)

wn_sparse_matrix mat;
double value;
int i,j;

{
  wn_sparse_matrix_entry entry;

  wn_gppush(mat->group);

  entry = wn_znew(wn_sparse_matrix_entry);

  entry->value = value;

  entry->i = i;
  entry->j = j;

  wn_insert_sparse_matrix_entry(mat,entry);

  wn_gppop();
}



wn_insert_sparse_matrix_entry(mat,entry)

wn_sparse_matrix mat;
wn_sparse_matrix_entry entry;

{
  wn_gppush(mat->group);

  wn_sllins(&((mat->i_lists)[entry->i]),entry);
  wn_sllins(&((mat->j_lists)[entry->j]),entry);

  wn_gppop();
}



wn_sort_sparse_matrix(mat)

wn_sparse_matrix mat;

{
  sort_i_lists_by_j_lists(mat->i_lists,mat->len_i,mat->j_lists,mat->len_j);
  sort_j_lists_by_i_lists(mat->j_lists,mat->len_j,mat->i_lists,mat->len_i);
}



local sort_i_lists_by_j_lists(i_lists,len_i,j_lists,len_j)

wn_sll i_lists[],j_lists[];
int len_i,len_j;

{
  wn_sll i_el_list,i_el,j_list,j_el;
  int j;
  wn_sparse_matrix_entry entry;

  make_i_el_list(&i_el_list,i_lists,len_i);
  wn_arrayfill(i_lists,len_i,NULL,wn_sll);

  for(j=len_j-1;j>=0;--j)
  {
    j_list = j_lists[j];

    for(j_el=j_list;j_el != NULL;j_el=j_el->next)
    {
      entry = (wn_sparse_matrix_entry)(j_el->contents);

      i_el = i_el_list;
      wn_slleunlink(&i_el_list);

      i_el->contents = (ptr)entry;

      wn_slleins(&(i_lists[entry->i]),i_el);
    }
  }

  wn_assert(i_el_list == NULL);
}



local make_i_el_list(pi_el_list,i_lists,len_i)

wn_sll *pi_el_list,i_lists[];
int len_i;

{
  int i;
  wn_sll list;

  *pi_el_list = NULL;

  for(i=0;i<len_i;++i)
  {
    list = i_lists[i];

    wn_sllcat(&list,*pi_el_list);

    *pi_el_list = list;
  }
}



local sort_j_lists_by_i_lists(j_lists,len_j,i_lists,len_i)

wn_sll i_lists[],j_lists[];
int len_i,len_j;

{
  wn_sll j_el_list,j_el,i_list,i_el;
  int i;
  wn_sparse_matrix_entry entry;

  make_j_el_list(&j_el_list,j_lists,len_j);
  wn_arrayfill(j_lists,len_j,NULL,wn_sll);

  for(i=len_i-1;i>=0;--i)
  {
    i_list = i_lists[i];

    for(i_el=i_list;i_el != NULL;i_el=i_el->next)
    {
      entry = (wn_sparse_matrix_entry)(i_el->contents);

      j_el = j_el_list;
      wn_slleunlink(&j_el_list);

      j_el->contents = (ptr)entry;

      wn_slleins(&(j_lists[entry->j]),j_el);
    }
  }

  wn_assert(j_el_list == NULL);
}



local make_j_el_list(pj_el_list,j_lists,len_j)

wn_sll *pj_el_list,j_lists[];
int len_j;

{
  int j;
  wn_sll list;

  *pj_el_list = NULL;

  for(j=0;j<len_j;++j)
  {
    list = j_lists[j];

    wn_sllcat(&list,*pj_el_list);

    *pj_el_list = list;
  }
}



