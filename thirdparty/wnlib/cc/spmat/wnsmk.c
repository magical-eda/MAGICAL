/**********************************************************************

wn_make_sparse_matrix(&mat,len_i,len_j)

wn_free_sparse_matrix(mat)

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
#include "wnsmat.h"



wn_make_sparse_matrix(pmat,len_i,len_j)

wn_sparse_matrix *pmat;
int len_i,len_j;

{
  wn_assert(len_i >= 0);
  wn_assert(len_j >= 0);

  *pmat = wn_znew(wn_sparse_matrix);

  (*pmat)->len_i = len_i;
  (*pmat)->len_j = len_j;

  (*pmat)->i_lists = (wn_sll *)wn_zalloc(len_i*wn_sizeof(wn_structof(wn_sll)));
  (*pmat)->j_lists = (wn_sll *)wn_zalloc(len_j*wn_sizeof(wn_structof(wn_sll)));

  (*pmat)->group = wn_curgp();
}



wn_free_sparse_matrix(mat)

wn_sparse_matrix mat;

{
  int i,j,len_i,len_j;
  wn_sll *i_lists,*j_lists,list,el;
  wn_sparse_matrix_entry entry;

  wn_gppush(mat->group);

  len_i = mat->len_i;
  len_j = mat->len_j;

  i_lists = mat->i_lists;
  j_lists = mat->j_lists;

  for(i=0;i<len_i;i++)
  {
    list = i_lists[i];

    for(wn_loopinit(),el=list;wn_sllloop(&el,&entry);)
    {
      wn_free((ptr)entry);
    }

    wn_freesll_list(list);
  }
  for(j=0;j<len_j;j++)
  {
    list = j_lists[j];

    wn_freesll_list(list);
  }

  wn_free((ptr)i_lists);
  wn_free((ptr)j_lists);
  
  wn_free((ptr)mat);

  wn_gppop();
}



