/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnsqr.h"
#include "wnmem.h"
#include "wnvect.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wnffln.h"
#include "wnmqf.h"



int wn_minimize_quad_form_verbose = 0;



/*
  minimize f(x) = (1/2)*x^t*A*x + g*x
  whose gradient is
           grad(x) = A*x + g
*/
void wn_minimize_quad_form
(
  int *pcode,
  double *pob,     
  double *palpha,  /* recommend line search len of final iteration */
  double *p,       /* search direction (negative search direction) */
  double *x,       /* current best solution */
  double *g,
  wn_ffln_type A,
  int num_iterations
)
{
  int i;
  int num_vars;
  double *Ap;
  double pAp,gp,gAp;
  double beta;
  /*
  double g2,old_g2;
  */

  wn_gpmake("no_free");

  wn_assert(A->num_inputs == A->num_outputs);
  num_vars = A->num_inputs;
  if(num_iterations >= num_vars)
  {
    num_iterations = num_vars;
    *pcode = WN_SUCCESS;
  }
  else
  {
    *pcode = WN_SUBOPTIMAL;
  }

  wn_make_vect(&Ap,num_vars);

  *pob = 0.0;
  wn_zero_vect(x,num_vars);
  wn_copy_vect(p,g,num_vars);

  for(i=0;i<num_iterations;++i)
  {
    /*
    printf("g = \n");
    wn_print_vect(g,num_vars);
    printf("p = \n");
    wn_print_vect(p,num_vars);
    */
    wn_ffln_mult_vect(Ap,A,p);          /* Ap = A*p */
    pAp = wn_dot_vects(p,Ap,num_vars);  /* pAp = p*Ap */
    gp = wn_dot_vects(g,p,num_vars);    /* gp = g*p */

    if(wn_minimize_quad_form_verbose > 0)
    {
      printf("pAp=%lg,gp=%lg\n",
             pAp,gp);
    }
    /*
    printf("pAp=%lg,gp=%lg,norm(g)=%lg,norm(p)=%lg\n",
           pAp,gp,wn_norm_vect(g,num_vars),wn_norm_vect(p,num_vars));
    printf("norm(Ap)=%lg\n",wn_norm_vect(Ap,num_vars));
    */

    if(pAp <= 0.0)  /* non-positive curvature */
    {
      if(pAp == 0.0)
      {
        *palpha = 0.0;
      }
      else
      {
        *palpha = -gp/pAp; 
      }

      *pcode = WN_UNBOUNDED;  
      break;
    }

    *palpha = -gp/pAp; 
    /*
    printf("alpha = %lg\n",*palpha);
    */

    wn_add_scaled_vect(x,p,*palpha,num_vars);  /* x += alpha*p */
    /*
    printf("x = \n");
    wn_print_vect(x,num_vars);
    */
    (*pob) += 0.5*wn_square(*palpha)*pAp + (*palpha)*gp; 
    if(wn_minimize_quad_form_verbose > 0)
    {
      printf("i=%d,ob=%lg\n",i,*pob);
    }
    /*
    old_g2 = wn_norm2_vect(g,num_vars);
    */
    wn_add_scaled_vect(g,Ap,*palpha,num_vars); /* g += alpha*Ap */
    /*
    g2 = wn_norm2_vect(g,num_vars);
    printf("after g = \n");
    wn_print_vect(g,num_vars);
    */

    gAp = wn_dot_vects(g,Ap,num_vars);         /* gAp = g*Ap */
    beta = -gAp/pAp;
    /*
    beta = -pAp/gAp;
    beta = g2/old_g2;
    beta = 0.0;
    printf("beta = %lg\n",beta);
    */
    wn_add_vect_and_scaled_vect(p,g,p,beta,num_vars); /* p = g + beta*p */
  }

  wn_gpfree();
}

