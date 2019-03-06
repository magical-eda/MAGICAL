/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/********************************************************************
********************************************************************/
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnsqr.h"
#include "wnswap.h"
#include "wnmem.h"
#include "wnmemb.h"
#include "wnasrt.h"
#include "wnabs.h"
#include "wnmax.h"
#include "wnrnzsgn.h"
#include "wnrndd.h"
#include "wnrnd.h"

#include "wnvect.h"
#include "wnmat.h"

#include "wnconj.h"



#ifdef INFINITY
# undef INFINITY
#endif
#define INFINITY  WN_FHUGE

#define MAX_MAGNIFICATION  (100.0)



local bool force_stop_flag;



typedef struct context_struct
{
  wn_memgp this_group;

  double *vect;  /* from caller */
  int num_vars;
  double (*pfuncgrad)(double grad[],double vect[]);
  bool (*pterminate)(double vect[],double best_f);
  double first_jump_len;
  double last_jump_len;
  int depth,max_depth;
  int max_iterations;

  double *v_test,*g_test;
  double f_test;

  double *last_v_test,*last_g_test;
  double last_f_test;

  double *dir;
  double **s,**y;  
  double *alpha,*rho;
} cntxt_s,*cntxt;  


local bool is_valid_number(double x)
{
  return((-WN_FHUGE < x)&&(x < WN_FHUGE));
}


local bool default_terminate(double vect[],double best_f)
{
  return(FALSE);
}


local void initialize(cntxt c)
{
  wn_gpmake("no_free");
  c->this_group = wn_curgp();

  force_stop_flag = FALSE;

  c->v_test = c->vect;
  wn_make_vect(&(c->g_test),c->num_vars);
  c->f_test = WN_FHUGE;

  wn_make_vect(&(c->last_v_test),c->num_vars);
  wn_make_vect(&(c->last_g_test),c->num_vars);
  c->last_f_test = WN_FHUGE;

  wn_make_vect(&(c->dir),c->num_vars);

  wn_make_mat(&(c->s),c->max_depth,c->num_vars);
  wn_make_mat(&(c->y),c->max_depth,c->num_vars);

  wn_make_vect(&(c->alpha),c->max_depth);
  wn_make_vect(&(c->rho),c->max_depth);

  c->depth = 0;

  c->last_jump_len = 1.0;

  wn_gppop();
}


local void finish(cntxt c,int *pcode,double *pval_min)
{
  *pval_min = c->f_test;

  wn_copy_vect(c->vect,c->v_test,c->num_vars);

  if(!(*pval_min > -WN_FHUGE))
  {
    *pcode = WN_UNBOUNDED;
  }
  else if(force_stop_flag)
  {
    *pcode = WN_SUBOPTIMAL;
  }
  else
  {
    *pcode = WN_SUCCESS;
  }

  wn_gppush(c->this_group);
  wn_gpfree();
}


local double calc_first_step_mult(cntxt c,double *g)
{
  double norm2_g;
  double mult;

  norm2_g = wn_norm2_vect(g,c->num_vars);

  if(norm2_g == 0.0)
  {
    mult = 1.0;
  }
  else
  {
    mult = (c->first_jump_len)*sqrt(c->num_vars/norm2_g);
  }

  return(mult);
}


/*
  do steepest descent iteration, with step length "first_jump_len"
*/
local void do_first_funcgrad_calls(cntxt c)
{
  double mult;

  c->f_test = (*(c->pfuncgrad))(c->g_test,c->v_test);

  c->last_f_test = c->f_test;
  wn_swap(c->last_v_test,c->v_test,double *);  /* last_v_test <- v_test */
  wn_swap(c->last_g_test,c->g_test,double *);  /* last_g_test <- g_test */

  mult = calc_first_step_mult(c,c->last_g_test);
  wn_copy_vect(c->dir,c->last_g_test,c->num_vars);
  wn_scale_vect(c->dir,mult,c->num_vars);
}


local bool termination_condition(cntxt c)
{
#if 0
  if(force_stop_flag)
  {
    return(TRUE);
  }
  if((c->dotgg)[0][0] == 0.0)  /* unlikely */
  {
    return(TRUE);
  }
  if(c->iters_since_best_f0 > 10)  /* not improving any more */
  {
    return(TRUE);
  }
#endif

  return(FALSE);
}


local double signed_len(double sign, double len)
{
  /*
  if(sign < 0)
  {
    return(-len);
  }
  else if(sign > 0)
  {
    return(len);
  }
  else
  {
    if(wn_random_bit())
    {
      return(len);
    }
    else
    {
      return(-len);
    }
  }
  */

  return(len * (double)wn_sign_random_nonzero(sign));
}


