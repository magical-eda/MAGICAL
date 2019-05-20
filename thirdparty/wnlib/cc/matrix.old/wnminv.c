/**********************************************************************

wn_invert_matrix(&code,&mat_inverse,mat)
wn_invert_raw_matrix(&code,raw_mat_inverse,raw_mat,len_i)

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
#include "wnabs.h"
#include "wnprm.h"
#include "wnmat.h"



wn_invert_matrix(pcode,pmat_inverse,mat)

int *pcode;
wn_matrix *pmat_inverse,mat;

{
  int len_i;

  wn_assertmsg(mat->len_i == mat->len_j,"not square matrix");

  len_i = mat->len_i;

  wn_make_matrix(pmat_inverse,len_i,len_i);

  wn_invert_raw_matrix(pcode,(*pmat_inverse)->numbers,mat->numbers,len_i);
}



local int *row_permutation,*var_permutation,
          *row_permutation_inverse,*var_permutation_inverse,
          *temp_permutation;
local double **temp_mat;

wn_invert_raw_matrix(pcode,raw_mat_inverse,raw_mat,len_i)

int *pcode;
double **raw_mat_inverse,**raw_mat;
int len_i;

{
  wn_gpmake("no_free");
  wn_gplabel("invert matrix group");

  set_up_arrays(raw_mat_inverse,raw_mat,len_i);

  in_place_invert(pcode,raw_mat_inverse,len_i);

  if(*pcode == WN_SUCCESS)
  {
    unscramble_inverse(raw_mat_inverse,len_i);
  }

  wn_gpfree();
}



local set_up_arrays(raw_mat_inverse,raw_mat,len_i)

double **raw_mat_inverse,**raw_mat;
int len_i;

{
  int var;

  temp_mat = (double **)wn_zalloc(len_i*wn_sizeof(double *));

  row_permutation = (int *)wn_zalloc(len_i*wn_sizeof(int));
  var_permutation = (int *)wn_zalloc(len_i*wn_sizeof(int));
  row_permutation_inverse = (int *)wn_zalloc(len_i*wn_sizeof(int));
  var_permutation_inverse = (int *)wn_zalloc(len_i*wn_sizeof(int));
  temp_permutation = (int *)wn_zalloc(len_i*wn_sizeof(int));

  wn_random_permutation(row_permutation,len_i);
  wn_copy_raw_matrix(raw_mat_inverse,raw_mat,len_i,len_i);
  wn_blkcpy((ptr)temp_mat,(ptr)raw_mat_inverse,len_i*wn_sizeof(double *));
  wn_permute_array(raw_mat_inverse,row_permutation,temp_mat,len_i);

  for(var=0;var<len_i;var++)
  {
    var_permutation[var] = var+len_i;
  }
}



local in_place_invert(pcode,raw_mat,len_i)

int *pcode;
double **raw_mat;
int len_i;

{
  int pivot_row,pivot_var;

  for(pivot_row=0;pivot_row<len_i;pivot_row++)
  {
    get_best_var_from_pivot_row(pcode,&pivot_var,raw_mat,len_i,pivot_row);
    if(*pcode != WN_SUCCESS)
    {
      break;
    }

    eliminate_var_from_all_other_rows(raw_mat,len_i,pivot_row,pivot_var);

    wn_swap(row_permutation[pivot_row],var_permutation[pivot_var],int);
  }
}



local get_best_var_from_pivot_row(pcode,pvar,raw_mat,len_i,pivot_row)

int *pcode,*pvar;
double **raw_mat;
int len_i,pivot_row;

{
  int var,best_var;
  double best_a,a;

  best_var = -1;
  best_a = 0.0;

  for(var=0;var<len_i;var++)
  {
    if(var_permutation[var] >= len_i)
    {
      a = raw_mat[pivot_row][var];
      a = wn_abs(a);
  
      if(a > best_a)
      {
        best_a = a;
        best_var = var;
      }
    }
  }

  if(best_var == -1)
  {
    *pcode = WN_SINGULAR;

    return;
  }

  *pvar = best_var;

  *pcode = WN_SUCCESS;
}



/********************************************************************

  This is the slow part of the algorithm.

********************************************************************/
local eliminate_var_from_all_other_rows(raw_mat,len_i,pivot_row,pivot_var)

double **raw_mat;
int len_i,pivot_row,pivot_var;

{
  int row;
  double self_multiplier,multiplier;

  wn_assert(raw_mat[pivot_row][pivot_var] != 0.0);

  self_multiplier = 1.0 / raw_mat[pivot_row][pivot_var];
  
  raw_mat[pivot_row][pivot_var] = 1.0;      /* for new inverted var */

  wn_multiply_into_raw_vector_scalar(raw_mat[pivot_row],self_multiplier,
                                     len_i);

  for(row=0;row<len_i;row++)
  {
    if(row != pivot_row)
    {
      multiplier = -raw_mat[row][pivot_var];

      raw_mat[row][pivot_var] = 0.0;              /* for new inverted var */

      if(multiplier != 0.0)
      {
        wn_add_into_raw_vector_scaled_raw_vector(raw_mat[row],
                                                 raw_mat[pivot_row],
                                                 multiplier,len_i); 
      }
    }
  }
}



local unscramble_inverse(raw_mat_inverse,len_i)

double **raw_mat_inverse;
int len_i;

{
  int row;

  for(row=0;row<len_i;row++)
  {
    row_permutation[row] -= len_i;
  }

  wn_invert_permutation(row_permutation_inverse,row_permutation,len_i);
  wn_invert_permutation(var_permutation_inverse,var_permutation,len_i);

  apply_permutation_to_all_rows(raw_mat_inverse,var_permutation_inverse,len_i);

  wn_blkcpy((ptr)temp_mat,(ptr)raw_mat_inverse,len_i*wn_sizeof(double *));
  wn_permute_array(raw_mat_inverse,row_permutation_inverse,temp_mat,len_i);
}



local double *temp_row;

local apply_permutation_to_all_rows(raw_mat,permutation,len_i)

double **raw_mat;
int *permutation;
int len_i;

{
  double *row;
  int row_index;

  temp_row = (double *)wn_zalloc(len_i*wn_sizeof(double));

  for(row_index=0;row_index<len_i;row_index++)
  {
    row = raw_mat[row_index];

    wn_blkcpy((ptr)temp_row,(ptr)row,len_i*wn_sizeof(double));
    wn_permute_array(row,permutation,temp_row,len_i);
  }
}




