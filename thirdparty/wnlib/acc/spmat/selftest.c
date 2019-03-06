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

#include <stdio.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnspmat.h"
#include "wnmst.h"

local double lo_get_value(wn_sparse_matrix mat, int i, int j)
{
  wn_sll el;
  wn_sparse_matrix_entry entry;

  wn_assert(i < mat->len_i);
  wn_assert(i >= 0);
  wn_assert(j < mat->len_j);
  wn_assert(j >= 0);

  for (el = mat->i_lists[i];  el;  el = el->next)
  {
    entry = (wn_sparse_matrix_entry) el->contents;
    if (entry->j == j)
    {
      return entry->value;
    }
    else if (j < entry->j)
    {
      break;
    }
  }

  /* it's not there */
  return 0.0;
}


local void lo_test_min_spanning_tree(void)
{
  wn_sparse_matrix mat, result;
  int code;
  double objective;
  int i, j;
  const int len = 100;

  wn_gpmake("no_free");

    wn_make_sparse_matrix(&mat, len, len);

    /* 3, 7, 17, 84 & 92 connected.  1's in the mst, 2's not */

    wn_insert_sparse_matrix_value(mat, 1.0,  3, 84);
    wn_insert_sparse_matrix_value(mat, 2.0,  3, 17);
    wn_insert_sparse_matrix_value(mat, 1.0,  7, 17);
    wn_insert_sparse_matrix_value(mat, 2.0,  7, 92);
    wn_insert_sparse_matrix_value(mat, 1.0, 84, 92);
    wn_insert_sparse_matrix_value(mat, 2.0, 17, 84);
    wn_insert_sparse_matrix_value(mat, 1.0,  3,  7);

    wn_sort_sparse_matrix(mat);

    wn_min_spanning_tree(&code, &objective, &result, mat);

    wn_assert(lo_get_value(result,  3, 84) == 1.0);
    wn_assert(lo_get_value(result,  7, 17) == 1.0);
    wn_assert(lo_get_value(result, 84, 92) == 1.0);
    wn_assert(lo_get_value(result,  3,  7) == 1.0);

    wn_assert(lo_get_value(result,  3, 17) == 0.0);
    wn_assert(lo_get_value(result,  7, 92) == 0.0);
    wn_assert(lo_get_value(result, 17, 84) == 0.0);

    /* check all 10,000 locations */
    for (i = 0;  i < len;  ++i)
    for (j = 0;  j < len;  ++j)
    {
      if (   3 == i  &&  84 == j  ||
      /**/  84 == i  &&   3 == j  ||
      /**/   7 == i  &&  17 == j  ||
      /**/  17 == i  &&   7 == j  ||
      /**/  84 == i  &&  92 == j  ||
      /**/  92 == i  &&  84 == j  ||
      /**/   3 == i  &&   7 == j  ||
      /**/   7 == i  &&   3 == j)
      {
	wn_assert(lo_get_value(result, i, j) == 1.0);
      }
      else
      {
	wn_assert(lo_get_value(result, i, j) == 0.0);
      }
    }

  wn_gpfree();
} /* lo_test_min_spanning_tree */


int main(void)
{
  printf("testing sparse matrices...\n");

  lo_test_min_spanning_tree();

  printf("  ok!!!!!!\n");

  return 0;
} /* main */