local bool wolfe_condition_satisfied(double x0,double f0,double df0,
				     double xs,double fs,double dfs,
				     double c1,double c2)
{
  if(!(fs < f0-wn_abs(c1*(xs-x0)*df0)))
  {
    return(FALSE);
  }
  if(!(wn_abs(dfs) < c2*wn_abs(df0)))
  {
    return(FALSE);
  }

  return(TRUE);
}


local bool my_wolfe_condition_satisfied(double x0,double f0,double df0,
				        double xs,double fs,double dfs)
{
  return(wolfe_condition_satisfied(x0,f0,df0,xs,fs,dfs,1e-4,0.9));
  /*
  return(TRUE);
  */
}


local void do_linesearch(cntxt c)
{
  double orig_x0,orig_f0,orig_df0;
  double x0,f0,df0;
  double x1,f1,df1;
  double xs,fs,dfs;
  bool success;
  int code;
  double a,b;
  int count;

  /*
  */
  printf("linesearch ------------------\n");

  x0 = 0.0;
  f0 = c->last_f_test;
  df0 = wn_dot_vects(c->last_g_test,c->dir,c->num_vars); 

  orig_x0 = x0;
  orig_f0 = f0;
  orig_df0 = df0;

  /*
  x1 = -c->last_jump_len;
  */
  if(c->depth == 0)
  {
    x1 = -1.0;
  }
  else
  {
    x1 = -wn_dot_vects((c->s)[0],(c->y)[0],c->num_vars)/
	  wn_norm2_vect((c->y)[0],c->num_vars);
  }
  printf("norm2(y[0]) = %lg\n",wn_norm2_vect((c->y)[0],c->num_vars));
  printf("x1=%lg\n",x1);
  wn_add_vect_and_scaled_vect(c->v_test,c->last_v_test,c->dir,x1,
			      c->num_vars);
  c->f_test = (*(c->pfuncgrad))(c->g_test,c->v_test);
  f1 = c->f_test;
  df1 = wn_dot_vects(c->g_test,c->dir,c->num_vars);

  if(my_wolfe_condition_satisfied(x0,f0,df0,x1,f1,df1))
  {
    wn_fit_parabola_2d(&code,&a,&xs,x0,df0,x1,df1);
    if((code == WN_SUCCESS)&&(a > 0.0)&&
       (wn_abs(xs-x1)<=MAX_MAGNIFICATION*wn_abs(x0-x1)))
    {
      c->last_jump_len = wn_abs(xs);
    }
    else
    {
      c->last_jump_len = wn_abs(x1);
    }
    return;
  }

  count = 0;

  for(;;)   /* do until wolfe condition satisfied */
  {
    if(is_valid_number(f1))
    {
      wn_fit_parabola_2pd(&code,&a,&xs,&b,x0,f0,df0,x1,f1);
      success = (code == WN_SUCCESS);
    }
    else
    {
      success = TRUE;
      a = 1.0;
      xs = 0.5*(x0+x1);
      b = f0;
    }

    /* fit seems successful */
    if(success&&(a > 0.0)&&
       (wn_abs(xs-x1)<=MAX_MAGNIFICATION*wn_abs(x0-x1)))
    {
      /*
      printf("xs=%lg\n",xs);
      */
      if(wn_abs(xs-x0)<=(1.0/MAX_MAGNIFICATION)*wn_abs(x0-x1))
      {
        xs = signed_len(xs-x0,wn_abs(x0-x1)/MAX_MAGNIFICATION) + x0;
      }
      printf("xs=%lg\n",xs);

      wn_add_vect_and_scaled_vect(c->v_test,c->last_v_test,c->dir,xs,
			          c->num_vars);
      c->f_test = (*(c->pfuncgrad))(c->g_test,c->v_test);
      fs = c->f_test;
      dfs = wn_dot_vects(c->g_test,c->dir,c->num_vars);

      if(my_wolfe_condition_satisfied(orig_x0,orig_f0,orig_df0,xs,fs,dfs))
      {
        c->last_jump_len = wn_abs(xs);
	return;
      }
      else
      {
	if(f0 <= f1)
	{
          x1 = xs;
          f1 = fs;
	  df1 = dfs;
	}
	else /* f0 > f1 */
	{
	  if(fs < f0)
	  {
	    x0 = xs;
	    f0 = fs;
	    df0 = dfs;
	  }
	  else
	  {
	    if(wn_abs(xs-x0) <= wn_abs(xs-x1))
	    {
              x1 = xs;
              f1 = fs;
	      df1 = dfs;
	    }
	    else
	    {
	      x0 = xs;
	      f0 = fs;
	      df0 = dfs;
	    }
	  }
	}
      }
    }
    else  /* fit not successful: expand range */
    {
      xs = -signed_len(df0,MAX_MAGNIFICATION*wn_abs(x0-x1)) + x0;
      printf("xs=%lg\n",xs);

      wn_add_vect_and_scaled_vect(c->v_test,c->last_v_test,c->dir,xs,
			          c->num_vars);
      c->f_test = (*(c->pfuncgrad))(c->g_test,c->v_test);
      fs = c->f_test;
      dfs = wn_dot_vects(c->g_test,c->dir,c->num_vars);

      if(my_wolfe_condition_satisfied(orig_x0,orig_f0,orig_df0,xs,fs,dfs))
      {
        c->last_jump_len = wn_abs(xs);
	return;
      }
      else
      {
	if(wn_abs(xs-x0) <= wn_abs(xs-x1))
	{
          x1 = xs;
          f1 = fs;
	  df1 = dfs;
	}
	else
	{
	  x0 = xs;
	  f0 = fs;
	  df0 = dfs;
	}
      }
    }
  }
}


