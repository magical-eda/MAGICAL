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


/**********************************************************************

  Consult any probability textbook for an explanation of this 
  very well-known algorithm.

**********************************************************************/
double wn_normal_distribution(void)
{
  double x,r,theta;

  r = sqrt(-2.0*log(wn_flat_distribution()));
  theta = WN_PI*wn_flat_distribution();

  x = r*cos(theta);

  return(x);
}

/**********************************************************************

  Just like the above, but returns two values for (more or less) the
  price of one.

**********************************************************************/
void wn_normal_distribution_2vals(double *pval1, double *pval2)
{
  double x1, x2, rsq, adj, log_rsq;

  do
  {
    x1 = wn_flat_distribution();
    x2 = wn_flat_distribution();
    x1 = x1 + x1 - 1;
    x2 = x2 + x2 - 1;
    rsq = x1 * x1 + x2 * x2;
  } while (rsq >= 1);

  log_rsq = log(rsq);
  adj = sqrt(-(log_rsq + log_rsq) / rsq);
  *pval1 = x1 * adj;
  *pval2 = x2 * adj;
}

/**********************************************************************

  Another version of the above. Slower.

**********************************************************************/
void wn_normal_distribution_2vals_slow(double *pval1, double *pval2)
{
  double r,theta;
  /* double x; ** unused - bchapman 041111 */

  r = sqrt(-2.0*log(wn_flat_distribution()));
  theta = (WN_PI+WN_PI)*wn_flat_distribution();

  *pval1 = r*cos(theta);
  *pval2 = r*sin(theta);
}

