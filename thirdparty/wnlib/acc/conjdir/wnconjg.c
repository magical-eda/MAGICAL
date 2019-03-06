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

void conj_gradient_method(pcode,pval_min,vect,
			  len,pfunction,pgradient,max_iterations)

********************************************************************/
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnswap.h"
#include "wnmem.h"
#include "wnasrt.h"
#include "wnabs.h"

#include "wnvect.h"

#include "wnconj.h"



#define GOLDEN_RATIO          1.618034
#define GOLDEN_SECTION        0.3819660

/* #define INFINITY  WN_FHUGE ** not used, is redefinition sometimes */



extern bool wn_force_optimize_stop_flag;

local wn_memgp old_group;

double *buffer_vect;

local double dy1,last_dy1;



void wn_force_conj_gradient_stop(void)
{
  wn_force_optimize_stop_flag = TRUE;
}


#if 0 /* not used, commented out ** bchapman - 041111 */
local bool vects_equal(double v1[],double v2[],int len)
{
  int i;

  for(i=0;i<len;++i)
  {
    if(v1[i] != v2[i])
    {
      return(FALSE);
    }
  }

  return(TRUE);
}
#endif /* 0 */


local bool parabola_fit_improvement_wrong(double y1,double y0,double b,
					  double tolerance)
{
  double expected_improvement;

  expected_improvement = y1-b;
  if(!(expected_improvement > 0.0))  /* weird form to handle Nan problems */
  {
    return(TRUE);
  }

  return(
	  /* not enough improvement */
	  (y0 > y1-(1.0-tolerance)*expected_improvement) 
	    ||
	  /* too much improvement */
	  (y0 < y1-(1.0+tolerance)*expected_improvement) 
        );
#if 0
  return(
	  (y0 > y1-0.50*expected_improvement)  /* not enough improvement */
	    ||
	  (y0 < y1-1.50*expected_improvement)  /* too much improvement */
        );
  return(
	  (y0 > y1-0.75*expected_improvement)  /* not enough improvement */
	    ||
	  (y0 < y1-1.25*expected_improvement)  /* too much improvement */
        );
  return(TRUE);
  return(
	  (y0 > y1-0.99*expected_improvement)  /* not enough improvement */
	    ||
	  (y0 < y1-1.01*expected_improvement)  /* too much improvement */
        );
#endif
}


local void eval_function
(
  double *pval,
  double vect[],
  double direction[],
  double x,
  int len,
  double (*pfunction)(double vect[])
)
{
  wn_add_vect_and_scaled_vect(buffer_vect,vect,direction,x,len);

  *pval = (*pfunction)(buffer_vect);
}


local double *save_vect,*save_direction;
local int save_len;
local double (*save_pfunction)(double vect[]);

local double simple_eval_function(double x)
{
  double ret;

  eval_function(&ret,save_vect,save_direction,x,save_len,save_pfunction);

  return(ret);
}


local bool x0_too_far_out(double x0,double x2,double threshold)
{
  wn_assert(threshold > 1.0);

  if(x2 > 0.0)
  {
    return(x0 > x2*threshold);
  }
  else
  {
    wn_assert(x2 < 0.0);  /* x2 == 0 not allowed */

    return(x0_too_far_out(-x0,-x2,threshold));
  }
}


local bool x0_too_far_in(double x0,double x2,double threshold)
{
  wn_assert(threshold < 1.0);

  if(x2 > 0.0)
  {
    return(x0 < x2*threshold);
  }
  else
  {
    wn_assert(x2 < 0.0);  /* x0 == 0 not allowed */

    return(x0_too_far_in(-x0,-x2,threshold));
  }
}


