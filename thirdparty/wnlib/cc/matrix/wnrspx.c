/**********************************************************************

void wn_simplex_loop(&code,
		     mat,right_side,right_side_control,
		     non_zero_vars,zero_vars,
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

#include <stdio.h>

#include "wnlib.h"
#include "wnswap.h"
#include "wnabs.h"
#include "wnmat.h"
#include "wnmax.h"



#define CUTOFF_RATIO  (0.01)
#define QUIT_CUTOFF_RATIO  (1.0e-6)



local choose_leaving_row(pleaving_row,pdistance,
			 mat,right_side_control,len_i,entering_col)

int *pleaving_row,len_i,entering_col;
double *pdistance,**mat,*right_side_control;

{
  int i,best_row;
  double mat_entry,quotient,best_quotient;

  best_row = -1;
  best_quotient = WN_FHUGE;

  for(i=1;i<len_i;i++)
  {
    mat_entry = mat[i][entering_col];
    
    if(mat_entry > 0.0)
    {
      /* anti-cycling */
      quotient = right_side_control[i]/mat_entry;

      if(quotient < best_quotient)
      {
        best_quotient = quotient;

        best_row = i;
      }
    }
  }

  *pleaving_row = best_row;
  *pdistance = best_quotient;
}


local double max_abs_vect(vect,len)

double vect[];
int len;

{
  double max,neg_max,val;
  int i;

  max = neg_max = 0.0;

  for(i=0;i<len;++i)
  {
    val = vect[i];

    if(val > max)
    {
      max = val;
      neg_max = -val;
    }
    else if(val < neg_max)
    {
      max = -val;
      neg_max = val;
    }
  }

  return(max);
}


local void choose_col_and_row(pentering_col,pleaving_row,
		              mat,right_side_control,len_i,len_j)

int *pentering_col,*pleaving_row;
double **mat,*right_side_control;
int len_i,len_j;

{
  int j,count,leaving_row,best_col,best_leaving_row;
  double coef,best_coef,
	 ratio,best_ratio,distance;
  int improve_count;

  best_coef = 0.0;
  best_col = -1;
  best_leaving_row = -1;

  /* randomized search */
  for(count=0;count<len_j;++count)
  {
    j = wn_random_mod_int(len_j);

    coef = mat[0][j];

    if(coef > best_coef)
    {
      choose_leaving_row(&leaving_row,&distance,
		         mat,right_side_control,len_i,j);
      if(leaving_row == -1)
      {
        *pentering_col = j;
        *pleaving_row = -1;
        return;
      }

      ratio = mat[leaving_row][j]/max_abs_vect(mat[leaving_row],len_j);

      if(ratio >= CUTOFF_RATIO)   /* is this column numerically stable? */
      {
        best_coef = coef;
        best_col = j;
        best_leaving_row = leaving_row;
      }
    }
  }

  if(best_coef > 0.0)
  {
    *pentering_col = best_col;
    *pleaving_row = best_leaving_row;

    /* numerically stable improvement found, return */
    return;
  }

  /* cannot find improvement -- use systematic search */
  for(j=0;j<len_j;++j)
  {
    coef = mat[0][j];

    if(coef > best_coef)
    {
      choose_leaving_row(&leaving_row,&distance,
		         mat,right_side_control,len_i,j);
      if(leaving_row == -1)
      {
        *pentering_col = j;
        *pleaving_row = -1;
        return;
      }

      ratio = mat[leaving_row][j]/max_abs_vect(mat[leaving_row],len_j);

      if(ratio >= CUTOFF_RATIO)   /* is this column numerically stable? */
      {
        best_coef = coef;
        best_col = j;
        best_leaving_row = leaving_row;
      }
    }
  }

  if(best_coef > 0.0)
  {
    *pentering_col = best_col;
    *pleaving_row = best_leaving_row;

    /* numerically stable improvement found, return */
    return;
  }

  /* count possible improvements */
  improve_count = 0;

  for(j=0;j<len_j;++j)
  {
    coef = mat[0][j];

    if(coef > 0.0)
    {
      ++improve_count;
    }
  }

  if(improve_count == 0)   /* no further improvements possible, optimum */
  {
    *pentering_col = -1;
    return;
  }

  /* choose good pivot, for numerical stability, ignore convergence rate */
  best_ratio = 0.0;

  for(j=0;j<len_j;++j)
  {
    coef = mat[0][j];

    if(coef > 0.0)
    {
      choose_leaving_row(&leaving_row,&distance,
			 mat,right_side_control,len_i,j);
      if(leaving_row == -1)
      {
	*pentering_col = j;
	*pleaving_row = -1;
	return;
      }

      ratio = mat[leaving_row][j]/max_abs_vect(mat[leaving_row],len_j);

      if(ratio > best_ratio)
      {
        best_ratio = ratio;

	*pentering_col = j;
	*pleaving_row = leaving_row;
      }
    }
  }

  if(best_ratio < QUIT_CUTOFF_RATIO)  /* numerically very unstable, quit */
  {
    ratio = mat[0][*pentering_col]/max_abs_vect(mat[0],len_j);

    if(ratio < QUIT_CUTOFF_RATIO)
    {
      *pentering_col = -1;     /* success */
    }
    else
    {
      *pleaving_row = -1;      /* unbounded */
    }
  }

  /*
  printf("*best_ratio = %lf\n",best_ratio);
  */
}