local void shift_state_vects(cntxt c,double **vects)
{
  double *save_last_vect;
  int i;

  save_last_vect = vects[c->max_depth-1];

  for(i=c->max_depth-1;i>=1;--i)
  {
    vects[i] = vects[i-1];
  }

  vects[0] = save_last_vect;
}


local void shift_state_vect(cntxt c,double vect[])
{
  double save_last_val;
  int i;

  save_last_val = vect[c->max_depth-1];

  for(i=c->max_depth-1;i>=1;--i)
  {
    vect[i] = vect[i-1];
  }

  vect[0] = save_last_val;
}


local void advance_state(cntxt c)
{
  if(c->depth < c->max_depth)
  {
    ++(c->depth);
  }

  shift_state_vects(c,c->s);
  shift_state_vects(c,c->y);

  shift_state_vect(c,c->rho);

  wn_copy_vect((c->s)[0],c->v_test,c->num_vars);
  wn_subtract_vect((c->s)[0],c->last_v_test,c->num_vars);

  wn_copy_vect((c->y)[0],c->g_test,c->num_vars);
  wn_subtract_vect((c->y)[0],c->last_g_test,c->num_vars);

  (c->rho)[0] = 1.0/wn_dot_vects((c->y)[0],(c->s)[0],c->num_vars);

  c->last_f_test = c->f_test;
  wn_swap(c->last_v_test,c->v_test,double *);  /* last_v_test <- v_test */
  wn_swap(c->last_g_test,c->g_test,double *);  /* last_g_test <- g_test */
}


local void compute_lbfgs_search_direction(cntxt c)
{
  int i;
  double beta;

  wn_copy_vect(c->dir,c->last_g_test,c->num_vars);

  for(i=0;i<c->depth;++i)
  {
    (c->alpha)[i] = (c->rho)[i]*wn_dot_vects((c->s)[i],c->dir,c->num_vars);
    wn_add_scaled_vect(c->dir,(c->y)[i],(c->alpha)[i],c->num_vars);
  }
  for(i=c->depth-1;i>=0;--i)
  {
    beta = (c->rho)[i]*wn_dot_vects((c->y)[i],c->dir,c->num_vars);
    wn_add_scaled_vect(c->dir,(c->s)[i],(c->alpha)[i]-beta,c->num_vars);
  }
}


EXTERN void wn_lbfgs_funcgrad_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  int num_vars,
  double (*pfuncgrad)(double grad[],double vect[]),
  bool (*pterminate)(double vect[],double best_f),
  double first_jump_len,
  int max_depth, 
  int max_iterations
)
{
  cntxt_s cntxt_mem,*c;
  int iteration;

  c = &cntxt_mem;
  wn_memzero((ptr)c,sizeof(cntxt_s));

  c->vect = vect;
  c->num_vars = num_vars;
  c->pfuncgrad = pfuncgrad;
  if(pterminate == NULL)
  {
    c->pterminate = &default_terminate;
  }
  else
  {
    c->pterminate = pterminate;
  }
  c->first_jump_len = first_jump_len;
  c->max_depth = max_depth;
  c->max_iterations = max_iterations;

  initialize(c);

  do_first_funcgrad_calls(c);

  for(iteration=0;;++iteration)
  {
    do_linesearch(c);

    if(termination_condition(c))
    {
      break;
    }
    if((iteration >= max_iterations)&&(max_iterations < WN_IHUGE))
    {
      force_stop_flag = TRUE;
      break;
    }

    advance_state(c);
    compute_lbfgs_search_direction(c);
  }

  finish(c,pcode,pval_min);
}