local void line_minimize
(
  int *pcode,
  bool *pfirst_parabolic_fit_succeeded,
  double *pval,
  double *pjump_len,
  double vect[],
  double direction[],
  double last_val,
  double last_g[],
  double last_jump_len,
  int len,
  double (*pfunction)(double vect[])
)
{
  double a,x0,y0,b,x1,y1,x2,y2;
  int code;

  *pcode = WN_SUCCESS;

  x1 = 0.0;
  y1 = last_val;
  dy1 = wn_dot_vects(direction,last_g,len);

  if(last_jump_len == 0.0)
  {
    last_jump_len = 1.0;
  }
  last_jump_len = -wn_sign(dy1)*wn_abs(last_jump_len);
  x2 = last_jump_len;

  eval_function(&y2,vect,direction,x2,len,pfunction);

  wn_fit_parabola_2pd(&code,&a,&x0,&b,x1,y1,dy1,x2,y2);

  /* look for excuses to say that parabolic fit is no good */
  if(code != WN_SUCCESS)
  {
    /*
    printf("parabola fit failed - probably a line.\n");
    */
    x0 = x2+GOLDEN_RATIO*x2;  /* project outward */
    eval_function(&y0,vect,direction,x0,len,pfunction);
    goto parabolic_fit_failed;
  }
  if(!(a > 0))
  {
    /*
    printf("downward facing parabola.\n");
    */
    x0 = x2+GOLDEN_RATIO*x2;  /* project outward */
    eval_function(&y0,vect,direction,x0,len,pfunction);
    goto parabolic_fit_failed;
  }
  if(!(wn_abs(x0) < 10000.0*wn_abs(x2)))
  {
    /*
    printf("x0 too far out.\n");
    */
    x0 = 10000.0*x2;  /* project outward */
    eval_function(&y0,vect,direction,x0,len,pfunction);
    goto parabolic_fit_failed;
  }
  if(!(wn_abs(x0) > (1.0/10000.0)*wn_abs(x2)))
  {
    /*
    printf("x0 too far in.\n");
    */
    x0 = (1.0/10000.0)*x2;  /* project inward */
    eval_function(&y0,vect,direction,x0,len,pfunction);
    goto parabolic_fit_failed;
  }
  if(!(b < y1))  /* no improvement expected,weird form for Nan problems */
  {
    /*
    printf("no improvement expected.\n");
    */
    x0 = GOLDEN_SECTION*x2;
    eval_function(&y0,vect,direction,x0,len,pfunction);
    goto parabolic_fit_failed;
  }

  eval_function(&y0,vect,direction,x0,len,pfunction);

  if(parabola_fit_improvement_wrong(y1,y0,b,0.25))
  {
    /*
    printf("poor parabola fit detected.\n");
    */
    goto parabolic_fit_failed;
  }

  /* parabolic fit succeeded */

  if(y0 > y1)
  {
    x0 = x1;
    y0 = y1;
  }
  *pval = y0;
  *pjump_len = x0;
  wn_copy_vect(vect,buffer_vect,len);

  *pfirst_parabolic_fit_succeeded = TRUE;
  /*
  *pfirst_parabolic_fit_succeeded = 
	 !parabola_fit_improvement_wrong(y1,y0,b,0.25);
  */

  return;

parabolic_fit_failed:
  *pfirst_parabolic_fit_succeeded = FALSE;

  save_vect = vect;
  save_direction = direction;
  save_len = len;
  save_pfunction = pfunction;

  wn_minimize_1d_raw(pcode,&y1,&y0,&y2,&x1,&x0,&x2,y1,(simple_eval_function),
		     3,20);
  if(!((*pcode == WN_SUCCESS)||(*pcode == WN_SUBOPTIMAL)))
  {
    return;
  }
  *pcode = WN_SUCCESS;

  if(y0 <= last_val)
  {
    *pval = y0;
    *pjump_len = x0;
    wn_add_scaled_vect(vect,direction,x0,len);
  }
  else
  {
    *pval = last_val;
    *pjump_len = 0.0;
  }

  return;
}


