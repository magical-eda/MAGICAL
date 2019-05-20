/*
 * Calls the blas routine to compute a norm of a vector.
 */

#include <math.h>
#include "declarations.h"

double norm2(n,x)
     int n;
     double *x;
{
  double nrm;
  int incx=1;

#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
  nrm=DNRM2(&n,x,&incx);
#else
  nrm=dnrm2(&n,x,&incx);
#endif
#else
#ifdef CAPSBLAS
  nrm=DNRM2_(&n,x,&incx);
#else
  nrm=dnrm2_(&n,x,&incx);
#endif
#endif
  
  return(nrm);
}

double norm1(n,x)
     int n;
     double *x;
{
  double nrm;
  int incx=1;

#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
  nrm=DASUM(&n,x,&incx);
#else
  nrm=dasum(&n,x,&incx);
#endif
#else
#ifdef CAPSBLAS
  nrm=DASUM_(&n,x,&incx);
#else
  nrm=dasum_(&n,x,&incx);
#endif
#endif
  
  return(nrm);
}

double norminf(n,x)
     int n;
     double *x;
{
  int i;
  double nrm;
  int incx=1;

#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
  i=IDAMAX(&n,x,&incx);
  nrm=fabs(x[i-1]);
#else
  i=idamax(&n,x,&incx);
  nrm=fabs(x[i-1]);
#endif
#else
#ifdef CAPSBLAS
  i=IDAMAX_(&n,x,&incx);
  nrm=fabs(x[i-1]);
#else
  i=idamax_(&n,x,&incx);
  nrm=fabs(x[i-1]);
#endif
#endif
  
  return(nrm);
}



