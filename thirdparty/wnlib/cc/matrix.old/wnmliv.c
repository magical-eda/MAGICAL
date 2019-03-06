/**********************************************************************

wn_ls_invert_matrix(&code,&mat_inverse,mat)
wn_ls_invert_raw_matrix(&code,raw_mat_inverse,raw_mat,len_i,len_j)

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
#include "wnmat.h"



wn_ls_invert_matrix(pcode,pmat_inverse,mat)

int *pcode;
wn_matrix *pmat_inverse,mat;

{
  int len_i,len_j;

  len_i = mat->len_i;
  len_j = mat->len_j;

  wn_make_matrix(pmat_inverse,len_j,len_i);

  wn_ls_invert_raw_matrix(pcode,(*pmat_inverse)->numbers,mat->numbers,
                          len_i,len_j);
}



wn_ls_invert_raw_matrix(pcode,raw_mat_inverse,raw_mat,len_i,len_j)

int *pcode;
double **raw_mat_inverse,**raw_mat;
int len_i,len_j;

{
  double **raw_mat_transpose,**product,**product_inverse;

  wn_assertmsg(len_i >= len_j,"not tall matrix");

  wn_gpmake("no_free");
  wn_gplabel("least-square invert matrix group");

  wn_make_raw_matrix(&raw_mat_transpose,len_j,len_i);
  wn_make_raw_matrix(&product,len_j,len_j);
  wn_make_raw_matrix(&product_inverse,len_j,len_j);

  wn_transpose_raw_matrix(raw_mat_transpose,raw_mat,len_i,len_j);
  wn_raw_matrix_multiply_raw_matrix(product,raw_mat_transpose,raw_mat,
                                    len_j,len_i,len_j);
  wn_invert_raw_matrix(pcode,product_inverse,product,len_j);
  if(*pcode == WN_SUCCESS)
  {
    wn_raw_matrix_multiply_raw_matrix(raw_mat_inverse,
                                      product_inverse,raw_mat_transpose,
                                      len_j,len_j,len_i);
  }

  wn_gpfree();
}