EXTERN void wn_conj_gradient_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  int len,
  double (*pfunction)(double vect[]),
  void (*pgradient)(double grad[],double vect[]),
  int max_iterations
)
{
  int iteration,no_move_count;
  int stable_satisfy_count;
  double norm2_g,norm2_last_g,g_dot_last_g,val,last_val,beta,
	 jump_len,last_jump_len,alpha;
  double *g,*last_g,*direction;
  bool function_free_method,last_was_function_free_method;
  bool first_parabolic_fit_succeeded;
  double computed_g_dot_dlast;

  old_group = wn_curgp();
  wn_gpmake("no_free");

  wn_force_optimize_stop_flag = FALSE;

  wn_make_vect(&buffer_vect,len);
  wn_make_vect(&g,len);
  wn_make_vect(&last_g,len);
  wn_make_vect(&direction,len);

  function_free_method = FALSE;
  stable_satisfy_count = 0;
  last_was_function_free_method = FALSE;
  jump_len = 1.0;
  no_move_count = 0;
  beta = 0.0;

  wn_gppush(old_group);

  val = (*pfunction)(vect);
  (*pgradient)(g,vect);

  wn_copy_vect(direction,g,len);

  norm2_g = wn_norm2_vect(g,len);

  for(iteration=0;;++iteration)
  {
    last_dy1 = dy1;
    last_jump_len = jump_len;
    wn_swap(last_g,g,double *);  /* move g to last g */
    norm2_last_g = norm2_g;

    if(function_free_method)
    {
      double x2;
      double g0_dot_d0,g1s_dot_d0,dot_diff;

      dy1 = wn_dot_vects(direction,last_g,len);

      last_jump_len = -wn_sign(dy1)*wn_abs(last_jump_len);
      x2 = last_jump_len;

      /*
      printf("last_jump_len = %lg\n",last_jump_len);
      */

      wn_add_vect_and_scaled_vect(buffer_vect,vect,direction,last_jump_len,len);

      (*pgradient)(g,buffer_vect);

      /*
      g0_dot_d0 = wn_dot_vects(last_g,direction,len);
      */
      g0_dot_d0 = dy1;
      g1s_dot_d0 = wn_dot_vects(g,direction,len);
      dot_diff = g0_dot_d0-g1s_dot_d0;
     
      if(!(dot_diff > 0.0))  /* not upward facing parabola */
      {
        stable_satisfy_count = 0;
	goto function_based_method;
      }

      alpha = g0_dot_d0/dot_diff;
      /*
      printf("alpha = %lg\n",alpha);
      */

      if(!(((1.0-10000.0) < alpha)&&(alpha < (1.0+10000.0))))
      {
        stable_satisfy_count = 0;
	goto function_based_method;
      }

      jump_len = alpha*last_jump_len;

      /* g[j] = alpha*g[j] + (1.0-alpha)*last_g[j]; */
      wn_scale_vect(g,alpha,len);
      wn_add_scaled_vect(g,last_g,1.0-alpha,len);

      g_dot_last_g = wn_dot_vects(g,last_g,len);
      if(beta != 0.0)
      {
	computed_g_dot_dlast = -g_dot_last_g/beta;
	/*
	printf("computed_g_dot_dlast=%lg,last_dy1=%lg\n",
	       computed_g_dot_dlast,last_dy1);
	*/
	if(!(wn_abs(computed_g_dot_dlast) < 0.4*wn_abs(last_dy1)))
	{
          stable_satisfy_count = 0;
	  goto function_based_method;
        }
      }

      wn_add_scaled_vect(vect,direction,alpha*last_jump_len,len);

      if(
          wn_force_optimize_stop_flag
	    ||
	  ((max_iterations < WN_IHUGE)&&(iteration >= max_iterations))
        )
      {
        wn_force_optimize_stop_flag = FALSE;
        wn_gppop();
        wn_gpfree();
        val = (*pfunction)(vect);
        *pval_min = val;
        *pcode = WN_SUBOPTIMAL;
        return;
      }
    }
    else  /* function based method */
    {
      function_based_method: 

      if(last_was_function_free_method)
      {
        /* set so that next iteration will succeed */
        val = (*pfunction)(vect);  
      }
      function_free_method = FALSE;

      if(norm2_last_g == 0.0)   /* unlikely */
      {
        wn_gppop();
        wn_gpfree();
        *pval_min = val;
        *pcode = WN_SUCCESS;
        return;
      }

      last_val = val;

      line_minimize(pcode,&first_parabolic_fit_succeeded,&val,&jump_len,
		    vect,
		    direction,last_val,last_g,last_jump_len,len,pfunction);
      if(*pcode != WN_SUCCESS)
      {
        wn_gppop();
        wn_gpfree();
        *pval_min = val;
        return;
      }

      if(
	  wn_force_optimize_stop_flag
	    ||
	  ((max_iterations < WN_IHUGE)&&(iteration >= max_iterations))
        )
      {
        wn_force_optimize_stop_flag = FALSE;
        wn_gppop();
        wn_gpfree();
        *pval_min = val;
        *pcode = WN_SUBOPTIMAL;
        return;
      }
      wn_assert(val <= last_val);
      if(val == last_val)
      {
        if(no_move_count >= 2)
        {
          wn_gppop();
          wn_gpfree();
          *pval_min = val;
          *pcode = WN_SUCCESS;
          return;
        }
        else
        {
	  ++no_move_count;
	  jump_len = last_jump_len;
        }
      }
      else
      {
        no_move_count = 0;
      }

      (*pgradient)(g,vect);
      g_dot_last_g = wn_dot_vects(g,last_g,len);

      if((!first_parabolic_fit_succeeded)||(last_jump_len == 0.0))
      {
        stable_satisfy_count = 0;
	goto no_function_method_test_fail;
      }

      alpha = jump_len/last_jump_len;

      if(!(((1.0-3000.0) < alpha)&&(alpha < (1.0+3000.0))))
      {
        stable_satisfy_count = 0;
	goto no_function_method_test_fail;
      }

      if(beta != 0.0)
      {
	computed_g_dot_dlast = -g_dot_last_g/beta;
	/*
	printf("computed_g_dot_dlast=%lg,last_dy1=%lg\n",
	       computed_g_dot_dlast,last_dy1);
	*/
	if(!(wn_abs(computed_g_dot_dlast) < 0.2*wn_abs(last_dy1)))
	{
          stable_satisfy_count = 0;
	  goto no_function_method_test_fail;
        }
      }

      ++stable_satisfy_count;
      if(stable_satisfy_count > 3)
      {
        function_free_method = TRUE;
      }

      no_function_method_test_fail: ;
    }

    norm2_g = wn_norm2_vect(g,len);
    /*
    g_dot_last_g = wn_dot_vects(g,last_g,len);
    */

    beta = (norm2_g - g_dot_last_g)/norm2_last_g;

    wn_add_vect_and_scaled_vect(direction,g,direction,beta,len);

    /*
    printf("norm(g) = %lg\n",wn_norm_vect(g,len));
    printf("ob = %lg,beta = %lg,numerator=%lg,denom=%lg,norm2(direction)=%lg\n",
	   val,beta,numerator,norm2_last_g,wn_norm2(direction));
    printf("iteration = %d,ob = %lg\n",iteration,val);
    */

    last_was_function_free_method = function_free_method;
  }
}


