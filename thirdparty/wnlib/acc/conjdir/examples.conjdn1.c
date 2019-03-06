/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnvect.h"
#include "wnrndd.h"

#include "wnconj.h"


#define NUM_VARS  2


local double delta_vect[2];
local int grad_count,func_count;


/*
  function = 100(x2-x1^3)^2 + (1-x1)^2
*/
local double function(double vect[])
{
  double x1,x2;
  double diff1,diff2;
  double ob;

  x1 = vect[0];
  x2 = vect[1];

  diff1 = x2-x1*x1*x1;
  diff2 = 1.0-x1;

  ob = (100.0*diff1*diff1 + diff2*diff2);

  /*
  printf("ob = %lg,func_count = %d\n",ob,func_count);
  */
  ++func_count;

  /*
  ob += 0.01*wn_normal_distribution();
  printf("ob+random = %lg\n",ob);
  */

  return(ob);
}


local void repeat_function(double *pmean,double *psdev,
			   double vect[NUM_VARS],int num_evals)
{
  int i;
  double noise;
  double func_val;
  double sum,sum2;

  *pmean = function(vect);
  func_val = *pmean;
  /*
  *psdev = 1.0;
  *pmean += ((*psdev)/sqrt(num_evals))*wn_normal_distribution();
  */

  sum = 0.0;
  sum2 = 0.0;

  for(i=0;i<num_evals;++i)
  {
    noise = wn_normal_distribution();
    /*
    noise = 0.0;
    */

    sum += noise;
    sum2 += noise*noise;
  }

  sum /= num_evals;
  sum2 /= num_evals;

  *pmean += sum;
  *psdev = sqrt(sum2-sum*sum);

  printf("repeat_function: func_val = %lg, with noise = %lg\n",
	 func_val,*pmean);
}


local void gradient(double grad[],double vect[])
{
  double x1,x2;
  double diff1,diff2;
  double ob;

  printf("gradient.  count = %d\n",grad_count);
  ++grad_count;

  /*
  wn_numerical_gradient(grad,vect,delta_vect,2,&function);
  printf("end of numerical gradient.\n");
  */

  x1 = vect[0];
  x2 = vect[1];

  diff1 = x2-x1*x1*x1;
  diff2 = 1.0-x1;

  grad[0] = -600.0*diff1*x1*x1 - 2.0*diff2;
  grad[1] = 200.0*diff1;
  /*
  */
}


local double funcgrad(double grad[],double vect[])
{
  gradient(grad,vect);
  return(function(vect));
}


int main(void)
{
  double vect[2];
  double mins[2],maxes[2];
  int code;
  double val_min;

  /*
  wn_gpmake("no_free");
  */

  grad_count = func_count = 0;

  vect[0] = -1.2;
  vect[1] = -1.0;

  mins[0] = -1.0;
  mins[1] = -1.0;
  maxes[0] = 0.0;
  maxes[1] = 0.5;

  delta_vect[0] = 0.1;
  delta_vect[1] = 0.1;

  /*
  wn_conj_funcgrad_method(&code,&val_min,vect,NUM_VARS,
			  &funcgrad,(bool (*)(double *,double,int))NULL,
			  0.0001,
			  0,2.0,
			  WN_IHUGE);
  wn_conj_gradient_method(&code,&val_min,vect,NUM_VARS,(function),(gradient),WN_IHUGE);
  wn_conj_direction_method(&code,&val_min,vect,delta_vect,NUM_VARS,(function),200);
  wn_conj_direction_method(&code,&val_min,vect,NULL,NUM_VARS,(function),WN_IHUGE);
  */
  wn_conj_direction_noise_debug = WN_CONJ_DIR_DBG_ALL;
  wn_conj_direction_noise_method(&code,&val_min,vect,
				 /*mins,maxes,*/
				 NULL,NULL,
				 NULL,
				 0,
				 NUM_VARS,
				 &repeat_function,1,WN_IHUGE);

  printf("code = %d\n",code);
  printf("objective = %lg\n",val_min);
  val_min = function(vect);
  printf("actual objective = %lg\n",val_min);
  wn_print_vect(vect,NUM_VARS);
  printf("grad_count = %d,func_count = %d\n",grad_count,func_count);
}
