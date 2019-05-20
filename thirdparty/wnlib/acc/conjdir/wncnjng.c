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
#include "wnrndd.h"
#include "wnrnd.h"

#include "wnvect.h"

#include "wnconj.h"



#define INFINITY  WN_FHUGE

#define MAX_MAGNIFICATION  (100.0)



extern bool wn_force_optimize_stop_flag;



typedef struct context_struct
{
  wn_memgp this_group;

  double *vect;  /* from caller */
  int num_vars;
  double (*pfuncgrad)(double grad[],double vect[]);
  bool (*pterminate)(double vect[],double best_f);
  double first_jump_len;
  int max_iterations;

  double *v_test,*g_test;
  double f_test;

  double *(v[3]),*(g[3]);
  double f[3];
  double dotvv[3][3],dotgg[3][3],dotvg[3][3];

  double **local_vs;  /* local_vs[3][3] */
  double **local_gs;  /* local_gs[3][3] */
  double **m;  /* m[3][3] */
  double **eigen_vects;  /* eigen_vects[3][3] */
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
  int i;

  wn_gpmake("no_free");
  c->this_group = wn_curgp();

  wn_force_optimize_stop_flag = FALSE;

  c->v_test = c->vect;
  wn_make_vect(&(c->g_test),c->num_vars);

  for(i=0;i<3;++i)
  {
    wn_make_vect(&(c->v)[i],c->num_vars);
    wn_make_vect(&(c->g)[i],c->num_vars);

    (c->f)[i] = WN_FHUGE;
  }

  wn_make_mat(&(c->m),3,3);
  wn_make_mat(&(c->eigen_vects),3,3);

  wn_gppop();
}


