/**********************************************************************

wn_simplex_method(&code,&objective,&shadow_prices,&solution,
                  objective_vect,mat,right_side)
wn_simplex_method_raw(&code,&objective,raw_shadow_prices,raw_solution,
                      raw_objective_vect,raw_mat,raw_right_side,len_i,len_j)

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



wn_simplex_method(pcode,pobjective,pshadow_prices,psolution,
                  objective_vect,mat,right_side)

int *pcode;
double *pobjective;
wn_vector *pshadow_prices,*psolution,objective_vect;
wn_matrix mat;
wn_vector right_side;

{
  int len_i,len_j;
  double *raw_shadow_prices,*raw_objective_vect;

  wn_assert(mat->len_i == right_side->len_i);

  len_i = mat->len_i;
  len_j = mat->len_j;

  if(pshadow_prices != NULL)
  {
    wn_make_vector(pshadow_prices,len_j);
    raw_shadow_prices = (*pshadow_prices)->numbers;
  }
  else
  {
    raw_shadow_prices = NULL;
  }
  wn_make_vector(psolution,len_j);

  if(objective_vect != NULL)
  {
    wn_assert(objective_vect->len_i == len_j);
    raw_objective_vect = objective_vect->numbers;
  }
  else
  {
    raw_objective_vect = NULL;
  }

  wn_simplex_method_raw(pcode,pobjective,raw_shadow_prices,
                        (*psolution)->numbers,raw_objective_vect,
                        mat->numbers,right_side->numbers,len_i,len_j);
}



static double **mat,*right_side,
              *original_objective_row;
static int *non_zero_vars,*zero_vars;



wn_simplex_method_raw(pcode,pobjective,raw_shadow_prices,raw_solution,
                      raw_objective_vect,raw_mat,raw_right_side,len_i,len_j)

int *pcode;
double *pobjective;
double *raw_shadow_prices,*raw_solution,
       *raw_objective_vect,**raw_mat,*raw_right_side;
int len_i,len_j;

{
  wn_gpmake("no_free");
  wn_gplabel("simplex method group");

  initialize(raw_objective_vect,len_i,len_j);

  find_feasible_solution(pcode,raw_mat,raw_right_side,len_i,len_j);
  if(*pcode != WN_SUCCESS)
  {
    wn_gpfree();
    return;
  }

  wn_raw_simplex_loop(pcode,mat,right_side,non_zero_vars,zero_vars,
                      len_i+1,len_j-len_i);
  if(*pcode != WN_SUCCESS)
  {
    wn_gpfree();
    return;
  }

  copy_out_results(pobjective,raw_shadow_prices,raw_solution,len_i,len_j);

  wn_gpfree();
}



local initialize(raw_objective_vect,len_i,len_j)

double *raw_objective_vect;
int len_i,len_j;

{
  wn_make_raw_matrix(&mat,len_i+2,len_j);
  wn_make_raw_vector(&right_side,len_i+2);
  wn_make_raw_vector(&original_objective_row,len_j);

  non_zero_vars = (int *)wn_zalloc((len_i+2)*wn_sizeof(int));
  zero_vars = (int *)wn_zalloc(len_j*wn_sizeof(int));

  if(raw_objective_vect != NULL)
  {
    wn_copy_raw_vector(original_objective_row,raw_objective_vect,len_j);
  }
  else
  {
    wn_zero_raw_vector(original_objective_row,len_j);
  }
}



local find_feasible_solution(pcode,raw_mat,raw_right_side,len_i,len_j)

int *pcode;
double **raw_mat,*raw_right_side;
int len_i,len_j;

{
  wn_copy_raw_vector(right_side+1,raw_right_side,len_i);
  wn_copy_raw_matrix(mat+1,raw_mat,len_i,len_j);

  wn_copy_raw_vector(mat[len_i+1],original_objective_row,len_j);
  right_side[len_i+1] = WN_FHUGE;  /* no constraint on objective function */

  make_right_side_non_negative(len_i,len_j);

  make_slack_objective(len_i,len_j);

  set_up_zero_vars(len_i,len_j);

  wn_raw_simplex_loop(pcode,mat,right_side,non_zero_vars,zero_vars,
                      len_i+2,len_j);
  wn_assert(*pcode == WN_SUCCESS);

  if(not(solution_feasible(len_i)))
  {
    *pcode = WN_INFEASIBLE;
  }
  else
  {
    remove_slacks(len_i,len_j);
    copy_back_objective(len_i,len_j);
  }
}



