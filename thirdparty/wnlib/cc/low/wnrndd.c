/**********************************************************************

bool wn_true_with_probability(prob)

int wn_random_int_between(low,high)
double wn_random_double_between(low,high)

double wn_flat_distribution()
double wn_normal_distribution()
double wn_exponential_distribution()

double wn_arbitrary_distribution(pinverse_integral_density_func)

int wn_poisson_distribution(mean)

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
#include "wncons.h"

#include <math.h>




bool wn_true_with_probability(prob)

double prob;

{
  double wn_flat_distribution();

  return(wn_flat_distribution() < prob);
}


int wn_random_int_between(low,high)

int low,high;

{
  extern int wn_random_mod_int();

  return(wn_random_mod_int(high-low)+low);
}



double wn_random_double_between(low,high)

double low,high;

{
  double wn_flat_distribution();

  return((high-low)*wn_flat_distribution()+low);
}



#define RANGE_BITS  30
#define RANGE       (1<<RANGE_BITS)


double wn_flat_distribution()
{
  double ret;

  ret = (double)wn_random_n_bits(30);

  ret += 0.5;

  ret = ret/((double)RANGE);

  return(ret);
}



/**********************************************************************

  Consult any probability textbook for an explanation of this 
  very well-known algorithm.

**********************************************************************/
double wn_normal_distribution()
{
  double inverse_integral_r_density_func(),
         wn_flat_distribution(),
         wn_arbitrary_distribution();
  double x,r,theta;

  r = wn_arbitrary_distribution((inverse_integral_r_density_func));
  theta = WN_PI*wn_flat_distribution();

  x = r*cos(theta);

  return(x);
}



local double inverse_integral_r_density_func(prob_r)

double prob_r;

{
  extern double sqrt(),log();

  return(sqrt(-2.0*log(1.0-prob_r)));
}



double wn_exponential_distribution()
{
  double inverse_integral_exponential_density_func(),
         wn_arbitrary_distribution();

  return(
     wn_arbitrary_distribution((inverse_integral_exponential_density_func)));
}



local double inverse_integral_exponential_density_func(prob)

double prob;

{
  extern double log();

  return(-log(1.0-prob));
}



double wn_arbitrary_distribution(pinverse_integral_density_func)

double (*pinverse_integral_density_func)(/*integrated_prob*/);

{
  double integrated_prob;

  integrated_prob = wn_flat_distribution();

  return((*pinverse_integral_density_func)(integrated_prob));
}



int wn_poisson_distribution(mean)

double mean;

{
  extern double exp();
  int x;
  double prob,sum_prob_x,prob_x,mean_pow_x,x_factorial,full_integral;

  prob = wn_flat_distribution();

  full_integral = exp(-mean);
  sum_prob_x = 0.0;
  mean_pow_x = 1.0;
  x_factorial = 1.0;

  x = 0;

  while(TRUE)
  {
    prob_x = mean_pow_x*full_integral/x_factorial; 
    sum_prob_x += prob_x;

    if(sum_prob_x > prob)
    {
      break;
    }

    x++;

    mean_pow_x *= mean;
    x_factorial *= x;
  }

  return(x);
}



