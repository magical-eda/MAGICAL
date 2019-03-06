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
#include "wnasrt.h"

#include "wnspmat.h"
#include "wnspmk.h"



void wn_make_sparse_matrix
(
  wn_sparse_matrix *pmat,
  int len_i,
  int len_j
)
{
  wn_assert(len_i >= 0);
  wn_assert(len_j >= 0);

  *pmat = (wn_sparse_matrix)wn_zalloc(sizeof(struct wn_sparse_matrix_struct));

  (*pmat)->len_i = len_i;
  (*pmat)->len_j = len_j;

  (*pmat)->i_lists = (wn_sll *)wn_zalloc(len_i*sizeof(struct wn_sll_struct));
  (*pmat)->j_lists = (wn_sll *)wn_zalloc(len_j*sizeof(struct wn_sll_struct));

  (*pmat)->group = wn_curgp();
}


void wn_free_sparse_matrix(wn_sparse_matrix mat)
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

    for(el=list;el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);

      wn_free(entry);
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


void wn_make_sparse_matrix_entry
(
  wn_sparse_matrix_entry *pentry,
  double value,
  int i,
  int j
)
{
  *pentry = (wn_sparse_matrix_entry)
	    wn_zalloc(sizeof(struct wn_sparse_matrix_entry_struct));

  (*pentry)->value = value;

  (*pentry)->i = i;
  (*pentry)->j = j;
}