local make_right_side_non_negative(len_i,len_j)

int len_i,len_j;

{
  int i;

  for(i=1;i<(len_i+1);i++)
  {
    if(right_side[i] < 0.0)
    {
      wn_multiply_into_raw_vector_scalar(mat[i],-1.0,len_j);
      right_side[i] *= -1.0;
    }
  }
}



local make_slack_objective(len_i,len_j)

int len_i,len_j;

{
  int i;

  wn_zero_raw_vector(mat[0],len_j);
  right_side[0] = 0.0;

  for(i=1;i<(len_i+1);i++)   /* add rows to slack objective */
  {
    wn_add_into_raw_vector_raw_vector(mat[0],mat[i],len_j);
  }
}
    


local set_up_zero_vars(len_i,len_j)

int len_i,len_j;

{
  int i,j;

  for(j=0;j<len_j;j++)
  {
    zero_vars[j] = j; 
  }

  non_zero_vars[0] = -1;     /* slack objective */

  for(i=1;i<(len_i+1);i++)
  {
    non_zero_vars[i] = -1;   /* slacks */
  }

  non_zero_vars[len_i+1] = len_j;   /* objective */
}



local bool solution_feasible(len_i)

int len_i;

{
  int i;

  for(i=1;i<(len_i+1);i++)
  {
    if(non_zero_vars[i] == -1)
    {
      return(FALSE);     /* not all slacks removed */
    }
  }

  return(TRUE);
}



local remove_slacks(len_i,len_j)

int len_i,len_j;

{
  int start_j,end_j;

  end_j = len_j;
  start_j = 0;

  while(TRUE)
  {
    if((start_j >= end_j)||(start_j >= len_j))
    {
      break;
    }

    if(zero_vars[start_j] == -1)
    {
      end_j--;

      copy_mat_column(start_j,end_j,len_i);
    }
    else
    {
      start_j++;  
    }
  }
}



local copy_mat_column(to,from,len_i)

int to,from,len_i;

{
  int i;

  zero_vars[to] = zero_vars[from];

  for(i=0;i<(len_i+2);i++)
  {
    mat[i][to] = mat[i][from];
  }
}



local copy_back_objective(len_i,len_j)

int len_i,len_j;

{
  double objective;

  wn_copy_raw_vector(mat[0],mat[len_i+1],len_j);
  non_zero_vars[0] = non_zero_vars[len_i+1];

  calculate_objective(&objective,len_i);

  right_side[0] = -objective;
}



local calculate_objective(pobjective,len_i)

double *pobjective;
int len_i;

{
  double sum,weight;
  int i;

  sum = 0.0;

  for(i=1;i<(len_i+1);i++)
  {
    weight = original_objective_row[non_zero_vars[i]];

    sum += (right_side[i] * weight);
  }

  *pobjective = sum;
}



local copy_out_results(pobjective,raw_shadow_prices,raw_solution,len_i,len_j)

double *pobjective;
double *raw_shadow_prices,*raw_solution;
int len_i,len_j;

{
  int i,j,var,len_j_new;

  len_j_new = len_j-len_i;

  *pobjective = -right_side[0];

  if(raw_shadow_prices != NULL)
  {
    wn_zero_raw_vector(raw_shadow_prices,len_j);  

    for(j=0;j<len_j_new;j++)
    {
      var = zero_vars[j];

      raw_shadow_prices[var] = -mat[0][j];
    }
  }

  wn_zero_raw_vector(raw_solution,len_j);  

  for(i=1;i<(len_i+1);i++)
  {
    var = non_zero_vars[i];

    raw_solution[var] = right_side[i];
  }
}