local void
eliminate_entering_col_from_all_other_rows(mat,right_side,right_side_control,
                                           leaving_row,entering_col,
                                           len_i,len_j)

double **mat,*right_side,*right_side_control;
int leaving_row,entering_col,len_i,len_j;

{
  int row;
  double self_multiplier,multiplier;

  self_multiplier = 1.0 / mat[leaving_row][entering_col];
  
  mat[leaving_row][entering_col] = 1.0;            /* new var in column */

  wn_mult_vect_by_scalar(mat[leaving_row],self_multiplier,len_j);
  right_side[leaving_row] *= self_multiplier;
  right_side_control[leaving_row] *= self_multiplier;

  for(row=0;row<len_i;row++)
  {
    if(row != leaving_row)
    {
      multiplier = -mat[row][entering_col];
  
      mat[row][entering_col] = 0.0;                /* new var in column */

      if(multiplier != 0.0)
      {
	wn_add_scaled_vect(mat[row],mat[leaving_row],multiplier,len_j);
        right_side[row] += (right_side[leaving_row]*multiplier); 
        right_side_control[row] += (right_side_control[leaving_row]*multiplier); 
      }
    }
  }
}


void wn_simplex_loop(pcode,
		     mat,right_side,right_side_control,
		     non_zero_vars,zero_vars,
		     len_i,len_j)

int *pcode;
double **mat,         /* mat[len_i][len_j] */
       *right_side,   /* right_side[len_i] */
       *right_side_control; /*right_side_control[len_i] */
int *non_zero_vars,   /* non_zero_vars[len_i] */
    *zero_vars;       /* zero_vars[len_j]  */
int len_i,len_j;

{
  int leaving_row,entering_col;
  int count;

  count = 0;

  while(TRUE)
  {
    /*
    printf("count = %d, ob = %lf,contrl_ob = %lf\n",
	   count,-right_side[0],-right_side_control[0]);
    fflush(stdout);
    */

    choose_col_and_row(&entering_col,&leaving_row,
		       mat,right_side_control,len_i,len_j);
    if(entering_col == -1)
    {
      *pcode = WN_SUCCESS;
      /*
      printf("success!!!!!!\n");
      */

      return;
    }
    if(leaving_row == -1)
    {
      *pcode = WN_UNBOUNDED;
      /*
      printf("unbounded.\n");
      */

      return;
    }

    eliminate_entering_col_from_all_other_rows(mat,
					       right_side,right_side_control,
                                               leaving_row,entering_col,
                                               len_i,len_j);
    ++count;

    wn_swap(non_zero_vars[leaving_row],zero_vars[entering_col],int);
  }
}





