/**********************************************************************

wn_invert_mat(&code,mat,len_i)

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
#include "wnmemb.h"
#include "wnswap.h"
#include "wnabs.h"
#include "wnprm.h"
#include "wnvect.h"

#include "wnmat.h"


local int *row_permutation,*var_permutation,
          *row_permutation_inverse,*var_permutation_inverse;
local double **temp_mat;


local void set_up_arrays(double **mat,int len_i)
{
  int var;

  temp_mat = (double **)wn_zalloc(len_i*sizeof(double *));

  row_permutation = (int *)wn_zalloc(len_i*sizeof(int));
  var_permutation = (int *)wn_zalloc(len_i*sizeof(int));
  row_permutation_inverse = (int *)wn_zalloc(len_i*sizeof(int));
  var_permutation_inverse = (int *)wn_zalloc(len_i*sizeof(int));

  wn_random_permutation(row_permutation,len_i);
  wn_memcpy(temp_mat,mat,len_i*sizeof(double *));
  wn_permute_array(mat,row_permutation,temp_mat,len_i);

  for(var=0;var<len_i;var++)
  {
    var_permutation[var] = var+len_i;
  }
}


local void get_best_var_from_pivot_row
(
  int *pcode,
  int *pvar,
  double **mat,
  int len_i,
  int pivot_row
)
{
  int var,best_var;
  double best_a,a;

  best_var = -1;
  best_a = 0.0;

  for(var=0;var<len_i;var++)
  {
    if(var_permutation[var] >= len_i)
    {
      a = mat[pivot_row][var];
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
local void eliminate_var_from_all_other_rows
(
  double **mat,
  int len_i,
  int pivot_row,
  int pivot_var
)
{
  int row,col;
  double self_multiplier,multiplier;

  wn_assert(mat[pivot_row][pivot_var] != 0.0);

  self_multiplier = 1.0 / mat[pivot_row][pivot_var];
  
  mat[pivot_row][pivot_var] = 1.0;      /* for new inverted var */

  for(col=0;col<len_i;++col)
  {
    mat[pivot_row][col] *= self_multiplier;
  }

  for(row=0;row<len_i;row++)
  {
    if(row != pivot_row)
    {
      multiplier = -mat[row][pivot_var];

      mat[row][pivot_var] = 0.0;              /* for new inverted var */

      if(multiplier != 0.0)
      {
        wn_add_scaled_vect(mat[row],mat[pivot_row],multiplier,len_i); 
      }
    }
  }
}


local void in_place_invert(int *pcode,double **mat,int len_i)
{
  int pivot_row,pivot_var;

  for(pivot_row=0;pivot_row<len_i;pivot_row++)
  {
    get_best_var_from_pivot_row(pcode,&pivot_var,mat,len_i,pivot_row);
    if(*pcode != WN_SUCCESS)
    {
      break;
    }

    eliminate_var_from_all_other_rows(mat,len_i,pivot_row,pivot_var);

    wn_swap(row_permutation[pivot_row],var_permutation[pivot_var],int);
  }
}


local double *temp_row;

local void apply_permutation_to_all_rows(double **mat,int *permutation,
					 int len_i)
{
  double *row;
  int row_index;

  temp_row = (double *)wn_zalloc(len_i*sizeof(double));

  for(row_index=0;row_index<len_i;row_index++)
  {
    row = mat[row_index];

    wn_memcpy(temp_row,row,len_i*sizeof(double));
    wn_permute_array(row,permutation,temp_row,len_i);
  }
}


local void unscramble_inverse(double **mat,int len_i)
{
  int row;

  for(row=0;row<len_i;row++)
  {
    row_permutation[row] -= len_i;
  }

  wn_invert_permutation(row_permutation_inverse,row_permutation,len_i);
  wn_invert_permutation(var_permutation_inverse,var_permutation,len_i);

  apply_permutation_to_all_rows(mat,var_permutation_inverse,len_i);

  wn_memcpy(temp_mat,mat,len_i*sizeof(double *));
  wn_permute_array(mat,row_permutation_inverse,temp_mat,len_i);
}


void wn_invert_mat(int *pcode,double **mat,int len_i)
{
  wn_gpmake("no_free");
  wn_gplabel("invert matrix group");

  set_up_arrays(mat,len_i);

  in_place_invert(pcode,mat,len_i);

  if(*pcode == WN_SUCCESS)
  {
    unscramble_inverse(mat,len_i);
  }

  wn_gpfree();
}



