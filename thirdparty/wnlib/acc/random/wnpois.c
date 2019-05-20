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

#include <math.h>

#include "wnlib.h"

#include "wnrndd.h"



int wn_poisson_distribution(double mean)
{
  int x;
  double prob,sum_prob_x,prob_x,mean_pow_x,x_factorial,full_integral;

  prob = wn_flat_distribution();

  full_integral = exp(-mean);
  sum_prob_x = 0.0;
  mean_pow_x = 1.0;
  x_factorial = 1.0;

  x = 0;

  for(;;)
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



