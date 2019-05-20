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
#ifndef wnrnddH
#define wnrnddH


#include "wnlib.h"


EXTERN bool wn_true_with_probability(double prob);

EXTERN int wn_random_int_between(int low,int high);
EXTERN double wn_random_double_between(double low,double high);
EXTERN double wn_flat_distribution(void);

EXTERN double wn_arbitrary_distribution
(
  double (*pinverse_integral_density_func)(double integrated_prob)
);
EXTERN double wn_normal_distribution(void);
EXTERN void wn_normal_distribution_2vals(double *pval1, double *pval2);
EXTERN void wn_normal_distribution_2vals_slow(double *pval1, double *pval2);
EXTERN double wn_exponential_distribution(void);
EXTERN int wn_poisson_distribution(double mean);
EXTERN double wn_cauchy_distribution(void);


#endif