local void finish(cntxt c,int *pcode,double *pval_min)
{
  int best_index;

  *pval_min = (c->f)[0];

  wn_copy_vect(c->vect,(c->v)[0],c->num_vars);

  if(!(*pval_min > -WN_FHUGE))
  {
    *pcode = WN_UNBOUNDED;
  }
  else if(wn_force_optimize_stop_flag)
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


local void swap_tests(cntxt c,int i1,int i2)
{
  int i;

  if(i1 == i2)
  {
    return;
  }

  wn_swap((c->v)[i1],(c->v)[i2],double *);
  wn_swap((c->g)[i1],(c->g)[i2],double *);
  wn_swap((c->f)[i1],(c->f)[i2],double);

  wn_swap((c->dotvv)[i1][i1],(c->dotvv)[i2][i2],double);
  wn_swap((c->dotgg)[i1][i1],(c->dotgg)[i2][i2],double);
  wn_swap((c->dotvg)[i1][i1],(c->dotvg)[i2][i2],double);
  wn_swap((c->dotvg)[i1][i2],(c->dotvg)[i2][i1],double);

  for(i=0;i<3;++i)
  {
    if((i != i1)&&(i != i2))
    {
      wn_swap((c->dotvv)[i][i1],(c->dotvv)[i][i2],double);
      wn_swap((c->dotvv)[i1][i],(c->dotvv)[i2][i],double);
      wn_swap((c->dotgg)[i][i1],(c->dotgg)[i][i2],double);
      wn_swap((c->dotgg)[i1][i],(c->dotgg)[i2][i],double);

      wn_swap((c->dotvg)[i][i1],(c->dotvg)[i][i2],double);
      wn_swap((c->dotvg)[i1][i],(c->dotvg)[i2][i],double);
    }
  }
}


local void sort_tests(cntxt c)
{
  if((c->f)[0] > (c->f)[2])
  {
    swap_tests(c,0,2);
  }

  if((c->f)[1] > (c->f)[2])
  {
    swap_tests(c,1,2);
  }
  else if((c->f)[0] > (c->f)[1])
  {
    swap_tests(c,0,1);
  }
}


local void insert_new_test(cntxt c)
{
  int i;

  wn_assert(c->f_test < (c->f)[2]);

  if((c->f_test < (c->f)[0]))
  {
    if((*(c->pterminate))(c->v_test,c->f_test))
    {
      wn_force_optimize_stop_flag = TRUE;
    }
  }

  wn_swap((c->v)[2],c->v_test,double *);  /* v[2] <- v_test */
  wn_swap((c->g)[2],c->g_test,double *);  /* g[2] <- g_test */
  (c->f)[2] = c->f_test;                  /* f[2] <- f_test */

  for(i=0;i<2;++i)
  {
    (c->dotvv)[i][2] = (c->dotvv)[2][i] = 
	wn_dot_vects((c->v)[2],(c->v)[i],c->num_vars);
    (c->dotgg)[i][2] = (c->dotgg)[2][i] = 
	wn_dot_vects((c->g)[2],(c->g)[i],c->num_vars);

    (c->dotvg)[i][2] = 
	wn_dot_vects((c->v)[i],(c->g)[2],c->num_vars);
    (c->dotvg)[2][i] = 
	wn_dot_vects((c->v)[2],(c->g)[i],c->num_vars);
  }

  (c->dotvv)[2][2] = 
	wn_dot_vects((c->v)[2],(c->v)[2],c->num_vars);
  (c->dotgg)[2][2] = 
	wn_dot_vects((c->g)[2],(c->g)[2],c->num_vars);

  (c->dotvg)[2][2] = 
	wn_dot_vects((c->v)[2],(c->g)[2],c->num_vars);

  sort_tests(c);
}


local double calc_first_step_mult(cntxt c,double *g)
{
  double norm2_g;
  double mult;

  norm2_g = wn_norm2_vect(g,c->num_vars);

  if(norm2_g == 0.0)
  {
    mult = -1.0;
  }
  else
  {
    mult = -(c->first_jump_len)*sqrt(c->num_vars/norm2_g);
  }

  return(mult);
}


local void initialize_dots(cntxt c)
{
  int i1,i2;

  for(i1=0;i1<3;++i1)
  for(i2=0;i2<3;++i2)
  {
    (c->dotvg)[i1][i2] = 
	  wn_dot_vects((c->v)[i1],(c->g)[i2],c->num_vars);
  }

  for(i1=0;i1<3;++i1)
  {
    for(i2=0;i2<i1;++i2)
    {
      (c->dotvv)[i1][i2] = (c->dotvv)[i2][i1] = 
	  wn_dot_vects((c->v)[i1],(c->v)[i2],c->num_vars);
      (c->dotgg)[i1][i2] = (c->dotgg)[i2][i1] = 
	  wn_dot_vects((c->g)[i1],(c->g)[i2],c->num_vars);
    }

    (c->dotvv)[i1][i1] = 
        wn_dot_vects((c->v)[i1],(c->v)[i1],c->num_vars);
    (c->dotgg)[i1][i1] = 
        wn_dot_vects((c->g)[i1],(c->g)[i1],c->num_vars);
  }
}


/*
  do 2 steepest descent iterations, with step length "first_jump_len"
*/
local void do_first_3_funcgrad_calls(cntxt c)
{
  double mult;

  wn_copy_vect((c->v)[0],c->vect,c->num_vars);

  (c->f)[0] = (*(c->pfuncgrad))((c->g)[0],(c->v)[0]);

  mult = calc_first_step_mult(c,(c->g)[0]);
  wn_add_vect_and_scaled_vect((c->v)[1],(c->v)[0],(c->g)[0],mult,c->num_vars);

  (c->f)[1] = (*(c->pfuncgrad))((c->g)[1],(c->v)[1]);

  mult = calc_first_step_mult(c,(c->g)[1]);
  wn_add_vect_and_scaled_vect((c->v)[2],(c->v)[1],(c->g)[1],mult,c->num_vars);

  (c->f)[2] = (*(c->pfuncgrad))((c->g)[2],(c->v)[2]);

  initialize_dots(c);

  sort_tests(c);
}


local bool termination_condition(cntxt c)
{
  if(wn_force_optimize_stop_flag)
  {
    return(TRUE);
  }
  if((c->dotgg)[0][0] == 0.0)  /* unlikely */
  {
    return(TRUE);
  }
#if 0
  if(c->iters_since_best_f0 > 10)  /* not improving any more */
  {
    return(TRUE);
  }
#endif

  return(FALSE);
}


local void compute_v_test(cntxt c)
{
  double error;
  double b[3];
  double **m,**local_vs,**local_gs;

  m = c->m; local_vs = c->local_vs; local_gs = c->local_gs;

  m[0][0] = m[1][1] = 1.0;
  m[1][0] = m[0][1] = 0.0;
  b[0] = b[1] = 0.0;

  local_vs[0][0] = 0.0;  local_vs[0][1] = 0.0;
  /*
  local_vs[1][0] = 1.0;  local_vs[1][1] = 0.0;
  local_vs[2][0] = 0.0;  local_vs[2][1] = 1.0;
  */

  wn_fit_quadratic_to_grads(&error,m,b,local_vs,local_gs,3,2,10);
}


void wn_naylor_funcgrad_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  int num_vars,
  double (*pfuncgrad)(double grad[],double vect[]),
  bool (*pterminate)(double vect[],double best_f),
  double first_jump_len,
  int max_iterations
)
{
  cntxt_s cntxt_mem,*c;
  int iteration;
  bool success0,success1;

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
  c->max_iterations = max_iterations;

  initialize(c);

  do_first_3_funcgrad_calls(c);

  for(iteration=0;;++iteration)
  {
    if(termination_condition(c))
    {
      break;
    }
    if((iteration >= max_iterations)&&(max_iterations < WN_IHUGE))
    {
      wn_force_optimize_stop_flag = TRUE;
      break;
    }

    compute_v_test(c);  

    c->f_test = (*(c->pfuncgrad))(c->g_test,c->v_test);

    if(c->f_test < (c->f)[2])
    {
      insert_new_test(c);
    }
    else
    {
      wn_assert_notreached();
      /*
      do_linesearch(c);
      */
    }
  }

  finish(c,pcode,pval_min);
}

