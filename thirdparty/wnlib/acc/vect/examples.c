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
#include "wnmem.h"

#include "wnvect.h"
#include "wnpoly.h"


#define LEN   10


#if 0
int main(void)
{
  double *vect1,*vect2,*vect3;

  wn_gpmake("general_free");

  wn_make_vect(&vect1,LEN);
  wn_make_vect(&vect2,LEN);
  wn_make_vect(&vect3,LEN);

  wn_random_vect(vect1,LEN);
  wn_print_vect(vect1,LEN);
  wn_copy_vect(vect2,vect1,LEN);
  wn_print_vect(vect1,LEN);
  wn_random_vect(vect2,LEN);

  printf("v1.v2 = %lf\n",wn_dot_vects(vect1,vect2,LEN));
  printf("v1.v1 = %lf\n",wn_dot_vects(vect1,vect1,LEN));
  printf("norm2(v1) = %lf\n",wn_norm2_vect(vect1,LEN));
  printf("norm(v1) = %lf\n",wn_norm_vect(vect1,LEN));
  printf("corr(v1,v2)) = %lf\n",
	 wn_dot_vects(vect1,vect2,LEN)/
	 (wn_norm_vect(vect1,LEN)*wn_norm_vect(vect2,LEN)));

  wn_free_vect(vect1,LEN);

  wn_gpfree();

  return(0);
}
#endif




#define A0  (1.0)
#define A1  (1.0)
#define A2  (2.0*WN_E - 5.0)
#define A3  (3.0-WN_E)

#define DA0 (A1)
#define DA1 (2.0*A2)
#define DA2 (3.0*A3)



local double table[10];


local /*inline*/ double fast_exp(double x)
{
    int xi;
    double x_diff,y_est;

    if(x < 9.0)
    {
      xi = (int)x;
      x_diff = x-xi;

      y_est = table[xi]*wn_eval_poly3(x_diff,A0,A1,A2,A3);
    }
    else
    {
      y_est = WN_FHUGE;
    }

    return(y_est);
}


#define fast_exp_macro(_x) (table[(int)(_x)]*wn_eval_polynomial3((_x)-((int)(_x)),A0,A1,A2,A3))


int main(void)
{
  double x,x_diff,y,y_est;
  int xi,i;

  y = 1.0;
  for(i=0;i<10;++i)
  {
    table[i] = y;

    y *= WN_E;
  }

  /*
  for(x=0.0;x<=5.0;x+=5.0/20.0)
  for(x=0.0;x<=5.0;x+=5.0/100000000.0)
  */
  x = 0.01;
  for(i=0;i<10000000;++i)
  {
    /*
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    y_est = 1.1-0.5*exp(x);
    x = y_est;
    */
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    y_est = 1.1-0.5*fast_exp(x);
    x = y_est;
    /*
    y = exp(x);
    y_est = wn_eval_polynomial2(x,DA0,DA1,DA2);
    y_est = wn_eval_polynomial3(x,A0,A1,A2,A3);
    */

    /*
    xi = (int)x;
    x_diff = x-xi;

    y_est = table[xi]*wn_eval_polynomial3(x_diff,A0,A1,A2,A3);
    */

    /*
    y_est = fast_exp(x);
    */

    /*
    y_est = fast_exp_macro(x);
    */

    /*
    printf("x=%lg,y=%lg,y_est=%lg,quot=%lg\n",x,y,y_est,y_est/y-1.0);
    printf("y_est=%lg\n",y_est);
    x = y_est;
    */
  }

  printf("y_est=%lg\n",y_est);

  return(0);
}

