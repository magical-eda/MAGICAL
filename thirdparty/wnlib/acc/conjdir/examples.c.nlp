#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnsll.h"

#include "wnnlp.h"



/**********************************************************************
  This file uses the "wnnlp" non-linear constrained optimization package 
  to solve the minimum area buffering problem.  The problem
  formulation is below.
***********************************************************************

  PROBLEM FORMULATION:

    CHOOSE w[i] (1 <= i <= num_vars) TO MINIMIZE

      sum_over(i){ w[i] }

    WHILE SATISFYING THE CONSTRAINTS

      for_all(i) { w[i] >= 0 } 

      sum_over(0<=i<=NUM_VARIABLE_BUFFERS){ w[i+1]/w[i] } <= MAX_DELAY

      w[0] == DRIVER_BUFFER_SIZE

      w[NUM_VARIABLE_BUFFERS+1] == RECEIVER_BUFFER_SIZE

**********************************************************************/
#define NUM_VARIABLE_BUFFERS  1   /* in addition, 1 driver buffer, 
				     1 receiver buffer of fixed size */
#define MAX_DELAY             20.0
#define DRIVER_BUFFER_SIZE    1.0
#define RECEIVER_BUFFER_SIZE  20.0


local int num_vars;
local double *w_array;


/*
  It is necessary to use this routine instead of a true
  divide because 

    num/denom --> -infinity

  as denom -> 0- from the negative direction (assuming num > 0)
  and a negative infinity objective function attracts the
  minimizer and confuses the minimizer
*/
local double divide(double num,double denom)
{
  if(denom <= 0.0)
  {
    return(WN_FHUGE);
  }
  else
  {
    if(num < 0.0)
    {
      num = 0.0;
    }

    return(num/denom);
  }
}


local double delay(double w[],int num_bufs)
{
  int i;
  double stage_delay,total_delay;

  total_delay = 0.0;

  for(i=0;i<num_bufs-1;++i)
  {
    /*
    Using arithmetic divide will cause convergence problems
    stage_delay = w[i+1]/w[i];
    */
    stage_delay = divide(w[i+1],w[i]); /* special divide, see comment above */
    total_delay += stage_delay;
  }

  return(total_delay);
}


local double delay_constraint_func(int size,double *values,ptr client_data)
{
  int i;

  /* Copy the current solution values into the w_array and pass the
     w_array to delay(). This is done only to make the programming of the
     the delay function simpler.
  */
  for(i=0;i<num_vars;++i)
  {
    w_array[i+1] = values[i];
  }
  return(delay(w_array,num_vars+2) - MAX_DELAY);
}


local double area(double w[],int num_bufs)
{
  int i;
  double total_area;

  total_area = 0.0;

  for(i=1;i<num_bufs-1;++i)
  {
    total_area += w[i];
  }

  return(total_area);
}


int main(void)
{
  wn_sll constraint_list;
  wn_linear_constraint_type linear_constraint,objective;
  wn_nonlinear_constraint_type nonlinear_constraint;
  int i;
  int code;
  double val_min;
  double *vect,*delta_vect;

  wn_gpmake("no_free");

  num_vars = NUM_VARIABLE_BUFFERS;

  /* Initialize the w_array to contain the widths of the buffers in the chain
     with constant buffer widths at the start and end 
     w[0] == DRIVER_BUFFER_SIZE 
     w[NUM_VARIABLE_BUFFERS+1] == RECEIVER_BUFFER_SIZE
     and initialize the other widths to some reasonable value.
  */ 
  wn_make_vect(&w_array,num_vars+2);
  w_array[0] = DRIVER_BUFFER_SIZE; 
  w_array[num_vars+1] = RECEIVER_BUFFER_SIZE;

  for(i=1;i<num_vars+1;++i)
  {
    w_array[i] = 1.0;
  }
  wn_make_vect(&vect,num_vars);

  /* Create the solution vector which is passed to the minimizer */
  wn_copy_vect(vect,w_array+1,num_vars);

  /* In this example - let the minimizer compute gradients - so need to create 
     the delta vector. Generally this is a good idea to avoid programming 
     errors in the partial derivative functions.
  */
  wn_make_vect(&delta_vect,num_vars);
  for(i=0;i<num_vars;++i)
  {
    delta_vect[i] = 0.0001;
  }

  /* Create objective function to  MINIMIZE ( sum(i){ w[i] } ) */
  wn_make_linear_constraint(&linear_constraint,num_vars,0.0,WN_EQ_COMPARISON);
  for(i=0;i<num_vars;++i)
  {
    (linear_constraint->vars)[i] = i;
    (linear_constraint->weights)[i] = 1.0;
  }
  objective = linear_constraint;

  /* Create the constraints and add them to the list */ 
  constraint_list = NULL;

  /* Create constraint #1: for_all(i) { w[i] >= 0 } */ 
  for(i=0;i<num_vars;++i)
  {
    wn_make_linear_constraint(&linear_constraint,1,0.0,WN_GT_COMPARISON);
    (linear_constraint->vars)[0] = i;
    (linear_constraint->weights)[0] = 1.0;
    wn_sllins(&constraint_list,linear_constraint);
  }

  /* Create constraint #2: 
     sum_over(0<=i<=NUM_VARIABLE_BUFFERS){ w[i+1]/w[i] } <= MAX_DELAY 
  */
  wn_make_nonlinear_constraint(&nonlinear_constraint,num_vars,WN_LT_COMPARISON);
  nonlinear_constraint->pfunction = &delay_constraint_func; 
  for(i=0;i<num_vars;++i)
  {
    (nonlinear_constraint->vars)[i] = i;
  }
  wn_sllins(&constraint_list,nonlinear_constraint);

  /* Minimize */
  wn_nlp_conj_method(&code,&val_min,vect,
		     delta_vect,
		     (wn_nonlinear_constraint_type)objective,constraint_list,
		     num_vars,
		     /*conj_iterations*/ 2*num_vars,
		     /*offset_iterations*/ 200,
	             /*offset_adjust_rate*/ 1.0);

  wn_copy_vect(w_array+1,vect,num_vars);
  wn_print_vect(w_array,num_vars+2);
  printf("delay = %lg,area = %lg\n",
	 delay(w_array,num_vars+2),area(w_array,num_vars+2));

  wn_gpfree();
}

