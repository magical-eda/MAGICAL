/**********************************************************************

wn_raw_simplex_loop(&code,mat,right_side,non_zero_vars,zero_vars,
                    len_i,len_j)

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
#include "wnmat.h"



wn_raw_simplex_loop(pcode,mat,right_side,non_zero_vars,zero_vars,
                    len_i,len_j)

int *pcode;
double **mat,*right_side;
int *non_zero_vars,*zero_vars;
int len_i,len_j;

{
  int leaving_row,entering_col;

  while(TRUE)
  {
    choose_entering_col(&entering_col,mat,len_j);
    if(entering_col == -1)
    {
      *pcode = WN_SUCCESS;

      return;
    }

    choose_leaving_row(&leaving_row,mat,right_side,len_i,entering_col);
    if(leaving_row == -1)
    {
      *pcode = WN_UNBOUNDED;

      return;
    }

    eliminate_entering_col_from_all_other_rows(mat,right_side,
                                               leaving_row,entering_col,
                                               len_i,len_j);

    wn_swap(non_zero_vars[leaving_row],zero_vars[entering_col],int);
  }
}



local choose_entering_col(pentering_col,mat,len_j)

int *pentering_col;
double **mat;
int len_j;

{
  int j,best_cols[2];
  double best_coefs[2];

  best_coefs[0] = best_coefs[1] = 0.0;

  for(j=0;j<len_j;j++)
  {
    if(mat[0][j] > best_coefs[0])
    {
      best_coefs[1] = best_coefs[0];
      best_coefs[0] = mat[0][j];
    
      best_cols[1] = best_cols[0];
      best_cols[0] = j;
    }
    else if(mat[0][j] > best_coefs[1])
    {
      best_coefs[1] = mat[0][j];
    
      best_cols[1] = j;
    }
  }

  if(best_coefs[1] > 0.0)
  {
    *pentering_col = best_cols[wn_random_bit()];
  }
  else if(best_coefs[0] > 0.0)
  {
    *pentering_col = best_cols[0];
  }
  else
  {
    *pentering_col = -1;
  }
}



local choose_leaving_row(pleaving_row,mat,right_side,len_i,entering_col)

int *pleaving_row,len_i,entering_col;
double **mat,*right_side;

{
  int i,best_row;
  double quotient,best_quotient;

  best_row = -1;
  best_quotient = WN_FHUGE;

  for(i=1;i<len_i;i++)
  {
    if(mat[i][entering_col] > 0.0)
    {
      quotient = right_side[i]/mat[i][entering_col];

      if(quotient < best_quotient)
      {
        best_quotient = quotient;

        best_row = i;
      }
    }
  }

  *pleaving_row = best_row;
}



local eliminate_entering_col_from_all_other_rows(mat,right_side,
                                                 leaving_row,entering_col,
                                                 len_i,len_j)

double **mat,*right_side;
int leaving_row,entering_col,len_i,len_j;

{
  int row;
  double self_multiplier,multiplier;

  self_multiplier = 1.0 / mat[leaving_row][entering_col];
  
  mat[leaving_row][entering_col] = 1.0;            /* new var in column */

  wn_multiply_into_raw_vector_scalar(mat[leaving_row],self_multiplier,len_j);
  right_side[leaving_row] *= self_multiplier;

  for(row=0;row<len_i;row++)
  {
    if(row != leaving_row)
    {
      multiplier = -mat[row][entering_col];
  
      mat[row][entering_col] = 0.0;                /* new var in column */

      if(multiplier != 0.0)
      {
        wn_add_into_raw_vector_scaled_raw_vector(mat[row],
                                                 mat[leaving_row],
                                                 multiplier,
                                                 len_j);
        right_side[row] += (right_side[leaving_row]*multiplier); 
      }
    }
  }
}




